#include "../include/order_manager.h"

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

// Create and input a new order with stock validation
void insertOrderManual(OrderQueue* q) {
	Order order;
	CustomerNode* customer_node = NULL;
	int product_index = -1;
	int reserved_quantity = 0;
	int available_stock = 0;

	printf("\n\t\t\t\t\t\tNhap Ma Don: ");
	if (scanf("%d", &order.id) != 1 || order.id <= 0) {
		while (getchar() != '\n');
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Ma don khong hop le!");
		setColor(7);
		return;
	}

	if (findOrderById(q, order.id) != NULL) {
		while (getchar() != '\n');
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Ma don da ton tai trong hang doi!");
		setColor(7);
		return;
	}

	getchar();
	
	printf("\n\t\t\t\t\t\tNhap Ten Khach Hang: ");
	fgets(order.customer_name, sizeof(order.customer_name), stdin);
	order.customer_name[strcspn(order.customer_name, "\n")] = '\0';
	trimString(order.customer_name);

	if (strlen(order.customer_name) == 0) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Ten khach hang khong duoc de trong!");
		setColor(7);
		return;
	}

	customer_node = findCustomerByName(&customer_list, order.customer_name);
	if (customer_node == NULL) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Khong tim thay khach hang: %s", order.customer_name);
		setColor(7);
		return;
	}

	if (_stricmp(customer_node->info.status, "Khoa") == 0) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Khach hang %s dang bi khoa, khong the tao don hang!", customer_node->info.name);
		setColor(7);
		return;
	}

	order.priority = mapTierToPriority(customer_node->info.tier);

	printf("\n\t\t\t\t\t\tNhap Ten San Pham: ");
	fgets(order.product_name, sizeof(order.product_name), stdin);
	order.product_name[strcspn(order.product_name, "\n")] = '\0';
	trimString(order.product_name);

	if (strlen(order.product_name) == 0) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Ten san pham khong duoc de trong!");
		setColor(7);
		return;
	}

	product_index = getProductIndexByName(order.product_name);
	if (product_index == -1) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Khong tim thay san pham: %s", order.product_name);
		setColor(7);
		return;
	}

	// KIEM TRA SO LUONG TON KHO (co tru truoc so luong da dat trong queue)
	printf("\n\t\t\t\t\t\tNhap So Luong San Pham: ");
	if (scanf("%d", &order.quantity) != 1 || order.quantity <= 0) {
		while (getchar() != '\n');
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] So luong khong hop le!");
		setColor(7);
		return;
	}
	getchar();

	reserved_quantity = getReservedQuantityInQueue(q, inventory[product_index].name);
	available_stock = inventory[product_index].stock_quantity - reserved_quantity;
	if (available_stock < order.quantity) {
		setColor(4);
		printf("\n\t\t\t\t\t\tTU CHOI: SAN PHAM CHI CON %d CHUA DUOC DAT", available_stock > 0 ? available_stock : 0);
		setColor(7);
		return;
	}

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