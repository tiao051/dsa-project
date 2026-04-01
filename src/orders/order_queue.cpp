#include "../../include/order_manager.h"

static void updateCustomerRevenue(const Order* order) {
	if (order == NULL) return;

	CustomerNode* customer_node = findCustomerByName(&customer_list, order->customer_name);
	if (customer_node == NULL) return;

	long long order_revenue = order->price * (long long)order->quantity;
	customer_node->info.total_spent += order_revenue;
	autoUpgradeCustomerTier(&customer_list);
}

static int compareOrderPriority(const Order* lhs, const Order* rhs) {
	if (lhs->shipping_method != rhs->shipping_method) {
		return lhs->shipping_method == SHIPPING_EXPRESS ? 1 : -1;
	}

	if (lhs->priority != rhs->priority) {
		return lhs->priority > rhs->priority ? 1 : -1;
	}

	return 0;
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

	// Priority queue: Hoa toc > Tieu chuan, then customer tier, stable in same priority.
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
	
	// UPDATE INVENTORY - Decrease stock, increase sold quantity
	for (int i = 0; i < product_count; i++)
	{
		if (_stricmp(inventory[i].name, processed_order.product_name) == 0) {
			inventory[i].stock_quantity -= processed_order.quantity;
			inventory[i].sold_quantity += processed_order.quantity;
			break;
		}
	}
	saveInventoryToFile("data/inventory.txt", inventory, product_count);
	updateCustomerRevenue(&processed_order);
	q->head = q->head->next;
	if (q->head == NULL) q->tail = NULL;
	delete(temp);
	return 1;
}