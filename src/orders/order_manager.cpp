#include "../../include/order_manager.h"
#include <thread>
#include <atomic>
#include <mutex>

static std::atomic<bool> g_background_processing_running(false);
static std::mutex g_background_order_mutex;

static OrderNode* findOrderNodeById(OrderQueue* q, int id) {
	if (q == NULL) return NULL;
	OrderNode* node = q->head;
	while (node != NULL) {
		if (node->info.id == id) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

static int removeOrderById(OrderQueue* q, int id, Order* out_order) {
	if (q == NULL) return 0;

	OrderNode* prev = NULL;
	OrderNode* node = q->head;
	while (node != NULL) {
		if (node->info.id == id) {
			if (out_order != NULL) {
				*out_order = node->info;
			}

			if (prev == NULL) {
				q->head = node->next;
			}
			else {
				prev->next = node->next;
			}

			if (node == q->tail) {
				q->tail = prev;
			}

			delete node;
			return 1;
		}

		prev = node;
		node = node->next;
	}

	return 0;
}

static void sleepPhaseSeconds(int seconds) {
	if (seconds > 0) {
		Sleep(seconds * 1000);
	}
}

static const char* getPriorityText(PriorityLevel priority) {
	if (priority == PRIORITY_EXPRESS) return "Express";
	if (priority == PRIORITY_VIP) return "VIP";
	return "Thuong";
}

static const char* getShippingMethodText(ShippingMethod method) {
	if (method == SHIPPING_EXPRESS) return "Hoa toc";
	return "Tieu chuan";
}

static PriorityLevel mapTierToPriority(CustomerTier tier) {
	if (tier == TIER_EXPRESS) return PRIORITY_EXPRESS;
	if (tier == TIER_VIP) return PRIORITY_VIP;
	return PRIORITY_NORMAL;
}

static int getProductIndexByName(const char* product_name) {
	for (int i = 0; i < product_count; i++) {
		if (_stricmp(inventory[i].name, product_name) == 0) {
			return i;
		}
	}
	return -1;
}

static int getReservedQuantityInQueue(OrderQueue* q, const char* product_name) {
	std::lock_guard<std::mutex> guard(g_background_order_mutex);

	int reserved = 0;
	OrderNode* node = q->head;
	while (node != NULL) {
		if (_stricmp(node->info.product_name, product_name) == 0) {
			reserved += node->info.quantity;
		}
		node = node->next;
	}
	return reserved;
}

static int generateNextOrderId(OrderQueue* q) {
	std::lock_guard<std::mutex> guard(g_background_order_mutex);

	int max_id = 100;

	FILE* file_ptr = fopen("data/orders.txt", "rt");
	if (file_ptr != NULL) {
		char line[512];
		// Skip header count line.
		if (fgets(line, sizeof(line), file_ptr) != NULL) {
			while (fgets(line, sizeof(line), file_ptr) != NULL) {
				int file_order_id = 0;
				if (sscanf(line, "%d,", &file_order_id) == 1 && file_order_id > max_id) {
					max_id = file_order_id;
				}
			}
		}
		fclose(file_ptr);
	}

	OrderNode* node = q->head;

	while (node != NULL) {
		if (node->info.id > max_id) {
			max_id = node->info.id;
		}
		node = node->next;
	}

	return max_id + 1;
}

static int readTrimmedLine(const char* prompt, char* output, size_t output_size, const char* empty_message) {
	printf("\n\t\t\t\t\t\t%s", prompt);
	fgets(output, (int)output_size, stdin);
	output[strcspn(output, "\n")] = '\0';
	trimString(output);

	if (strlen(output) == 0) {
		showErrorMessage(empty_message);
		return 0;
	}
	return 1;
}

static int resolveOrderCustomer(const char* customer_name, CustomerNode** out_customer_node, PriorityLevel* out_priority) {
	*out_customer_node = findCustomerByName(&customer_list, customer_name);
	if (*out_customer_node == NULL) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Khong tim thay khach hang: %s", customer_name);
		setColor(7);
		return 0;
	}

	if (_stricmp((*out_customer_node)->info.status, "Khoa") == 0) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Khach hang %s dang bi khoa, khong the tao don hang!", (*out_customer_node)->info.name);
		setColor(7);
		return 0;
	}

	*out_priority = mapTierToPriority((*out_customer_node)->info.tier);
	return 1;
}

static int resolveOrderProduct(const char* product_name, int* out_product_index) {
	*out_product_index = getProductIndexByName(product_name);
	if (*out_product_index == -1) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Khong tim thay san pham: %s", product_name);
		setColor(7);
		return 0;
	}
	return 1;
}

static int readOrderQuantity(int* out_quantity) {
	printf("\n\t\t\t\t\t\tNhap So Luong San Pham: ");
	if (scanf("%d", out_quantity) != 1 || *out_quantity <= 0) {
		clearInputBuffer();
		showErrorMessage("[!] So luong khong hop le!");
		return 0;
	}
	clearInputBuffer();
	return 1;
}

// Return value:
// 1  -> valid stock for current quantity
// 0  -> product still has stock but requested quantity is too high
// -1 -> product is out of stock, caller should reselect another product
static int validateAvailableStock(OrderQueue* q, int product_index, int order_quantity) {
	int reserved_quantity = getReservedQuantityInQueue(q, inventory[product_index].name);
	int available_stock = inventory[product_index].stock_quantity - reserved_quantity;

	if (available_stock <= 0) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] SAN PHAM DA HET HANG. VUI LONG CHON SAN PHAM KHAC.");
		setColor(7);
		return -1;
	}

	if (available_stock < order_quantity) {
		setColor(4);
		printf("\n\t\t\t\t\t\tTU CHOI: SAN PHAM CHI CON %d CHUA DUOC DAT", available_stock);
		printf("\n\t\t\t\t\t\tGOI Y: Vui long giam so luong hoac chon san pham khac.");
		setColor(7);
		return 0;
	}

	return 1;
}

static int readShippingMethod(ShippingMethod* out_shipping_method) {
	int shipping_choice;
	printf("\n\t\t\t\t\t\tChon phuong thuc van chuyen (1. Hoa toc | 2. Tieu chuan): ");
	if (scanf("%d", &shipping_choice) != 1) {
		clearInputBuffer();
		showErrorMessage("[!] Phuong thuc van chuyen khong hop le!");
		return 0;
	}
	clearInputBuffer();

	if (shipping_choice == 1) {
		*out_shipping_method = SHIPPING_EXPRESS;
		return 1;
	}
	if (shipping_choice == 2) {
		*out_shipping_method = SHIPPING_STANDARD;
		return 1;
	}

	showErrorMessage("[!] Chi duoc chon 1 hoac 2!");
	return 0;
}

// Create and input a new order with stock validation
int insertOrderManual(OrderQueue* q) {
	Order order;
	CustomerNode* customer_node = NULL;
	int product_index = -1;
	PriorityLevel tier_priority;
	int created_success = 0;

	order.id = generateNextOrderId(q);
	printf("\n\t\t\t\t\t\tMa don duoc tao tu dong: %d", order.id);

	while (1) {
		if (!readTrimmedLine("Nhap Ten Khach Hang: ", order.customer_name, sizeof(order.customer_name),
			"[!] Ten khach hang khong duoc de trong!")) {
			continue;
		}

		if (resolveOrderCustomer(order.customer_name, &customer_node, &tier_priority)) {
			break;
		}
	}

	while (1) {
		while (1) {
			if (!readTrimmedLine("Nhap Ten San Pham: ", order.product_name, sizeof(order.product_name),
				"[!] Ten san pham khong duoc de trong!")) {
				continue;
			}

			if (resolveOrderProduct(order.product_name, &product_index)) {
				break;
			}
		}

		int stock_state = 0;
		while (1) {
			if (!readOrderQuantity(&order.quantity)) {
				continue;
			}

			stock_state = validateAvailableStock(q, product_index, order.quantity);
			if (stock_state == 1) {
				break;
			}

			if (stock_state == -1) {
				break;
			}
		}

		if (stock_state == 1) {
			break;
		}

		if (stock_state == -1) {
			continue;
		}
	}

	while (!readShippingMethod(&order.shipping_method)) {}

	order.priority = tier_priority;
	if (order.shipping_method == SHIPPING_EXPRESS) {
		order.priority = PRIORITY_EXPRESS;
	}

	order.price = inventory[product_index].price;
	strcpy(order.status, "Cho dong goi");

	printf("\n\t\t\t\t\t\tVan chuyen: %s", getShippingMethodText(order.shipping_method));
	printf("\n\t\t\t\t\t\tUu tien don: %s", getPriorityText(order.priority));
	printf("\n\t\t\t\t\t\tTrang thai: %s", order.status);
	printf("\n\t\t\t\t\t\tDon gia ap dung: %lld", order.price);

	{
		std::lock_guard<std::mutex> guard(g_background_order_mutex);
		if (enqueueOrder(q, order)) {
			saveOrderQueueWithHistory("data/orders.txt", q);
			printf("\n\t\t\t\t\t\t-> THEM DON HANG THANH CONG!!!!\n");
			created_success = 1;
		}
		else {
			showErrorMessage("[!] Khong the them don hang vao hang doi!");
		}
	}

	return created_success;
}

static void backgroundProcessOrders(OrderQueue* q) {
	if (q == NULL) {
		g_background_processing_running = false;
		return;
	}

	while (1) {
		int active_order_id = 0;

		{
			std::lock_guard<std::mutex> guard(g_background_order_mutex);
			if (isOrderQueueEmpty(q)) {
				break;
			}

			active_order_id = q->head->info.id;
			strcpy(q->head->info.status, "Dang xac nhan");
			saveOrderQueueWithHistory("data/orders.txt", q);
		}

		sleepPhaseSeconds(3);

		{
			std::lock_guard<std::mutex> guard(g_background_order_mutex);
			OrderNode* node = findOrderNodeById(q, active_order_id);
			if (node == NULL) {
				continue;
			}
			strcpy(node->info.status, "Dang dong goi");
			saveOrderQueueWithHistory("data/orders.txt", q);
		}

		sleepPhaseSeconds(3);

		{
			std::lock_guard<std::mutex> guard(g_background_order_mutex);
			OrderNode* node = findOrderNodeById(q, active_order_id);
			if (node == NULL) {
				continue;
			}
			strcpy(node->info.status, "Dang van chuyen");
			saveOrderQueueWithHistory("data/orders.txt", q);
		}

		sleepPhaseSeconds(3);

		{
			std::lock_guard<std::mutex> guard(g_background_order_mutex);
			Order completed_order;
			if (!removeOrderById(q, active_order_id, &completed_order)) {
				continue;
			}

			strcpy(completed_order.status, "Hoan thanh");
			processCompletedOrder(&completed_order);
			saveOrderQueueWithHistory("data/orders.txt", q);
		}
	}

	g_background_processing_running = false;
}

void ensureBackgroundOrderProcessing(OrderQueue* q) {
	if (q == NULL || isOrderQueueEmpty(q) || g_background_processing_running) {
		return;
	}

	g_background_processing_running = true;
	std::thread(backgroundProcessOrders, q).detach();
}

int isBackgroundOrderProcessing() {
	return g_background_processing_running ? 1 : 0;
}

int cancelOrderByIdSafe(OrderQueue* q, int order_id) {
	if (q == NULL || order_id <= 0) {
		return 0;
	}

	std::lock_guard<std::mutex> guard(g_background_order_mutex);
	Order removed_order;
	if (!removeOrderById(q, order_id, &removed_order)) {
		return 0;
	}

	saveOrderQueueWithHistory("data/orders.txt", q);
	return 1;
}

// Print single order
void printSingleOrder(Order order) {
	char ma_don[24];
	sprintf(ma_don, "Order%d", order.id);
	printf("\t\t| %-16s | %-20s | %-20s | %-8d | %-11s | %-10s | %-18s | %10lld |\n",
		ma_don,
		order.customer_name,
		order.product_name,
		order.quantity,
		getShippingMethodText(order.shipping_method),
		getPriorityText(order.priority),
		order.status,
		order.price);
}


void displayOrderProgressFromFile(const char* filename) {
	std::lock_guard<std::mutex> guard(g_background_order_mutex);

	FILE* file_ptr = fopen(filename, "rt");
	if (file_ptr == NULL) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Khong tim thay file du lieu don hang!");
		setColor(7);
		return;
	}

	int total = 0;
	if (fscanf(file_ptr, "%d\n", &total) != 1 || total <= 0) {
		fclose(file_ptr);
		setColor(3);
		printf("\n\t\t\t\t\t\tChua co don hang nao de theo doi.");
		setColor(7);
		return;
	}

	int waiting_count = 0;
	int processing_count = 0;
	int done_count = 0;

	printf("\t\t============================= TIEN DO XU LY DON HANG =============================\n");
	printOrderHeader();

	char line[512];
	while (fgets(line, sizeof(line), file_ptr) != NULL) {
		Order order;
		int priority = 0;
		int shipping_method = 0;

		int parsed = sscanf(line, "%d,%99[^,],%99[^,],%d,%lld,%d,%d,%99[^\n]",
			&order.id,
			order.customer_name,
			order.product_name,
			&order.quantity,
			&order.price,
			&priority,
			&shipping_method,
			order.status);

		if (parsed != 8) {
			continue;
		}

		order.priority = (PriorityLevel)priority;
		order.shipping_method = (ShippingMethod)shipping_method;
		trimString(order.customer_name);
		trimString(order.product_name);
		trimString(order.status);

		if (_stricmp(order.status, "Hoan thanh") == 0 || _stricmp(order.status, "Da giao cho DVVC") == 0) {
			done_count++;
		}
		else if (_stricmp(order.status, "Cho dong goi") == 0) {
			waiting_count++;
		}
		else {
			processing_count++;
		}

		printSingleOrder(order);
	}

	fclose(file_ptr);

	printf("\t\t====================================================================================\n");
	printf("\n\t\t\t\t\t\tTong don: %d | Cho dong goi: %d | Dang xu ly: %d | Hoan thanh: %d",
		total,
		waiting_count,
		processing_count,
		done_count);
}

