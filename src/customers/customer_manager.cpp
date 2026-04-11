#include "../../include/order_manager.h"

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

	char line[512];
	int maxId = 100;

	// Skip header line (stored count) and compute next ID from actual records.
	if (fgets(line, sizeof(line), file_ptr) == NULL) {
		fclose(file_ptr);
		sprintf(id, "101");
		return;
	}

	while (fgets(line, sizeof(line), file_ptr) != NULL) {
		char idText[37];
		if (sscanf(line, "%36[^,]", idText) == 1) {
			trimString(idText);
			int validNumber = (idText[0] != '\0');
			for (int i = 0; idText[i] != '\0'; i++) {
				if (!isdigit((unsigned char)idText[i])) {
					validNumber = 0;
					break;
				}
			}

			if (validNumber) {
				int value = atoi(idText);
				if (value > maxId) {
					maxId = value;
				}
			}
		}
	}
	fclose(file_ptr);

	int nextId = maxId + 1;
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

// Helper function to check duplicate by composite key: name + phone
int isCustomerNamePhoneDuplicate(CustomerList* list, const char* name, const char* phone) {
	if (list == NULL || list->head == NULL) return 0;
	
	CustomerNode* node = list->head;
	while (node != NULL) {
		if (strcmp(node->info.name, name) == 0 && strcmp(node->info.phone, phone) == 0) {
			return 1;  // Duplicate found
		}
		node = node->next;
	}
	return 0;  // No duplicate
}

static int countCustomersByName(CustomerList* list, const char* name) {
	if (list == NULL || list->head == NULL) return 0;

	int count = 0;
	CustomerNode* node = list->head;
	while (node != NULL) {
		if (strcmp(node->info.name, name) == 0) {
			count++;
		}
		node = node->next;
	}

	return count;
}

static CustomerNode* findCustomerByNameAndPhone(CustomerList* list, const char* name, const char* phone) {
	if (list == NULL || list->head == NULL) return NULL;

	CustomerNode* node = list->head;
	while (node != NULL) {
		if (strcmp(node->info.name, name) == 0 && strcmp(node->info.phone, phone) == 0) {
			return node;
		}
		node = node->next;
	}

	return NULL;
}

static void printDuplicatePhoneSuggestions(CustomerList* list, const char* name) {
	CustomerNode* node = list->head;
	int index = 1;

	printf("\n\t\t\t\t\t\tCac SDT trung ten:");
	while (node != NULL) {
		if (strcmp(node->info.name, name) == 0) {
			printf("\n\t\t\t\t\t\t%d. %s", index, node->info.phone);
			index++;
		}
		node = node->next;
	}
	printf("\n");
}

static const char* getTierName(CustomerTier tier) {
	if (tier == TIER_NORMAL) return "Thuong";
	if (tier == TIER_VIP) return "VIP";
	return "Express";
}

static long long getMinSpentByTier(CustomerTier tier) {
	if (tier == TIER_VIP) return 3000000;
	if (tier == TIER_EXPRESS) return 10000000;
	return 0;
}

static int countCustomers(CustomerList* customer_list) {
	int count = 0;
	CustomerNode* node = customer_list->head;
	while (node != NULL) {
		count++;
		node = node->next;
	}
	return count;
}

static int readValidCustomerName(CustomerList* customer_list, char* out_name, size_t out_size) {
	do {
		printf("\n\t\t\t\t\t\tNhap ten khach hang: ");
		fgets(out_name, (int)out_size, stdin);
		out_name[strcspn(out_name, "\n")] = '\0';
		trimString(out_name);

		if (strlen(out_name) == 0) {
			showErrorMessage("[!] Ten khach hang khong duoc de trong!");
		}
		else if (!isValidName(out_name)) {
			showErrorMessage("[!] Ten khach hang khong duoc chua so hoac ki tu dac biet!");
		}
	} while (strlen(out_name) == 0 || !isValidName(out_name));

	return 1;
}

static int readValidCustomerPhone(char* out_phone, size_t out_size) {
	do {
		printf("\n\t\t\t\t\t\tNhap vao so dien thoai: ");
		fgets(out_phone, (int)out_size, stdin);
		out_phone[strcspn(out_phone, "\n")] = '\0';
		trimString(out_phone);

		if (!isNumeric(out_phone)) {
			showErrorMessage("[!] So dien thoai khong hop le");
		}
	} while (!isNumeric(out_phone));

	return 1;
}

static void saveCurrentCustomerList(CustomerList* customer_list) {
	int count = countCustomers(customer_list);
	saveCustomerFile("data/customers.txt", customer_list, count);
}

// Register new customer
void registerNewCustomer(CustomerList* customer_list) {
	Customer customer;

	do {
		readValidCustomerName(customer_list, customer.name, sizeof(customer.name));
		readValidCustomerPhone(customer.phone, sizeof(customer.phone));

		if (isCustomerNamePhoneDuplicate(customer_list, customer.name, customer.phone)) {
			showErrorMessage("[!] Khach hang voi ten va so dien thoai nay da ton tai!");
		}
	} while (isCustomerNamePhoneDuplicate(customer_list, customer.name, customer.phone));

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
		printf("\n\t\t\t\t\t\tTen: %s", customer.name);
		printf("\n\t\t\t\t\t\tHang: %s (Mac dinh)", getTierName(customer.tier));
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
	printf("\t\t| %-12s | %-20s | %-12s | %-8s | %-12s | %12lld |\n",
			maKH,
			customer.name,
			customer.phone,
			getTierName(customer.tier),
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
void autoUpgradeCustomerTier(CustomerList* customer_list, int show_log) {
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
	if (show_log) {
		setColor(2);
		printf("\n\t\t\t\t\t\tDA CAP NHAT HANG THANH VIEN!");
		setColor(7);
	}
}

void adjustCustomerTierManual(CustomerList* customer_list) {
	if (customer_list == NULL || customer_list->head == NULL) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Danh sach khach hang trong!");
		setColor(7);
		return;
	}

	int action = 0;
	printf("\n\t\t\t\t\t\tCHON CHUC NANG (1. Nang hang | 2. Ha hang): ");
	scanf("%d", &action);

	if (action != 1 && action != 2) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Lua chon khong hop le!");
		setColor(7);
		return;
	}

	char customer_name[100];
	clearInputBuffer();

	if (action == 1) {
		printf("\n\t\t\t\t\t\tNhap ten khach hang can nang hang: ");
	}
	else {
		printf("\n\t\t\t\t\t\tNhap ten khach hang can ha hang: ");
	}
	fgets(customer_name, sizeof(customer_name), stdin);
	customer_name[strcspn(customer_name, "\n")] = '\0';
	trimString(customer_name);

	if (strlen(customer_name) == 0) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Ten khach hang khong duoc de trong!");
		setColor(7);
		return;
	}

	CustomerNode* customer_node = findCustomerByName(customer_list, customer_name);
	if (customer_node == NULL) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Khong tim thay khach hang ten: %s", customer_name);
		setColor(7);
		return;
	}

	int same_name_count = countCustomersByName(customer_list, customer_name);
	if (same_name_count > 1) {
		char phone[32];
		setColor(3);
		printf("\n\t\t\t\t\t\t[!] Co %d khach hang trung ten. Vui long nhap them so dien thoai.", same_name_count);
		setColor(7);
		printDuplicatePhoneSuggestions(customer_list, customer_name);
		readValidCustomerPhone(phone, sizeof(phone));

		customer_node = findCustomerByNameAndPhone(customer_list, customer_name, phone);
		if (customer_node == NULL) {
			setColor(4);
			printf("\n\t\t\t\t\t\t[!] Khong tim thay khach hang voi ten '%s' va SDT '%s'", customer_name, phone);
			setColor(7);
			return;
		}
	}

	Customer* customer = &customer_node->info;
	if (action == 1) {
		if (customer->tier == TIER_EXPRESS) {
			setColor(3);
			printf("\n\t\t\t\t\t\tKhach hang %s dang o hang cao nhat!", customer->name);
			setColor(7);
			return;
		}

		CustomerTier target_tier = (CustomerTier)(customer->tier + 1);
		long long min_spent = getMinSpentByTier(target_tier);
		char confirm;

		printf("\n\t\t\t\t\t\tDe nang len hang %s can tong tien tieu la %lld", getTierName(target_tier), min_spent);
		char prompt[180];
		sprintf(prompt, "\n\t\t\t\t\t\tVan muon nang %s len hang %s? (Y/N): ", customer->name, getTierName(target_tier));
		confirm = readYesNoChoice(prompt);

		if (confirm == 'Y' || confirm == 'y') {
			customer->tier = target_tier;
			saveCurrentCustomerList(customer_list);
			setColor(2);
			printf("\n\t\t\t\t\t\t-> NANG HANG KHACH HANG THANH CONG!");
			setColor(7);
		}
		else {
			setColor(3);
			printf("\n\t\t\t\t\t\tDA HUY THAO TAC NANG HANG!");
			setColor(7);
		}
	}
	else {
		if (customer->tier == TIER_NORMAL) {
			setColor(3);
			printf("\n\t\t\t\t\t\tKhach hang %s dang o hang thap nhat!", customer->name);
			setColor(7);
			return;
		}

		CustomerTier target_tier = (CustomerTier)(customer->tier - 1);
		char confirm;
		char prompt[220];
		sprintf(prompt, "\n\t\t\t\t\t\tBan co chac chan muon ha hang khach hang %s xuong hang %s? (Y/N): ", customer->name, getTierName(target_tier));
		confirm = readYesNoChoice(prompt);

		if (confirm == 'Y' || confirm == 'y') {
			customer->tier = target_tier;
			saveCurrentCustomerList(customer_list);
			setColor(2);
			printf("\n\t\t\t\t\t\t-> HA HANG KHACH HANG THANH CONG!");
			setColor(7);
		}
		else {
			setColor(3);
			printf("\n\t\t\t\t\t\tDA HUY THAO TAC HA HANG!");
			setColor(7);
		}
	}
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
	char phone[32];

	printf("\n\t\t\t\t\t\tNhap ten khach hang can xoa: ");
	fgets(name, sizeof(name), stdin);
	name[strcspn(name, "\n")] = '\0';
	trimString(name);

	if (strlen(name) == 0) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Ten khach hang khong duoc de trong!");
		setColor(7);
		return;
	}

	int same_name_count = countCustomersByName(customer_list, name);
	if (same_name_count == 0) {
		setColor(4);
		printf("\n\t\t\t\t\t\t[!] Khong tim thay khach hang ten: %s", name);
		setColor(7);
		return;
	}

	int need_phone = same_name_count > 1;
	if (need_phone) {
		setColor(3);
		printf("\n\t\t\t\t\t\t[!] Co %d khach hang trung ten. Vui long nhap them so dien thoai.", same_name_count);
		setColor(7);
		printDuplicatePhoneSuggestions(customer_list, name);
		readValidCustomerPhone(phone, sizeof(phone));
	}

	CustomerNode* node = customer_list->head;
	CustomerNode* prev = NULL;

	// Find customer
	while (node != NULL) {
		int matched = 0;
		if (!need_phone) {
			matched = (strcmp(node->info.name, name) == 0);
		}
		else {
			matched = (strcmp(node->info.name, name) == 0 && strcmp(node->info.phone, phone) == 0);
		}

		if (matched) {
			printf("\n\t\t\t\t\t\tKHACH HANG CAN XOA: %s", node->info.name);
			printf("\n\t\t\t\t\t\tSDT: %s", node->info.phone);
			char confirm;
			confirm = readYesNoChoice("\n\t\t\t\t\t\tBAN CO CHAC CHAN MUON XOA? (Y/N): ");

			if (confirm == 'Y' || confirm == 'y') {
				// Remove from list
				if (prev == NULL) {
					customer_list->head = node->next;
				} else {
					prev->next = node->next;
				}

				delete node;

				// Count remaining customers
				saveCurrentCustomerList(customer_list);

				setColor(2);
				printf("\n\t\t\t\t\t\t-> XOA KHACH HANG THANH CONG!");
				setColor(7);
			} else {
				setColor(3);
				printf("\n\t\t\t\t\t\tHUY BO XOA KHACH HANG!");
				setColor(7);
			}
			return;
		}

		prev = node;
		node = node->next;
	}

	setColor(4);
	if (need_phone) {
		printf("\n\t\t\t\t\t\t[!] Khong tim thay khach hang voi ten '%s' va SDT '%s'", name, phone);
	}
	else {
		printf("\n\t\t\t\t\t\t[!] Khong tim thay khach hang ten: %s", name);
	}
	setColor(7);
}