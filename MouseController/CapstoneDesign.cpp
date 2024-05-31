#include <Windows.h>
#include <iostream>
#include <thread>
#include <sstream>

#include <vector>
#include <string>
#include <chrono>
#include <map>

#include "InteractionBehaviour.h"
#include "SerialClass.h"


HHOOK mouseHook;
HHOOK keyboardHook;

bool isMouseLockActive = true;
bool ctrlPressed = false;        // State of the Ctrl key


static auto last_time = std::chrono::high_resolution_clock::now();
static int last_sensor = 0;
static int click_count = 0;
static std::map<int, std::chrono::time_point<std::chrono::high_resolution_clock>> press_time;

// 벡터를 사용하여 센서 상태를 관리
std::vector<std::atomic<bool>> sensorsPressed(4); // 인덱스 0은 사용하지 않음

std::vector<int> sensorSequence;  // 센서 입력 순서 추적
const double maxInterval = 1.5; // 센서 입력 간의 최대 허용 시간 간격 (초)

// Keyboard hook procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if (p->vkCode == 'T') {
            if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
                // Toggle the state of the mouse hook
                isMouseLockActive = !isMouseLockActive;
				std::cout << "Mouse Hook " << (isMouseLockActive ? "Activated" : "Deactivated") << std::endl;
            }
        }
        if (p->vkCode==VK_ESCAPE){
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                // UnhookWindowsHookEx(mouseHook);  // Clean up the hook
                // UnhookWindowsHookEx(keyboardHook);  // Clean up the hook
                //
                // PostQuitMessage(0);  // Post a quit message to end the application
                
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
                break;
            case WM_RBUTTONUP:
                break;
            case WM_MOUSEMOVE:
                // Handle mouse movement
                if (isMouseLockActive) {
					//std::cout << "Mouse Movement Blocked" << std::endl;
                    return 1; // Block mouse movement
                }
                break;
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
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

// 센서 입력 처리 함수
void handleSensorInput(int sensor, char action, double duration, int clickCount) {
    if (sensor == 1) {
        if (action == 't') {
            //isMouseLockActive = false;
            if (clickCount == 2 && duration < 0.3) {
                InteractionBehaviour::doubleClick();
            }
        }
        else if (action == 'r') {
            if (clickCount == 1) {
                InteractionBehaviour::singleClick();
            }
        }
    }
    else if (sensor == 2) {
        if (action == 't') {
            if (clickCount == 2 && duration < 0.3) {
                InteractionBehaviour::moveToCenter();
            }
        }
        else if (action == 'r'){
            if (clickCount == 1) {
                InteractionBehaviour::rightClick();
            }
        }
    }
    else if (sensor == 3) {
        if (action == 'r') {
            if(clickCount == 1){
                InteractionBehaviour::middleClick();
            }
        }
    }
        
}

// 센서 꾹 누르기를 감지하는 함수
void detectLongPress(int sensor) {
    while (true) {
        if (sensorsPressed[sensor]) {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            if (sensorsPressed[sensor]) {
                std::cout << "Long Press detected for sensor " << sensor << std::endl;
                // Implement sensor-specific functionality
                switch (sensor)
                {
                case 1:
                    if (click_count == 2) {
                        InteractionBehaviour::startDrag();
                    }
                    isMouseLockActive = false;
                    break;
                case 2:
                    InteractionBehaviour::startDrag();
                    isMouseLockActive = false;
                    break;
                case 3:
                    InteractionBehaviour::startWheel();
                    isMouseLockActive = false;
                    break;
                default:
                    std::cout << "Invalid Sensor" << sensor << "\n";
                }
                while (sensorsPressed[sensor]) {
                    // 지속적으로 동작을 수행
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                switch (sensor)
                {
                case 1:
                    if (click_count == 2) {
						InteractionBehaviour::endDrag();
					}
                    isMouseLockActive = true;
                    break;
                case 2:
                    InteractionBehaviour::startDrag();
                    isMouseLockActive = true;
                    break;
                case 3:
                    InteractionBehaviour::startWheel();
                    isMouseLockActive = true;
                    break;
                default:
                    std::cout << "Invalid Sensor" << sensor << "\n";
                }
                click_count = 0;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// 입력 데이터를 처리하는 함수
void processInput(int sensor, char action) {
    auto now = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = now - last_time;
    
    if (action == 't') {
        if(diff.count() > 0.3)
            click_count = 0;
        
        press_time[sensor] = now;
        click_count++;
        last_sensor = sensor;
        last_time = now;
        sensorsPressed[sensor] = true;

        std::cout << diff.count() << " " << click_count<< std::endl;
        handleSensorInput(sensor, 't', diff.count(), click_count);
    }
    else if (action == 'r') {
        sensorsPressed[sensor] = false;
        std::cout << diff.count() << " " << click_count << std::endl;

        handleSensorInput(sensor, 'r', diff.count(), click_count);
    }
    
    // 1, 2, 3 순서로 센서가 터치되었는지 확인
    if (action == 't' && (sensor == 1 || sensor == 2 || sensor == 3)) {
        if (!sensorSequence.empty() && std::chrono::duration<double>(now - press_time[sensorSequence.back()]).count() > maxInterval) {
            sensorSequence.clear(); // 간격이 너무 길면 순서 초기화
        } else {
            if (sensorSequence.empty() || sensorSequence.back() != sensor) {
                sensorSequence.push_back(sensor);
            }
        }
        
        if (sensorSequence == std::vector<int>{1, 2, 3}) {
            InteractionBehaviour::pageForward();
            sensorSequence.clear();  // 순서 초기화
        }
        else if (sensorSequence.size() > 3) {
            sensorSequence.clear();  // 잘못된 순서가 입력되면 초기화
        }
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

// Serial 통신 함수
void f1() {
    Serial* SP = new Serial(R"(\\.\COM3)");

    if (SP->IsConnected()) {
        std::cout << "Connected" << std::endl;
    }
    else
    {
        isMouseLockActive = true;
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

// 프로그램을 최상위로 유지하는 함수
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

    // 센서 꾹 누르기 감지 스레드 생성t
    std::vector<std::thread> sensorThreads;
    for (int i = 1; i <= 3; i++) {
        sensorThreads.emplace_back(detectLongPress, i);
    }

    for (auto& thread : sensorThreads) {
        thread.detach();
    }
    
    MSG msg;
    
    // Message loop to keep the hook active
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnhookWindowsHookEx(mouseHook);  // Clean up the hook
    UnhookWindowsHookEx(keyboardHook);  // Clean up the hook
    return 0;
}