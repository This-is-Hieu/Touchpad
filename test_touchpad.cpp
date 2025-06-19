// Test script để verify logic tính toán touchpad
// Compile: g++ -o test_touchpad test_touchpad.cpp -lm

#include <iostream>
#include <cmath>

using namespace std;

// Constants from the actual implementation
const uint16_t SCREEN_WIDTH = 240;
const uint16_t SCREEN_HEIGHT = 320;
const uint16_t SCREEN_CENTER_X = SCREEN_WIDTH / 2;  // 120
const uint16_t SCREEN_CENTER_Y = SCREEN_HEIGHT / 2; // 160
const uint8_t MOUSE_SENSITIVITY = 8;                // Tăng từ 3 lên 8
const uint8_t DEAD_ZONE_RADIUS = 5;                 // Giảm từ 10 xuống 5

void calculateMouseMovementFromCenter(int16_t touchX, int16_t touchY, int16_t &deltaX, int16_t &deltaY)
{
    // Tính khoảng cách từ điểm touch đến tâm màn hình
    int16_t distanceFromCenterX = touchX - SCREEN_CENTER_X;
    int16_t distanceFromCenterY = touchY - SCREEN_CENTER_Y;

    // Tính khoảng cách tổng (radius)
    float distance = sqrt(distanceFromCenterX * distanceFromCenterX + distanceFromCenterY * distanceFromCenterY);

    // Nếu trong vùng chết (gần tâm) thì không di chuyển
    if (distance < DEAD_ZONE_RADIUS)
    {
        deltaX = 0;
        deltaY = 0;
        return;
    } // Tính toán movement dựa trên khoảng cách từ tâm
    // Tối ưu cho màn hình nhỏ - tăng độ nhạy
    float movementScale = (distance / (SCREEN_WIDTH / 4)) * MOUSE_SENSITIVITY; // Chia 4 thay vì 2

    // Thêm boost cho khoảng cách gần để responsive hơn
    if (distance < 30)
    {
        movementScale *= 1.5f; // Boost 50% cho touch gần tâm
    }

    // Giới hạn movement tối đa - tăng lên cho màn hình nhỏ
    if (movementScale > 20.0f)
        movementScale = 20.0f;

    // Tính delta X và Y theo hướng từ tâm đến điểm touch
    deltaX = (int16_t)(distanceFromCenterX * movementScale / distance);
    deltaY = (int16_t)(distanceFromCenterY * movementScale / distance);

    // Clamp values to int8_t range for HID
    if (deltaX > 127)
        deltaX = 127;
    if (deltaX < -128)
        deltaX = -128;
    if (deltaY > 127)
        deltaY = 127;
    if (deltaY < -128)
        deltaY = -128;
}

int main()
{
    cout << "=== TouchPad Center-Based Movement Test ===" << endl;
    cout << "Screen Size: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << endl;
    cout << "Center: (" << SCREEN_CENTER_X << ", " << SCREEN_CENTER_Y << ")" << endl;
    cout << "Dead Zone: " << (int)DEAD_ZONE_RADIUS << " pixels" << endl;
    cout << "Sensitivity: " << (int)MOUSE_SENSITIVITY << endl;
    cout << endl;

    // Test cases
    struct TestCase
    {
        int16_t touchX, touchY;
        string description;
    };

    TestCase tests[] = {
        {120, 160, "Center (should be 0,0)"},
        {125, 160, "5px right (should be small movement)"},
        {140, 160, "20px right"},
        {120, 140, "20px up"},
        {100, 180, "Left-down diagonal"},
        {200, 100, "Far right-up corner"},
        {40, 280, "Far left-down corner"},
        {0, 0, "Top-left corner"},
        {240, 320, "Bottom-right corner"}};

    cout << "Touch Position -> Mouse Delta (Distance)" << endl;
    cout << "----------------------------------------" << endl;

    for (auto &test : tests)
    {
        int16_t deltaX, deltaY;
        calculateMouseMovementFromCenter(test.touchX, test.touchY, deltaX, deltaY);

        // Calculate distance for reference
        int16_t distX = test.touchX - SCREEN_CENTER_X;
        int16_t distY = test.touchY - SCREEN_CENTER_Y;
        float distance = sqrt(distX * distX + distY * distY);

        cout << "(" << test.touchX << "," << test.touchY << ") -> ("
             << deltaX << "," << deltaY << ") [d=" << (int)distance << "] "
             << test.description << endl;
    }

    cout << endl
         << "=== Interactive Test ===" << endl;
    cout << "Enter touch coordinates (x y) or -1 -1 to exit:" << endl;

    int16_t x, y;
    while (true)
    {
        cout << "Touch (x y): ";
        cin >> x >> y;

        if (x == -1 && y == -1)
            break;

        if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        {
            cout << "Invalid coordinates! Range: 0-" << SCREEN_WIDTH - 1 << ", 0-" << SCREEN_HEIGHT - 1 << endl;
            continue;
        }

        int16_t deltaX, deltaY;
        calculateMouseMovementFromCenter(x, y, deltaX, deltaY);

        int16_t distX = x - SCREEN_CENTER_X;
        int16_t distY = y - SCREEN_CENTER_Y;
        float distance = sqrt(distX * distX + distY * distY);

        cout << "  Distance from center: " << (int)distance << " pixels" << endl;
        cout << "  Mouse movement: (" << deltaX << ", " << deltaY << ")" << endl;

        if (distance < DEAD_ZONE_RADIUS)
        {
            cout << "  Status: In dead zone - no movement" << endl;
        }
        else
        {
            string direction = "";
            if (deltaY < 0)
                direction += "UP ";
            if (deltaY > 0)
                direction += "DOWN ";
            if (deltaX < 0)
                direction += "LEFT ";
            if (deltaX > 0)
                direction += "RIGHT ";
            cout << "  Direction: " << direction << endl;
        }
        cout << endl;
    }

    return 0;
}
