#define _CRT_SECURE_NO_WARNINGS

#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#define MAXSIZE 100
#define PACKING_STATION_COUNT 3

// Enums 

typedef enum {
    PRIORITY_NORMAL  = 0,
    PRIORITY_VIP     = 1,
    PRIORITY_EXPRESS = 2
} PriorityLevel;

typedef enum {
    SHIPPING_STANDARD = 0,
    SHIPPING_EXPRESS  = 1
} ShippingMethod;

typedef enum {
    TIER_NORMAL  = 0,
    TIER_VIP     = 1,
    TIER_EXPRESS = 2
} CustomerTier;

// Custom type definitions
typedef char NameType[100];
typedef long long AmountType;
typedef char KeyType[100];

// Product / Inventory
typedef struct Product {
    int id;
    NameType name;
    int stock_quantity;
    AmountType price;    
    int sold_quantity;
} Product;

// Order / Queue
typedef struct Order {
    int id;
    NameType customer_name;
    NameType product_name;
    int quantity;
    AmountType price;
    PriorityLevel priority;
    ShippingMethod shipping_method;
    NameType status;
} Order;

typedef struct OrderNode {
    Order info;
    struct OrderNode* next;
} OrderNode;

typedef struct OrderQueue {
    OrderNode* head;
    OrderNode* tail;
} OrderQueue;

// Customer / Linked list
typedef struct Customer {
    char id[37];
    NameType name;
    NameType phone;
    CustomerTier tier;   
    NameType status;
    OrderQueue history;
    AmountType total_spent;
} Customer;

typedef struct CustomerNode {
    Customer info;
    struct CustomerNode* next;
} CustomerNode;

typedef struct CustomerList {
    CustomerNode* head;
    CustomerNode* tail;
} CustomerList;

extern Product inventory[MAXSIZE];
extern int product_count;
extern OrderQueue order_queue;
extern CustomerList customer_list;

// Function Prototypes
// Initialization
void initOrderQueue(OrderQueue* q);
void initCustomerList(CustomerList* l);

// Check Status
int isOrderQueueEmpty(OrderQueue* q);
int isCustomerListEmpty(CustomerList* l);

// Memory Allocation
OrderNode* createOrderNode(Order x);
CustomerNode* createCustomerNode(Customer x);

// File I/O
void loadProductRecord(Product inv[], FILE* file_ptr, int count);
void loadInventoryFile(const char* filename, Product inv[], int* count);
void loadCustomerRecord(FILE* file_ptr, Customer* customer_ptr);
void loadCustomerFile(const char* filename, CustomerList* list, int* count);
void loadOrderFile(const char* filename, OrderQueue* q);
void appendCustomerToFile(const char* filename, Customer* customer);
void appendProductToFile(const char* filename, Product* product);
void appendOrderToFile(const char* filename, Order* order);
void saveInventoryToFile(const char* filename, Product inventory[], int count);
void saveOrderQueueToFile(const char* filename, OrderQueue* q);
void updateCustomerCountInFile(const char* filename);
void decreaseCustomerCountInFile(const char* filename);
void updateProductCountInFile(const char* filename);
void decreaseProductCountInFile(const char* filename);
void generateNextCustomerId(char* id);
void generateNextProductId(int* id);
void trimString(char* str);

// Core Operations - Order Queue
int enqueueOrder(OrderQueue* q, Order x);
int dequeueOrder(OrderQueue* q, Order* out_order);
int getOrderPriorityRank(const Order* order);
void processCompletedOrder(const Order* order);
void resetCompletedOrderHistory();
void saveOrderQueueWithHistory(const char* filename, OrderQueue* pending_queue);
void insertOrderManual(OrderQueue* q); 
void processParallelPackaging(OrderQueue* q);

// Core Operations - Customer List
int insertCustomerTail(CustomerList* l, Customer x);
void registerNewCustomer(CustomerList* l);
void autoUpgradeCustomerTier(CustomerList* l, int show_log);
void adjustCustomerTierManual(CustomerList* l);

// Validation helpers
int isNumeric(const char* str);
int isValidName(const char* name);
int isCustomerNameDuplicate(CustomerList* list, const char* name);

// File I/O for customers
void saveCustomerFile(const char* filename, CustomerList* list, int count);

// Customer operations
void deleteCustomer(CustomerList* customer_list);

// Search & Sort
OrderNode* findOrderById(OrderQueue* q, int id);
CustomerNode* findCustomerByName(CustomerList* l, const char* name);
int findProductById(Product inv[], int count, int id);

void swapProduct(Product* a, Product* b);
void sortProductsBySalesDesc(Product inv[], int count);
void swapCustomerInfo(Customer* a, Customer* b);
void sortCustomersBySpentDesc(CustomerList* l);

// Inventory Management
void createInventory(Product inv[], int* count);
void importStock(Product inv[], int* count);
void updateInventory(Product inv[], int* count);
void deleteProduct(Product inv[], int* count);
void displayInventory(Product inv[], int count);

// Display
void printSingleOrder(Order x);
void displayOrderQueue(OrderQueue* q);
void printSingleCustomer(Customer x);
void displayCustomerList(CustomerList* l);

// UI Headers
void printProductHeader();
void printOrderHeader();
void printCustomerHeader();

// Menu System
void showMainMenu();
void menuCustomer();
void menuOrder();
void menuInventory();
void menuSearchSort();
void menuStatistics();

// Utilities
void setColor(int color_code);
void pause();
void clearInputBuffer();
void showErrorMessage(const char* message);
char readYesNoChoice(const char* prompt);

// Secondary Process Functions
void processStatistics();
void processSearchSort(OrderQueue* q, CustomerList* l);
void startApp();
void processInventory(Product inv[], int* count);
void processOrder(OrderQueue* q);
void processSearchSort(OrderQueue* q, CustomerList* l);
void processCustomer(CustomerList* l);
void processStatistics();

#endif