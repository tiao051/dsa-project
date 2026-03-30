#include "../include/order_manager.h"

// Load a single product record from file
void loadProductRecord(Product inventory[], FILE* file_ptr) {
	int i = 0;
	while (i < product_count) {
		fscanf(file_ptr, "%d,%[^,],%d,%lld,%d\n",
			&inventory[i].id,
			inventory[i].name,
			&inventory[i].stock_quantity,
			&inventory[i].price,
			&inventory[i].sold_quantity);
		i++;
	}
}
// Load entire inventory file
void loadInventoryFile(const char* filename, Product inventory[], int* count) {
	FILE* file_ptr = fopen(filename, "rt");
	if (file_ptr == NULL) {
		printf("\nERROR: Cannot read file %s. Please check file path.", filename);
		return;
	}
	fscanf(file_ptr, "%d\n", count);
	loadProductRecord(inventory, file_ptr);
	fclose(file_ptr);
}
// Load a single customer record from file
void loadCustomerRecord(FILE* file_ptr, Customer* customer_ptr) {
	fscanf(file_ptr, "%d,%[^,],%[^,],%d,%[^,],%lld\n",
		&customer_ptr->id,
		customer_ptr->name,
		customer_ptr->phone,
		(int*)&customer_ptr->tier,
		customer_ptr->status,
		&customer_ptr->total_spent);
}
// Load entire customer file
void loadCustomerFile(const char* filename, CustomerList* list, int* count) {
	FILE* file_ptr = fopen(filename, "rt");
	if (file_ptr == NULL) {
		printf("\n\t\t\t\t\t\tERROR: Cannot read file %s. Please check file path.", filename);
		return;
	}

	fscanf(file_ptr, "%d\n", count);
	int i = 0;
	while (i < *count) {
		Customer customer;
		loadCustomerRecord(file_ptr, &customer);
		initOrderQueue(&customer.history);
		customer.total_spent = 0;
		insertCustomerTail(list, customer);
		i++;
	}

	fclose(file_ptr);
}

// Save entire customer list to file
void saveCustomerListToFile(const char* filename, CustomerList* list) {
	FILE* file_ptr = fopen(filename, "wt");
	if (file_ptr == NULL) {
		printf("\n\t\t\t\t\t\tKhong the luu thong tin khach hang %s", filename);
		return;
	}

	// Count total customers
	int count = 0;
	CustomerNode* node = list->head;
	while (node != NULL) {
		count++;
		node = node->next;
	}

	// Write total count
	fprintf(file_ptr, "%d\n", count);

	// Write all customer records
	node = list->head;
	while (node != NULL) {
		fprintf(file_ptr, "%s,%s,%s,%d,%s,%lld\n",
			node->info.id,
			node->info.name,
			node->info.phone,
			node->info.tier,
			node->info.status,
			node->info.total_spent);
		node = node->next;
	}

	fclose(file_ptr);
}
