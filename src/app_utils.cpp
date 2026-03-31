#include "../include/order_manager.h"

int search_order_id;
const char* INVENTORY_FILE = "data/inventory.txt";
const char* CUSTOMER_FILE = "data/customers.txt";

static int readMenuChoice() {
	int choice;
	setColor(10);
	printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: ");
	if (scanf("%d", &choice) != 1) {
		clearInputBuffer();
		return -1;
	}
	clearInputBuffer();
	return choice;
}

static void printBackToMainMessage() {
	setColor(10);
	system("cls");
	printf("\n\t\t\t\t\t\tDA QUAY LAI MAN HINH CHINH\n");
}

static void printInvalidChoiceMessage() {
	setColor(4);
	system("cls");
	printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
}

static void processSearchOrderById(OrderQueue* order_queue) {
	setColor(7);
	printf("\n\t\t\t\t\t\tNHAP MA DON: ");
	scanf("%d", &search_order_id);
	getchar();

	OrderNode* foundOrderNode = findOrderById(order_queue, search_order_id);
	if (foundOrderNode != NULL) {
		printf("\n\t\t\t\t\t\tTIM THAY DON HANG!\n");
		printOrderHeader();
		printSingleOrder(foundOrderNode->info);
	}
	else {
		printf("\n\t\t\t\t\t\tKHONG TIM THAY DON DAT HANG!\n");
	}
	pause();
}

static void processSearchCustomerByName(CustomerList* customer_list) {
	char search_name[100];

	setColor(7);
	printf("\n\t\t\t\t\t\tNHAP TEN KHACH HANG: ");
	getchar();
	fgets(search_name, sizeof(search_name), stdin);
	search_name[strcspn(search_name, "\n")] = '\0';

	CustomerNode* foundCustomerNode = findCustomerByName(customer_list, search_name);
	if (foundCustomerNode != NULL) {
		printf("\n\t\t\t\t\t\tTIM THAY KHACH HANG!\n");
		printCustomerHeader();
		printSingleCustomer(foundCustomerNode->info);
	}
	else {
		printf("\n\t\t\t\t\t\tKHONG TIM THAY KHACH HANG!\n");
	}
	pause();
}

// ================= INVENTORY MANAGEMENT =================
void processInventory(Product inventory[MAXSIZE], int* product_count) {
	int choice;
	//*product_count = 0;
	do {
		system("cls");
		menuInventory();
		choice = readMenuChoice();

		switch (choice) {
			case 1:
				setColor(7);
				loadInventoryFile(INVENTORY_FILE, inventory, product_count);
				displayInventory(inventory, *product_count);
				pause();
				break;
			case 2:
				setColor(7);
				createInventory(inventory, product_count);
				pause();
				break;
			case 3:
				setColor(7);
				importStock(inventory, product_count);
				pause();
				break;
			case 4:
				setColor(7);
				updateInventory(inventory, product_count);
				pause();
				break;
			case 5:
				setColor(7);
				deleteProduct(inventory, product_count);
				pause();
				break;
			case 0:
				printBackToMainMessage();
				break;
			default:
				printInvalidChoiceMessage();
				break;
		}
	} while (choice != 0);
}

// ================= ORDER MANAGEMENT =================
void processOrder(OrderQueue* q) {
	int choice;
	Order out_order;
	do {
		system("cls");
		menuOrder();
		choice = readMenuChoice();

		switch (choice) {
		case 1:
			setColor(7);
			insertOrderManual(q);
			pause();
			break;

		case 2:
			setColor(7);
			dequeueOrder(q, &out_order);
			pause();
			break;

		case 3:
			setColor(7);
			displayOrderQueue(q);
			pause();
			break;

		case 0:
			printBackToMainMessage();
			break;
		default:
			printInvalidChoiceMessage();
			break;
		}

	} while (choice != 0);
}

// ============ SEARCH & SORT =================
void processSearchSort(OrderQueue* order_queue, CustomerList* customer_list) {
	int choice;

	do {
		system("cls");
		menuSearchSort();
		choice = readMenuChoice();

		switch (choice) {
		case 1:
			processSearchOrderById(order_queue);
			break;
		case 2:
			processSearchCustomerByName(customer_list);
			break;
		case 3:
			setColor(7);
			sortProductsBySalesDesc(inventory, product_count);
			displayInventory(inventory, product_count);
			pause();
			break;
		case 4:
			setColor(7);
			sortCustomersBySpentDesc(customer_list);
			displayCustomerList(customer_list);
			pause();
			break;
		case 0:
			printBackToMainMessage();
			break;
		default:
			printInvalidChoiceMessage();
			break;
		}

	} while (choice != 0);
}

void processCustomer(CustomerList* customer_list) {
	int choice;
	do {
		system("cls");
		menuCustomer();
		choice = readMenuChoice();
		switch (choice) {
		case 1:
			setColor(7);
			displayCustomerList(customer_list);
			pause();
			break;
		case 2:
			setColor(7);
			registerNewCustomer(customer_list);
			pause();
			break;
		case 3:
			setColor(7);
			adjustCustomerTierManual(customer_list);
			pause();
			break;
		case 4:
			setColor(7);
			deleteCustomer(customer_list);
			pause();
			break;
		case 0:
			printBackToMainMessage();
			break;
		default:
			printInvalidChoiceMessage();
			break;
		}
	} while (choice != 0);
	system("cls");
}

void processStatistics() {
	int choice;
	do {
		system("cls");
		menuStatistics();
		choice = readMenuChoice();
		switch (choice) {
		case 1:break;
		case 2:break;
		case 3:break;
		case 4:break;
		case 0:
			printBackToMainMessage();
			break;
		default:
			printInvalidChoiceMessage();
			break;
		}
	} while (choice != 0);
	system("cls");
}