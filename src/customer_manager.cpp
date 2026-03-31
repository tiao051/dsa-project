#include "../include/order_manager.h"

// Helper function to convert status to display format
const char* getDisplayStatus(const char* status) {
	if (strcmp(status, "HoatDong") == 0) {
		return "Hoat dong";
	}
	else if (strcmp(status, "Khoa") == 0) {
		return "Khoa";
	}
	return status;
}

// Generate next customer ID based on customer count from file
void generateNextCustomerId(char* id) {
	FILE* file_ptr = fopen("data/customers.txt", "rt");
	if (file_ptr == NULL) {
		sprintf(id, "101");
		return;
	}

	int count = 0;
	fscanf(file_ptr, "%d\n", &count);
	fclose(file_ptr);

	int nextId = 100 + count + 1;
	sprintf(id, "%d", nextId);
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

	strcpy(customer.status, "HoatDong");

	initOrderQueue(&customer.history);
	customer.total_spent = 0;

	// Automatically generate next customer id
	generateNextCustomerId(customer.id);

	if (insertCustomerTail(customer_list, customer)) {
		printf("\n\t\t\t\t\t\t->THEM KHACH HANG THANH CONG");
		printf("\n\t\t\t\t\t\tID: %s", customer.id);
		printf("\n\t\t\t\t\t\tLevel: Khach vang lai (Mac dinh)");
		printf("\n\t\t\t\t\t\tTrang thai: %s (Mac dinh)", getDisplayStatus(customer.status));
		appendCustomerToFile("data/customers.txt", &customer);
	}
	else {
		printf("\n\t\t\t\t\t\t-> DANG KY THAT BAI!\n");
	}
}

// Print single customer
void printSingleCustomer(Customer customer) {
	char maKH[20];
	sprintf(maKH, "CustID%s", customer.id);
	printf("\t\t| %-12s | %-20s | %-12s | %-8d | %-12s | %12lld |\n",
			maKH,
			customer.name,
			customer.phone,
			customer.tier,
			getDisplayStatus(customer.status),
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