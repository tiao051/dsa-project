#include "../include/order_manager.h"

//LOAD 1 FILE SA PA
void Load1File_SanPham(KhoHangSanPham kho[MAXSIZE], FILE* fi) {
	int i = 0;
	while (i < nSP) {
		fscanf(fi, "%d,%[^,],%d,%lld,%d\n",
			&kho[i].MaSP,
			kho[i].TenSP,
			&kho[i].SoLuongTonTai,
			&kho[i].DonGia,
			&kho[i].SoLuongDaBan);
		i++;
	}
}
//LOAD FILE SAN PHAM
void LoadFile_SanPham(KeyType TenFILE, KhoHangSanPham kho[MAXSIZE], int& nSP) {
	FILE* fi = fopen(TenFILE, "rt");
	if (fi == NULL) {
		printf("\nLOI DOC FILE %s VUI LONG KIEM TRA LAI", TenFILE);
		return;
	}
	fscanf(fi, "%d\n", &nSP);
	Load1File_SanPham(kho, fi);
	fclose(fi);
}
//LOAD 1 FILE KHACH HANG
void Load1File_KhachHang(FILE* fi, KhachHang& kh) {

	fscanf(fi, "%d,%[^,],%[^,],%d,%[^,],%lld\n",
		&kh.MaKH,
		kh.TenKH,
		kh.SoDienThoai,
		&kh.UuTienKH,
		kh.TrangThai,
		&kh.TongTien);

}
//LOAD FILE KHACH HANG
void LoadFile_KhachHang(KeyType TenFILE, ListKhachHang& lkh, int& n) {
	FILE* fi = fopen(TenFILE, "rt");
	if (fi == NULL) {
		printf("\n\t\t\t\t\t\tLOI DOC FILE %s VUI LONG KIEM TRA LAI", TenFILE);
		return;
	}

	fscanf(fi, "%d\n", &n);
	int i = 0;
	while (i < n) {
		KhachHang kh;
		Load1File_KhachHang(fi, kh); 
		SNodeKhachHang* p = CreateKhachHang(kh);
		InsertTaiL_KhachHang(lkh, p->Info);
		i++;
	}

	fclose(fi);
}
