#include "../include/order_manager.h"

SNodeKhachHang* CreateKhachHang(KhachHang kh) {
	SNodeKhachHang* p = new SNodeKhachHang();
	if (p == NULL) return NULL;
	p->Info = kh;
	p->Next = NULL;
	return p;
}
// Kiem tra danh sach khach hang rong
int IsEmptyKH(ListKhachHang& lkh) {
	return lkh.Head == NULL ? 1 : 0;
}

// Chen 1 khach hang vao cuoi danh sach lien ket (DSLK)
int InsertTaiL_KhachHang(ListKhachHang& lkh, KhachHang kh) {
	SNodeKhachHang* p = CreateKhachHang(kh);
	if (p == NULL) return 0;
	if (IsEmptyKH(lkh)) {
		lkh.Head = lkh.Tail = p;
	}
	else {
		lkh.Tail->Next = p;
		lkh.Tail = p;
	}
	return 1;
}