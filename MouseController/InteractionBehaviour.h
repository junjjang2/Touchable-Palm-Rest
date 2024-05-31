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
    static void middleClick();
    static void startWheel();
    static void pageForward();
    static void switchTab();
};
