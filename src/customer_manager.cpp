#include "../include/order_manager.h"

//ĐĂNG KÝ THÀNH VIÊN MỚI
void DangKy_ThanhVienMoi(ListKhachHang& lkh) {
	KhachHang kh;

	printf("\n\t\t\t\t\t\tNhap Ma Khach Hang: ");
	scanf("%d", &kh.MaKH);
	getchar();

	printf("\n\t\t\t\t\t\tNhap Ten Khach Hang: ");
	fgets(kh.TenKH, sizeof(kh.TenKH), stdin);
	kh.TenKH[strlen(kh.TenKH) - 1] = '\0';

	printf("\n\t\t\t\t\t\tNhap So Dien Thoai: ");
	fgets(kh.SoDienThoai, sizeof(kh.SoDienThoai), stdin);
	kh.SoDienThoai[strlen(kh.SoDienThoai) - 1] = '\0';

	printf("\n\t\t\t\t\t\tUu Tien (1: Hoa Toc, 2: VIP, 3: THUONG): ");
	scanf("%d", &kh.UuTienKH);
	getchar();

	printf("\n\t\t\t\t\t\tTrang Thai: ");
	fgets(kh.TrangThai, sizeof(kh.TrangThai), stdin);
	kh.TrangThai[strlen(kh.TrangThai) - 1] = '\0';

	InitQueueDH(kh.LichSuDH);
	kh.TongTien = 0;

	if (InsertTaiL_KhachHang(lkh, kh)) {
		printf("\n\t\t\t\t\t\t->THEM KHACH HANG THANH CONG");
	}
	else {
		printf("\n\t\t\t\t\t\t-> DANG KY THAT BAI!\n");
	}
}


//IN 1 KHÁCH HÀNG
void in1KhachHang(KhachHang kh) {
	printf("\t\t\t| KH%-4d | %-25s | %-15s | %-8d | %-12s | %12lld |\n",
			kh.MaKH,
			kh.TenKH,
			kh.SoDienThoai,
			kh.UuTienKH,
			kh.TrangThai,
			kh.TongTien);
}
// XUẤT DANH SÁCH KHÁCH HÀNG
void XuatKhachHang(ListKhachHang& lkh) {
	printf("\t\t\t------------------------------------- DANH SACH KHACH HANG --------------------------------------\n");
	TieuDeKhachHang();
	SNodeKhachHang* p = lkh.Head;
	while (p != NULL) {
		in1KhachHang(p->Info);
		p = p->Next;
	}
	printf("\t\t\t-------------------------------------------------------------------------------------------------\n");
}

//QUẢN LÝ KHÁCH HÀNG: NÂNG HẠNG THÀNH VIÊN 
void NangHang_ThanhVienTuDong(ListKhachHang& lkh);