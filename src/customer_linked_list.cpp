#include "../include/order_manager.h"

CustomerNode* createCustomerNode(Customer customer) {
	CustomerNode* node = new CustomerNode();
	if (node == NULL) return NULL;
	node->info = customer;
	node->next = NULL;
	return node;
}
// Check if customer list is empty
int isCustomerListEmpty(CustomerList* l) {
	return l->head == NULL ? 1 : 0;
}

// Insert a customer at the end of linked list
int insertCustomerTail(CustomerList* l, Customer customer) {
	CustomerNode* node = createCustomerNode(customer);
	if (node == NULL) return 0;
	if (isCustomerListEmpty(l)) {
		l->head = l->tail = node;
	}
	else {
		l->tail->next = node;
		l->tail = node;
	}
	return 1;
}