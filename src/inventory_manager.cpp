#include "../include/order_manager.h"

void createInventory(Product inventory[MAXSIZE], int* nSP) {

	if (*nSP >= MAXSIZE) {
		printf("\n\t\t\t\t\t\tKHO DA DAY, KHONG THE THEM!");
		return;
	}

	int c;
	while ((c = getchar()) != '\n' && c != EOF);

	// Auto-generate product ID
	inventory[*nSP].id = 101 + *nSP;

	// Validate product name
	do {
		printf("\n\t\t\t\t\t\tNhap ten san pham: ");
		fgets(inventory[*nSP].name, sizeof(inventory[*nSP].name), stdin);
		inventory[*nSP].name[strcspn(inventory[*nSP].name, "\n")] = '\0';

		if (strlen(inventory[*nSP].name) == 0) {
			setColor(4); printf("\t\t\t\t\t\t[!] Ten san pham khong duoc de trong!\n"); setColor(7);
			continue;
		}

		bool exists = false;
		for (int i = 0; i < *nSP; i++) {
			if (_stricmp(inventory[i].name, inventory[*nSP].name) == 0) {
				exists = true;
				break;
			}
		}

		if (exists) {
			setColor(4); printf("\t\t\t\t\t\t[!] San pham da ton tai trong kho!\n"); setColor(7);
			inventory[*nSP].name[0] = '\0'; // Clear name to trigger re-entry
		}
	} while (strlen(inventory[*nSP].name) == 0);

	// Validate stock quantity
	do {
		printf("\n\t\t\t\t\t\tNhap so luong san pham: ");
		if (scanf("%d", &inventory[*nSP].stock_quantity) != 1) {
			while (getchar() != '\n'); // Clear buffer
			continue;
		}
		getchar();

		if (inventory[*nSP].stock_quantity <= 0) {
			setColor(4); printf("\t\t\t\t\t\t[!] So luong phai lon hon 0!\n"); setColor(7);
		}
		else if (inventory[*nSP].stock_quantity > 1000) { // Giả sử giới hạn là 1000 đơn vị
			setColor(4); printf("\t\t\t\t\t\t[!] So luong qua lon (Toi da 1000)!\n"); setColor(7);
			inventory[*nSP].stock_quantity = 0; // Trigger re-entry
		}
	} while (inventory[*nSP].stock_quantity <= 0);

	// Validate price
	do {
		printf("\n\t\t\t\t\t\tNhap don gia: ");
		scanf("%lld", &inventory[*nSP].price);
		getchar();

		if (inventory[*nSP].price <= 0) {
			setColor(4); printf("\t\t\t\t\t\t[!] Don gia phai lon hon 0!\n"); setColor(7);
		}
	} while (inventory[*nSP].price <= 0);

	inventory[*nSP].sold_quantity = 0;
	(*nSP)++;

	printf("\n\t\t\t\t\t\t->THEM KHO HANG THANH CONG");
	printf("\n\t\t\t\t\t\tMa san pham: %d", inventory[*nSP - 1].id);
	printf("\n\t\t\t\t\t\tTen san pham: %s", inventory[*nSP - 1].name);
	printf("\n\t\t\t\t\t\tSo luong: %d", inventory[*nSP - 1].stock_quantity);
	printf("\n\t\t\t\t\t\tDon gia: %lld", inventory[*nSP - 1].price);

}

void importStock(Product inventory[MAXSIZE], int* product_count) {
	int product_id, additional_quantity;

	printf("\n\t\t\t\t\t\tNHAP TEN SAN PHAM DE THEM VAO KHO: ");
	scanf("%d", &product_id);

	int index = findProductById(inventory, *product_count, product_id);

	if (index == -1) {
		printf("\n\t\t\t\t\t\tKHONG TIM THAY SAN PHAM!");
		return;
	}
	else {
		printf("\n\t\t\t\t\t\tSO LUONG THEM VAO KHO: ");
		scanf("%d", &additional_quantity);
		while (getchar() != '\n');

		if (additional_quantity <= 0) {
			printf("\n\t\t\t\t\t\tSO LUONG KHONG HOP LE!");
			return;
		}
		else {
			if (inventory[index].stock_quantity + additional_quantity > 1000) {
				setColor(4); printf("\n\t\t\t\t\t\tTON KHO VUOT QUA GIOI HAN (1000)!"); setColor(7);
				return;
			}
			inventory[index].stock_quantity += additional_quantity;
			printf("\n\t\t\t\t\t\THANH CONG! SAN PHAM MOI: %d", inventory[index].stock_quantity);
		}
	}
}

void updateInventory(Product inventory[MAXSIZE], int* product_count) {
	char search_name[50];
	AmountType new_price;

	int c;
	while ((c = getchar()) != '\n' && c != EOF);

	printf("\n\t\t\t\t\t\tNhap ten san pham can cap nhat: ");
	fgets(search_name, sizeof(search_name), stdin);
	search_name[strcspn(search_name, "\n")] = '\0';

	int find = -1;
	for (int i = 0; i < *product_count; i++) {
		if (_stricmp(inventory[i].name, search_name) == 0) {
			find = i;
			break;
		}
	}

	if (find == -1){
		printf("\n\t\t\t\t\t\tKHONG TIM THAY SAN PHAM!");
		return;
	}
	else {
		printf("\n\t\t\t\t\t\tNhap don gia moi: ");
		scanf("%lld", &new_price);
		getchar();
		if (new_price <= 0) {
			setColor(4); printf("\n\t\t\t\t\t\tDON GIA KHONG HOP LE!"); setColor(7);
			return;
		}
		else {
			inventory[find].price = new_price;
			printf("\n\t\t\t\t\t\THANH CONG! DON GIA MOI: %lld", inventory[find].price);
		}
	}
}

void displayInventory(Product inventory[MAXSIZE], int product_count) {
	printf("\n\t\t\t\t------------------------------------DANH SACH KHO HANG---------------------------------\n");
	printProductHeader();
	for (int i = 0; i < product_count; i++)
	{
		printf("\t\t\t\t| SP%-14d | %-25s | %-12d | %-10lld | %8d |\n",
			inventory[i].id,
			inventory[i].name,
			inventory[i].stock_quantity,
			inventory[i].price,
			inventory[i].sold_quantity);
	}
	printf("\t\t\t\t---------------------------------------------------------------------------------------\n");
}