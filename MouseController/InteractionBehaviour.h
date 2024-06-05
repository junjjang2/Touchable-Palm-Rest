#pragma once
#include <Windows.h>
#include <iostream>


class InteractionBehaviour
{
public:
    static void singleClick();
    static void doubleClick();
    static void rightClick();
    static void moveToCenter();
    static void startDrag();
    static void endDrag();
    static void middleClick();
    static void startWheel();
    static void endWheel();
    static void pageForward();
    static void switchTab();
    static void toggleMouseLock();
    static void scrollUp();
    static void scrollDown();

    static float wheelSpeed;
};
