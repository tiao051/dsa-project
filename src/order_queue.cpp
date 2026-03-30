#include "../include/order_manager.h"

// Define missing defines for priority levels
#define PRIORITY_EXPRESS PRIORITY_EXPRESS
#define PRIORITY_VIP PRIORITY_VIP

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

	if (isOrderQueueEmpty(q) == 1) {
		q->head = q->tail = node;
		return 1;
	}
	else {
		// EXPRESS/HOATOC priority
		if (order.priority == PRIORITY_EXPRESS) {
			OrderNode* cur = q->head;
			OrderNode* prev = NULL;
			while (cur != NULL && cur->info.priority == order.priority)
			{
				prev = cur;
				cur = cur->next;
			}
			if (prev == NULL) {
				node->next = q->head;
				q->head = node;
			}
			else {
				prev->next = node;
				node->next = cur;
			}
			return 1;
		}
		// VIP priority
		else if (order.priority == PRIORITY_VIP) {
			OrderNode* cur = q->head;
			OrderNode* prev = NULL;
			while (cur != NULL && cur->info.priority < order.priority) {
				prev = cur;
				cur = cur->next;
			}
			prev->next = node;
			node->next = cur;
			if (prev == NULL) q->tail = node;
			return 1;
		}
		// NORMAL priority
		else {
			q->tail->next = node;
			q->tail = node;
			return 1;
		}
	}

}

// Dequeue - Process Order
int dequeueOrder(OrderQueue* q, Order* out_order) {
	if (isOrderQueueEmpty(q) == 1) {
		printf("\n\t\t\t\t\t\tDANH SACH DON HANG RONG !!!");
		return 0;
	}

	OrderNode* temp = q->head;
	if (out_order != NULL) {
		*out_order = temp->info;
	}
	printf("\n\t\t\t\t\t\tXU LY DON HANG: %d - %s", temp->info.id, temp->info.customer_name);
	
	// UPDATE INVENTORY - Decrease stock, increase sold quantity
	for (int i = 0; i < product_count; i++)
	{
		if (strcmp(inventory[i].name, temp->info.product_name) == 0) {
			inventory[i].stock_quantity -= temp->info.quantity;
			inventory[i].sold_quantity += temp->info.quantity;
		}
	}
	q->head = q->head->next;
	if (q->head == NULL) q->tail = NULL;
	delete(temp);
	return 1;
}