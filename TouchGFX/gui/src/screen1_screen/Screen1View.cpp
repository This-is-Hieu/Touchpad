#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/Application.hpp>
#include <cmath>
#include "main.h"
#include "usbd_hid.h"
#include "stm32f4xx_hal.h"
#include <touchgfx/widgets/canvas/Circle.hpp>
#include "stm32f4xx_hal_rng.h"   // Dùng RNG hardware
#include <touchgfx/widgets/canvas/AbstractPainterARGB8888.hpp>

extern RNG_HandleTypeDef hrng;
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

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
    circle1.setVisible(false);

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
        randomizeColor();
        circle1.setRadius(currentRadius);
        circle1.setLineWidth(5);
        circle1.setVisible(true);
        circle1.invalidate();
        shrinkStartTick = HAL_GetTick();
        shrinking = true;
    }
    else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
    {
		sendMouseClick(true);
		HAL_Delay(20);
		sendMouseClick(false);
    }
}

int16_t deltaX = 0;
int16_t deltaY = 0;
void Screen1View::handleDragEvent(const touchgfx::DragEvent &evt)
{

	const int DRAG_THRESHOLD = 4;
    deltaX = evt.getDeltaX();
    deltaY = evt.getDeltaY();
    shrinkStartTick = HAL_GetTick();
    if (abs(deltaX) < DRAG_THRESHOLD && abs(deltaY) < DRAG_THRESHOLD)
        return;
    circle1.setCenter(evt.getNewX(), evt.getNewY());
    currentRadius = 35;
    randomizeColor();
    circle1.setRadius(currentRadius);
    circle1.setLineWidth(5);
    circle1.setVisible(true);
    circle1.invalidate();

    shrinking = true;  // Bắt đầu thu nhỏ trong tick

}


void Screen1View::handleTickEvent()
{
	if (deltaX != 0 || deltaY != 0)
	{
	        sendMousePosition(deltaX, deltaY);
	        deltaX = 0;
	        deltaY = 0;
	}
    if (shrinking)
    {
        uint32_t elapsed = HAL_GetTick() - shrinkStartTick;
        const uint32_t DURATION = 500;

        if (elapsed < DURATION)
        {
            float progress = (float)elapsed / DURATION;  //  0.0 -> 1.0
            currentRadius = 35 - static_cast<int>(30 * progress); // 35 → 5
            circle1.setRadius(currentRadius);
            invalidate();
        }
        else
        {
            // Kết thúc thu nhỏ
            shrinking = false;
            circle1.setVisible(false);
            invalidate();
        }
    }
}
void Screen1View::randomizeColor()
{
    uint32_t rnd;
    HAL_StatusTypeDef status = HAL_RNG_GenerateRandomNumber(&hrng, &rnd);

    if (status == HAL_OK)
    {
        uint8_t r = (rnd >> 16) & 0xFF;
        uint8_t g = (rnd >> 8)  & 0xFF;
        uint8_t b = rnd & 0xFF;

        currentColor = touchgfx::Color::getColorFromRGB(r, g, b);
    }
    else
    {
        currentColor = touchgfx::Color::getColorFromRGB(255, 255, 255);
    }

    circlePainter.setColor(currentColor);
    circle1.setPainter(circlePainter);
}
void Screen1View::leftClick()
{
    sendMouseClick(1);       // Gửi tín hiệu nhấn chuột trái
    HAL_Delay(10);
    sendMouseClick(0);       // Gửi tín hiệu thả chuột
}
void Screen1View::rightClick()
{
    sendMouseClick(2);       // Gửi tín hiệu nhấn chuột phải
    HAL_Delay(10);
    sendMouseClick(0);       // Gửi tín hiệu thả chuột
}
void Screen1View::sendMousePosition(int16_t deltaX, int16_t deltaY)
{
    const int DEADZONE = 1;


    if (abs(deltaX) < DEADZONE) deltaX = 0;
    if (abs(deltaY) < DEADZONE) deltaY = 0;


    if (deltaX == 0 && deltaY == 0) return;


    int16_t adjustedX = deltaY;
    int16_t adjustedY = -deltaX;

    // Apply exponential smoothing để giảm jitter
            const float smoothingFactor = 0.3f; // 0.0 = no smoothing, 1.0 = maximum smoothing
            smoothedDeltaX = smoothedDeltaX * smoothingFactor + adjustedX * (1.0f - smoothingFactor);
            smoothedDeltaY = smoothedDeltaY * smoothingFactor + adjustedY * (1.0f - smoothingFactor);

            smoothedDeltaX *= 1;
            smoothedDeltaY *= 1;


    if (smoothedDeltaX > 127) smoothedDeltaX = 127;
    if (smoothedDeltaX < -128) smoothedDeltaX = -128;
    if (smoothedDeltaY > 127) smoothedDeltaY = 127;
    if (smoothedDeltaY < -128) smoothedDeltaY = -128;

    mouseHID mouseReport = {0};
    mouseReport.mouse_x = (int8_t)smoothedDeltaX;
    mouseReport.mouse_y = (int8_t)smoothedDeltaY;
    mouseReport.button = 0;
    mouseReport.wheel = 0;

    USBD_HID_SendReport(&hUsbDeviceHS, (uint8_t *)&mouseReport, sizeof(mouseReport));
}

void Screen1View::sendMouseClick(int8_t click)
{
    mouseHID mouseReport = {0};

    mouseReport.button = click;
    mouseReport.mouse_x = 0;
    mouseReport.mouse_y = 0;
    mouseReport.wheel = 0;

    USBD_HID_SendReport(&hUsbDeviceHS, (uint8_t *)&mouseReport, sizeof(mouseReport));
}

