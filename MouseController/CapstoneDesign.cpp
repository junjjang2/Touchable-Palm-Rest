#include <Windows.h>
#include <iostream>
#include <thread>
#include <sstream>

#include <vector>
#include <string>
#include <chrono>
#include <map>


#include "SerialClass.h"


HHOOK mouseHook;
HHOOK keyboardHook;

bool isMouseHookActive = false;
bool ctrlPressed = false;        // State of the Ctrl key


static auto last_time = std::chrono::high_resolution_clock::now();
static int last_sensor = 0;
static int click_count = 0;
static std::map<int, std::chrono::time_point<std::chrono::high_resolution_clock>> press_time;
static std::map<int, bool> long_press_detected;

// Keyboard hook procedure

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if (p->vkCode == 'T') {
            if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
                // Toggle the state of the mouse hook
                isMouseHookActive = !isMouseHookActive;
				std::cout << "Mouse Hook " << (isMouseHookActive ? "Activated" : "Deactivated") << std::endl;
            }
        }
        if (p->vkCode==VK_ESCAPE){
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                UnhookWindowsHookEx(mouseHook);  // Clean up the hook
                UnhookWindowsHookEx(keyboardHook);  // Clean up the hook

                PostQuitMessage(0);  // Post a quit message to end the application
                return 1;  // Block further processing of this key press
            }
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void SetKeyboardHook() {
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!keyboardHook) {
        // Error handling
		std::cout << "Error : Keyboard Hook Not Valid" << std::endl;
    }
}

// Mouse hook procedure

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    auto now = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = now - last_time;

    if (nCode >= 0) {
        MSLLHOOKSTRUCT *pMouseStruct = (MSLLHOOKSTRUCT *)lParam;
        switch (wParam) {
            case WM_RBUTTONDOWN:
                // Right mouse button is pressed
                isMouseHookActive = true;
                break;
            case WM_RBUTTONUP:
                // Right mouse button is released
                isMouseHookActive = false;
                break;
            case WM_MOUSEMOVE:
                // Handle mouse movement
                if (!isMouseHookActive) {
					//std::cout << "Mouse Movement Blocked" << std::endl;
                    return 1; // Block mouse movement
                }
                break;
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
                // Handle left mouse clicks
                if (!isMouseHookActive) {
                    return 1; // Block left clicks
                }
                break;
        }
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

void SetMouseHook() {
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    if (!mouseHook) {
        // Error handling
		std::cout << "Error : Mouse Hook Not Valid" << std::endl;

    }
}

// 마우스 좌클릭
void singleClick() {
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

// 마우스 더블 클릭
void doubleClick() {
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
}

// 마우스 우클릭
void rightClick() {
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

// 화면 중앙으로 커서 이동
void moveToCenter() {
    SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);
}

// 휠 클릭 이동
void middleClick() {
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

// 페이지 앞으로 이동
void pageForward() {
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_NEXT;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_NEXT;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

// 마우스 좌우 이동 -> 탭 전환
void switchTab() {
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_TAB;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_TAB;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

// 센서 입력 처리 함수
void handleSensorInput(int sensor, char action, double duration, int clickCount) {
    if (sensor == 1) {
        if (action == 't') {
            if (clickCount == 2 && duration < 0.3) {
                doubleClick();
            }
            else if (clickCount == 1 && duration >= 0.3) {
                // 마우스 이동
            }
            else if (clickCount == 1) {
                singleClick();
            }
        }
    }
    else if (sensor == 2) {
        if (action == 't') {
            if (clickCount == 2 && duration < 0.3) {
                moveToCenter();
            }
            else if (clickCount == 1) {
                rightClick();
            }
        }
    }
    else if (sensor == 3) {
        if (action == 't') {
            middleClick();
        }
    }
}




// 입력 데이터를 처리하는 함수
void processInput(int sensor, char action) {
    auto now = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = now - last_time;

    if (action == 't') {
        press_time[sensor] = now;
        long_press_detected[sensor] = false;
        click_count++;
        last_sensor = sensor;
        last_time = now;

        if (sensor == 1) {
            //long_press_detected[sensor] = true;
            isMouseHookActive = true;
        }
    }
    else if (action == 'r') {
        auto press_duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - press_time[sensor]).count();
        if (press_duration >= 300) {
            //long_press_detected[sensor] = true;

            // 길게 터치 처리
            if (sensor == 1) {
                //long_press_detected[sensor] = true;
                isMouseHookActive = false;
            }
            else if (sensor == 2) {
                // 드래그 등
            }
            else if (sensor == 3) {
                // 휠 클릭 이동
                middleClick();
            }
        }
        else {
            handleSensorInput(sensor, 't', diff.count(), click_count);
        }
        click_count = 0;
    }

    // 추가 기능 구현
    if (sensor == 1 && action == 't') {
        if (diff.count() > 0.3) {
            //long_press_detected[0] = true;
        }
        last_sensor = 1;
    }
    else if (sensor == 2 && action == 't' && last_sensor == 1) {
        last_sensor = 2;
    }
    else if (sensor == 3 && action == 't' && last_sensor == 2) {
        pageForward();
        last_sensor = 0;
    }

    if (long_press_detected[0] || long_press_detected[1] || long_press_detected[2]) {
        //isMouseHookActive = true;
        //POINT cursorPos;
        //GetCursorPos(&cursorPos);
        //static int last_x = cursorPos.x;
        //if (abs(cursorPos.x - last_x) > 100) { // 100은 예시 값, 필요에 따라 조정
        //    switchTab();
        //    last_x = cursorPos.x;
        //}
    }
    else {
        //isMouseHookActive = false;
    }
}


// 문자열을 ',' 구분자로 분할하는 함수
std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

void f1() {
    Serial* SP = new Serial("\\\\.\\COM3");

    if (SP->IsConnected()) {
        std::cout << "Connected" << std::endl;
    }

    char buffer[256] = "";
    int dataLength = 255;
    int readResult = 0;
    std::string line;

    while (SP->IsConnected()) {
        int bytesRead = SP->ReadData(buffer, sizeof(buffer) - 1);

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0'; // 문자열 종료 문자 추가
            line += buffer;

            size_t pos; 
            while ((pos = line.find('\n')) != std::string::npos) {
                std::string completeLine = line.substr(0, pos);
                std::cout << "Received: " << completeLine << std::endl;

                std::vector<std::string> result = splitString(completeLine, ',');
                if (result.size() == 2)
                {
                    processInput(stoi(result[0]), result[1][0]);
                }
                line.erase(0, pos + 1);

            }
        }

    }
}

void setAlwaysOnTop(HWND hWnd) {
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

int main() {
     std::cout << "T를 눌러 마우스를 활성화/비활성화 합니다" << std::endl;

    // 윈도우 핸들을 가져옴
    HWND hWnd = GetConsoleWindow();
    // 프로그램을 최상위로 유지
    setAlwaysOnTop(hWnd);

    SetMouseHook();
    SetKeyboardHook();

    std::thread t1(f1);
    t1.detach();

    
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)) {

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    //// Message loop to keep the hook active
    //while (GetMessage(&msg, NULL, 0, 0)) {
    //    TranslateMessage(&msg);
    //    DispatchMessage(&msg);
    //}

    UnhookWindowsHookEx(mouseHook);  // Clean up the hook
    UnhookWindowsHookEx(keyboardHook);  // Clean up the hook
    return 0;
}