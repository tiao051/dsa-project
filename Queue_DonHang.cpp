#include "QuanLy_DonHangThuongMaiDienTu.h"

SNodeDonHang* CreateDonHang(DonHang x) {
	SNodeDonHang* q = new SNodeDonHang();
	if (q == NULL) return NULL;
	q->Info = x;
	q->Next = NULL;
	return q;
}

int IsEmptyDH(ListDonHang& ldh) {
	return ldh.Head == NULL ? 1 : 0;
}

void InitQueueDH(ListDonHang& ldh) {
	ldh.Head = ldh.Tail = NULL;
}


//ENQUEUE
int EnqueueDonHang(ListDonHang& ldh, DonHang dh) {
	SNodeDonHang* q = CreateDonHang(dh);

	if (IsEmptyDH(ldh) == 1) {
		ldh.Head = ldh.Tail = q;
		return 1;
	}
	else {
		// HỎA TỐC
		if (dh.UuTien == DH_HOATOC) {
			SNodeDonHang* Cur = ldh.Head;
			SNodeDonHang* Prev = NULL;
			while (Cur != NULL && Cur->Info.UuTien == dh.UuTien)
			{
				Prev = Cur;
				Cur = Cur->Next;
			}
			if (Prev == NULL) {
				q->Next = ldh.Head;
				ldh.Head = q;
			}
			else {
				Prev->Next = q;
				q->Next = Cur;
			}
			return 1;
		}
		// VIP
		else if (dh.UuTien == DH_VIP) {
			SNodeDonHang* Cur = ldh.Head;
			SNodeDonHang* Prev = NULL;
			while (Cur != NULL && Cur->Info.UuTien < dh.UuTien) {
				Prev = Cur;
				Cur = Cur->Next;
			}
			Prev->Next = q;
			q->Next = Cur;
			if (Prev == NULL) ldh.Tail = q;
			return 1;
		}
		// THƯỜNG
		else {
			ldh.Tail->Next = q;
			ldh.Tail = q;
			return 1;
		}
	}

}

//DEQUEUE

int DequeueXuLyDonHang(ListDonHang& ldh) {
	if (IsEmptyDH(ldh) == 1) {
		printf("\n\t\t\t\t\t\tQUEUE DANG RONG !!!");
		return 1;
	}

	SNodeDonHang* temp = ldh.Head;
	printf("\n\t\t\t\t\t\tDA XU LY DON: %d - %s", temp->Info.MaDonHang, temp->Info.TenKH);
	//CẬP NHẬT TRỪ TỒN KHO
	for (int i = 0; i < nSP; i++)
	{
		if (strcmp(kho[i].TenSP, temp->Info.TenSP) == 0) {
			kho[i].SoLuongTonTai -= temp->Info.SoLuong;
			kho[i].SoLuongDaBan += temp->Info.SoLuong;
		}
	}
	ldh.Head = ldh.Head->Next;
	if (ldh.Head == NULL) ldh.Tail = NULL;
	delete(temp);
	return 1;
}