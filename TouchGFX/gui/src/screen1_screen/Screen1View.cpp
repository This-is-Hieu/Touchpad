#include <gui/screen1_screen/Screen1View.hpp>
#include "cmsis_os.h"
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/Application.hpp>
#include <cmath>
#include <cstdint>
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

    // Initialize swipe data
    swipeData.active = false;
    swipeData.gestureDetected = false; // Initialize touch tracking variables
    touchStartX = -1;
    touchStartY = -1;
    lastDragX = -1;
    lastDragY = -1;
    dragging = false;
    accumulatedDeltaX = 0.0f;
    accumulatedDeltaY = 0.0f;
    lastMovementX = 0;
    lastMovementY = 0;
    smoothedDeltaX = 0.0f;
    smoothedDeltaY = 0.0f;
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
    // Gửi left click press
    mouseHID mouseReport = {0};
    mouseReport.button = 0x01; // Bit 0 = left button
    mouseReport.mouse_x = 0;
    mouseReport.mouse_y = 0;
    mouseReport.wheel = 0;

    if (osMessageQueueGetCount(myQueue01Handle) < 5)
    {
        osMessageQueuePut(myQueue01Handle, &mouseReport, 0, 10);
    }

    // Gửi left click release
    mouseReport.button = 0x00; // Release all buttons
    if (osMessageQueueGetCount(myQueue01Handle) < 5)
    {
        osMessageQueuePut(myQueue01Handle, &mouseReport, 0, 10);
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

// Touch event handling - Với swipe gesture detection
void Screen1View::handleClickEvent(const touchgfx::ClickEvent &evt)
{
    if (evt.getType() == touchgfx::ClickEvent::PRESSED)
    {
        // Lưu vị trí bắt đầu và reset drag tracking
        touchStartX = evt.getX();
        touchStartY = evt.getY();
        lastDragX = touchStartX;
        lastDragY = touchStartY;
        dragging = false;

        // Initialize swipe detection
        swipeData.startX = evt.getX();
        swipeData.startY = evt.getY();
        swipeData.startTime = HAL_GetTick();
        swipeData.active = true;
        swipeData.gestureDetected = false;

        // Thêm visual effect để confirm touch được detect
        addTouchEffect(evt.getX(), evt.getY());
    }
    else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
    { // End swipe detection
        if (swipeData.active)
        {
            swipeData.active = false;
            // Swipe detection disabled - only mouse movement now
        }

        // Reset touch state
        touchStartX = -1;
        touchStartY = -1;
        lastDragX = -1;
        lastDragY = -1;
        dragging = false;
        accumulatedDeltaX = 0.0f;
        accumulatedDeltaY = 0.0f;
        lastMovementX = 0;
        lastMovementY = 0;
    }
}

void Screen1View::handleDragEvent(const touchgfx::DragEvent &evt)
{
    if (touchStartX >= 0 && touchStartY >= 0)
    {
        // Tính delta movement từ vị trí drag trước đó
        int16_t deltaX = evt.getNewX() - lastDragX;
        int16_t deltaY = evt.getNewY() - lastDragY;

        // Filter out extreme jumps (hardware glitches)
        if (abs(deltaX) > 30 || abs(deltaY) > 30)
        {
            // Possible hardware glitch, ignore this event
            lastDragX = evt.getNewX();
            lastDragY = evt.getNewY();
            return;
        }

        // Apply exponential smoothing để giảm jitter
        const float smoothingFactor = 0.7f; // 0.0 = no smoothing, 1.0 = maximum smoothing
        smoothedDeltaX = smoothedDeltaX * smoothingFactor + deltaX * (1.0f - smoothingFactor);
        smoothedDeltaY = smoothedDeltaY * smoothingFactor + deltaY * (1.0f - smoothingFactor);

        // Apply sensitivity
        float finalDeltaX = smoothedDeltaX * TOUCHPAD_SENSITIVITY;
        float finalDeltaY = smoothedDeltaY * TOUCHPAD_SENSITIVITY; // Send movement only if significant enough
        if (abs(finalDeltaX) >= 0.8f || abs(finalDeltaY) >= 0.8f)
        {
            dragging = true;
            sendMousePosition((int16_t)finalDeltaX, (int16_t)finalDeltaY);
        }

        // Luôn update last drag position
        lastDragX = evt.getNewX();
        lastDragY = evt.getNewY();
    }
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
    std::uint32_t currentTime = HAL_GetTick();

    for (std::uint8_t i = 0; i < MAX_TOUCH_EFFECTS; i++)
    {
        if (touchEffects[i].active)
        {
            std::uint32_t elapsedTime = currentTime - touchEffects[i].startTime;

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
    static std::uint32_t lastSendTime = 0;
    std::uint32_t currentTime = HAL_GetTick();

    // Rate limiting: không gửi quá 100 reports/giây (10ms interval)
    if (currentTime - lastSendTime < 10)
    {
        return;
    }
    lastSendTime = currentTime;

    mouseHID mouseReport = {0};

    // Clamp delta values to reasonable range trước khi convert
    if (deltaX > 20)
        deltaX = 20;
    if (deltaX < -20)
        deltaX = -20;
    if (deltaY > 20)
        deltaY = 20;
    if (deltaY < -20)
        deltaY = -20;

    mouseReport.mouse_x = (int8_t)deltaX;
    mouseReport.mouse_y = (int8_t)deltaY;
    mouseReport.button = 0;
    mouseReport.wheel = 0;

    // Send directly via USB HID
    USBD_HID_SendReport(&hUsbDeviceHS, (uint8_t *)&mouseReport, sizeof(mouseReport));

    // Backup via message queue
    if (osMessageQueueGetCount(myQueue01Handle) < 3) // Giảm queue size
    {
        osMessageQueuePut(myQueue01Handle, &mouseReport, 0, 5);
    }
}

void Screen1View::sendMouseClick(bool leftClick)
{
    mouseHID mouseReport = {0};

    mouseReport.button = leftClick ? 1 : 0; // Left button bit
    mouseReport.mouse_x = 0;
    mouseReport.mouse_y = 0;
    mouseReport.wheel = 0; // Send HID report
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
    // 3. Or use built-in shapes/circles from TouchGFX    // Placeholder - effect is tracked but not visually drawn
    (void)effect;          // Suppress unused variable warning
    (void)invalidatedArea; // Suppress unused variable warning
}

void Screen1View::calculateMouseMovementFromCenter(int16_t touchX, int16_t touchY, int16_t &deltaX, int16_t &deltaY)
{
    int16_t distanceFromCenterX = touchX - SCREEN_CENTER_X;
    int16_t distanceFromCenterY = touchY - SCREEN_CENTER_Y;

    // Calculate distance from center
    float distance = sqrtf((float)(distanceFromCenterX * distanceFromCenterX + distanceFromCenterY * distanceFromCenterY));

    if (distance < DEAD_ZONE_RADIUS)
    {
        deltaX = 0;
        deltaY = 0;
        return;
    }

    // Apply sensitivity
    float movementScale = (distance / (SCREEN_WIDTH / 4)) * TOUCHPAD_SENSITIVITY; // Chia 4 thay vì 2

    // Apply scale and clamp
    deltaX = (int16_t)(distanceFromCenterX * movementScale);
    deltaY = (int16_t)(distanceFromCenterY * movementScale);

    // Clamp to prevent overflow
    if (deltaX > 127)
        deltaX = 127;
    if (deltaX < -128)
        deltaX = -128;
    if (deltaY > 127)
        deltaY = 127;
    if (deltaY < -128)
        deltaY = -128;
}
