# Hệ Thống Quản Lý Đơn Hàng Thương Mại Điện Tử
**Chủ đề 13 - Nhóm 3**

Ứng dụng quản lý đơn hàng thương mại điện tử xây dựng trên nền tảng C/C++, minh họa các cấu trúc dữ liệu như danh sách liên kết, hàng đợi ưu tiên và mảng cùng với các giải thuật tìm kiếm, sắp xếp.

---

## Mục lục
- [Yêu cầu hệ thống](#yêu-cầu-hệ-thống)
- [Cài đặt và chạy dự án](#cài-đặt-và-chạy-dự-án)
- [Cấu trúc dự án](#cấu-trúc-dự-án)
- [Chức năng chính](#chức-năng-chính)
- [Cấu trúc dữ liệu](#cấu-trúc-dữ-liệu)
- [File dữ liệu](#file-dữ-liệu)

---

## Yêu cầu hệ thống

| Thành phần | Yêu cầu |
|---|---|
| Hệ điều hành | Windows 10 / Windows 11 |
| IDE | Visual Studio 2022 (Community / Professional / Enterprise) |
| Workload | **Desktop development with C++** |
| C++ Standard | C++11 trở lên |

> **Lưu ý:** Dự án sử dụng Windows API (`windows.h`, `conio.h`, `_stricmp`) nên **chỉ chạy được trên Windows**.

---

## Cài đặt và chạy dự án

### Bước 1: Cài đặt Visual Studio 2022

1. Tải Visual Studio 2022 tại [visualstudio.microsoft.com](https://visualstudio.microsoft.com/).
2. Trong quá trình cài đặt, chọn workload **"Desktop development with C++"**.
3. Hoàn tất cài đặt và khởi động lại máy nếu cần.

### Bước 2: Clone hoặc tải dự án

```bash
git clone <repository-url>
```

Hoặc tải file ZIP và giải nén vào thư mục tùy chọn.

### Bước 3: Mở dự án bằng Visual Studio

1. Mở Visual Studio 2022.
2. Chọn **File → Open → Project/Solution**.
3. Tìm đến thư mục dự án và mở file:
   ```
   ChuDe_013_QuanLyDonHang_ThuongMaiDienTu_Nhom3.vcxproj
   ```

### Bước 4: Cấu hình Build

1. Trên thanh toolbar, chọn cấu hình build:
   - **Debug** hoặc **Release**
   - **x64** (khuyến nghị) hoặc **x86**
2. Đảm bảo thư mục `data/` tồn tại trong thư mục gốc của dự án (đã có sẵn trong repo).

### Bước 5: Build và chạy

- Nhấn **F5** để build và chạy có debug.
- Hoặc nhấn **Ctrl + F5** để chạy không có debug.
- Hoặc vào menu **Build → Build Solution** (Ctrl+Shift+B), sau đó **Debug → Start Without Debugging**.

> **Quan trọng:** Khi chạy, thư mục làm việc (Working Directory) phải là thư mục gốc của dự án (chứa thư mục `data/`). Visual Studio mặc định đã cấu hình đúng điều này.

---

## Cấu trúc dự án

```
dsa-project/
├── include/
│   ├── order_manager.h          # Header chính: định nghĩa struct, enum, prototype hàm
│   └── station_simulator.h      # Định nghĩa trạm đóng gói
├── src/
│   ├── common/
│   │   ├── file_handler.cpp     # Đọc/ghi dữ liệu từ file (inventory, khách hàng, đơn hàng)
│   │   └── console_utils.cpp    # Tiện ích hiển thị console, màu sắc
│   ├── core/
│   │   ├── main.cpp             # Điểm khởi chạy, vòng lặp chính
│   │   ├── app_utils.cpp        # Menu tìm kiếm và sắp xếp
│   │   ├── benchmark/
│   │   │   └── queue_benchmark.cpp    # Benchmark hiệu năng hàng đợi
│   │   ├── reports/
│   │   │   └── statistics_reports.cpp # Báo cáo thống kê doanh thu
│   │   └── system/
│   │       └── system_utilities.cpp   # Sao lưu và khôi phục dữ liệu
│   ├── customers/
│   │   ├── customer_linked_list.cpp   # Thao tác danh sách liên kết khách hàng
│   │   └── customer_manager.cpp       # Quản lý vòng đời khách hàng, phân hạng
│   ├── inventory/
│   │   └── inventory_manager.cpp      # CRUD sản phẩm kho hàng
│   ├── orders/
│   │   ├── order_queue.cpp            # Hàng đợi ưu tiên đơn hàng
│   │   ├── order_manager.cpp          # Tạo đơn, xử lý nền đa luồng
│   │   └── station_simulator.cpp      # Mô phỏng trạm đóng gói song song
│   ├── search/
│   │   └── search_sort.cpp            # Tìm kiếm và sắp xếp (Interchange Sort)
│   └── ui/
│       ├── menu.cpp                   # Hiển thị menu
│       └── ui_header.cpp              # Header bảng dữ liệu
├── data/
│   ├── customers.txt            # CSDL khách hàng
│   ├── inventory.txt            # CSDL kho hàng
│   ├── orders.txt               # Hàng đợi và lịch sử đơn hàng
│   ├── order_revenue_log.txt    # Nhật ký doanh thu theo ngày
│   └── order_scenario.txt       # Dữ liệu kịch bản test
└── ChuDe_013_QuanLyDonHang_ThuongMaiDienTu_Nhom3.vcxproj
```

---

## Chức năng chính

### Module 1 - Quản lý kho hàng
| Chức năng | Mô tả |
|---|---|
| Thêm sản phẩm | Tạo sản phẩm mới với ID tự động, kiểm tra trùng tên |
| Xem danh sách | Hiển thị toàn bộ sản phẩm dạng bảng |
| Cập nhật giá | Thay đổi đơn giá sản phẩm |
| Nhập thêm hàng | Tăng số lượng tồn kho (tối đa 1000 đơn vị/sản phẩm) |
| Xóa sản phẩm | Xóa sản phẩm khỏi kho |

### Module 2 - Quản lý khách hàng
| Chức năng | Mô tả |
|---|---|
| Đăng ký khách hàng | Thêm khách hàng mới với kiểm tra trùng lặp (tên + SĐT) |
| Xem danh sách | Hiển thị toàn bộ khách hàng |
| Xem lịch sử mua hàng | Tra cứu đơn hàng của khách hàng |
| Điều chỉnh hạng | Nâng/hạ hạng thành viên thủ công |
| Nâng hạng tự động | Tự động nâng hạng theo tổng chi tiêu |
| Xóa khách hàng | Xóa khách hàng khỏi hệ thống |

**Quy tắc phân hạng:**
- **Thường:** Mặc định (≥ 0 VNĐ)
- **VIP:** Tổng chi tiêu ≥ 3.000.000 VNĐ
- **Express:** Tổng chi tiêu ≥ 10.000.000 VNĐ

### Module 3 - Quản lý đơn hàng
| Chức năng | Mô tả |
|---|---|
| Tạo đơn hàng | Chọn khách hàng, sản phẩm, phương thức vận chuyển |
| Xem hàng đợi | Hiển thị đơn hàng đang chờ xử lý |
| Xử lý nền | Tự động xử lý đơn theo 3 giai đoạn song song |
| Hủy đơn hàng | Hủy đơn hàng trước khi được xử lý |

**Mức độ ưu tiên:**
- `Express (1)` > `VIP (2)` > `Thường (3)`

**3 giai đoạn xử lý:**
1. Đang xác nhận
2. Đang đóng gói (4 trạm song song)
3. Đang vận chuyển

### Module 4 - Tìm kiếm & Sắp xếp
- Tìm đơn hàng theo ID
- Tìm khách hàng theo tên
- Sắp xếp sản phẩm theo lượt bán (Interchange Sort)
- Sắp xếp khách hàng theo tổng chi tiêu

### Module 5 - Thống kê & Báo cáo
- Doanh thu theo ngày/tháng
- Tỷ lệ đơn Express vs. Thường
- Sản phẩm hết hàng nhu cầu cao
- Hiệu suất xử lý đơn hàng
- Benchmark hàng đợi (1 triệu thao tác)
- Sao lưu và khôi phục dữ liệu

---

## Cấu trúc dữ liệu

### Sản phẩm (`Product`)
```c
typedef struct Product {
    int id;                  // Mã sản phẩm (tự động tăng từ 101)
    char name[100];          // Tên sản phẩm
    int stock_quantity;      // Số lượng tồn kho
    long long price;         // Đơn giá (VNĐ)
    int sold_quantity;       // Số lượng đã bán
} Product;
```

### Đơn hàng (`Order`) - Node trong hàng đợi liên kết
```c
typedef struct Order {
    int id;
    char customer_name[100];
    char product_name[100];
    int quantity;
    long long price;
    PriorityLevel priority;       // 1=Express, 2=VIP, 3=Thường
    ShippingMethod shipping_method; // 0=Standard, 1=Express
    char status[100];
} Order;
```

### Khách hàng (`Customer`) - Node trong danh sách liên kết
```c
typedef struct Customer {
    char id[37];             // ID dạng chuỗi số
    char name[100];
    char phone[100];         // SĐT 10 chữ số
    CustomerTier tier;       // 0=Thường, 1=VIP, 2=Express
    char status[100];        // "HoatDong" hoặc "Khoa"
    OrderQueue history;      // Lịch sử đơn hàng (hàng đợi)
    long long total_spent;   // Tổng tiền đã chi tiêu
} Customer;
```

---

## File dữ liệu

Tất cả dữ liệu được lưu dưới dạng CSV trong thư mục `data/`:

### `data/inventory.txt`
```
<số lượng sản phẩm>
<id>,<tên>,<tồn kho>,<giá>,<đã bán>
```

### `data/customers.txt`
```
<số lượng khách hàng>
<id>,<tên>,<SĐT>,<hạng>,<trạng thái>,<tổng chi tiêu>
```

### `data/orders.txt`
```
<số lượng đơn hàng>
<id>,<tên KH>,<tên SP>,<số lượng>,<giá>,<ưu tiên>,<vận chuyển>,<trạng thái>
```

> **Lưu ý:** Không chỉnh sửa thủ công các file dữ liệu khi ứng dụng đang chạy để tránh mất dữ liệu.

---

## Thành viên nhóm

| Thành viên | Chức năng phụ trách |
|---|---|
| Nhóm 3 | Chủ đề 13 - Quản lý Đơn hàng TMĐT |
