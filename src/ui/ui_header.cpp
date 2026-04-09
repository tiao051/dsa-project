#include "../../include/order_manager.h"

// Inventory table header
void printProductHeader() {
	printf("\t\t\t\t| %-16s | %-25s | %-12s | %-10s | %8s |\n",
		"MaSP", "TenSP", "SoLuong", "DonGia", "DaBan");
	printf("\t\t\t\t|------------------|---------------------------|--------------|------------|----------|\n");
}

// Order table header
void printOrderHeader() {
	printf("\t\t| %-16s | %-20s | %-20s | %-8s | %-11s | %-10s | %-18s | %10s |\n",
		"Ma Don Hang", "Ten Khach Hang", "Ten San Pham", "So Luong", "Van Chuyen", "Uu Tien", "Trang Thai", "Don Gia");
	printf("\t\t|------------------|----------------------|----------------------|----------|-------------|------------|--------------------|------------|\n");
}
// Customer table header
void printCustomerHeader() {
	printf("\t\t| %-12s | %-20s | %-12s | %-8s | %-12s | %12s |\n",
		"MaKH", "TenKH", "DienThoai", "Hang", "TrangThai", "TongTien");
	printf("\t\t|--------------|----------------------|--------------|----------|--------------|--------------|\n");
}