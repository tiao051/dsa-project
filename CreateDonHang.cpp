#include "QuanLy_DonHangThuongMaiDienTu.h"

//NHẬP THÀNH PHẦN ĐƠN HÀNG
void InsertTail_DonHang(ListDonHang& ldh) {
	DonHang dh;
	printf("\n\t\t\t\t\t\tNhap Ma Don: ");
	scanf("%d", &dh.MaDonHang);
	getchar();
	
	printf("\n\t\t\t\t\t\tNhap Ten Khach Hang: ");
	fgets(dh.TenKH, sizeof(dh.TenKH), stdin);
	dh.TenKH[strlen(dh.TenKH) - 1] = '\0';

	printf("\n\t\t\t\t\t\tNhap Ten San Pham: ");
	fgets(dh.TenSP, sizeof(dh.TenSP), stdin);
	dh.TenSP[strlen(dh.TenSP) - 1] = '\0';

	//KIỂM SỐ LƯỢNG TỒN KHO	
	printf("\n\t\t\t\t\t\tNhap So Luong San Pham: ");
	scanf("%d", &dh.SoLuong);
	for (int i = 0; i < nSP; i++)
	{
		if (strcmp(kho[i].TenSP, dh.TenSP) == 0) {
			if (kho[i].SoLuongTonTai < dh.SoLuong) {
				printf("\n\t\t\t\t\t\tTU CHOI: KHO CON %d SAN PHAM, VUI LONG MUA IT NHAT HOAC CHON SAN PHAM KHAC", kho[i].SoLuongTonTai);
				return;
			}
			dh.DonGia = kho[i].DonGia;
		}

	}
	printf("\n\t\t\t\t\t\tHAY LUA CHON CAC UU TIEN SAU: 1: HOA TOC, 2: VIP, 3: THUONG: ");
	scanf("%d", &dh.UuTien);
	getchar();

	printf("\n\t\t\t\t\t\tNhap Don Gia: ");
	scanf("%lld", &dh.DonGia);
	getchar();

	if (EnqueueDonHang(ldh, dh)) {
		printf("\n\t\t\t\t\t\t-> THEM DON HANG THANH CONG!!!!\n");
	}

}

//TẠO DANH SÁCH ĐƠN HÀNG
void InputYourHand_InsertTailDonHang(ListDonHang& ldh) {
	int n;
	printf("\n\t\t\t\t\t\tNhap So Luong Don Hang: ");
	scanf("%d", &n);
	for (int i = 0; i < n; i++)
	{
		printf("\n\t\t\t\t\t\tTHU TU THU %d\n", i + 1);
		InsertTail_DonHang(ldh);
	}
}

//IN 1 ĐƠN HÀNG
void in1DonHang(DonHang dh) {
	printf("\t\t| DH%-14d | %-25s | %-25s | %-12d | %-10d | %8lld |\n",
		dh.MaDonHang,
		dh.TenKH,
		dh.TenSP,
		dh.SoLuong,
		dh.UuTien,
		dh.DonGia);
}

//XUẤT DANH SÁCH ĐƠN HÀNG
void XuatDonHang(ListDonHang& ldh) {
	printf("\t\t------------------------------------------------DANH SACH DON HANG-------------------------------------------------\n");
	TieuDeDonHang();
	SNodeDonHang* q = ldh.Head;
	while(q != NULL)
	{
		in1DonHang(q->Info);
		q = q->Next;
	}
	printf("\t\t-------------------------------------------------------------------------------------------------------------------\n");
}