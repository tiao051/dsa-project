#include "../include/order_manager.h"

// Create and input a new order with stock validation
void insertOrderManual(OrderQueue* q) {
	Order order;
	printf("\n\t\t\t\t\t\tNhap Ma Don: ");
	scanf("%d", &order.id);
	getchar();
	
	printf("\n\t\t\t\t\t\tNhap Ten Khach Hang: ");
	fgets(order.customer_name, sizeof(order.customer_name), stdin);
	order.customer_name[strlen(order.customer_name) - 1] = '\0';

	printf("\n\t\t\t\t\t\tNhap Ten San Pham: ");
	fgets(order.product_name, sizeof(order.product_name), stdin);
	order.product_name[strlen(order.product_name) - 1] = '\0';

	// KIEM TRA SO LUONG TON KHO
	printf("\n\t\t\t\t\t\tNhap So Luong San Pham: ");
	scanf("%d", &order.quantity);
	for (int i = 0; i < product_count; i++)
	{
		if (strcmp(inventory[i].name, order.product_name) == 0) {
			if (inventory[i].stock_quantity < order.quantity) {
				printf("\n\t\t\t\t\t\tTU CHOI: KHO CON %d SAN PHAM, VUI LONG MUA IT NHAT HOAC CHON SAN PHAM KHAC", inventory[i].stock_quantity);
				return;
			}
			order.price = inventory[i].price;
		}

	}
	printf("\n\t\t\t\t\t\tHAY LUA CHON CAC UU TIEN SAU: 2: HOA TOC, 1: VIP, 0: THUONG: ");
	int priority_choice;
	scanf("%d", &priority_choice);
	getchar();
	if (priority_choice >= 0 && priority_choice <= 2) {
		order.priority = (PriorityLevel)priority_choice;
	} else {
		order.priority = PRIORITY_NORMAL;
	}

	printf("\n\t\t\t\t\t\tNhap Don Gia: ");
	scanf("%lld", &order.price);
	getchar();

	if (enqueueOrder(q, order)) {
		printf("\n\t\t\t\t\t\t-> THEM DON HANG THANH CONG!!!!\n");
	}

}

// Print single order
void printSingleOrder(Order order) {
	printf("\t\t| Order%-10d | %-25s | %-25s | %-12d | %-10d | %8lld |\n",
		order.id,
		order.customer_name,
		order.product_name,
		order.quantity,
		order.priority,
		order.price);
}

// Display all orders in queue
void displayOrderQueue(OrderQueue* q) {
	printf("\t\t------------------------------------------------ORDER LIST-------------------------------------------------\n");
	printOrderHeader();
	OrderNode* node = q->head;
	while(node != NULL)
	{
		printSingleOrder(node->info);
		node = node->next;
	}
	printf("\t\t-------------------------------------------------------------------------------------------------------------------\n");
}