# TouchPad Center-Based Mouse Control

## Concept mới: Touchpad theo tâm màn hình

Thay vì di chuyển chuột theo drag gesture, hệ thống này hoạt động như một touchpad thật:

### 🎯 **Cách hoạt động:**

1. **Tâm màn hình = Điểm chuẩn**
   - Tâm màn hình (120, 160) là điểm không di chuyển
   - Vùng chết bán kính 10 pixel xung quanh tâm

2. **Touch ở bất kỳ đâu**
   - Lấy tọa độ touch (x, y)
   - So sánh với tâm màn hình
   - Tính vector hướng và khoảng cách

3. **Di chuyển chuột theo hướng**
   - Hướng: Từ tâm màn hình → điểm touch
   - Tốc độ: Tỷ lệ với khoảng cách từ tâm
   - Càng xa tâm = di chuyển càng nhanh

### 📐 **Công thức tính toán:**

```cpp
// Khoảng cách từ tâm
distanceX = touchX - centerX  // touchX - 120
distanceY = touchY - centerY  // touchY - 160

// Khoảng cách tổng
distance = sqrt(distanceX² + distanceY²)

// Tốc độ di chuyển (scale)
movementScale = (distance / maxDistance) * sensitivity

// Mouse delta
deltaX = distanceX * movementScale / distance
deltaY = distanceY * movementScale / distance
```

### 🎮 **Ví dụ cụ thể:**

| Touch Position | Distance from Center | Mouse Movement |
|----------------|---------------------|----------------|
| (120, 160) | 0 | (0, 0) - Không di chuyển |
| (140, 160) | 20 | (+3, 0) - Di chuyển phải |
| (120, 140) | 20 | (0, -3) - Di chuyển lên |
| (100, 180) | 28 | (-4, +4) - Di chuyển trái-xuống |
| (200, 260) | 141 | (+10, +10) - Di chuyển nhanh phải-xuống |

### ⚙️ **Parameters có thể tuning:**

```cpp
SCREEN_CENTER_X = 120        // Tâm X
SCREEN_CENTER_Y = 160        // Tâm Y  
DEAD_ZONE_RADIUS = 10        // Vùng chết (pixel)
MOUSE_SENSITIVITY = 3        // Độ nhạy (1-10)
```

### 🎯 **Ưu điểm:**

- ✅ Điều khiển trực quan như touchpad laptop
- ✅ Không cần drag - chỉ cần touch
- ✅ Tốc độ tỷ lệ với khoảng cách  
- ✅ Vùng chết tránh rung lắc
- ✅ Có thể touch ở bất kỳ đâu trên màn hình

### 🔧 **Usage:**

1. **Touch gần tâm**: Di chuyển chậm, chính xác
2. **Touch xa tâm**: Di chuyển nhanh
3. **Touch góc màn hình**: Di chuyển theo đường chéo
4. **Touch tâm**: Không di chuyển (dead zone)

### 📱 **Visual Feedback:**

- Touch effect vẫn hiển thị tại vị trí touch
- Có thể thêm crosshair ở tâm màn hình để reference
- Có thể vẽ vector line từ tâm đến touch point

### 🛠️ **Customization Ideas:**

```cpp
// Tăng độ nhạy
MOUSE_SENSITIVITY = 5;

// Giảm vùng chết
DEAD_ZONE_RADIUS = 5;

// Thay đổi tâm (offset từ center)
SCREEN_CENTER_X = 120 + 20;  // Shift right
SCREEN_CENTER_Y = 160 - 10;  // Shift up
```

Hệ thống này tạo ra trải nghiệm tự nhiên hơn, giống như sử dụng touchpad thật!
