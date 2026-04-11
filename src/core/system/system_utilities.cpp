#include "../../../include/order_manager.h"

static void clearOrderQueueNodes(OrderQueue* q) {
	if (q == NULL) return;

	OrderNode* node = q->head;
	while (node != NULL) {
		OrderNode* next = node->next;
		delete node;
		node = next;
	}

	q->head = NULL;
	q->tail = NULL;
}

static void clearCustomerListNodes(CustomerList* list) {
	if (list == NULL) return;

	CustomerNode* node = list->head;
	while (node != NULL) {
		CustomerNode* next = node->next;
		delete node;
		node = next;
	}

	list->head = NULL;
	list->tail = NULL;
}

static int writeFileSection(FILE* backup_file, const char* section_name, const char* source_path) {
	if (backup_file == NULL || section_name == NULL || source_path == NULL) return 0;

	FILE* source = fopen(source_path, "rt");
	if (source == NULL) {
		return 0;
	}

	int line_count = 0;
	char line[1024];
	while (fgets(line, sizeof(line), source) != NULL) {
		line_count++;
	}

	fseek(source, 0, SEEK_SET);
	fprintf(backup_file, "SECTION,%s,%d\n", section_name, line_count);
	while (fgets(line, sizeof(line), source) != NULL) {
		fputs(line, backup_file);
	}

	fclose(source);
	return 1;
}

static int restoreFileSection(FILE* backup_file, const char* expected_section_name, const char* target_path) {
	if (backup_file == NULL || expected_section_name == NULL || target_path == NULL) return 0;

	char header[256];
	if (fgets(header, sizeof(header), backup_file) == NULL) {
		return 0;
	}

	char section_name[64];
	int line_count = 0;
	if (sscanf(header, "SECTION,%63[^,],%d", section_name, &line_count) != 2 || line_count < 0) {
		return 0;
	}

	trimString(section_name);
	if (_stricmp(section_name, expected_section_name) != 0) {
		return 0;
	}

	FILE* target = fopen(target_path, "wt");
	if (target == NULL) {
		return 0;
	}

	char line[1024];
	for (int i = 0; i < line_count; i++) {
		if (fgets(line, sizeof(line), backup_file) == NULL) {
			fclose(target);
			return 0;
		}
		fputs(line, target);
	}

	fclose(target);
	return 1;
}

static int appendOrderTailRaw(OrderQueue* q, const Order* order) {
	if (q == NULL || order == NULL) return 0;

	OrderNode* node = createOrderNode(*order);
	if (node == NULL) return 0;

	if (q->head == NULL) {
		q->head = q->tail = node;
	}
	else {
		q->tail->next = node;
		q->tail = node;
	}
	return 1;
}

static void copyOrderQueue(OrderQueue* dest, const OrderQueue* src) {
	if (dest == NULL || src == NULL) return;
	dest->head = NULL;
	dest->tail = NULL;

	for (OrderNode* node = src->head; node != NULL; node = node->next) {
		appendOrderTailRaw(dest, &node->info);
	}
}

int backupSystemState(const char* filename, Product inventory[], int product_count, OrderQueue* q) {
	if (filename == NULL || inventory == NULL || q == NULL) {
		return 0;
	}

	FILE* file_ptr = fopen(filename, "wt");
	if (file_ptr == NULL) {
		return 0;
	}

	fprintf(file_ptr, "BACKUP_V2\n");
	if (!writeFileSection(file_ptr, "INVENTORY", "data/inventory.txt") ||
		!writeFileSection(file_ptr, "ORDERS", "data/orders.txt") ||
		!writeFileSection(file_ptr, "CUSTOMERS", "data/customers.txt")) {
		fclose(file_ptr);
		return 0;
	}

	fclose(file_ptr);
	return 1;
}

int restoreSystemState(const char* filename, Product inventory[], int* product_count, OrderQueue* q) {
	if (filename == NULL || inventory == NULL || product_count == NULL || q == NULL) {
		return 0;
	}

	FILE* file_ptr = fopen(filename, "rt");
	if (file_ptr == NULL) {
		return 0;
	}

	char header[64] = { 0 };
	if (fgets(header, sizeof(header), file_ptr) == NULL) {
		fclose(file_ptr);
		return 0;
	}
	trimString(header);
	if (strcmp(header, "BACKUP_V2") != 0) {
		fclose(file_ptr);
		return 0;
	}

	int ok_inventory = restoreFileSection(file_ptr, "INVENTORY", "data/inventory.txt");
	int ok_orders = restoreFileSection(file_ptr, "ORDERS", "data/orders.txt");
	int ok_customers = restoreFileSection(file_ptr, "CUSTOMERS", "data/customers.txt");

	fclose(file_ptr);
	if (!ok_inventory || !ok_orders || !ok_customers) {
		return 0;
	}

	// Reload runtime structures from restored files.
	loadInventoryFile("data/inventory.txt", inventory, product_count);
	loadOrderFile("data/orders.txt", q);
	clearCustomerListNodes(&customer_list);
	int restored_customer_count = 0;
	loadCustomerFile("data/customers.txt", &customer_list, &restored_customer_count);
	return 1;
}

void processBackupSystemState(Product inventory[], int product_count, OrderQueue* q) {
	if (isBackgroundOrderProcessing()) {
		showErrorMessage("[!] He thong dang xu ly don hang ngam. Vui long doi hoan tat roi backup.");
		return;
	}

	if (backupSystemState("data/system_backup.txt", inventory, product_count, q)) {
		setColor(2);
		printf("\n\t\t\t\t\t\tSao luu he thong thanh cong -> data/system_backup.txt");
		setColor(7);
	}
	else {
		showErrorMessage("[!] Sao luu that bai!");
	}
}

void processRestoreSystemState(Product inventory[], int* product_count, OrderQueue* q) {
	if (isBackgroundOrderProcessing()) {
		showErrorMessage("[!] He thong dang xu ly don hang ngam. Vui long doi hoan tat roi restore.");
		return;
	}

	if (restoreSystemState("data/system_backup.txt", inventory, product_count, q)) {
		setColor(2);
		printf("\n\t\t\t\t\t\tPhuc hoi he thong thanh cong tu data/system_backup.txt");
		setColor(7);
	}
	else {
		showErrorMessage("[!] Phuc hoi that bai hoac khong co file backup!");
	}
}
