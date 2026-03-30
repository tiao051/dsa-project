#define _CRT_SECURE_NO_WARNINGS

#ifndef QuanLy_DonHangThuongMaiDienTu_h
#define QuanLy_DonHangThuongMaiDienTu_h

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#define MAXSIZE 100

#define DH_HOATOC 1
#define DH_VIP 2
#define DH_THUONG 3

#define KH_HOATOC 2
#define KH_VIP 1
#define KH_THUONG 0

typedef char KeyType[100];
typedef long long ItemLongType;

//QUAN LY KHO HANG SAN PHAM
struct KhoHangSanPham {
	int MaSP;
	KeyType TenSP;
	int SoLuongTonTai;
	ItemLongType DonGia;	
	int SoLuongDaBan;
};
extern KhoHangSanPham kho[MAXSIZE];
extern int nSP;
//QUAN LY DANH SACH DON HANG QUEUE
struct DanhSachDonHang {
	int MaDonHang;
	KeyType TenKH;
	KeyType TenSP;
	int SoLuong;
	ItemLongType DonGia;
	int UuTien;
};

typedef DanhSachDonHang DonHang;

struct SNodeDonHang {
	DonHang Info;
	SNodeDonHang* Next;
};

struct ListDonHang {
	SNodeDonHang* Head = NULL;
	SNodeDonHang* Tail = NULL;
};
//QUAN LY DANH SACH KHACH HANG DSLK
struct DanhSachKhachHang {
	int MaKH;
	KeyType TenKH;
	KeyType SoDienThoai;
	int UuTienKH; //1 - HỎA TỐC , 2 - VIP , 3 - THƯỜNG
	KeyType TrangThai;
	ListDonHang LichSuDH;
	ItemLongType TongTien;
};

typedef DanhSachKhachHang KhachHang;

struct SNodeKhachHang {
	KhachHang Info;
	SNodeKhachHang* Next;
};
struct ListKhachHang {
	SNodeKhachHang* Head = NULL;
	SNodeKhachHang* Tail = NULL;
};

//INIT QUEUE
void InitQueueDH(ListDonHang& ldh);
//KIEM TRA DANH SACH RONG
int IsEmptyDH(ListDonHang& ldh);
//TAO KHACH HANG
SNodeKhachHang* CreateKhachHang(KhachHang x);
//KIEM TRA KHACH HANG RONG
int IsEmptyKH(ListKhachHang& lkh);
//TAO DON HANG
SNodeDonHang* CreateDonHang(DonHang x);
//TAO KHO HANG BANG MANG
void CreateKhoHang(KhoHangSanPham kho[MAXSIZE], int& nSP);
//TAO KHACH HANG BANG DSLK
int InsertTaiL_KhachHang(ListKhachHang& lkh, KhachHang kh);
//TAO DON HANG BANG ENQUEUE(THEM)
int EnqueueDonHang(ListDonHang& ldh, DonHang dh);
//THEM DON HANG
void InsertTail_DonHang(ListDonHang& ldh);
//THEM KHACH HANG
void DangKy_ThanhVienMoi(ListKhachHang& lkh);
//DOC 1 FILE SAN PHAM
void Load1File_SanPham(KhoHangSanPham kho[MAXSIZE], FILE* fi);
//DOC FILE SAN PHAM
void LoadFile_SanPham(KeyType TenFILE, KhoHangSanPham kho[MAXSIZE], int& nSP);
//DOC 1 FILE KHACH HANG
void Load1File_KhachHang(FILE* fi, KhachHang& kh);
//DOC FILE KHACH HANG
void LoadFile_KhachHang(KeyType TenFILE, ListKhachHang& lkh, int& n);
//XOA DON HANG(DEQUEUE)
int DequeueXuLyDonHang(ListDonHang& ldh);
//TIM KIEM DON HANG (MADON)
SNodeDonHang* TimKiem_MaDon(ListDonHang& ldh, int ma, DonHang& dh);
//TIM KIEM DON HANG (TenKH)
SNodeKhachHang* TimKiem_TenKhachHang(ListKhachHang& ldh, KeyType ten, KhachHang& kh);
//TIM KIEM MA SAN PHAM 
int TimKiem_MaSP(KhoHangSanPham kho[MAXSIZE], int& nSP, int masp);
//NHAP KHO HANG
void NhapKhoHang(KhoHangSanPham kho[MAXSIZE], int& nSP);
//CAP NHAT KHO HANG
void CapNhatKho(KhoHangSanPham kho[MAXSIZE], int& nSP);
//SAP XEP GIAM DAN SAN PHAM THEO SO LUONG
void SwapKhoHang(KhoHangSanPham& a, KhoHangSanPham& b);
void SapXepSoluong_SanPham_GiamDan(KhoHangSanPham kho[MAXSIZE], int& nSP);
//SAP XEP GIAM DAN SAN PHAM THEO SO LUONG
void SwapKhachHang(KhachHang& a, KhachHang& b);
void SapXepTongTien_KhachHang_GiamDan(ListKhachHang& lkh);
//XUAT DANH SACH KHO HANG SAN PHAM
void XuatKhoHangSanPham(KhoHangSanPham kho[MAXSIZE], int& nSP);
//IN 1 DON HANG
void in1DonHang(DonHang dh);
//XUAT DANH SACH DON HANG
void XuatDonHang(ListDonHang& ldh);
//IN 1 KHACH HANG
void in1KhachHang(KhachHang kh);
//XUAT DANH SACH KHACH HANG
void XuatKhachHang(ListKhachHang& lkh);
//NÂNG HẠNG THÀNH VIÊN TỰ ĐỘNG
void NangHang_ThanhVienTuDong(ListKhachHang& lkh);
//TAO DANH SACH DON HANG
void InputYourHand_InsertTailDonHang(ListDonHang& ldh);
//MENU
void menu();
void menuKhachHang();
void menuDonHang();
void menuKhoSanPham();
void menuSapXep_TimKiem();
void menuThongKe();
//TIEU DE KHO HANG
void TieuDeKhoHang();
//TIEU DE DON HANG
void TieuDeDonHang();
//TIEU DE KHACH HANG
void TieuDeKhachHang();
//COLOR
void color(int color);
void pause();
//PROCESS 
void process();
//PROCESS DON HANG
void processKhoHang(KhoHangSanPham kho[MAXSIZE], int& nSP);
//PROCES KHO HANG SAN PHAM
void processDonHang(ListDonHang& ldh);
//PROCESS TIM KIEM - SAP XEP
void processTimKiemSapXep(ListDonHang& ldh, ListKhachHang& lkh);
//PROCESS KHACH HANG
void processKhachHang(ListKhachHang& lkh);
//PROCESS THONG KE
void processThongKe();
#endif