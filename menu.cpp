#include "QuanLy_DonHangThuongMaiDienTu.h"

void menu() {
	color(12);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|                   NHOM 3 - CHU DE 13                |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	color(5);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|     DE TAI: QUAN LY DON HANG THUONG MAI DIEN TU     |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	color(8);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|     HE THONG QUAN LY DON HANG THUONG MAI DIEN TU    |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|1. Quan ly kho hang san pham                         |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|2. Quan ly khach hang                                |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|3. Quan ly don hang                                  |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|4. Quan ly tim kiem va sap xep                       |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|5. Quan ly tong thu doanh cao                        |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|0. Thoat chuong trinh                                |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
}
void menuKhoSanPham(){
	color(11);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|            HE THONG QUAN LY KHO SAN PHAM            |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	color(8);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|1. Load danh sach kho san pham                       |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|2. Them san pham                                     |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|3. Nhap them hang                                    |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|4. Cap nhat gia san pham                             |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|5. Xuat danh sach kho san pham                       |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|0. Thoat                                             |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
}
void menuKhachHang() {
	color(11);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|            HE THONG QUAN LY KHACH HANG              |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	color(8);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|1. Load danh sach khach hang                         |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|2. Dang ky them thanh vien moi                       |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|3. Nang hang thanh vien                              |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|4. Xuat danh sach khach hang                         |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|0. Thoat                                             |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
}
void menuDonHang() {
	color(11);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|            HE THONG QUAN LY DON HANG                |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	color(8);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|1. Tao danh sach don hang                            |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|2. Xu ly don hang                                    |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|3. Xuat danh sach don hang                           |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|0. Thoat                                             |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
}
void menuSapXep_TimKiem() {
	color(11);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|        HE THONG QUAN LY TIM KIEM - SAP XEP          |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	color(8);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|1. Tim kiem ma don                                   |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|2. Tim kiem ten khach hang                           |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|3. Sap xep san pham (Best seller)                    |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|4. Sap xep khach hang (Top Spender)                  |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|0. Thoat                                             |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
}
void menuThongKe() {
	color(11);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|              HE THONG QUAN LY THONG KE              |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	color(8);
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|1. Tong doanh thu                                    |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|2. So don da xu ly                                   |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|3. So luong don con ton dong trong Queue             |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|4. Ti le don hoa toc                                 |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t|0. Thoat                                             |\t\t\t\t\t\n");
	printf("\t\t\t\t\t\t -----------------------------------------------------\t\t\t\t\t\n");
}

void color(int color) {// Set color giao diện
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void pause() {
	printf("\n\t\t\t\t\t\tNhan phim bat ky de tiep tuc...");
	_getch();
	system("cls");
}