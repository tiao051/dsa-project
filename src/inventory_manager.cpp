#include "../include/order_manager.h"

KhoHangSanPham kho[MAXSIZE];
int nSP = 0;
void CreateKhoHang(KhoHangSanPham kho[MAXSIZE], int& nSP) {
	printf("\n\t\t\t\t\t\tNhap ma kho hang san pham: ");
	scanf("%d", &kho[nSP].MaSP);
	getchar();

	printf("\n\t\t\t\t\t\tNhap ten san pham: ");
	fgets(kho[nSP].TenSP, sizeof(kho[nSP].TenSP), stdin);
	kho[nSP].TenSP[strlen(kho[nSP].TenSP) - 1] = '\0';

	printf("\n\t\t\t\t\t\tNhap so luong ton tai: ");
	scanf("%d", &kho[nSP].SoLuongTonTai);
	getchar();
	
	printf("\n\t\t\t\t\t\tNhap don gia: ");
	scanf("%lld", &kho[nSP].DonGia);
	getchar();

	kho[nSP].SoLuongDaBan = 0;
	nSP++;

	printf("\n\t\t\t\t\t\t->THEM KHO HANG THANH CONG");

}
void NhapKhoHang(KhoHangSanPham kho[MAXSIZE], int& nSP) {
	int masp, soluong;

	printf("\n\t\t\t\t\t\tNhap ma san pham: ");
	scanf("%d", &masp);

	int find = TimKiem_MaSP(kho, nSP, masp);
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
			kho[find].SoLuongTonTai += soluong;
			printf("\n\t\t\t\t\t\tDa cap nhat so luong. Tong cong: %d", kho[find].SoLuongTonTai);
		}
	}
}
void CapNhatKho(KhoHangSanPham kho[MAXSIZE], int& nSP) {
	int masp;
	ItemLongType GiaMoi;

	printf("\n\t\t\t\t\t\tNhap ma san pham: ");
	scanf("%d", &masp);

	int find = TimKiem_MaSP(kho, nSP, masp);
	if (find == -1) {
		printf("\n\t\t\t\t\t\tKHONG TIM THAY MASP");
		return;
	}
	else {
		printf("\n\t\t\t\t\t\tGIA CU: %d", kho[find].DonGia);

		printf("\n\t\t\t\t\t\tNHAP GIA MOI: ");
		scanf("%lld", &GiaMoi);
		getchar();

		if (GiaMoi <= 0) {
			printf("\n\t\t\t\t\t\tGIA KHONG HOP LE!!!");
			return;
		}
		else {
			kho[find].DonGia = GiaMoi;
			printf("\n\t\t\t\t\t\tDa cap nhat gia thanh: %lld", kho[find].DonGia);
		}
	}
}
void XuatKhoHangSanPham(KhoHangSanPham kho[MAXSIZE], int& nSP) {
	printf("\n\t\t\t\t------------------------------------DANH SACH KHO HANG---------------------------------\n");
	TieuDeKhoHang();
	for (int i = 0; i < nSP; i++)
	{
		printf("\t\t\t\t| SP%-14d | %-25s | %-12d | %-10lld | %8d |\n",
			kho[i].MaSP,
			kho[i].TenSP,
			kho[i].SoLuongTonTai,
			kho[i].DonGia,
			kho[i].SoLuongDaBan);
	}
	printf("\t\t\t\t---------------------------------------------------------------------------------------\n");
}