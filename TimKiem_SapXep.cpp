#include "QuanLy_DonHangThuongMaiDienTu.h"
//TÌM KIẾM MÃ SẢN PHẨM
int TimKiem_MaSP(KhoHangSanPham kho[MAXSIZE], int& nSP, int masp) {
	for (int i = 0; i < nSP; i++)
	{
		if (kho[i].MaSP == masp)
			return i;
	}
	return -1;
}
//TÌM KIẾM MÃ ĐƠN
SNodeDonHang* TimKiem_MaDon(ListDonHang& ldh, int ma, DonHang& dh) {
	if (IsEmptyDH(ldh) == 1) return NULL;
	SNodeDonHang* q = ldh.Head;
	while(q != NULL)
	{
		if (q->Info.MaDonHang == ma) {
			dh = q->Info;
			return q;
		}
		q = q->Next;
	}
	return NULL;
}
//TÌM KIẾM TÊN KHÁCH HÀNG
SNodeKhachHang* TimKiem_TenKhachHang(ListKhachHang& lkh, KeyType ten, KhachHang& kh) {
	SNodeKhachHang* p = lkh.Head;
	if (ten == NULL)return NULL;
	while (p != NULL) {
		if (strcmp(p->Info.TenKH, ten) == 0) {
			kh = p->Info;
			return p;
		}
		p = p->Next;
	}
	return NULL;
}

//SWAP KHO HANG
void SwapKhoHang(KhoHangSanPham& a, KhoHangSanPham& b) {
	KhoHangSanPham temp = a;
	a = b;
	b = temp;
}
//SAP XEP SO LONG SAN PHAM GIAM DAN
void SapXepSoluong_SanPham_GiamDan(KhoHangSanPham kho[MAXSIZE], int& nSP) {
	for (int i = 0; i < nSP - 1; i++) {
		for (int j = i + 1; j < nSP; j++) {
			if (kho[i].SoLuongDaBan < kho[j].SoLuongDaBan) {
				SwapKhoHang(kho[i], kho[j]);
			}
		}
	}
}

//SWAP KHACH HANG
void SwapKhachHang(KhachHang& a, KhachHang& b) {
	KhachHang temp = a;
	a = b;
	b = temp;
}
//SAP ZEP
void SapXepTongTien_KhachHang_GiamDan(ListKhachHang& lkh) {
	for (SNodeKhachHang* i = lkh.Head; i->Next != NULL; i = i->Next) {
		for (SNodeKhachHang* j = i->Next; j != NULL; j = j->Next)
		{
			if (i->Info.TongTien < j->Info.TongTien) {
				SwapKhachHang(i->Info, j->Info);
			}
		}
	}
}
