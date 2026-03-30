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

// Enums 

typedef enum {
    PRIORITY_NORMAL  = 0,
    PRIORITY_VIP     = 1,
    PRIORITY_EXPRESS = 2
} PriorityLevel;

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
void loadProductRecord(Product inv[], FILE* file_ptr);
void loadInventoryFile(const char* filename, Product inv[], int* count);
void loadCustomerRecord(FILE* file_ptr, Customer* customer_ptr);
void loadCustomerFile(const char* filename, CustomerList* list, int* count);
void saveCustomerListToFile(const char* filename, CustomerList* list);

// Core Operations - Order Queue
int enqueueOrder(OrderQueue* q, Order x);
int dequeueOrder(OrderQueue* q, Order* out_order);
void insertOrderManual(OrderQueue* q); 

// Core Operations - Customer List
int insertCustomerTail(CustomerList* l, Customer x);
void registerNewCustomer(CustomerList* l);
void autoUpgradeCustomerTier(CustomerList* l);

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