# STM32F429 TouchGFX Mouse Touchpad

Dự án này tạo ra một touchpad mouse sử dụng STM32F429 Discovery Board với TouchGFX và USB HID.

## Tính năng

1. **Phát hiện Touch**: Sử dụng touch screen của STM32F429 Discovery Board
2. **Hiệu ứng Visual**: Vẽ hiệu ứng circle thu nhỏ về tâm khi touch trong 1 giây
3. **Mouse HID**: Gửi tín hiệu mouse về máy tính qua USB

## Cách hoạt động

### Touch Detection
- Touch được phát hiện qua `handleClickEvent()` và `handleDragEvent()`
- Khi touch, tạo hiệu ứng visual tại vị trí touch
- Đồng thời gửi mouse click event về máy tính

### Visual Effects
- Mỗi touch tạo ra một hiệu ứng hình vuông nhỏ màu đỏ
- Hiệu ứng fade out trong 1 giây với animation thu nhỏ
- Tối đa 5 hiệu ứng cùng lúc

### Mouse HID
- Touch = Left click
- Drag = Mouse movement (scaled down 4x để mượt hơn)
- Gửi qua USB HID với struct:
```c
typedef struct {
    uint8_t button;    // Mouse buttons (bit 0 = left, bit 1 = right)
    int8_t mouse_x;    // X movement delta
    int8_t mouse_y;    // Y movement delta  
    int8_t wheel;      // Scroll wheel
} mouseHID;
```

## Files chính đã chỉnh sửa

### TouchGFX GUI
- `TouchGFX/gui/include/gui/screen1_screen/Screen1View.hpp`
- `TouchGFX/gui/src/screen1_screen/Screen1View.cpp`

### Core System
- `Core/Src/main.c` (đã có sẵn USB HID setup)

## Cách sử dụng

1. Build project bằng STM32CubeIDE hoặc GCC
2. Flash firmware lên STM32F429 Discovery Board
3. Kết nối USB cable từ board về máy tính
4. Board sẽ xuất hiện như một USB HID mouse device
5. Touch màn hình để điều khiển con trở chuột trên máy tính

## Thông số kỹ thuật

- **Screen Resolution**: 240x320 pixels
- **Touch Effect Duration**: 1000ms (1 giây)
- **Max Touch Effects**: 5 đồng thời
- **Mouse Movement Scale**: 1/4 (để movement mượt hơn)
- **USB Interface**: HID Mouse Class

## Customization

### Thay đổi hiệu ứng touch:
```cpp
// Trong Screen1View.hpp
static const uint16_t EFFECT_DURATION_MS = 1000;  // Thời gian hiệu ứng
static const uint16_t MAX_CIRCLE_RADIUS = 50;     // Kích thước ban đầu
static const uint8_t MAX_TOUCH_EFFECTS = 5;       // Số hiệu ứng tối đa
```

### Thay đổi độ nhạy mouse:
```cpp
// Trong handleDragEvent()
sendMousePosition(deltaX / 4, deltaY / 4); // Thay đổi scale factor
```

### Thay đổi màu hiệu ứng:
```cpp
// Trong drawSimpleTouchEffect()
touchgfx::colortype color = touchgfx::Color::getColorFrom24BitRGB(255, 100, 100);
// RGB(255, 100, 100) = màu đỏ nhạt
```

## Troubleshooting

1. **Touch không hoạt động**: Kiểm tra touch controller initialization
2. **Mouse không được nhận diện**: Kiểm tra USB HID descriptor
3. **Hiệu ứng visual không hiển thị**: Kiểm tra draw function và LCD interface
4. **Movement quá nhanh/chậm**: Điều chỉnh scale factor trong handleDragEvent

## Hardware Requirements

- STM32F429 Discovery Board
- USB Cable (Micro USB)
- Máy tính Windows/Linux/Mac với USB port

## Software Requirements

- STM32CubeIDE hoặc ARM GCC Toolchain
- TouchGFX Designer (optional, để modify GUI)
- STM32CubeMX (optional, để modify hardware config)
