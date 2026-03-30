#include "QuanLy_DonHangThuongMaiDienTu.h"

int madon;
KeyType DocFILE_KhoHangSanPham = "DocFile_DanhSachKhoHangSanPham.txt";
KeyType DocFILE_KhachHang = "DocFile_DanhSachKhachHang.txt";

// ================= KHO =================
void processKhoHang(KhoHangSanPham kho[MAXSIZE], int& nSP) {
	int c;
	nSP = 0;
	do {
		system("cls");
		menuKhoSanPham();
		color(10); printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: "); scanf("%d", &c);

		switch (c) {
		case 1:
			color(7);
			LoadFile_SanPham(DocFILE_KhoHangSanPham, kho, nSP);
			XuatKhoHangSanPham(kho, nSP);
			pause();
			break;
		case 2:
			color(7);
			CreateKhoHang(kho, nSP);
			pause();
			break;
		case 3:
			color(7);
			NhapKhoHang(kho, nSP);
			pause();
			break;
		case 4:
			color(7);
			CapNhatKho(kho, nSP);
			pause();
			break;
		case 5:
			color(7);
			XuatKhoHangSanPham(kho, nSP);
			pause();
			break;

		case 0:
			color(10);
			system("cls");
			printf("\n\t\t\t\t\t\tDA QUAY LAI MAN HINH CHINH\n");
			break;
		default:
			color(4);
			system("cls");
			printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
			break;
		}

	} while (c != 0);
}

// ================= DON HANG =================
void processDonHang(ListDonHang& ldh) {
	int c;
	do {
		system("cls");
		menuDonHang();
		color(10); printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: "); scanf("%d", &c);

		switch (c) {
		case 1:
			color(7);
			InputYourHand_InsertTailDonHang(ldh);
			pause();
			break;

		case 2:
			color(7);
			DequeueXuLyDonHang(ldh);
			pause();
			break;

		case 3:
			color(7);
			XuatDonHang(ldh);
			pause();
			break;

		case 0:
			color(10);
			system("cls");
			printf("\n\t\t\t\t\t\tDA QUAY LAI MAN HINH CHINH\n");
			break;
		default:
			color(4);
			system("cls");
			printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
			break;
		}

	} while (c != 0);
}

// ============ TIM KIEM - SAP XEP=================
void processTimKiemSapXep(ListDonHang& ldh, ListKhachHang& lkh) {
	DonHang dh;
	KhachHang kh;
	int c;

	do {
		system("cls");
		menuSapXep_TimKiem();
		color(10); printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: "); scanf("%d", &c);

		switch (c) {
		case 1:
			color(7);
			printf("\n\t\t\t\t\t\tNHAP MA DON: ");
			scanf("%d", &madon);
			getchar();

			if (TimKiem_MaDon(ldh, madon, dh) != NULL) {
				printf("\n\t\t\t\t\t\tTIM THANH CONG!\n");
				TieuDeDonHang();
				in1DonHang(dh);
			}
			else {
				printf("\n\t\t\t\t\t\tKHONG TIM THAY!\n");
			}
			pause();
			break;
		case 2:
			color(7);
			printf("\n\t\t\t\t\t\tNHAP TEN KHACH HANG: ");
			getchar();
			fgets(kh.TenKH, sizeof(kh.TenKH), stdin);
			kh.TenKH[strlen(kh.TenKH) - 1] = '\0';

			if (TimKiem_TenKhachHang(lkh, kh.TenKH, kh) != NULL) {
				printf("\n\t\t\t\t\t\tTIM THANH CONG!\n");
				TieuDeKhachHang();
				in1KhachHang(kh);
			}
			else {
				printf("\n\t\t\t\t\t\tKHONG TIM THAY!\n");
			}
			pause();
			break;
		case 3:
			color(7);
			SapXepSoluong_SanPham_GiamDan(kho, nSP);
			XuatKhoHangSanPham(kho, nSP);
			pause();
			break;
		case 4:
			color(7);
			SapXepTongTien_KhachHang_GiamDan(lkh);
			XuatKhachHang(lkh);
			pause();
			break;
		case 0:
			color(10);
			system("cls");
			printf("\n\t\t\t\t\t\tDA QUAY LAI MAN HINH CHINH\n");
			break;
		default:
			color(4);
			system("cls");
			printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
			break;
		}

	} while (c != 0);
}

void processKhachHang(ListKhachHang& lkh) {
	int c,n;
	do {
		system("cls");
		menuKhachHang();
		color(10); printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: "); scanf("%d", &c);
		switch (c) {
		case 1:
			color(7);
			LoadFile_KhachHang(DocFILE_KhachHang, lkh, n);
			XuatKhachHang(lkh);
			pause();
			break;
		case 2:
			color(7);
			DangKy_ThanhVienMoi(lkh);
			pause();
			break;
		case 3:
			color(7);
			NangHang_ThanhVienTuDong(lkh);
			pause();
			break;
		case 4:
			color(7);
			XuatKhachHang(lkh);
			pause();
			break;
		case 0:
			color(10);
			system("cls");
			printf("\n\t\t\t\t\t\tDA QUAY LAI MAN HINH CHINH\n");
			break;
		default:
			color(4);
			system("cls");
			printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
			break;
		}
	} while (c != 0);
	system("cls");
}

void processThongKe() {
	int c;
	do {
		system("cls");
		menuThongKe();
		color(10); printf("\n\t\t\t\t\t\t->LUA CHON CHUC NANG: "); scanf("%d", &c);
		switch (c) {
		case 1:break;
		case 2:break;
		case 3:break;
		case 4:break;
		case 0:
			color(10);
			system("cls");
			printf("\n\t\t\t\t\t\tDA QUAY LAI MAN HINH CHINH\n");
			break;
		default:
			color(4);
			system("cls");
			printf("\n\t\t\t\t\tBAN DANG VUOT MUC CHUC NANG LUA CHON, VUI LONG LUA CHON CHUC NANG TREN\n");
			break;
		}
	} while (c != 0);
	system("cls");
}