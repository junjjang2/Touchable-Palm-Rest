#include <Windows.h>
#include <iostream>

HHOOK mouseHook;
HHOOK keyboardHook;

bool isMouseHookActive = false;
bool ctrlPressed = false;        // State of the Ctrl key

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

int main() {
    std::cout << "T를 눌러 마우스를 활성화/비활성화 합니다" << std::endl;

    SetMouseHook();
    SetKeyboardHook();

    // Message loop to keep the hook active
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(mouseHook);  // Clean up the hook
    UnhookWindowsHookEx(keyboardHook);  // Clean up the hook
    return 0;
}