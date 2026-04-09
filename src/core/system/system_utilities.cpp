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

	fprintf(file_ptr, "BACKUP_V1\n");
	fprintf(file_ptr, "INVENTORY,%d\n", product_count);
	for (int i = 0; i < product_count; i++) {
		fprintf(file_ptr, "%d,%s,%d,%lld,%d\n",
			inventory[i].id,
			inventory[i].name,
			inventory[i].stock_quantity,
			inventory[i].price,
			inventory[i].sold_quantity);
	}

	int queue_count = 0;
	for (OrderNode* node = q->head; node != NULL; node = node->next) {
		queue_count++;
	}

	fprintf(file_ptr, "ORDERS,%d\n", queue_count);
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
	if (strcmp(header, "BACKUP_V1") != 0) {
		fclose(file_ptr);
		return 0;
	}

	char line[512];
	if (fgets(line, sizeof(line), file_ptr) == NULL) {
		fclose(file_ptr);
		return 0;
	}

	int inv_count = 0;
	if (sscanf(line, "INVENTORY,%d", &inv_count) != 1 || inv_count < 0 || inv_count > MAXSIZE) {
		fclose(file_ptr);
		return 0;
	}

	Product inventory_staging[MAXSIZE];
	for (int i = 0; i < inv_count; i++) {
		if (fgets(line, sizeof(line), file_ptr) == NULL) {
			fclose(file_ptr);
			return 0;
		}

		if (sscanf(line, "%d,%99[^,],%d,%lld,%d",
			&inventory_staging[i].id,
			inventory_staging[i].name,
			&inventory_staging[i].stock_quantity,
			&inventory_staging[i].price,
			&inventory_staging[i].sold_quantity) != 5) {
			fclose(file_ptr);
			return 0;
		}
		trimString(inventory_staging[i].name);
	}

	if (fgets(line, sizeof(line), file_ptr) == NULL) {
		fclose(file_ptr);
		return 0;
	}

	int order_count = 0;
	if (sscanf(line, "ORDERS,%d", &order_count) != 1 || order_count < 0) {
		fclose(file_ptr);
		return 0;
	}

	OrderQueue queue_staging;
	queue_staging.head = NULL;
	queue_staging.tail = NULL;
	for (int i = 0; i < order_count; i++) {
		if (fgets(line, sizeof(line), file_ptr) == NULL) {
			clearOrderQueueNodes(&queue_staging);
			fclose(file_ptr);
			return 0;
		}

		Order order;
		int priority = 0;
		int shipping_method = 0;
		if (sscanf(line, "%d,%99[^,],%99[^,],%d,%lld,%d,%d,%99[^\n]",
			&order.id,
			order.customer_name,
			order.product_name,
			&order.quantity,
			&order.price,
			&priority,
			&shipping_method,
			order.status) != 8) {
			clearOrderQueueNodes(&queue_staging);
			fclose(file_ptr);
			return 0;
		}

		order.priority = (PriorityLevel)priority;
		order.shipping_method = (ShippingMethod)shipping_method;
		trimString(order.customer_name);
		trimString(order.product_name);
		trimString(order.status);

		if (!appendOrderTailRaw(&queue_staging, &order)) {
			clearOrderQueueNodes(&queue_staging);
			fclose(file_ptr);
			return 0;
		}
	}

	fclose(file_ptr);

	// Commit only after full parse success to avoid half-restored runtime state.
	clearOrderQueueNodes(q);
	copyOrderQueue(q, &queue_staging);
	clearOrderQueueNodes(&queue_staging);
	for (int i = 0; i < inv_count; i++) {
		inventory[i] = inventory_staging[i];
	}
	*product_count = inv_count;

	saveInventoryToFile("data/inventory.txt", inventory, *product_count);
	saveOrderQueueToFile("data/orders.txt", q);
	return 1;
}

void processCancelPendingOrder(OrderQueue* q) {
	int cancel_id = 0;
	printf("\n\t\t\t\t\t\tNhap ma don can huy: ");
	if (scanf("%d", &cancel_id) != 1) {
		clearInputBuffer();
		showErrorMessage("[!] Ma don khong hop le!");
		return;
	}

	clearInputBuffer();
	if (cancelPendingOrderById(q, cancel_id)) {
		setColor(2);
		printf("\n\t\t\t\t\t\tDa huy don %d trong Queue thanh cong.", cancel_id);
		setColor(7);
	}
	else {
		showErrorMessage("[!] Khong tim thay don trong Queue de huy.");
	}
}

void processBackupSystemState(Product inventory[], int product_count, OrderQueue* q) {
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
	if (restoreSystemState("data/system_backup.txt", inventory, product_count, q)) {
		setColor(2);
		printf("\n\t\t\t\t\t\tPhuc hoi he thong thanh cong tu data/system_backup.txt");
		setColor(7);
	}
	else {
		showErrorMessage("[!] Phuc hoi that bai hoac khong co file backup!");
	}
}
