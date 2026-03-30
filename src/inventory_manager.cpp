#include "../include/order_manager.h"

KhoHangSanPham kho[MAXSIZE];
int nSP = 0;
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
void displayInventory(Product inventory[MAXSIZE], int& nSP) {
	int masp, soluong;

	printf("\n\t\t\t\t\t\tNhap ma san pham: ");
	scanf("%d", &masp);

	int find = findProductById(inventory, nSP, masp);
	if (find == -1) {
		printf("\n\t\t\t\t\t\tKHONG TIM THAY MASP");
		return;
	}
	else {
		printf("\n\t\t\t\t\t\tSO LUONG NHAP THEM: ");
		scanf("%d", &soluong);
		getchar();

		if (soluong <= 0) {
			printf("\n\t\t\t\t\t\tLOI NHAP SO LUONG !!!!");
			return;
		}
		else {
			inventory[find].stock_quantity += soluong;
			printf("\n\t\t\t\t\t\tDa cap nhat so luong. Tong cong: %d", inventory[find].stock_quantity);
		}
	}
}
void updateInventory(Product inventory[MAXSIZE], int* count) {
	int masp;
	AmountType GiaMoi;

	printf("\n\t\t\t\t\t\tNhap ma san pham: ");
	scanf("%d", &masp);

	int find = findProductById(inventory, *count, masp);
	if (find == -1) {
		printf("\n\t\t\t\t\t\tKHONG TIM THAY MASP");
		return;
	}
	else {
		printf("\n\t\t\t\t\t\tGIA CU: %lld", inventory[find].price);

		printf("\n\t\t\t\t\t\tNHAP GIA MOI: ");
		scanf("%lld", &GiaMoi);
		getchar();

		if (GiaMoi <= 0) {
			printf("\n\t\t\t\t\t\tGIA KHONG HOP LE!!!");
			return;
		}
		else {
			inventory[find].price = GiaMoi;
			printf("\n\t\t\t\t\t\tDa cap nhat gia thanh: %lld", inventory[find].price);
		}
	}
}
void displayInventory(Product inventory[MAXSIZE], int& nSP) {
	printf("\n\t\t\t\t------------------------------------DANH SACH KHO HANG---------------------------------\n");
	TieuDeKhoHang();
	for (int i = 0; i < nSP; i++)
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