#include "QuanLy_DonHangThuongMaiDienTu.h"

//TIÊU ĐỀ KHO HÀNG SẢN PHẨM
void TieuDeKhoHang() {
	printf("\t\t\t\t| %-16s | %-25s | %-12s | %-10s | %8s |\n",
		"MaSP", "TenSP", "SoLuong", "DonGia", "DaBan");
	printf("\t\t\t\t|------------------|---------------------------|--------------|------------|----------|\n");
}

//TIÊU ĐỀ ĐƠN HÀNG
void TieuDeDonHang() {
	printf("\t\t| %-16s | %-25s | %-25s | %-12s | %-10s | %8s |\n",
		"Ma Don Hang", "Ten Khach Hang", "Ten San Pham", "So Luong", "Uu Tien", "Don Gia");
	printf("\t\t|------------------|---------------------------|---------------------------|--------------|------------|----------|\n");
}
//TIÊU ĐỀ KHÁCH HÀNG
void TieuDeKhachHang() {
	printf("\t\t\t| %-6s | %-25s | %-15s | %-8s | %-12s | %-12s |\n",
		"MaKH", "TenKH", "DienThoai", "UuTien", "TrangThai", "TongTien");
	printf("\t\t\t|--------|---------------------------|-----------------|----------|--------------|--------------|\n");
}