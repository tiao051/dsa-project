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

// Helper function to validate phone number (exactly 10 digits)
int isNumeric(const char* str) {
	if (strlen(str) != 10) return 0;
	for (int i = 0; str[i] != '\0'; i++) {
		if (!isdigit(str[i])) return 0;
	}
	return 1;
}

// Helper function to validate customer name (only letters and spaces allowed)
int isValidName(const char* name) {
	for (int i = 0; name[i] != '\0'; i++) {
		// Allow letters (a-z, A-Z) and spaces
		if (!isalpha(name[i]) && name[i] != ' ') {
			return 0;
		}
	}
	return 1;
}

// Helper function to check if customer name already exists
int isCustomerNameDuplicate(CustomerList* list, const char* name) {
	if (list == NULL || list->head == NULL) return 0;
	
	CustomerNode* node = list->head;
	while (node != NULL) {
		if (strcmp(node->info.name, name) == 0) {
			return 1;  // Duplicate found
		}
		node = node->next;
	}
	return 0;  // No duplicate
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
		else if (!isValidName(customer.name)) {
			setColor(4); printf("\t\t\t\t\t\t[!] Ten khach hang khong duoc chua so hoac ki tu dac biet!\n"); setColor(7);
		}
		else if (isCustomerNameDuplicate(customer_list, customer.name)) {
			setColor(4); printf("\t\t\t\t\t\t[!] Ten khach hang nay da ton tai!\n"); setColor(7);
		}
	} while (strlen(customer.name) == 0 || !isValidName(customer.name) || isCustomerNameDuplicate(customer_list, customer.name));

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
	int count = 0;
	CustomerNode* node = customer_list->head;
	
	// Count total customers
	while (node != NULL) {
		count++;
		node = node->next;
	}

	// Update tiers
	node = customer_list->head;
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

	// Save updated customer list to file
	saveCustomerFile("data/customers.txt", customer_list, count);
	setColor(2);
	printf("\n\t\t\t\t\t\tDA CAP NHAT HANG THANH VIEN!");
	setColor(7);
}

// Delete customer
void deleteCustomer(CustomerList* customer_list) {
	if (customer_list == NULL || customer_list->head == NULL) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Danh sach khach hang trong!");
		setColor(7);
		return;
	}

	char name[100];
	int c;
	while ((c = getchar()) != '\n' && c != EOF);

	printf("\n\t\t\t\t\t\tNhap ten khach hang can xoa: ");
	fgets(name, sizeof(name), stdin);
	name[strcspn(name, "\n")] = '\0';
	trimString(name);

	CustomerNode* node = customer_list->head;
	CustomerNode* prev = NULL;

	// Find customer
	while (node != NULL) {
		if (strcmp(node->info.name, name) == 0) {
			printf("\n\t\t\t\t\t\tTim thay khach hang: %s", node->info.name);
			printf("\n\t\t\t\t\t\nBan co chac chan muon xoa? (Y/N): ");

			char confirm;
			scanf("%c", &confirm);

			if (confirm == 'Y' || confirm == 'y') {
				// Remove from list
				if (prev == NULL) {
					customer_list->head = node->next;
				} else {
					prev->next = node->next;
				}

				free(node);

				// Count remaining customers
				int count = 0;
				CustomerNode* temp = customer_list->head;
				while (temp != NULL) {
					count++;
					temp = temp->next;
				}

				// Save updated list to file
				saveCustomerFile("data/customers.txt", customer_list, count);

				setColor(2);
				printf("\n\t\t\t\t\t\t-> XOA KHACH HANG THANH CONG!");
				setColor(7);
			} else {
				setColor(3);
				printf("\n\t\t\t\t\t\tHuy phep xoa.");
				setColor(7);
			}
			return;
		}

		prev = node;
		node = node->next;
	}

	setColor(4);
	printf("\n\t\t\t\t\t\t[!] Khong tim thay khach hang ten: %s", name);
	setColor(7);
}