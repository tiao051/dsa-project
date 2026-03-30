#include "../include/order_manager.h"

ListDonHang ldh;
ListKhachHang lkh;
int luachon;
void process() {

	do {
		menu();
		color(10);
		printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: "); scanf("%d", &luachon);
		switch (luachon) {
		case 1:
			processKhoHang(kho, nSP);
			break;
		case 2:
			processKhachHang(lkh);
			break;
		case 3:
			processDonHang(ldh);
			break;
		case 4:
			processTimKiemSapXep(ldh, lkh);
			break;
		case 5:

			processThongKe();
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
			color(4);
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
			color(4); 
			system("cls");
			printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n"); 
			break;
		}
	} while (luachon != 0);
	
}

int main() {
	KhoHangSanPham kho[MAXSIZE];
	DonHang dh;
	KhachHang kh;
	int nSP = 0;
	InitQueueDH(ldh);
	process();
	return 0;
}