#include <gui/screen1_screen/Screen1View.hpp>
#include "cmsis_os.h"
#include "stdio.h"
extern osMessageQueueId_t myQueue01Handle;

Screen1View::Screen1View()
{
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}
void Screen1View::leftMouse() {
    uint8_t move = 'L';
    if (osMessageQueueGetCount(myQueue01Handle) < 5) {
        osMessageQueuePut(myQueue01Handle, &move, 0, 10);
    }
}

void Screen1View::rightMouse() {
    uint8_t move = 'R';
    if (osMessageQueueGetCount(myQueue01Handle) < 5) {
        osMessageQueuePut(myQueue01Handle, &move, 0, 10);
    }
}

void Screen1View::handleClickEvent(const ClickEvent& evt)
{
    if (evt.getType() == ClickEvent::PRESSED)
    {
        int x = evt.getX();
        int y = evt.getY();




    }
}

