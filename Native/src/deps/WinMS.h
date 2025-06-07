#pragma once
#include <windows.h>
#include <string>
#include <stdexcept>

class Win {
public:
    Win(const std::string& id) : className(id) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = Win::WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = className.c_str();
        RegisterClass(&wc);

        hwnd = CreateWindowEx(0, className.c_str(), "", WS_POPUP,
                              CW_USEDEFAULT, CW_USEDEFAULT, 100, 100,
                              nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

        if (!hwnd)
            throw std::runtime_error("Failed to create window");

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }

    ~Win() {
        destroy();
    }

    int loop() {
        MSG msg = {};
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return static_cast<int>(msg.wParam);
    }

    int width(int w = -1) {
        RECT r;
        GetWindowRect(hwnd, &r);
        if (w >= 0) SetWindowPos(hwnd, nullptr, 0, 0, w, r.bottom - r.top, SWP_NOMOVE | SWP_NOZORDER);
        return r.right - r.left;
    }

    int height(int h = -1) {
        RECT r;
        GetWindowRect(hwnd, &r);
        if (h >= 0) SetWindowPos(hwnd, nullptr, 0, 0, r.right - r.left, h, SWP_NOMOVE | SWP_NOZORDER);
        return r.bottom - r.top;
    }

    int x(int newX = -1) {
        RECT r;
        GetWindowRect(hwnd, &r);
        if (newX >= 0) SetWindowPos(hwnd, nullptr, newX, r.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        return r.left;
    }

    int y(int newY = -1) {
        RECT r;
        GetWindowRect(hwnd, &r);
        if (newY >= 0) SetWindowPos(hwnd, nullptr, r.left, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        return r.top;
    }

    int state(int s = -1) {
        if (s == -1) {
            if (IsZoomed(hwnd)) return 2;
            if (IsIconic(hwnd)) return 0;
            LONG style = GetWindowLong(hwnd, GWL_STYLE);
            return (style & WS_POPUP && !(style & WS_OVERLAPPEDWINDOW)) ? 3 : 1;
        }

        if (s == 0) ShowWindow(hwnd, SW_MINIMIZE);
        else if (s == 1) ShowWindow(hwnd, SW_RESTORE);
        else if (s == 2) ShowWindow(hwnd, SW_MAXIMIZE);
        else if (s == 3) {
            SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);
            MONITORINFO mi = { sizeof(mi) };
            GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);
            SetWindowPos(hwnd, nullptr, mi.rcMonitor.left, mi.rcMonitor.top,
                         mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_FRAMECHANGED);
        }

        return s;
    }

    void destroy() {
        if (hwnd) {
            DestroyWindow(hwnd);
            hwnd = nullptr;
        }
    }

private:
    HWND hwnd;
    std::string className;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_SETCURSOR:
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            return TRUE;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
};
