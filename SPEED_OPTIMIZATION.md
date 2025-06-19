# Touchpad Speed Optimization for Small Screen

## Vấn đề với màn hình nhỏ

STM32F429 Discovery có màn hình **240x320 pixels** - rất nhỏ so với touchpad laptop thông thường. Điều này gây ra:

- ❌ Di chuyển chuột quá chậm
- ❌ Phải touch xa tâm mới có movement đáng kể  
- ❌ Khó điều khiển chính xác trên màn hình PC lớn

## Giải pháp tối ưu

### 📈 **Tăng độ nhạy:**

```cpp
// Trước đây
MOUSE_SENSITIVITY = 3
DEAD_ZONE_RADIUS = 10

// Sau khi tối ưu  
MOUSE_SENSITIVITY = 8    // Tăng 2.67x
DEAD_ZONE_RADIUS = 5     // Giảm 50%
```

### 🔧 **Cải thiện công thức tính toán:**

#### 1. **Tăng base sensitivity:**
```cpp
// Trước: chia SCREEN_WIDTH/2 (120)
movementScale = (distance / (SCREEN_WIDTH / 2)) * SENSITIVITY

// Sau: chia SCREEN_WIDTH/4 (60) - tăng 2x
movementScale = (distance / (SCREEN_WIDTH / 4)) * SENSITIVITY
```

#### 2. **Boost cho touch gần tâm:**
```cpp
if (distance < 30) {
    movementScale *= 1.5f; // Tăng 50% cho khoảng cách gần
}
```

#### 3. **Tăng movement tối đa:**
```cpp
// Trước: max = 10
if (movementScale > 10.0f) movementScale = 10.0f;

// Sau: max = 20 (tăng 2x)
if (movementScale > 20.0f) movementScale = 20.0f;
```

### 📊 **So sánh tốc độ:**

| Touch Position | Distance | Old Speed | New Speed | Improvement |
|----------------|----------|-----------|-----------|-------------|
| (130, 160) | 10px | 0.25 | 2.0 | **8x faster** |
| (140, 160) | 20px | 0.5 | 4.0 | **8x faster** |
| (160, 160) | 40px | 1.0 | 5.3 | **5.3x faster** |
| (180, 160) | 60px | 1.5 | 8.0 | **5.3x faster** |
| (200, 200) | 89px | 2.2 | 11.9 | **5.4x faster** |

### 🎯 **Kết quả mong đợi:**

- ✅ **Touch nhẹ gần tâm**: Di chuyển ngay lập tức (không còn quá chậm)
- ✅ **Touch xa tâm**: Di chuyển nhanh, phù hợp với PC
- ✅ **Vùng chết nhỏ hơn**: Responsive hơn
- ✅ **Boost gần tâm**: Điều khiển chính xác dễ dàng hơn

### ⚙️ **Fine-tuning thêm (nếu cần):**

```cpp
// Nếu vẫn chậm - tăng thêm
MOUSE_SENSITIVITY = 12;

// Nếu quá nhanh - giảm xuống  
MOUSE_SENSITIVITY = 6;

// Điều chỉnh boost cho touch gần
if (distance < 30) {
    movementScale *= 2.0f; // Tăng lên 2x thay vì 1.5x
}

// Điều chỉnh vùng chết
DEAD_ZONE_RADIUS = 3; // Rất nhạy
DEAD_ZONE_RADIUS = 8; // Ít nhạy hơn
```

### 🔬 **Test với script:**

```bash
# Compile và test
g++ -o test_touchpad test_touchpad.cpp -lm
./test_touchpad

# Test cases quan trọng:
Touch (125, 160): Gần tâm - nên có movement ~2-3
Touch (140, 160): 20px - nên có movement ~4-5  
Touch (180, 160): 60px - nên có movement ~8-10
```

Với những thay đổi này, touchpad sẽ responsive hơn nhiều trên màn hình nhỏ của STM32F429!
