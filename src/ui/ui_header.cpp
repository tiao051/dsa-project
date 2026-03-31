#include "../../include/order_manager.h"

//TIÊU Ð? KHO HÀNG S?N PH?M
void printProductHeader() {
	printf("\t\t\t\t| %-16s | %-25s | %-12s | %-10s | %8s |\n",
		"MaSP", "TenSP", "SoLuong", "DonGia", "DaBan");
	printf("\t\t\t\t|------------------|---------------------------|--------------|------------|----------|\n");
}

//TIÊU Ð? ÐON HÀNG
void printOrderHeader() {
	printf("\t\t| %-16s | %-25s | %-25s | %-12s | %-10s | %8s |\n",
		"Ma Don Hang", "Ten Khach Hang", "Ten San Pham", "So Luong", "Uu Tien", "Don Gia");
	printf("\t\t|------------------|---------------------------|---------------------------|--------------|------------|----------|\n");
}
//TIÊU Ð? KHÁCH HÀNG
void printCustomerHeader() {
	printf("\t\t| %-12s | %-20s | %-12s | %-8s | %-12s | %12s |\n",
		"MaKH", "TenKH", "DienThoai", "Hang", "TrangThai", "TongTien");
	printf("\t\t|--------------|----------------------|--------------|----------|--------------|--------------|\n");
}