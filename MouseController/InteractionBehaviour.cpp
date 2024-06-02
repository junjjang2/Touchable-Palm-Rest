#include "InteractionBehaviour.h"


// 마우스 좌클릭
void InteractionBehaviour::singleClick() {
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    std::cout << "Single Click executed" << std::endl;
}

// 마우스 더블 클릭
void InteractionBehaviour::doubleClick() {
    INPUT inputs[4] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    inputs[2].type = INPUT_MOUSE;
    inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    inputs[3].type = INPUT_MOUSE;
    inputs[3].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    std::cout << "Double Click executed" << std::endl;
}

// 마우스 우클릭
void InteractionBehaviour::rightClick() {
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    std::cout << "Right Click executed" << std::endl;
}

// 화면 중앙으로 커서 이동
void InteractionBehaviour::moveToCenter() {
    SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);
    std::cout << "Move to Center executed" << std::endl;
}

// 드래그 시작
void InteractionBehaviour::startDrag()
{
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    std::cout << "Start Drag executed" << std::endl;
}

void InteractionBehaviour::endDrag() 
{
    INPUT inputs[2] = {};
	ZeroMemory(inputs, sizeof(inputs));

	inputs[0].type = INPUT_MOUSE;
	inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
	std::cout << "End Drag executed" << std::endl;
}

// 휠 클릭 이동
void InteractionBehaviour::middleClick() {
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    std::cout << "Middle Click executed" << std::endl;
}

// 휠 드래그 시작
void InteractionBehaviour::startWheel() {
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    std::cout << "Start Wheel Drag executed" << std::endl;
}

void InteractionBehaviour::endWheel() {
	INPUT inputs[1] = {};
	ZeroMemory(inputs, sizeof(inputs));

	inputs[0].type = INPUT_MOUSE;
	inputs[0].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;

	SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
	std::cout << "End Wheel Drag executed" << std::endl;
}

// 페이지 앞으로 이동
void InteractionBehaviour::pageForward() {
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_NEXT;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_NEXT;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    std::cout << "Page Forward executed" << std::endl;

}

// 마우스 좌우 이동 -> 탭 전환
void InteractionBehaviour::switchTab() {
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_TAB;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_TAB;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    std::cout << "Switch Tab executed" << std::endl;
}

void InteractionBehaviour::toggleMouseLock()
{
    //isMouseLockActive = !isMouseLockActive;
}


// 스크롤 업 함수
void InteractionBehaviour::scrollUp() {
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_WHEEL;
    inputs[0].mi.mouseData = WHEEL_DELTA;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    std::cout << "Scroll Up executed" << std::endl;
}

// 스크롤 다운 함수
void InteractionBehaviour::scrollDown() {
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_WHEEL;
    inputs[0].mi.mouseData = -WHEEL_DELTA;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    std::cout << "Scroll Down executed" << std::endl;
}