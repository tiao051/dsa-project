#include "../../include/order_manager.h"

// Global data structures
OrderQueue order_queue;
CustomerList customer_list;
Product inventory[MAXSIZE];
int product_count = 0;

int menu_choice;

static void releaseOrderQueueMemory(OrderQueue* q) {
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

static void releaseCustomerListMemory(CustomerList* list) {
	if (list == NULL) return;
	CustomerNode* node = list->head;
	while (node != NULL) {
		CustomerNode* next = node->next;
		delete node;
		node = next;
	}
	list->head = NULL;
	list->tail = NULL;
}

static int readMainMenuChoice() {
	int choice;
	while (1) {
		setColor(10);
		printf("\n\t\t\t\t\t\t->VUI LONG CHON CHUC NANG: ");

		if (scanf("%d", &choice) != 1) {
			clearInputBuffer();
			showErrorMessage("[!] Loi nhap lieu! Vui long nhap so.");
			continue;
		}

		clearInputBuffer();
		if (choice >= 0 && choice <= 5) {
			return choice;
		}

		showErrorMessage("[!] Lua chon khong hop le! Vui long nhap lai.");
	}
}

void startApp() {

	do {
		showMainMenu();
		menu_choice = readMainMenuChoice();
		switch (menu_choice) {
		case 1:
			processInventory(inventory, &product_count);
			break;
		case 2:
			processCustomer(&customer_list);
			break;
		case 3:
			processOrder(&order_queue);
			break;
		case 4:
			processSearchSort(&order_queue, &customer_list);
			break;
		case 5:
			processStatistics();
			break;
		case 0: 
			system("cls");
			printf("\n");
			printf("\n");
			printf("\n");
			printf("\n");
			printf("\n");
			printf("\n");
			printf("\n");
			printf("\n");
			printf("\n");
			setColor(4);
			printf("\t");
			printf("\t");
			printf("\t");
			printf("\t");
			printf("\n\t\t\t\t\t ----------------------------------------------------------------------------\t\t\t\t\t\n");
			printf("\n\t\t\t\t\t|                                                                            |\t\t\t\t\t\n");
			printf("\n\t\t\t\t\t|              CAM ON BAN DA SU DUNG CHUONG TRINH CUA NHOM <3                |\t\t\t\t\t\n");
			printf("\n\t\t\t\t\t|        NHOM 03 CHUNG EM CHUC THAY VA CAC BAN THAT NHIEU SUC KHOE           |\t\t\t\t\t\n");
			printf("\n\t\t\t\t\t|      LUON LUON NGAY CANG TIEN BUOC TRONG VIEC GIANG DAY VA HOC TAP         |\t\t\t\t\t\n");
			printf("\n\t\t\t\t\t|                                                                            |\t\t\t\t\t\n");
			printf("\n\t\t\t\t\t ----------------------------------------------------------------------------\t\t\t\t\t\n");
			printf("\t");
			printf("\t");
			printf("\t");
			printf("\t");
			pause();
			break;
		default:
			setColor(4);
			system("cls");
			printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
			break;
		}
	} while (menu_choice != 0);
	
}

int main() {
	// Initialize data structures
	initOrderQueue(&order_queue);
	initCustomerList(&customer_list);

	// Auto-load data files at startup
	int customer_count = 0;
	loadInventoryFile("data/inventory.txt", inventory, &product_count);
	loadCustomerFile("data/customers.txt", &customer_list, &customer_count);
	loadOrderFile("data/orders.txt", &order_queue);

	startApp();

	// Explicit cleanup keeps leak detectors quiet on normal process shutdown.
	releaseOrderQueueMemory(&order_queue);
	releaseCustomerListMemory(&customer_list);
	resetCompletedOrderHistory();
	return 0;
}