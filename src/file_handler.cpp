#include "../include/order_manager.h"
#include <ctype.h>

// Trim whitespace from string
void trimString(char* str) {
	int i = 0;
	int j = strlen(str) - 1;
	
	// Trim leading spaces
	while (i <= j && isspace(str[i])) {
		i++;
	}
	
	// Trim trailing spaces
	while (j >= i && isspace(str[j])) {
		j--;
	}
	
	// Shift string to beginning
	int k = 0;
	while (i <= j) {
		str[k++] = str[i++];
	}
	str[k] = '\0';
}

// Load a single product record from file
void loadProductRecord(Product inventory[], FILE* file_ptr, int count) {
	int i = 0;
	while (i < count) {
		fscanf(file_ptr, "%d,%[^,],%d,%lld,%d\n",
			&inventory[i].id,
			inventory[i].name,
			&inventory[i].stock_quantity,
			&inventory[i].price,
			&inventory[i].sold_quantity);
		
		// Trim whitespace from product name
		trimString(inventory[i].name);
		i++;
	}
}

// Load entire inventory file
void loadInventoryFile(const char* filename, Product inventory[], int* count) {
	FILE* file_ptr = fopen(filename, "rt");
	if (file_ptr == NULL) {
		printf("\nLoi doc file %s.", filename);
		return;
	}
	fscanf(file_ptr, "%d\n", count);
	loadProductRecord(inventory, file_ptr, *count);
	fclose(file_ptr);
}

// Load a single customer record from file
void loadCustomerRecord(FILE* file_ptr, Customer* customer_ptr) {
	fscanf(file_ptr, "%36[^,],%[^,],%[^,],%d,%[^,],%lld\n",
		customer_ptr->id,
		customer_ptr->name,
		customer_ptr->phone,
		(int*)&customer_ptr->tier,
		customer_ptr->status,
		&customer_ptr->total_spent);
}

static void clearCustomerList(CustomerList* list) {
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

// Load entire customer file
void loadCustomerFile(const char* filename, CustomerList* list, int* count) {
	FILE* file_ptr = fopen(filename, "rt");
	if (file_ptr == NULL) {
		printf("\n\t\t\t\t\t\tLoi doc file %s.", filename);
		return;
	}

	clearCustomerList(list);
	if (fscanf(file_ptr, "%d\n", count) != 1) {
		*count = 0;
		fclose(file_ptr);
		return;
	}

	int loaded_count = 0;
	char line[512];
	while (loaded_count < *count && fgets(line, sizeof(line), file_ptr) != NULL) {
		Customer customer;
		char extra[256];
		extra[0] = '\0';

		int parsed = sscanf(line, "%36[^,],%99[^,],%99[^,],%d,%99[^,],%lld,%255[^\n]",
			customer.id,
			customer.name,
			customer.phone,
			(int*)&customer.tier,
			customer.status,
			&customer.total_spent,
			extra);

		if (parsed < 6) {
			continue;
		}

		// Trim whitespace from customer data
		trimString(customer.id);
		trimString(customer.name);
		trimString(customer.phone);
		trimString(customer.status);

		initOrderQueue(&customer.history);
		if (insertCustomerTail(list, customer)) {
			loaded_count++;
		}
	}

	*count = loaded_count;

	fclose(file_ptr);
}

// Update product count at the beginning of file
void updateProductCountInFile(const char* filename) {
	FILE* file_ptr = fopen(filename, "r+t");
	if (file_ptr == NULL) {
		return;
	}

	// Read current count
	int count = 0;
	fscanf(file_ptr, "%d\n", &count);

	// Seek back to beginning and update count
	fseek(file_ptr, 0, SEEK_SET);
	fprintf(file_ptr, "%d\n", count + 1);

	fclose(file_ptr);
}

// Decrease product count at the beginning of file
void decreaseProductCountInFile(const char* filename) {
	FILE* file_ptr = fopen(filename, "r+t");
	if (file_ptr == NULL) {
		return;
	}

	// Read current count
	int count = 0;
	fscanf(file_ptr, "%d\n", &count);

	// Seek back to beginning and update count
	fseek(file_ptr, 0, SEEK_SET);
	if (count > 0) {
		fprintf(file_ptr, "%d\n", count - 1);
	}

	fclose(file_ptr);
}

// Save entire inventory to file
void saveInventoryToFile(const char* filename, Product inventory[], int count) {
	FILE* file_ptr = fopen(filename, "wt");
	if (file_ptr == NULL) {
		printf("\n\t\t\t\t\t\tKhong the luu file %s", filename);
		return;
	}

	// Write count at the beginning
	fprintf(file_ptr, "%d\n", count);

	// Write all products
	for (int i = 0; i < count; i++) {
		fprintf(file_ptr, "%d,%s,%d,%lld,%d\n",
			inventory[i].id,
			inventory[i].name,
			inventory[i].stock_quantity,
			inventory[i].price,
			inventory[i].sold_quantity);
	}

	fclose(file_ptr);
}

// Append a new product to the end of the file
void appendProductToFile(const char* filename, Product* product) {
	FILE* file_ptr = fopen(filename, "a+t");
	if (file_ptr == NULL) {
		printf("\n\t\t\t\t\t\tKhong the luu thong tin san pham %s", filename);
		return;
	}

	// Write product record
	fprintf(file_ptr, "%d,%s,%d,%lld,%d\n",
		product->id,
		product->name,
		product->stock_quantity,
		product->price,
		product->sold_quantity);

	fclose(file_ptr);

	// Auto update product count
	updateProductCountInFile(filename);
}

// Generate next product ID based on product count from file
void generateNextProductId(int* id) {
	FILE* file_ptr = fopen("data/inventory.txt", "rt");
	if (file_ptr == NULL) {
		*id = 101;
		return;
	}

	int count = 0;
	fscanf(file_ptr, "%d\n", &count);
	fclose(file_ptr);

	*id = 100 + count + 1;
}



// Append a new customer to the end of the file
void appendCustomerToFile(const char* filename, Customer* customer) {
	FILE* file_ptr = fopen(filename, "a+t");
	if (file_ptr == NULL) {
		printf("\n\t\t\t\t\t\tKhong the luu thong tin khach hang %s", filename);
		return;
	}

	// Write customer record
	fprintf(file_ptr, "%s,%s,%s,%d,%s,%lld\n",
		customer->id,
		customer->name,
		customer->phone,
		customer->tier,
		customer->status,
		customer->total_spent);

	fclose(file_ptr);

	// Auto update customer count
	updateCustomerCountInFile(filename);
}

void updateCustomerCountInFile(const char* filename) {
	FILE* file_ptr = fopen(filename, "r+t");
	if (file_ptr == NULL) {
		return;
	}

	// Read current count
	int count = 0;
	fscanf(file_ptr, "%d\n", &count);

	// Seek back to beginning and update count
	fseek(file_ptr, 0, SEEK_SET);
	fprintf(file_ptr, "%d\n", count + 1);

	fclose(file_ptr);
}

// Decrease customer count at the beginning of file
void decreaseCustomerCountInFile(const char* filename) {
	FILE* file_ptr = fopen(filename, "r+t");
	if (file_ptr == NULL) {
		return;
	}

	// Read current count
	int count = 0;
	fscanf(file_ptr, "%d\n", &count);

	// Seek back to beginning and update count
	fseek(file_ptr, 0, SEEK_SET);
	if (count > 0) {
		fprintf(file_ptr, "%d\n", count - 1);
	}

	fclose(file_ptr);
}

// Save entire customer list to file
void saveCustomerFile(const char* filename, CustomerList* list, int count) {
	FILE* file_ptr = fopen(filename, "w+t");
	if (file_ptr == NULL) {
		printf("\n\t\t\t\t\t\tLoi ghi file %s.", filename);
		return;
	}

	// Write count first
	fprintf(file_ptr, "%d\n", count);

	// Write all customer records
	CustomerNode* node = list->head;
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