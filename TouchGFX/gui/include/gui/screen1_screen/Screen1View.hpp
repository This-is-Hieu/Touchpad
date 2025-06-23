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


    void sendMousePosition(int16_t deltaX, int16_t deltaY);
    void sendMouseClick(bool leftClick);


protected:
    float smoothedDeltaX = 0.0f;
    float smoothedDeltaY = 0.0f;
    bool shrinking = false;
    int currentRadius = 35;
    uint32_t shrinkStartTick = 0; //lưu thời điểm có hình tròn
};

#endif // SCREEN1VIEW_HPP
