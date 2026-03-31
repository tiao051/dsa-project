#include "../../include/order_manager.h"

static const char* getPriorityText(PriorityLevel priority) {
	if (priority == PRIORITY_EXPRESS) return "Express";
	if (priority == PRIORITY_VIP) return "VIP";
	return "Thuong";
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

static int readOrderId(OrderQueue* q, int* out_order_id) {
	printf("\n\t\t\t\t\t\tNhap Ma Don: ");
	if (scanf("%d", out_order_id) != 1 || *out_order_id <= 0) {
		clearInputBuffer();
		showErrorMessage("[!] Ma don khong hop le!");
		return 0;
	}

	if (findOrderById(q, *out_order_id) != NULL) {
		clearInputBuffer();
		showErrorMessage("[!] Ma don da ton tai trong hang doi!");
		return 0;
	}

	clearInputBuffer();
	return 1;
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
		setColor(7);
		return 0;
	}
	return 1;
}

// Create and input a new order with stock validation
void insertOrderManual(OrderQueue* q) {
	Order order;
	CustomerNode* customer_node = NULL;
	int product_index = -1;

	if (!readOrderId(q, &order.id)) return;

	if (!readTrimmedLine("Nhap Ten Khach Hang: ", order.customer_name, sizeof(order.customer_name),
		"[!] Ten khach hang khong duoc de trong!")) return;

	if (!resolveOrderCustomer(order.customer_name, &customer_node, &order.priority)) return;

	if (!readTrimmedLine("Nhap Ten San Pham: ", order.product_name, sizeof(order.product_name),
		"[!] Ten san pham khong duoc de trong!")) return;

	if (!resolveOrderProduct(order.product_name, &product_index)) return;

	if (!readOrderQuantity(&order.quantity)) return;

	if (!validateAvailableStock(q, product_index, order.quantity)) return;

	order.price = inventory[product_index].price;

	printf("\n\t\t\t\t\t\tUu tien don (theo hang KH): %s", getPriorityText(order.priority));
	printf("\n\t\t\t\t\t\tDon gia ap dung: %lld", order.price);

	if (enqueueOrder(q, order)) {
		printf("\n\t\t\t\t\t\t-> THEM DON HANG THANH CONG!!!!\n");
	}

}

// Print single order
void printSingleOrder(Order order) {
	char ma_don[24];
	sprintf(ma_don, "Order%d", order.id);
	printf("\t\t| %-16s | %-25s | %-25s | %-12d | %-10s | %8lld |\n",
		ma_don,
		order.customer_name,
		order.product_name,
		order.quantity,
		getPriorityText(order.priority),
		order.price);
}

// Display all orders in queue
void displayOrderQueue(OrderQueue* q) {
	printf("\t\t------------------------------------------------DANH SACH DON HANG-------------------------------------------------\n");
	printOrderHeader();
	OrderNode* node = q->head;
	while(node != NULL)
	{
		printSingleOrder(node->info);
		node = node->next;
	}
	printf("\t\t-------------------------------------------------------------------------------------------------------------------\n");
}