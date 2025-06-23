#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/Drawable.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/Color.hpp>
#include <cstdint>



class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    // Touch handling functions
    virtual void handleClickEvent(const touchgfx::ClickEvent &evt);
    virtual void handleDragEvent(const touchgfx::DragEvent &evt);
    virtual void handleTickEvent();

    // Drawing override

    // Touch effect drawing
//    void addTouchEffect(int16_t x, int16_t y);
//    void updateTouchEffects();
//    void drawTouchEffects();
//    void drawSimpleTouchEffect(uint8_t index, const touchgfx::Rect &invalidatedArea); // Mouse HID functions
    void sendMousePosition(int16_t deltaX, int16_t deltaY);
    void sendMouseClick(bool leftClick);


protected:
    int16_t touchStartX = -1;
    int16_t touchStartY = -1;
    bool dragging = false;

    // Animation parameters
    struct TouchEffect
    {
        int16_t x, y;
        uint16_t radius;
        uint8_t alpha;
        bool active;
        uint16_t startRadius;
        uint32_t startTime;
    };
    static const uint16_t EFFECT_DURATION_MS = 1000;
    static const uint16_t MAX_CIRCLE_RADIUS = 50;
    static const uint8_t MAX_TOUCH_EFFECTS = 5;

    // Touch effects
    TouchEffect touchEffects[MAX_TOUCH_EFFECTS];

    // Screen dimensions for mouse mapping
    static const uint16_t SCREEN_WIDTH = 240;
    static const uint16_t SCREEN_HEIGHT = 320;
    static const uint16_t SCREEN_CENTER_X = SCREEN_WIDTH / 2;  // 120
    static const uint16_t SCREEN_CENTER_Y = SCREEN_HEIGHT / 2; // 160    // Mouse sensitivity settings - Tăng cho màn hình nhỏ
    static const uint8_t MOUSE_SENSITIVITY = 3;                // Tăng từ 3 lên 8 cho màn hình nhỏ
    static const uint8_t DEAD_ZONE_RADIUS = 5;                 // Giảm từ 10 xuống 5 để nhạy hơn
};

#endif // SCREEN1VIEW_HPP
