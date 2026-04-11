#include "../../include/order_manager.h"

int search_order_id;
const char* INVENTORY_FILE = "data/inventory.txt";
const char* CUSTOMER_FILE = "data/customers.txt";

static int readMenuChoiceInRange(int min_choice, int max_choice) {
	int choice;
	while (1) {
		setColor(10);
		printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: ");

		if (scanf("%d", &choice) != 1) {
			clearInputBuffer();
			showErrorMessage("[!] Loi nhap lieu! Vui long nhap so.");
			continue;
		}

		clearInputBuffer();
		if (choice >= min_choice && choice <= max_choice) {
			return choice;
		}

		showErrorMessage("[!] Lua chon khong hop le! Vui long nhap lai.");
	}
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
	clearInputBuffer();

	Order found_order;
	if (findOrderByIdInFile("data/orders.txt", search_order_id, &found_order)) {
		printf("\n\t\t\t\t\t\tTIM THAY DON HANG!\n");
		printOrderHeader();
		printSingleOrder(found_order);
	}
	else {
		printf("\n\t\t\t\t\t\tKHONG TIM THAY DON DAT HANG!\n");
	}
	pause();
}

static void processSearchCustomerByName(CustomerList* customer_list) {
	(void)customer_list;

	char search_name[100];
	int matched_count = 0;

	setColor(7);
	printf("\n\t\t\t\t\t\tNHAP TEN KHACH HANG: ");
	fgets(search_name, sizeof(search_name), stdin);
	search_name[strcspn(search_name, "\n")] = '\0';
	trimString(search_name);

	if (strlen(search_name) == 0) {
		showErrorMessage("[!] Ten khach hang khong duoc de trong!");
		pause();
		return;
	}

	FILE* file_ptr = fopen("data/orders.txt", "rt");
	if (file_ptr == NULL) {
		showErrorMessage("[!] Khong tim thay file du lieu don hang!");
		pause();
		return;
	}

	int total = 0;
	fscanf(file_ptr, "%d\n", &total);

	char line[512];
	printf("\n\t\t\t\t\t\tKET QUA TIM KIEM DON HANG THEO TEN KHACH: %s\n", search_name);
	printOrderHeader();

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

		order.priority = (PriorityLevel)priority;
		order.shipping_method = (ShippingMethod)shipping_method;
		trimString(order.customer_name);
		trimString(order.product_name);
		trimString(order.status);

		if (_stricmp(order.customer_name, search_name) == 0) {
			printSingleOrder(order);
			matched_count++;
		}
	}

	fclose(file_ptr);

	if (matched_count == 0) {
		printf("\n\t\t\t\t\t\tKHONG TIM THAY DON HANG CUA KHACH '%s'!\n", search_name);
	}
	else {
		printf("\n\t\t\t\t\t\tTong don tim thay: %d\n", matched_count);
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
		choice = readMenuChoiceInRange(0, 5);

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
		}
	} while (choice != 0);
}

// ================= ORDER MANAGEMENT =================
void processOrder(OrderQueue* q) {
	int choice;
	do {
		system("cls");
		menuOrder();
		choice = readMenuChoiceInRange(0, 6);

		switch (choice) {
		case 1:
			setColor(7);
			if (insertOrderManual(q)) {
				ensureBackgroundOrderProcessing(q);
			}
			pause();
			break;

		case 2:
			setColor(7);
			displayOrderProgressFromFile("data/orders.txt");
			pause();
			break;

		case 3:
			setColor(7);
			if (isBackgroundOrderProcessing()) {
				showErrorMessage("[!] He thong dang xu ly don hang ngam. Vui long doi hoan tat truoc khi chay kich ban.");
			}
			else {
				runPackagingScenarioFromFile(q, "data/order_scenario.txt");
			}
			pause();
			break;

		case 4:
		{
			setColor(7);
			int cancel_id = 0;
			printf("\n\t\t\t\t\t\tNhap ma don can huy: ");
			if (scanf("%d", &cancel_id) != 1) {
				clearInputBuffer();
				showErrorMessage("[!] Ma don khong hop le!");
			}
			else {
				clearInputBuffer();
				if (cancelOrderByIdSafe(q, cancel_id)) {
					setColor(2);
					printf("\n\t\t\t\t\t\tDa huy don %d thanh cong.", cancel_id);
					setColor(7);
				}
				else {
					showErrorMessage("[!] Khong tim thay don de huy.");
				}
			}
			pause();
			break;
		}

		case 5:
			setColor(7);
			processBackupSystemState(inventory, product_count, q);
			pause();
			break;

		case 6:
			setColor(7);
			processRestoreSystemState(inventory, &product_count, q);
			pause();
			break;

		case 0:
			printBackToMainMessage();
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
		choice = readMenuChoiceInRange(0, 4);

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
		}

	} while (choice != 0);
}

void processCustomer(CustomerList* customer_list) {
	int choice;
	do {
		system("cls");
		menuCustomer();
		choice = readMenuChoiceInRange(0, 5);
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
		case 5:
			setColor(7);
			viewCustomerPurchaseHistory(customer_list);
			pause();
			break;
		case 0:
			printBackToMainMessage();
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
		choice = readMenuChoiceInRange(0, 5);
		switch (choice) {
		case 1:
			reportRevenueByDayMonth();
			pause();
			break;
		case 2:
			reportOrderProcessingPerformance();
			pause();
			break;
		case 3:
			reportOutOfStockHighDemandProducts();
			pause();
			break;
		case 4:
			reportExpressVsStandardRatio();
			pause();
			break;
		case 5:
			runQueueBenchmarkComparison();
			pause();
			break;
		case 0:
			printBackToMainMessage();
			break;
		}
	} while (choice != 0);
	system("cls");
}