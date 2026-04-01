#include "../../include/order_manager.h"

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
	int max_id = 100;
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

static int validateAvailableStock(OrderQueue* q, int product_index, int order_quantity) {
	int reserved_quantity = getReservedQuantityInQueue(q, inventory[product_index].name);
	int available_stock = inventory[product_index].stock_quantity - reserved_quantity;
	if (available_stock < order_quantity) {
		setColor(4);
		printf("\n\t\t\t\t\t\tTU CHOI: SAN PHAM CHI CON %d CHUA DUOC DAT", available_stock > 0 ? available_stock : 0);
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
void insertOrderManual(OrderQueue* q) {
	Order order;
	CustomerNode* customer_node = NULL;
	int product_index = -1;
	PriorityLevel tier_priority;

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
		if (!readTrimmedLine("Nhap Ten San Pham: ", order.product_name, sizeof(order.product_name),
			"[!] Ten san pham khong duoc de trong!")) {
			continue;
		}

		if (resolveOrderProduct(order.product_name, &product_index)) {
			break;
		}
	}

	while (1) {
		if (!readOrderQuantity(&order.quantity)) {
			continue;
		}

		if (validateAvailableStock(q, product_index, order.quantity)) {
			break;
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

	if (enqueueOrder(q, order)) {
		printf("\n\t\t\t\t\t\t-> THEM DON HANG THANH CONG!!!!\n");
	}

}

// Print single order
void printSingleOrder(Order order) {
	char ma_don[24];
	sprintf(ma_don, "Order%d", order.id);
	printf("\t| %-16s | %-20s | %-20s | %-8d | %-10s | %-10s | %-18s | %10lld |\n",
		ma_don,
		order.customer_name,
		order.product_name,
		order.quantity,
		getShippingMethodText(order.shipping_method),
		getPriorityText(order.priority),
		order.status,
		order.price);
}

// Display all orders in queue
void displayOrderQueue(OrderQueue* q) {
	printf("\t\t----------------------------------------------- DANH SACH DON HANG -----------------------------------------------\n");
	printOrderHeader();
	OrderNode* node = q->head;
	while(node != NULL)
	{
		printSingleOrder(node->info);
		node = node->next;
	}
	printf("\t\t-------------------------------------------------------------------------------------------------------------------\n");
}

void processParallelPackaging(OrderQueue* q) {
	int station_count = PACKING_STATION_COUNT;
	int processed_count = 0;
	long long total_revenue = 0;

	if (isOrderQueueEmpty(q)) {
		setColor(4);
		printf("\n\t\t\t\t\t\tKhong co don nao trong hang doi!");
		setColor(7);
		return;
	}

	if (station_count <= 0) {
		showErrorMessage("[!] Cau hinh PACKING_STATION_COUNT khong hop le!");
		return;
	}

	printf("\n\t\t\t\t\t\tSo tram dong goi cau hinh san: %d", station_count);

	int round = 1;
	while (!isOrderQueueEmpty(q)) {
		printf("\n\n\t\t\t\t\t\t--- Dot dong goi %d ---", round);
		for (int station = 1; station <= station_count && !isOrderQueueEmpty(q); station++) {
			Order processed_order;
			printf("\n\t\t\t\t\t\t[Tram %d] Dang nhan don uu tien cao nhat...", station);
			if (dequeueOrder(q, &processed_order)) {
				long long revenue = processed_order.price * (long long)processed_order.quantity;
				total_revenue += revenue;
				processed_count++;
			}
		}
		round++;
	}

	setColor(2);
	printf("\n\n\t\t\t\t\t\tHOAN TAT DONG GOI!");
	printf("\n\t\t\t\t\t\tTong so don da giao cho DVVC: %d", processed_count);
	printf("\n\t\t\t\t\t\tTong doanh thu ghi nhan: %lld", total_revenue);
	setColor(7);
}