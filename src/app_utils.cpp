#include "../include/order_manager.h"

int search_order_id;
const char* INVENTORY_FILE = "data/inventory.txt";
const char* CUSTOMER_FILE = "data/customers.txt";

// ================= INVENTORY MANAGEMENT =================
void processInventory(Product inventory[MAXSIZE], int* product_count) {
	int choice;
	//*product_count = 0;
	do {
		system("cls");
		menuInventory();
		setColor(10);
		printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: "); 
		scanf("%d", &choice);

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
				setColor(10);
				system("cls");
				printf("\n\t\t\t\t\t\tDA QUAY LAI MAN HINH CHINH\n");
				break;
			default:
				setColor(4);
				system("cls");
				printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
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
		setColor(10); printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: "); 
		scanf("%d", &choice);

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
			setColor(10);
			system("cls");
			printf("\n\t\t\t\t\t\tTRO VE MAN HINH CHINH\n");
			break;
		default:
			setColor(4);
			system("cls");
			printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
			break;
		}

	} while (choice != 0);
}

// ============ SEARCH & SORT =================
void processSearchSort(OrderQueue* order_queue, CustomerList* customer_list) {
	int choice;
	char search_name[100];
	OrderNode* foundOrderNode = NULL;
	CustomerNode* foundCustomerNode = NULL;

	do {
		system("cls");
		menuSearchSort();
		setColor(10); 
		printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: "); 
		scanf("%d", &choice);

		switch (choice) {
		case 1: 
			setColor(7);
			printf("\n\t\t\t\t\t\tNHAP MA DON: ");
			scanf("%d", &search_order_id);
			getchar();

			foundOrderNode = findOrderById(order_queue, search_order_id);

			if (foundOrderNode != NULL) {
				printf("\n\t\t\t\t\t\tTIM THAY DON HANG!\n");
				printOrderHeader();
				printSingleOrder(foundOrderNode->info);
			}
			else {
				printf("\n\t\t\t\t\t\tKHONG TIM THAY DON DAT HANG!\n");
			}
			pause();
			break;
		case 2:
			setColor(7);
			printf("\n\t\t\t\t\t\tNHAP TEN KHACH HANG: ");
			getchar();
			fgets(search_name, sizeof(search_name), stdin);
			search_name[strcspn(search_name, "\n")] = '\0';

			foundCustomerNode = findCustomerByName(customer_list, search_name);

			if (foundCustomerNode != NULL) {
				printf("\n\t\t\t\t\t\tTIM THAY KHACH HANG!\n");
				printCustomerHeader();
				printSingleCustomer(foundCustomerNode->info);
			}
			else {
				printf("\n\t\t\t\t\t\tKHONG TIM THAY KHACH HANG!\n");
			}
			pause();
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
			setColor(10);
			system("cls");
			printf("\n\t\t\t\t\t\tDA QUAY LAI MAN HINH CHINH\n");
			break;
		default:
			setColor(4);
			system("cls");
			printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
			break;
		}

	} while (choice != 0);
}

void processCustomer(CustomerList* customer_list) {
	int choice, count;
	do {
		system("cls");
		menuCustomer();
		setColor(10); 
		printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: "); 
		scanf("%d", &choice);
		switch (choice) {
		case 1:
			setColor(7);
			loadCustomerFile(CUSTOMER_FILE, customer_list, &count);
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
			setColor(10);
			system("cls");
			printf("\n\t\t\t\t\t\tDA QUAY LAI MAN HINH CHINH\n");
			break;
		default:
			setColor(4);
			system("cls");
			printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
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
		setColor(10); 
		printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: "); 
		scanf("%d", &choice);
		switch (choice) {
		case 1:break;
		case 2:break;
		case 3:break;
		case 4:break;
		case 0:
			setColor(10);
			system("cls");
			printf("\n\t\t\t\t\t\tDA QUAY LAI MAN HINH CHINH\n");
			break;
		default:
			setColor(4);
			system("cls");
			printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
			break;
		}
	} while (choice != 0);
	system("cls");
}