#include "../../include/order_manager.h"
#include <vector>

static OrderQueue completed_history_queue;
static int completed_history_initialized = 0;

static int isDoneOrderStatus(const char* status) {
	if (status == NULL) return 0;
	return (_stricmp(status, "Hoan thanh") == 0 || _stricmp(status, "Da giao cho DVVC") == 0) ? 1 : 0;
}

static int queueContainsOrderId(const OrderQueue* q, int id) {
	if (q == NULL) return 0;
	for (OrderNode* node = q->head; node != NULL; node = node->next) {
		if (node->info.id == id) {
			return 1;
		}
	}
	return 0;
}

static int vectorContainsOrderId(const std::vector<Order>& orders, int id) {
	for (size_t i = 0; i < orders.size(); i++) {
		if (orders[i].id == id) {
			return 1;
		}
	}
	return 0;
}

static void ensureCompletedHistoryInitialized() {
	if (completed_history_initialized == 0) {
		initOrderQueue(&completed_history_queue);
		completed_history_initialized = 1;
	}
}

static void clearOrderNodes(OrderQueue* q) {
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

static void appendOrderTail(OrderQueue* q, const Order* order) {
	if (q == NULL || order == NULL) return;

	OrderNode* node = createOrderNode(*order);
	if (node == NULL) return;

	if (q->head == NULL) {
		q->head = q->tail = node;
		return;
	}

	q->tail->next = node;
	q->tail = node;
}

void resetCompletedOrderHistory() {
	ensureCompletedHistoryInitialized();
	clearOrderNodes(&completed_history_queue);
}

static void updateCustomerRevenue(const Order* order) {
	if (order == NULL) return;

	CustomerNode* customer_node = findCustomerByName(&customer_list, order->customer_name);
	if (customer_node == NULL) return;

	long long order_revenue = order->price * (long long)order->quantity;
	customer_node->info.total_spent += order_revenue;
	autoUpgradeCustomerTier(&customer_list, 0);
}

int getOrderPriorityRank(const Order* order) {
	if (order == NULL) return 3;

	// Why this helper exists:
	// We compare priorities in many places (queue sorting, preemption, logging).
	// A single normalization point prevents duplicated if/else chains and keeps
	// all modules aligned on the same business rule: 1 (highest) -> 3 (lowest).
	int raw = (int)order->priority;
	if (raw < 1 || raw > 3) return PRIORITY_NORMAL;
	return raw;
}

static int compareOrderPriority(const Order* lhs, const Order* rhs) {
	// Returning positive means lhs should be placed ahead of rhs in the queue.
	int lhs_rank = getOrderPriorityRank(lhs);
	int rhs_rank = getOrderPriorityRank(rhs);
	if (lhs_rank != rhs_rank) {
		return lhs_rank < rhs_rank ? 1 : -1;
	}

	return 0;
}

static void appendRevenueLog(const Order* order) {
	if (order == NULL) return;

	FILE* file_ptr = fopen("data/order_revenue_log.txt", "a+t");
	if (file_ptr == NULL) {
		return;
	}

	time_t now = time(NULL);
	struct tm local_tm;
	localtime_s(&local_tm, &now);

	char day[11];
	strftime(day, sizeof(day), "%Y-%m-%d", &local_tm);
	long long revenue = order->price * (long long)order->quantity;

	fprintf(file_ptr, "%s,%d,%lld\n", day, order->id, revenue);
	fclose(file_ptr);
}

void processCompletedOrder(const Order* order) {
	if (order == NULL) return;
	ensureCompletedHistoryInitialized();

	for (int i = 0; i < product_count; i++) {
		if (_stricmp(inventory[i].name, order->product_name) == 0) {
			int deducted_quantity = order->quantity;
			if (deducted_quantity > inventory[i].stock_quantity) {
				printf("\n\t\t\t\t\t\t[CANH BAO] Don %d vuot ton kho %s (ton=%d, don=%d).",
					order->id,
					inventory[i].name,
					inventory[i].stock_quantity,
					order->quantity);
				deducted_quantity = inventory[i].stock_quantity;
			}

			inventory[i].stock_quantity -= deducted_quantity;
			inventory[i].sold_quantity += deducted_quantity;
			break;
		}
	}

	saveInventoryToFile("data/inventory.txt", inventory, product_count);
	updateCustomerRevenue(order);
	appendRevenueLog(order);

	Order completed_order = *order;
	strcpy(completed_order.status, "Hoan thanh");
	appendOrderTail(&completed_history_queue, &completed_order);
}

void saveOrderQueueWithHistory(const char* filename, OrderQueue* pending_queue) {
	if (filename == NULL || pending_queue == NULL) return;
	ensureCompletedHistoryInitialized();

	std::vector<Order> persisted_completed_orders;
	FILE* read_ptr = fopen(filename, "rt");
	if (read_ptr != NULL) {
		char line[512];
		if (fgets(line, sizeof(line), read_ptr) != NULL) {
			while (fgets(line, sizeof(line), read_ptr) != NULL) {
				Order order;
				int priority = 0;
				int shipping_method = 0;

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
					continue;
				}

				order.priority = (PriorityLevel)priority;
				order.shipping_method = (ShippingMethod)shipping_method;
				trimString(order.customer_name);
				trimString(order.product_name);
				trimString(order.status);

				if (!isDoneOrderStatus(order.status)) {
					continue;
				}

				if (queueContainsOrderId(pending_queue, order.id)) {
					continue;
				}

				if (queueContainsOrderId(&completed_history_queue, order.id)) {
					continue;
				}

				if (vectorContainsOrderId(persisted_completed_orders, order.id)) {
					continue;
				}

				persisted_completed_orders.push_back(order);
			}
		}
		fclose(read_ptr);
	}

	FILE* file_ptr = fopen(filename, "wt");
	if (file_ptr == NULL) {
		printf("\n\t\t\t\t\t\tKhong the luu file %s", filename);
		return;
	}

	size_t total_count = 0;
	for (OrderNode* node = pending_queue->head; node != NULL; node = node->next) total_count++;
	for (OrderNode* node = completed_history_queue.head; node != NULL; node = node->next) total_count++;
	total_count += persisted_completed_orders.size();

	fprintf(file_ptr, "%zu\n", total_count);

	for (OrderNode* node = pending_queue->head; node != NULL; node = node->next) {
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

	for (OrderNode* node = completed_history_queue.head; node != NULL; node = node->next) {
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

	for (size_t i = 0; i < persisted_completed_orders.size(); i++) {
		Order* order = &persisted_completed_orders[i];
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

OrderNode* createOrderNode(Order x) {
	OrderNode* node = new OrderNode();
	if (node == NULL) return NULL;
	node->info = x;
	node->next = NULL;
	return node;
}

int isOrderQueueEmpty(OrderQueue* q) {
	return q->head == NULL ? 1 : 0;
}

void initOrderQueue(OrderQueue* q) {
	q->head = q->tail = NULL;
}

// Core Operations - Order Queue
int enqueueOrder(OrderQueue* q, Order order) {
	OrderNode* node = createOrderNode(order);
	if (node == NULL) return 0;

	if (isOrderQueueEmpty(q) == 1) {
		q->head = q->tail = node;
		return 1;
	}

	// Why linked-list insertion is done here instead of sorting later:
	// keeping queue always sorted makes dequeue O(1) for highest-priority order,
	// which is critical because dispatch/preemption reads queue head every tick.
	// Ordering rule: lower rank means higher priority (1 > 2 > 3), stable on ties.
	if (compareOrderPriority(&order, &q->head->info) > 0) {
		node->next = q->head;
		q->head = node;
		return 1;
	}

	OrderNode* prev = q->head;
	OrderNode* cur = q->head->next;
	while (cur != NULL && compareOrderPriority(&cur->info, &order) >= 0) {
		prev = cur;
		cur = cur->next;
	}

	prev->next = node;
	node->next = cur;
	if (cur == NULL) {
		q->tail = node;
	}
	return 1;

}

// Dequeue - Process Order
int dequeueOrder(OrderQueue* q, Order* out_order) {
	if (isOrderQueueEmpty(q) == 1) {
		printf("\n\t\t\t\t\t\tDANH SACH DON HANG RONG !!!");
		return 0;
	}

	OrderNode* temp = q->head;
	Order processed_order = temp->info;
	strcpy(processed_order.status, "Da giao cho DVVC");
	long long order_revenue = processed_order.price * (long long)processed_order.quantity;

	if (out_order != NULL) {
		*out_order = processed_order;
	}
	printf("\n\t\t\t\t\t\tXU LY DON HANG: %d - %s", processed_order.id, processed_order.customer_name);
	printf("\n\t\t\t\t\t\tTrang thai: %s", processed_order.status);
	printf("\n\t\t\t\t\t\tDoanh thu ghi nhan: %lld", order_revenue);

	processCompletedOrder(&processed_order);
	q->head = q->head->next;
	if (q->head == NULL) q->tail = NULL;
	delete(temp);
	saveOrderQueueToFile("data/orders.txt", q);
	return 1;
}

