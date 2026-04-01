#include "../../include/order_manager.h"

//TI�U �? KHO H�NG S?N PH?M
void printProductHeader() {
	printf("\t\t\t\t| %-16s | %-25s | %-12s | %-10s | %8s |\n",
		"MaSP", "TenSP", "SoLuong", "DonGia", "DaBan");
	printf("\t\t\t\t|------------------|---------------------------|--------------|------------|----------|\n");
}

//TI�U �? �ON H�NG
void printOrderHeader() {
	printf("\t| %-16s | %-20s | %-20s | %-8s | %-10s | %-10s | %-18s | %10s |\n",
		"Ma Don Hang", "Ten Khach Hang", "Ten San Pham", "SoLuong", "VanChuyen", "UuTien", "TrangThai", "Don Gia");
	printf("\t|------------------|----------------------|----------------------|----------|------------|------------|--------------------|------------|\n");
}
//TI�U �? KH�CH H�NG
void printCustomerHeader() {
	printf("\t\t| %-12s | %-20s | %-12s | %-8s | %-12s | %12s |\n",
		"MaKH", "TenKH", "DienThoai", "Hang", "TrangThai", "TongTien");
	printf("\t\t|--------------|----------------------|--------------|----------|--------------|--------------|\n");
}