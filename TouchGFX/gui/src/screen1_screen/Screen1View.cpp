#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/Application.hpp>
#include <cmath>
#include "main.h"
#include "usbd_hid.h"
#include "stm32f4xx_hal.h"
#include <touchgfx/widgets/canvas/Circle.hpp>

#include <touchgfx/widgets/canvas/AbstractPainterARGB8888.hpp>

// float scale_x = 1080.0f / 240.0f;
// float scale_y = 1920.0f / 320.0f;
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
    //    // Initialize touch effects
    //    for (uint8_t i = 0; i < MAX_TOUCH_EFFECTS; i++)
    //    {
    //        touchEffects[i].active = false;
    //    }
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
    circle1.setVisible(false);
    // Touch effects will be handled in draw function
    // No need for timer registration
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::handleClickEvent(const touchgfx::ClickEvent &evt)
{
    if (evt.getType() == touchgfx::ClickEvent::PRESSED)
    {
        circle1.setCenter(evt.getX(), evt.getY());
        currentRadius = 35;
        circle1.setRadius(currentRadius);
        circle1.setLineWidth(5);
        circle1.setVisible(true);
        circle1.invalidate();
        shrinkStartTick = HAL_GetTick();
        shrinking = true; // Bắt đầu thu nhỏ trong tick
    }
    else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
    {

        //        if (!dragging)
        //        {
        //            sendMouseClick(true);
        //            HAL_Delay(20);
        //            sendMouseClick(false);
        //        }

        //        dragging = false;
        //        touchStartX = -1;
        //        touchStartY = -1;
    }
}

void Screen1View::handleDragEvent(const touchgfx::DragEvent &evt)
{
    int16_t deltaX = evt.getDeltaX();
    int16_t deltaY = evt.getDeltaY();

    shrinkStartTick = HAL_GetTick();
    circle1.setCenter(evt.getNewX(), evt.getNewY());
    currentRadius = 35;
    circle1.setRadius(currentRadius);
    circle1.setLineWidth(5);
    circle1.setVisible(true);
    circle1.invalidate();
    shrinking = true;

    if (deltaX != 0 || deltaY != 0)
    {
        // bool isStraightLineX = (abs(deltaX) > abs(deltaY) * 1.5f) && (deltaX * lastDeltaX >= 0);
        // bool isStraightLineY = (abs(deltaY) > abs(deltaX) * 1.5f) && (deltaY * lastDeltaY >= 0);

        float adjustedX = deltaX * TOUCHPAD_SENSITIVITY;
        float adjustedY = deltaY * TOUCHPAD_SENSITIVITY;

        float smoothFactor = 0.2f;
        ;
        smoothedDeltaX = smoothedDeltaX * smoothFactor + adjustedX * (1.0f - smoothFactor);
        smoothedDeltaY = smoothedDeltaY * smoothFactor + adjustedY * (1.0f - smoothFactor);

        sendMousePosition((int16_t)smoothedDeltaX, (int16_t)smoothedDeltaY);

        lastDeltaX = deltaX;
        lastDeltaY = deltaY;
    }
}

void Screen1View::handleTickEvent()
{
    if (shrinking)
    {
        unsigned long elapsed = HAL_GetTick() - shrinkStartTick;
        const unsigned long DURATION = 500;

        if (elapsed < DURATION)
        {
            float progress = (float)elapsed / DURATION;
            currentRadius = 35 - static_cast<int>(30 * progress);
            circle1.setRadius(currentRadius);
            invalidate();
        }
        else
        {
            shrinking = false;
            circle1.setVisible(false);
            invalidate();
        }
    }
}

void Screen1View::sendMousePosition(int16_t deltaX, int16_t deltaY)
{
    if (deltaX == 0 && deltaY == 0)
        return;

    int16_t adjustedX = deltaY;
    int16_t adjustedY = -deltaX;

    accumulatedX += adjustedX;
    accumulatedY += adjustedY;

    int8_t sendX = (int8_t)accumulatedX;
    int8_t sendY = (int8_t)accumulatedY;

    accumulatedX -= sendX;
    accumulatedY -= sendY;

    if (sendX > 127)
        sendX = 127;
    if (sendX < -128)
        sendX = -128;
    if (sendY > 127)
        sendY = 127;
    if (sendY < -128)
        sendY = -128;

    if (sendX != 0 || sendY != 0)
    {
        mouseHID mouseReport = {0};
        mouseReport.mouse_x = sendX;
        mouseReport.mouse_y = sendY;
        mouseReport.button = 0;
        mouseReport.wheel = 0;

        USBD_HID_SendReport(&hUsbDeviceHS, (uint8_t *)&mouseReport, sizeof(mouseReport));
    }
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

// Implementation of missing virtual functions
void Screen1View::leftMouse()
{
    // Left mouse click implementation
    sendMouseClick(true);
    // Small delay between press and release
    for (volatile int i = 0; i < 100000; i++)
        ; // Simple delay
    sendMouseClick(false);
}

void Screen1View::moveUp()
{
    // Move cursor up
    sendMousePosition(0, -10);
}

void Screen1View::moveDown()
{
    // Move cursor down
    sendMousePosition(0, 10);
}

void Screen1View::moveRight()
{
    // Move cursor right
    sendMousePosition(10, 0);
}

void Screen1View::moveLeft()
{
    // Move cursor left
    sendMousePosition(-10, 0);
}

void Screen1View::draw(touchgfx::Rect &invalidatedArea)
{
    // Call parent draw method
    Screen1ViewBase::draw(invalidatedArea);

    // Add custom drawing if needed
    // drawTouchEffects();
}

// Touch effect functions (placeholders for now)
void Screen1View::addTouchEffect(int16_t x, int16_t y)
{
    // Implementation for adding touch effects
    // Currently not used but required by header
}

void Screen1View::updateTouchEffects()
{
    // Implementation for updating touch effects
    // Currently not used but required by header
}

void Screen1View::drawTouchEffects()
{
    // Implementation for drawing touch effects
    // Currently not used but required by header
}

void Screen1View::drawSimpleTouchEffect(uint8_t index, const touchgfx::Rect &invalidatedArea)
{
    // Implementation for drawing simple touch effects
    // Currently not used but required by header
}

void Screen1View::calculateMouseMovementFromCenter(int16_t touchX, int16_t touchY, int16_t &deltaX, int16_t &deltaY)
{
    // Calculate movement from screen center
    deltaX = touchX - SCREEN_CENTER_X;
    deltaY = touchY - SCREEN_CENTER_Y;
}