#pragma once
#include <Windows.h>
#include <iostream>
#include <thread>
#include <sstream>

#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <queue>

#include "InteractionBehaviour.h"
#include "SerialClass.h"
#include <mutex>


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
const double maxInterval = 0.1; // 센서 입력 간의 최대 허용 시간 간격 (초)

std::vector<std::pair<int, char>> sensorVector; // 센서 입력을 저장하는 큐
const std::vector<std::pair<int, char>> scrollUpSequence = { {1, 't'}, {1, 'r'}, {2, 't'}, {2, 'r'} };
const std::vector<std::pair<int, char>> scrollUpSequence2 = { {1, 't'}, {2, 't'}, {1, 'r'}, {2, 'r'} };
const std::vector<std::pair<int, char>> scrollDownSequence = { {2, 't'}, {2, 'r'}, {1, 't'}, {1, 'r'} };
const std::vector<std::pair<int, char>> scrollDownSequence2 = { {2, 't'}, {1, 't'}, {2, 'r'}, {1, 'r'} };

const std::vector<std::pair<int, char>> singleClickSequence = { {1, 't'}, {1, 'r'} };
const std::vector<std::pair<int, char>> doubleClickSequence = { {1, 't'}, {1, 'r'}, {1, 't'}, {1, 'r'}};
const std::vector<std::pair<int, char>> rightClickSequence = { {2, 't'}, {2, 'r'} };

const std::vector<std::pair<int, char>> toggleMouseLock = { {3, 't'}, {3, 'r'} };
//const std::vector<std::pair<int, char>> middleClickSequence = { {1, 't'}, {1, 'r'} };
//const std::vector<std::pair<int, char>> singleClickSequence = { {1, 't'}, {1, 'r'} };

std::mutex queueMutex; // 큐 보호를 위한 뮤텍스

// Keyboard hook procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if (p->vkCode == VK_ESCAPE) {
            if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
                // Toggle the state of the mouse hook
                isMouseLockActive = !isMouseLockActive;
				std::cout << "Mouse Hook " << (isMouseLockActive ? "Activated" : "Deactivated") << std::endl;
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
                    InteractionBehaviour::endDrag();
                    isMouseLockActive = true;
                    break;
                case 3:
                    InteractionBehaviour::endWheel();
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
    }
    else if (action == 'r') {
        sensorsPressed[sensor] = false;
        std::cout << diff.count() << " " << click_count << std::endl;
    }
    sensorVector.emplace_back(sensor, action);
}

void handleSensorInputQueue(std::vector<std::pair<int, char>> &inputQueue) {
    // 큐에 저장된 센서 입력을 처리하는 함수

    if (inputQueue == scrollUpSequence || inputQueue == scrollUpSequence2) {
        // Execute scroll up function
        InteractionBehaviour::scrollUp();
    } else if (inputQueue == scrollDownSequence || inputQueue == scrollDownSequence2) {
        // Execute scroll down function
        InteractionBehaviour::scrollDown();
    } else if (inputQueue == singleClickSequence) {
        // Execute single click function
        InteractionBehaviour::singleClick();
    } else if (inputQueue == doubleClickSequence) {
        // Execute double click function
        InteractionBehaviour::doubleClick();
    } else if (inputQueue == rightClickSequence) {
        // Execute right click function
        InteractionBehaviour::rightClick();
    } else if (inputQueue == toggleMouseLock) {
		// Toggle mouse lock
		isMouseLockActive = !isMouseLockActive;
		std::cout << "Mouse Lock " << (isMouseLockActive ? "Activated" : "Deactivated") << std::endl;
	} else {
		// Invalid input sequence
		std::cout << "Invalid Input Sequence" << std::endl;
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

void evaluateSensorQueue() {
    std::lock_guard<std::mutex> lock(queueMutex);
    handleSensorInputQueue(sensorVector);

    std::cout << "Sensor Queue Evaluated:";
    for(auto& sensor : sensorVector)
    {
		std::cout << sensor.first << ","<< sensor.second << " ";
	}
    std::cout << std::endl;
    sensorVector.clear();
}

void sensorQueueMonitor() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(maxInterval * 1000)));
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = now - last_time;

        if (sensorVector.size() > 0 && diff.count() > maxInterval) {
            evaluateSensorQueue();
        }
    }
}

// Serial 통신 함수
void f1() {
    Serial* SP = new Serial(R"(\\.\COM4)");

    if (SP->IsConnected()) {
        std::cout << "Connected" << std::endl;
    }
    else
    {
        isMouseLockActive = false;
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

    // 큐 모니터링 스레드 생성
    std::thread queueMonitor(sensorQueueMonitor);
    queueMonitor.detach();


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