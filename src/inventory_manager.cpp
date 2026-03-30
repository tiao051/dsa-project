#include "../include/order_manager.h"

void createInventory(Product inventory[MAXSIZE], int& nSP) {
	printf("\n\t\t\t\t\t\tNhap ma kho hang san pham: ");
	scanf("%d", &inventory[nSP].id);
	getchar();

	printf("\n\t\t\t\t\t\tNhap ten san pham: ");
	fgets(inventory[nSP].name, sizeof(inventory[nSP].name), stdin);
	inventory[nSP].name[strlen(inventory[nSP].name) - 1] = '\0';

	printf("\n\t\t\t\t\t\tNhap so luong ton tai: ");
	scanf("%d", &inventory[nSP].stock_quantity);
	getchar();
	
	printf("\n\t\t\t\t\t\tNhap don gia: ");
	scanf("%lld", &inventory[nSP].price);
	getchar();

	inventory[nSP].sold_quantity = 0;
	nSP++;

	printf("\n\t\t\t\t\t\t->THEM KHO HANG THANH CONG");

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
			inventory[index].stock_quantity += additional_quantity;
			printf("\n\t\t\t\t\t\THANH CONG! SAN PHAM MOI: %d", inventory[index].stock_quantity);
		}
	}
}

void updateInventory(Product inventory[MAXSIZE], int* product_count) {
	int product_id;
	AmountType new_price;

	printf("\n\t\t\t\t\t\tNhap ma san pham: ");
	scanf("%d", &product_id);

	int find = findProductById(inventory, *product_count, product_id);
	if (find == -1) {
		printf("\n\t\t\t\t\t\tKHONG TIM THAY MASP");
		return;
	}
	else {
		printf("\n\t\t\t\t\t\tGIA CU: %lld", inventory[find].price);

		printf("\n\t\t\t\t\t\tNHAP GIA MOI: ");
		scanf("%lld", &new_price);
		getchar();

		if (new_price <= 0) {
			printf("\n\t\t\t\t\t\tGIA KHONG HOP LE!!!");
			return;
		}
		else {
			inventory[find].price = new_price;
			printf("\n\t\t\t\t\t\tDa cap nhat gia thanh: %lld", inventory[find].price);
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