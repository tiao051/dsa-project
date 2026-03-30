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
	int tier_choice;
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

	while (1) {
		printf("\n\t\t\t\t\t\tTier (2: Express, 1: VIP, 0: Normal): ");
		if (scanf("%d", &tier_choice) == 1 && (tier_choice >= 0 && tier_choice <= 2)) {
			customer.tier = (CustomerTier)tier_choice;
			while (getchar() != '\n'); // Clear buffer
			break;
		}
		else {
			setColor(4); printf("\t\t\t\t\t\t[!] Please enter 0, 1, or 2.\n"); setColor(7);
			while (getchar() != '\n');
		}
	}

	strcpy(customer.status, "Active");
	printf("\n\t\t\t\t\t\tStatus: %s (Default)", customer.status);

	initOrderQueue(&customer.history);
	customer.total_spent = 0;

	// Automatically generate user id
	generateUUID(customer.id);
	printf("\n\t\t\t\t\t\tGenerated ID: %s", customer.id);

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
void autoUpgradeCustomerTier(CustomerList* customer_list) {}