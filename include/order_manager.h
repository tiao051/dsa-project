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

// Product / Inventory
typedef struct Product {
    int id;
    NameType name;
    int stock_quantity;
    AmountType price;    
    int sold_quantity;
} Product;

extern Product inventory[MAXSIZE];
extern int product_count;

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
    int id;
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

// Function Prototypes

// Initialization
void initOrderQueue(OrderQueue* q);
void initCustomerList(CustomerList* l);

// Memory Allocation
OrderNode* createOrderNode(Order x);
CustomerNode* createCustomerNode(Customer x);

// File I/O 
void loadInventoryFile(const char* filename, Product inv[], int* count);
void loadCustomerFile(const char* filename, CustomerList* list, int* count);

// Core Logic
void createInventory(Product inventory[MAXSIZE], int* count);
void updateInventory(Product inventory[MAXSIZE], int* count);
int enqueueOrder(OrderQueue* q, Order x);
int dequeueOrder(OrderQueue* q, Order* out_order);
void insertCustomerTail(CustomerList* l, Customer x);
void autoUpgradeCustomerTier(CustomerList* l);

// Search & Sort
int findProductById(Product inv[], int count, int id);
CustomerNode* findCustomerByName(CustomerList* l, const char* name);
void sortProductsBySalesDesc(Product inv[], int count);
void sortCustomersBySpentDesc(CustomerList* l);

// UI & Menu
void showMainMenu();
void displayInventory(Product inv[], int count);
void displayOrderQueue(OrderQueue* q);
void displayCustomerList(CustomerList* l);

// Utilities
void setColor(int color_code);
void pause();

#endif