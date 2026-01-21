# LAST OF EARTH - PREMIUM MOD MENU

Chào mừng bạn đến với dự án Mod Menu cao cấp dành cho iOS. Đây là hướng dẫn chi tiết về cách cài đặt, sử dụng và triển khai các tính năng trong menu.

## 🛠 1. Hướng dẫn Cài đặt (Installation)

Dự án này được xây dựng bằng **Theos**. Đảm bảo bạn đã cài đặt môi trường Theos trên macOS hoặc Linux (WSL cho Windows).

### Bước 1: Chuẩn bị Tools

- Cài đặt Theos: [Theos Installation Guide](https://theos.dev/docs/installation)
- Cài đặt SDK iOS (đề xuất iOS 14.5 trở lên).

### Bước 2: Cấu hình Makefile

Mở file `Makefile` và kiểm tra các thông số sau để phù hợp với môi trường của bạn:

```makefile
THEOS_DEVICE_IP = 192.168.1.x  # IP của iPhone đã Jailbreak
ARCHS = arm64 arm64e
```

### Bước 3: Biên dịch và Cài đặt

Chạy các lệnh sau trong terminal tại thư mục gốc của dự án:

```bash
make clean           # Xóa các file rác cũ
make package         # Đóng gói file .deb
make install         # Cài đặt trực tiếp lên iPhone (yêu cầu IP ở bước 2)
```

---

## 📱 2. Hướng dẫn Sử dụng (Usage)

1. **Mở Menu**: Sau khi vào game, đợi khoảng 5 giây (theo cấu hình `WAIT_TIME` trong `Config.h`), một nút biểu tượng (Floating Button) sẽ xuất hiện trên màn hình.
2. **Thao tác**:
   - Chạm vào nút biểu tượng để **Mở/Đóng** menu.
   - Giữ và kéo nút biểu tượng để di chuyển vị trí.
   - Menu hỗ trợ kéo (drag) để di chuyển toàn bộ giao diện bảng điều khiển.
3. **Tab**: Menu được chia làm 4 tab: **Giao chiến**, **Hình ảnh**, **Khác**, và **Cài đặt**. Chạm vào thanh tab để chuyển đổi.

---

## 💻 3. Hướng dẫn Triển khai (Implementation)

Để triển khai các tính năng hack vào game, bạn cần biết cách kiểm tra trạng thái của các thành phần UI từ file `Tweak.mm`.

### Cách lấy đối tượng Menu:

```objectivec
MenuView *menu = [UIManager shared].menu;
```

### 1. Kiểm tra Switch (Bật/Tắt)

Sử dụng tên tiêu đề (title) bạn đã đặt khi thêm switch để kiểm tra.

```objectivec
// Ví dụ: Kiểm tra trạng thái "Bất tử"
if ([menu.switches[@"Bất tử (God Mode)"] isOn]) {
    // Thực hiện logic hack ở đây
}
```

### 2. Kiểm tra Slider (Thanh trượt)

Lấy giá trị hiện tại của thanh trượt (trả về kiểu `float`).

```objectivec
// Ví dụ: Lấy tốc độ chạy
float speedValue = [menu.sliders[@"Tốc độ chạy"] value];
// Sử dụng giá trị speedValue cho nhân vật
```

### 3. Kiểm tra Combo Selector (Danh sách chọn)

Dựa vào `handler` bạn đã viết khi khởi tạo hoặc kiểm tra tiêu đề nút.

```objectivec
// Thông thường xử lý ngay trong handler khi khởi tạo:
[menu addComboSelector:@"Màu sắc ESP" options:@[@"Xanh", @"Đỏ"] selectedIndex:0 handler:^(NSInteger index) {
    if (index == 0) { /* Màu xanh */ }
    else { /* Màu đỏ */ }
}];
```

### 4. Xử lý Button (Nút bấm)

Thực hiện hành động ngay lập tức thông qua block `withHandler`.

```objectivec
[menu addButton:@"Hồi máu" withHandler:^{
    // Code hồi máu ngay lập tức
}];
```

---

## 🔍 4. Mẫu cấu trúc trong Tweak.mm

Dưới đây là cách bạn nên tổ chức code để kiểm tra trạng thái liên tục:

```objectivec
static void update_hacks() {
    MenuView *menu = [UIManager shared].menu;
    if (!menu) return;

    // --- Tab Giao chiến ---
    if ([menu.switches[@"Vô hạn đạn"] isOn]) {
        // Code set ammo = 999
    }

    // --- Tab Khác ---
    float jumpHeight = [menu.sliders[@"Độ cao nhảy"] value];
    // Code set jump_power = jumpHeight
}

// Gọi update_hacks trong một vòng lặp hoặc Hook
```

## ⚠️ Lưu ý Quan trọng

- Tên các thành phần (`@"Tiêu đề"`) trong `menu.switches[@"Tiêu đề"]` phải **trùng khớp hoàn toàn** với tên bạn đã khai báo trong hàm `SetupUI()`.
- Dự án sử dụng `os_log` để debug. Bạn có thể xem log thông qua Console.app trên macOS hoặc lệnh `socat` trên iOS.

---

_Phát triển bởi: IOS TUTORIALS_
