#include <gui/screen1_screen/Screen1View.hpp>
#include "cmsis_os.h"
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/Application.hpp>
#include <cmath>
#include "main.h"
#include "usbd_hid.h"
#include "stm32f4xx_hal.h"

extern osMessageQueueId_t myQueue01Handle;
extern USBD_HandleTypeDef hUsbDeviceHS;

// Mouse HID report structure
typedef struct
{
    uint8_t button;
    int8_t mouse_x;
    int8_t mouse_y;
    int8_t wheel;
} mouseHID;

Screen1View::Screen1View()
{
    // Initialize touch effects
    for (uint8_t i = 0; i < MAX_TOUCH_EFFECTS; i++)
    {
        touchEffects[i].active = false;
    }
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    // Touch effects will be handled in draw function
    // No need for timer registration
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}
void Screen1View::leftMouse()
{
    uint8_t move = 'L';
    if (osMessageQueueGetCount(myQueue01Handle) < 5)
    {
        osMessageQueuePut(myQueue01Handle, &move, 0, 10);
    }
}

void Screen1View::rightMouse()
{
    uint8_t move = 'R';
    if (osMessageQueueGetCount(myQueue01Handle) < 5)
    {
        osMessageQueuePut(myQueue01Handle, &move, 0, 10);
    }
}

void Screen1View::moveRight()
{
    uint8_t move = 'D';
    if (osMessageQueueGetCount(myQueue01Handle) < 5)
    {
        osMessageQueuePut(myQueue01Handle, &move, 0, 10);
    }
}

void Screen1View::moveLeft()
{
    uint8_t move = 'A';
    if (osMessageQueueGetCount(myQueue01Handle) < 5)
    {
        osMessageQueuePut(myQueue01Handle, &move, 0, 10);
    }
}

void Screen1View::moveUp()
{
    uint8_t move = 'W';
    if (osMessageQueueGetCount(myQueue01Handle) < 5)
    {
        osMessageQueuePut(myQueue01Handle, &move, 0, 10);
    }
}

void Screen1View::moveDown()
{
    uint8_t move = 'S';
    if (osMessageQueueGetCount(myQueue01Handle) < 5)
    {
        osMessageQueuePut(myQueue01Handle, &move, 0, 10);
    }
}

// Touch event handling
void Screen1View::handleClickEvent(const touchgfx::ClickEvent &evt)
{
    if (evt.getType() == touchgfx::ClickEvent::PRESSED)
    {
        // Add touch effect at touch position
        addTouchEffect(evt.getX(), evt.getY());

        // Calculate mouse movement based on distance from center
        int16_t deltaX, deltaY;
        calculateMouseMovementFromCenter(evt.getX(), evt.getY(), deltaX, deltaY);

        // Send mouse movement if not in dead zone
        if (deltaX != 0 || deltaY != 0)
        {
            sendMousePosition(deltaX, deltaY);
        }

        // Send mouse click event
        sendMouseClick(true);

        // Store touch position
        touchStartX = evt.getX();
        touchStartY = evt.getY();
        dragging = false;
    }
    else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
    {
        // Send mouse release event
        sendMouseClick(false);

        touchStartX = -1;
        touchStartY = -1;
        dragging = false;
    }
}

void Screen1View::handleDragEvent(const touchgfx::DragEvent &evt)
{
    // Khi drag, liên tục tính toán movement từ tâm màn hình
    int16_t deltaX, deltaY;
    calculateMouseMovementFromCenter(evt.getNewX(), evt.getNewY(), deltaX, deltaY);

    // Send mouse movement nếu không trong dead zone
    if (deltaX != 0 || deltaY != 0)
    {
        sendMousePosition(deltaX, deltaY);
    }

    // Update touch position
    touchStartX = evt.getNewX();
    touchStartY = evt.getNewY();
    dragging = true;
}

void Screen1View::handleTickEvent()
{
    // Update touch effects animation
    updateTouchEffects();

    // Check if any effects are still active
    bool hasActiveEffects = false;
    for (uint8_t i = 0; i < MAX_TOUCH_EFFECTS; i++)
    {
        if (touchEffects[i].active)
        {
            hasActiveEffects = true;
            break;
        }
    }

    // Only invalidate if we have active effects to avoid unnecessary redraws
    if (hasActiveEffects)
    {
        invalidate();
    }
}

void Screen1View::addTouchEffect(int16_t x, int16_t y)
{
    // Find an inactive effect slot
    for (uint8_t i = 0; i < MAX_TOUCH_EFFECTS; i++)
    {
        if (!touchEffects[i].active)
        {
            touchEffects[i].x = x;
            touchEffects[i].y = y;
            touchEffects[i].radius = MAX_CIRCLE_RADIUS;
            touchEffects[i].startRadius = MAX_CIRCLE_RADIUS;
            touchEffects[i].alpha = 255;
            touchEffects[i].active = true;
            touchEffects[i].startTime = HAL_GetTick();
            break;
        }
    }
}

void Screen1View::updateTouchEffects()
{
    uint32_t currentTime = HAL_GetTick();

    for (uint8_t i = 0; i < MAX_TOUCH_EFFECTS; i++)
    {
        if (touchEffects[i].active)
        {
            uint32_t elapsedTime = currentTime - touchEffects[i].startTime;

            if (elapsedTime >= EFFECT_DURATION_MS)
            {
                // Effect finished
                touchEffects[i].active = false;
            }
            else
            {
                // Calculate animation progress (0.0 to 1.0)
                float progress = (float)elapsedTime / EFFECT_DURATION_MS;

                // Shrink circle to center (radius reduces to 0)
                touchEffects[i].radius = (uint16_t)(touchEffects[i].startRadius * (1.0f - progress));

                // Fade out (alpha reduces to 0)
                touchEffects[i].alpha = (uint8_t)(255 * (1.0f - progress));
            }
        }
    }
}

void Screen1View::sendMousePosition(int16_t deltaX, int16_t deltaY)
{
    mouseHID mouseReport = {0};

    // Clamp delta values to int8_t range
    if (deltaX > 127)
        deltaX = 127;
    if (deltaX < -128)
        deltaX = -128;
    if (deltaY > 127)
        deltaY = 127;
    if (deltaY < -128)
        deltaY = -128;

    mouseReport.mouse_x = (int8_t)deltaX;
    mouseReport.mouse_y = (int8_t)deltaY;
    mouseReport.button = 0;
    mouseReport.wheel = 0;

    // Send HID report
    USBD_HID_SendReport(&hUsbDeviceHS, (uint8_t *)&mouseReport, sizeof(mouseReport));
}

void Screen1View::sendMouseClick(bool leftClick)
{
    mouseHID mouseReport = {0};

    mouseReport.button = leftClick ? 1 : 0; // Left button bit
    mouseReport.mouse_x = 0;
    mouseReport.mouse_y = 0;
    mouseReport.wheel = 0;

    // Send HID report
    USBD_HID_SendReport(&hUsbDeviceHS, (uint8_t *)&mouseReport, sizeof(mouseReport));
}

// Override draw function to render touch effects
void Screen1View::draw(touchgfx::Rect &invalidatedArea)
{
    // Call parent draw first
    Screen1ViewBase::draw(invalidatedArea);

    // Draw simple touch effects (just small rectangles for now)
    for (uint8_t i = 0; i < MAX_TOUCH_EFFECTS; i++)
    {
        if (touchEffects[i].active && touchEffects[i].radius > 0)
        {
            drawSimpleTouchEffect(i, invalidatedArea);
        }
    }
}

void Screen1View::drawSimpleTouchEffect(uint8_t index, const touchgfx::Rect &invalidatedArea)
{
    if (!touchEffects[index].active)
        return;

    const TouchEffect &effect = touchEffects[index];

    // For now, just log the touch effect without actual drawing
    // Real implementation would require proper TouchGFX widgets or Canvas
    // This avoids the compilation errors with LCD API

    // In a production app, you would:
    // 1. Use TouchGFX Designer to add visual widgets
    // 2. Use Canvas widget with custom painter
    // 3. Or use built-in shapes/circles from TouchGFX

    // Placeholder - effect is tracked but not visually drawn
    (void)effect;          // Suppress unused variable warning
    (void)invalidatedArea; // Suppress unused variable warning
}

void Screen1View::calculateMouseMovementFromCenter(int16_t touchX, int16_t touchY, int16_t &deltaX, int16_t &deltaY)
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
