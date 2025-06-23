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

// Struct để lưu thông tin touch và animation
struct TouchEffect
{
    int16_t x, y;
    unsigned short radius;
    unsigned char alpha;
    bool active;
    unsigned short startRadius;
    unsigned long startTime;
};

// Struct cho swipe gesture detection
struct SwipeData
{
    int16_t startX, startY;
    int16_t endX, endY;
    unsigned long startTime;
    unsigned long endTime;
    bool active;
    bool gestureDetected;
};

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void leftMouse();
    virtual void moveUp();
    virtual void moveDown();
    virtual void moveRight();
    virtual void moveLeft(); // Touch handling functions (gesture events removed)
    virtual void handleClickEvent(const touchgfx::ClickEvent &evt);
    virtual void handleDragEvent(const touchgfx::DragEvent &evt);
    virtual void handleTickEvent(); // Drawing override
    virtual void draw(touchgfx::Rect &invalidatedArea);

    void addTouchEffect(int16_t x, int16_t y);
    void updateTouchEffects();
    void drawTouchEffects();
    void drawSimpleTouchEffect(uint8_t index, const touchgfx::Rect &invalidatedArea);

    void sendMousePosition(int16_t deltaX, int16_t deltaY);
    void sendMouseClick(bool leftClick);
    void calculateMouseMovementFromCenter(int16_t touchX, int16_t touchY, int16_t &deltaX, int16_t &deltaY);

protected:
    int16_t touchStartX = -1;
    int16_t touchStartY = -1;
    int16_t lastDragX = -1;
    int16_t lastDragY = -1;
    bool dragging = false;
    float accumulatedDeltaX = 0.0f;
    float accumulatedDeltaY = 0.0f;
    int16_t lastMovementX = 0;
    int16_t lastMovementY = 0;

    // Smoothing for stable movement
    float smoothedDeltaX = 0.0f;
    float smoothedDeltaY = 0.0f; // Swipe gesture detection
    SwipeData swipeData;

    // Animation parameters
    static const std::uint16_t EFFECT_DURATION_MS = 1000;
    static const std::uint16_t MAX_CIRCLE_RADIUS = 50;
    static const std::uint8_t MAX_TOUCH_EFFECTS = 5;

    // Touch effects
    TouchEffect touchEffects[MAX_TOUCH_EFFECTS];

    // Screen dimensions for mouse mapping
    static const std::uint16_t SCREEN_WIDTH = 240;
    static const std::uint16_t SCREEN_HEIGHT = 320;
    static const std::uint16_t SCREEN_CENTER_X = SCREEN_WIDTH / 2;  // 120
    static const std::uint16_t SCREEN_CENTER_Y = SCREEN_HEIGHT / 2; // 160    // Mouse sensitivity settings - tối ưu cho touchpad
    static constexpr float TOUCHPAD_SENSITIVITY = 3.5f;             // Tăng sensitivity để responsive hơn
    static const std::uint8_t DEAD_ZONE_RADIUS = 5;                 // Dead zone for small movements    // Circle animation variables (giữ nguyên cho circle effects)
    int currentRadius = 35;
    unsigned long shrinkStartTick = 0;
    bool shrinking = false;

    // Mouse movement optimization variables
    float accumulatedX = 0.0f;
    float accumulatedY = 0.0f;
    int16_t lastDeltaX = 0;
    int16_t lastDeltaY = 0;
};

#endif // SCREEN1VIEW_HPP
