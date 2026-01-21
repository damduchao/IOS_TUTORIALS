# GGD PREMIUM CHEATS - UI FRAMEWORK

Chào mừng bạn đến với hướng dẫn sử dụng phiên bản mới của Mod Menu. Hệ thống đã được nâng cấp lên cơ chế **Block-based Handlers**, giúp việc lập trình tính năng trở nên đơn giản và gọn gàng hơn rất nhiều.

## 🛠 1. Hướng dẫn Cài đặt (Installation)

Dự án sử dụng **Theos** để biên dịch.

### Bước 1: Cấu hình Makefile

Kiểm tra `THEOS_DEVICE_IP` và `ARCHS` (thường là `arm64`).

### Bước 2: Biên dịch

```bash
make clean && make package install
```

_Lưu ý: Nếu gặp lỗi `___isOSVersionAtLeast`, hệ thống đã tự động chuyển sang kiểm tra phiên bản bằng `UIDevice` để fix lỗi này._

---

## 📱 2. Hướng dẫn Sử dụng (Usage)

- **Kích hoạt**: Menu tự động hiện sau 5 giây vào game.
- **Thao tác**: Chạm icon để Mở/Đóng. Kéo icon để thay đổi vị trí.
- **Tính năng**: Được chia làm 4 Tab: Giao chiến, Hình ảnh, Khác, Cài đặt.

---

## 💻 3. Cách triển khai tính năng (New Block System)

Thay vì phải tạo các hàm `handle...` rắc rối, giờ đây bạn có thể gán logic trực tiếp khi tạo UI.

### 1. Thêm Switch (Bật/Tắt)

Sử dụng block `handler:^(BOOL isOn)` để cập nhật biến ngay lập tức.

```objectivec
[menu addFeatureSwitch:@"Tăng tốc" description:@"Di chuyển nhanh hơn" handler:^(BOOL isOn) {
    Vars.Speedhack = isOn; // Gán trực tiếp vào biến lưu trữ
}];
```

### 2. Thêm Slider (Thanh trượt)

Sử dụng block `handler:^(CGFloat value)` để nhận giá trị số.

```objectivec
[menu addSlider:@"Tốc độ" max:10.0 min:1.0 value:2.0 handler:^(CGFloat value) {
    Vars.SpeedMultiplier = value;
}];
```

### 3. Thêm Button (Nút bấm)

```objectivec
[menu addButton:@"Hoàn thành Nhiệm vụ" withHandler:^{
    Vars.InstantTask = true;
}];
```

### 4. Thêm Combo Selector (Danh sách chọn)

```objectivec
[menu addComboSelector:@"Màu sắc" options:@[@"Đỏ", @"Xanh"] selectedIndex:0 handler:^(NSInteger index) {
    // index 0 = Đỏ, index 1 = Xanh
}];
```

---

## 🔍 4. Cấu trúc vòng lặp Hack (Vô cùng quan trọng)

Để các tính năng hoạt động mượt mà, game cần được cập nhật giá trị liên tục thông qua `CADisplayLink` (60 FPS).

Trong `Tweak.mm`, chúng ta đã thiết lập:

```objectivec
// Vòng lặp này gọi render_lop() 60 lần/giây
- (void)onTimer {
    render_lop(); // Logic thực thi nằm trong file GameFunction.h
}
```

**Cách viết logic trong `GameFunction.h`:**

```cpp
void apply_speedhack() {
    if (!Vars.Speedhack) return; // Nếu switch OFF thì dừng
    // ... code thực hiện tăng tốc ...
}
```

---

## ⚠️ Lưu ý kỹ thuật

- **Đồng bộ biến**: Luôn gán giá trị từ Menu vào cấu trúc `Vars` để hàm `render_lop()` có thể đọc được.
- **Tên UI**: Tên trong `menu.switches` hoặc `menu.sliders` phải khớp 100% với tên lúc khởi tạo nếu bạn muốn truy cập thủ công.
- **Màu sắc**: Có thể đổi màu chủ đạo (Accent Color) ngay trong tab Cài đặt.

---

_Phát triển bởi: Hào Đàm - Phiên bản v2.5_
