#include "../../include/order_manager.h"

/**
 * Find product index in inventory array by ID
 * @return Index if found, -1 otherwise
 */
int findProductById(Product inv[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (inv[i].id == id)
            return i;
    }
    return -1;
}

/**
 * Search for an order node in the queue by ID
 * @return Pointer to OrderNode if found, NULL otherwise
 */
OrderNode* findOrderById(OrderQueue* q, int id) {
    if (q == NULL || q->head == NULL) return NULL;

    OrderNode* current = q->head;
    while (current != NULL) {
        if (current->info.id == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int findOrderByIdInFile(const char* filename, int id, Order* out_order) {
    if (filename == NULL || out_order == NULL) return 0;

    FILE* file_ptr = fopen(filename, "rt");
    if (file_ptr == NULL) return 0;

    int count = 0;
    if (fscanf(file_ptr, "%d\n", &count) != 1 || count <= 0) {
        fclose(file_ptr);
        return 0;
    }

    char line[512];
    while (fgets(line, sizeof(line), file_ptr) != NULL) {
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

        if (order.id == id) {
            order.priority = (PriorityLevel)priority;
            order.shipping_method = (ShippingMethod)shipping_method;
            trimString(order.customer_name);
            trimString(order.product_name);
            trimString(order.status);
            *out_order = order;
            fclose(file_ptr);
            return 1;
        }
    }

    fclose(file_ptr);
    return 0;
}

/**
 * Search for a customer in the linked list by name
 * @return Pointer to CustomerNode if found, NULL otherwise
 */
CustomerNode* findCustomerByName(CustomerList* l, const char* name) {
    if (l == NULL || l->head == NULL || name == NULL) return NULL;

    CustomerNode* current = l->head;
    while (current != NULL) {
        if (_stricmp(current->info.name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// ================= SORT FUNCTIONS =================

/**
 * Swap two product records using pointers
 */
void swapProduct(Product* a, Product* b) {
    Product temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Sort products in descending order based on units sold
 * Algorithm: Interchange Sort
 */
void sortProductsBySalesDesc(Product inv[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (inv[i].sold_quantity < inv[j].sold_quantity) {
                swapProduct(&inv[i], &inv[j]);
            }
        }
    }
}

/**
 * Swap only the info content between two customer nodes
 */
void swapCustomerInfo(Customer* a, Customer* b) {
    Customer temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Sort the customer linked list in descending order by total spent
 * Only the data (info) is swapped, node links (next) remain unchanged
 */
void sortCustomersBySpentDesc(CustomerList* l) {
    if (l == NULL || l->head == NULL) return;

    for (CustomerNode* i = l->head; i->next != NULL; i = i->next) {
        for (CustomerNode* j = i->next; j != NULL; j = j->next) {
            if (i->info.total_spent < j->info.total_spent) {
                swapCustomerInfo(&i->info, &j->info);
            }
        }
    }
}
