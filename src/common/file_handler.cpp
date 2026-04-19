#include "../../include/order_manager.h"
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

void saveOrderQueueToFile(const char* filename, OrderQueue* q) {
	if (q == NULL) return;

	FILE* file_ptr = fopen(filename, "wt");
	if (file_ptr == NULL) {
		printf("\n\t\t\t\t\t\tKhong the luu file %s", filename);
		return;
	}

	int count = 0;
	for (OrderNode* node = q->head; node != NULL; node = node->next) {
		count++;
	}

	fprintf(file_ptr, "%d\n", count);
	for (OrderNode* node = q->head; node != NULL; node = node->next) {
		Order* order = &node->info;
		fprintf(file_ptr, "%d,%s,%s,%d,%lld,%d,%d,%s\n",
			order->id,
			order->customer_name,
			order->product_name,
			order->quantity,
			order->price,
			(int)order->priority,
			(int)order->shipping_method,
			order->status);
	}

	fclose(file_ptr);
}

void appendOrderToFile(const char* filename, Order* order) {
	if (order == NULL) return;

	// If the folder/file doesn't exist, create it and write the header count first.
	FILE* file_ptr = fopen(filename, "rt");
	if (file_ptr == NULL) {
		file_ptr = fopen(filename, "wt");
		if (file_ptr == NULL) {
			printf("\n\t\t\t\t\t\tKhong the tao file %s", filename);
			return;
		}
		fprintf(file_ptr, "0\n");
		fclose(file_ptr);
	}
	else {
		fclose(file_ptr);
	}

	// Reopen in append mode to add the new order record at the end of file.
	file_ptr = fopen(filename, "a+t");
	if (file_ptr == NULL) {
		printf("\n\t\t\t\t\t\tKhong the luu don hang vao file %s", filename);
		return;
	}

	fprintf(file_ptr, "%d,%s,%s,%d,%lld,%d,%d,%s\n",
		order->id,
		order->customer_name,
		order->product_name,
		order->quantity,
		order->price,
		(int)order->priority,
		(int)order->shipping_method,
		order->status);
	fclose(file_ptr);

	// Reopen in read+write mode to update the count at the beginning of the file.
	// fseek to the beginning of the file and overwrite the count — this is the only way
	// to update the header without rewriting the entire file.
	file_ptr = fopen(filename, "r+t");
	if (file_ptr == NULL) return;

	int count = 0;
	fscanf(file_ptr, "%d\n", &count);
	fseek(file_ptr, 0, SEEK_SET);
	fprintf(file_ptr, "%d\n", count + 1);
	fclose(file_ptr);
}

void loadOrderFile(const char* filename, OrderQueue* q) {
	if (q == NULL) return;

	FILE* file_ptr = fopen(filename, "rt");
	if (file_ptr == NULL) {
		// File doesn't exist, create it with a header count of 0.
		file_ptr = fopen(filename, "wt");
		if (file_ptr != NULL) {
			fprintf(file_ptr, "0\n");
			fclose(file_ptr);
		}
		return;
	}

	clearOrderQueueNodes(q);

	int count = 0;
	if (fscanf(file_ptr, "%d\n", &count) != 1 || count <= 0) {
		fclose(file_ptr);
		return;
	}

	char line[512];
	while (fgets(line, sizeof(line), file_ptr) != NULL) {
		Order order;
		int priority = 0;
		int shipping_method = 0;

		// Use sscanf with format [^,] to read each field separated by commas.
		// [^\n] at the end to read the entire status string which may contain spaces.
		int parsed = sscanf(line, "%d,%99[^,],%99[^,],%d,%lld,%d,%d,%99[^\n]",
			&order.id,
			order.customer_name,
			order.product_name,
			&order.quantity,
			&order.price,
			&priority,
			&shipping_method,
			order.status);

		if (parsed != 8) {
			continue; // Skip the line if it has parsing errors or insufficient fields
		}

		order.priority = (PriorityLevel)priority;
		order.shipping_method = (ShippingMethod)shipping_method;
		trimString(order.customer_name);
		trimString(order.product_name);
		trimString(order.status);

		// Only load incomplete orders into the queue.
		// Completed orders don't need further processing, only for historical lookup.
		if (_stricmp(order.status, "Hoan thanh") == 0 || _stricmp(order.status, "Da giao cho DVVC") == 0) {
			continue;
		}
		enqueueOrder(q, order);
	}

	fclose(file_ptr);
}

// Load entire customer file
void loadCustomerFile(const char* filename, CustomerList* list, int* count) {
	FILE* file_ptr = fopen(filename, "rt");
	if (file_ptr == NULL) {
		printf("\n\t\t\t\t\t\tLoi doc file %s.", filename);
		return;
	}

	clearCustomerList(list);
	// Header count can be stale/corrupted; parse all valid records from file instead.
	char header_line[128];
	if (fgets(header_line, sizeof(header_line), file_ptr) == NULL) {
		*count = 0;
		fclose(file_ptr);
		return;
	}

	int loaded_count = 0;
	char line[512];
	while (fgets(line, sizeof(line), file_ptr) != NULL) {
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

// Generate next product ID by scanning for the actual max ID in the file
void generateNextProductId(int* id) {
	FILE* file_ptr = fopen("data/inventory.txt", "rt");
	if (file_ptr == NULL) {
		*id = 101;
		return;
	}

	int maxId = 100;
	char line[256];
	fgets(line, sizeof(line), file_ptr); // skip count header
	while (fgets(line, sizeof(line), file_ptr) != NULL) {
		int pid = 0;
		if (sscanf(line, "%d,", &pid) == 1 && pid > maxId)
			maxId = pid;
	}
	fclose(file_ptr);

	*id = maxId + 1;
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

	// Count valid customer records by parsing the file instead of relying on the header count,
	// which can become inaccurate due to crashes or manual edits. This way the function can
	// This will always ensure the count at the beginning of the file is correct even if records were added/removed outside of normal operations.
	int count = 0;
	char line[512];
	if (fgets(line, sizeof(line), file_ptr) != NULL) { // Bỏ qua dòng header cũ
		while (fgets(line, sizeof(line), file_ptr) != NULL) {
			char id[37], name[100], phone[100], status[100], extra[256];
			int tier;
			long long total;
			extra[0] = '\0';

			int parsed = sscanf(line, "%36[^,],%99[^,],%99[^,],%d,%99[^,],%lld,%255[^\n]",
				id, name, phone, &tier, status, &total, extra);
			if (parsed >= 6) {
				count++;
			}
		}
	}

	// Seek back to beginning and update count
	fseek(file_ptr, 0, SEEK_SET);
	fprintf(file_ptr, "%d\n", count);

	fclose(file_ptr);
}

// Decrease customer count at the beginning of file
void decreaseCustomerCountInFile(const char* filename) {
	FILE* file_ptr = fopen(filename, "r+t");
	if (file_ptr == NULL) {
		return;
	}

	int count = 0;
	char line[512];
	if (fgets(line, sizeof(line), file_ptr) != NULL) {
		while (fgets(line, sizeof(line), file_ptr) != NULL) {
			char id[37], name[100], phone[100], status[100], extra[256];
			int tier;
			long long total;
			extra[0] = '\0';

			int parsed = sscanf(line, "%36[^,],%99[^,],%99[^,],%d,%99[^,],%lld,%255[^\n]",
				id, name, phone, &tier, status, &total, extra);
			if (parsed >= 6) {
				count++;
			}
		}
	}

	fseek(file_ptr, 0, SEEK_SET);
	fprintf(file_ptr, "%d\n", count);

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