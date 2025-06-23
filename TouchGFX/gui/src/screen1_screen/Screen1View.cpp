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



void Screen1View::handleClickEvent(const touchgfx::ClickEvent &evt)
{
    if (evt.getType() == touchgfx::ClickEvent::PRESSED)
    {


        touchStartX = evt.getX();
        touchStartY = evt.getY();
        dragging = false;
    }
    else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
    {


//        if (!dragging)
//        {
//            sendMouseClick(true);
//            HAL_Delay(20);
//            sendMouseClick(false);
//        }

        dragging = false;
        touchStartX = -1;
        touchStartY = -1;
    }
}


void Screen1View::handleDragEvent(const touchgfx::DragEvent &evt)
{
    // Nếu đây là frame đầu tiên khi bắt đầu kéo

    int16_t deltaX = evt.getDeltaX();
    int16_t deltaY = evt.getDeltaY();


    if (deltaX != 0 || deltaY != 0)
    {
        sendMousePosition(deltaX, deltaY);
    }

    // Cập nhật lại tọa độ làm mốc cho frame tiếp theo
//    touchStartX = evt.getNewX();
//    touchStartY = evt.getNewY();


}



void Screen1View::handleTickEvent()
{

}





void Screen1View::sendMousePosition(int16_t deltaX, int16_t deltaY)
{
    const int DEADZONE = 1;


    if (abs(deltaX) < DEADZONE) deltaX = 0;
    if (abs(deltaY) < DEADZONE) deltaY = 0;

    // Nếu không di chuyển thì không gửi
    if (deltaX == 0 && deltaY == 0) return;

    // Có thể đảo trục tùy theo hướng cảm ứng
    int16_t adjustedX = deltaY;      // Di chuyển tay theo Y → chuột theo X
    int16_t adjustedY = -deltaX;     // Di chuyển tay theo X → chuột theo Y

    // Tăng độ nhạy nếu cần
    adjustedX *= 1;
    adjustedY *= 1;

    // Clamp vào vùng hợp lệ cho HID
    if (adjustedX > 127) adjustedX = 127;
    if (adjustedX < -128) adjustedX = -128;
    if (adjustedY > 127) adjustedY = 127;
    if (adjustedY < -128) adjustedY = -128;

    mouseHID mouseReport = {0};
    mouseReport.mouse_x = (int8_t)adjustedX;
    mouseReport.mouse_y = (int8_t)adjustedY;
    mouseReport.button = 0;
    mouseReport.wheel = 0;

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



