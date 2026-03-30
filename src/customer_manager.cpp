#include "../include/order_manager.h"

// Register new customer
void registerNewCustomer(CustomerList* customer_list) {
	Customer customer;

	printf("\n\t\t\t\t\t\tNhap Ma Khach Hang: ");
	scanf("%d", &customer.id);
	getchar();

	printf("\n\t\t\t\t\t\tNhap Ten Khach Hang: ");
	fgets(customer.name, sizeof(customer.name), stdin);
	customer.name[strlen(customer.name) - 1] = '\0';

	printf("\n\t\t\t\t\t\tNhap So Dien Thoai: ");
	fgets(customer.phone, sizeof(customer.phone), stdin);
	customer.phone[strlen(customer.phone) - 1] = '\0';

	printf("\n\t\t\t\t\t\tUu Tien (2: Hoa Toc, 1: VIP, 0: THUONG): ");
	int tier_choice;
	scanf("%d", &tier_choice);
	getchar();
	customer.tier = (CustomerTier)tier_choice;

	printf("\n\t\t\t\t\t\tTrang Thai: ");
	fgets(customer.status, sizeof(customer.status), stdin);
	customer.status[strlen(customer.status) - 1] = '\0';

	initOrderQueue(&customer.history);
	customer.total_spent = 0;

	if (insertCustomerTail(customer_list, customer)) {
		printf("\n\t\t\t\t\t\t->THEM KHACH HANG THANH CONG");
	}
	else {
		printf("\n\t\t\t\t\t\t-> DANG KY THAT BAI!\n");
	}
}


// Print single customer
void printSingleCustomer(Customer customer) {
	printf("\t\t\t| CustID%-4d | %-25s | %-15s | %-8d | %-12s | %12lld |\n",
			customer.id,
			customer.name,
			customer.phone,
			customer.tier,
			customer.status,
			customer.total_spent);
}
// Display all customers
void displayCustomerList(CustomerList* customer_list) {
	printf("\t\t\t------------------------------------- CUSTOMER LIST --------------------------------------\n");
	printCustomerHeader();
	CustomerNode* node = customer_list->head;
	while (node != NULL) {
		printSingleCustomer(node->info);
		node = node->next;
	}
	printf("\t\t\t-------------------------------------------------------------------------------------------------\n");
}

// Auto-upgrade customer tier
void autoUpgradeCustomerTier(CustomerList* customer_list) {