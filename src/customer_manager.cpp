#include "../include/order_manager.h"
#include "time.h"

// Helper function creating UUID for customer (format: xxxxxxxx-xxxx)
static void generateUUID(char* uuid) {
	const char* chars = "0123456789abcdef";
	srand((unsigned int)time(NULL));
	for (int i = 0; i < 13; i++) {
		if (i == 8) uuid[i] = '-';
		else uuid[i] = chars[rand() % 16];
	}
	uuid[13] = '\0';
}

// Helper function to validate phone number
int isNumeric(const char* str) {
	if (strlen(str) < 10) return 0;
	for (int i = 0; str[i] != '\0'; i++) {
		if (!isdigit(str[i])) return 0;
	}
	return 1;
}

// Register new customer
void registerNewCustomer(CustomerList* customer_list) {
	Customer customer;
	char buffer[100];

	int c;
	while ((c = getchar()) != '\n' && c != EOF);

	do {
		printf("\n\t\t\t\t\t\tNhap ten khach hang: ");
		fgets(customer.name, sizeof(customer.name), stdin);
		customer.name[strcspn(customer.name, "\n")] = '\0';

		if (strlen(customer.name) == 0) {
			setColor(4); printf("\t\t\t\t\t\t[!] Ten khach hang khong duoc de trong!\n"); setColor(7);
		}
	} while (strlen(customer.name) == 0);

	do {
		printf("\n\t\t\t\t\t\tNhap vao so dien thoai: ");
		fgets(customer.phone, sizeof(customer.phone), stdin);
		customer.phone[strcspn(customer.phone, "\n")] = '\0';

		if (!isNumeric(customer.phone)) {
			setColor(4);
			printf("\t\t\t\t\t\t[!] So dien thoai khong hop le\n");
			setColor(7);
		}
	} while (!isNumeric(customer.phone));

	// Set default tier to Normal
	customer.tier = TIER_NORMAL;

	strcpy(customer.status, "Active");

	initOrderQueue(&customer.history);
	customer.total_spent = 0;

	// Automatically generate user id
	generateUUID(customer.id);
	printf("\n\t\t\t\t\t\tID: %s", customer.id);

	if (insertCustomerTail(customer_list, customer)) {
		printf("\n\t\t\t\t\t\t->THEM KHACH HANG THANH CONG");
		printf("\n\t\t\t\t\t\tID: %s", customer.id);
		printf("\n\t\t\t\t\t\tLevel: Khach vang lai (Mac dinh)");
		printf("\n\t\t\t\t\t\tTrang thai: %s (Mac dinh)", customer.status);
		saveCustomerListToFile("data/customers.txt", customer_list);
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
	printf("\t\t\t------------------------------------- DANH SACH KHACH HANG --------------------------------------\n");
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
	CustomerNode* node = customer_list->head;
	
	while (node != NULL) {
		Customer* customer = &node->info;
		
		if (customer->total_spent >= 10000000) {
			customer->tier = TIER_EXPRESS;
		}
		else if (customer->total_spent >= 3000000) {
			customer->tier = TIER_VIP;
		}

		else {
			customer->tier = TIER_NORMAL;
		}
		
		node = node->next;
	}
}