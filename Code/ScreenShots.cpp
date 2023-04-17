// Disable screenshotting
#include <windows.h>

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    if (!(dwStyle & WS_VISIBLE)) return TRUE;

    DWORD dwExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (dwExStyle & WS_EX_TOOLWINDOW) return TRUE;

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;

    SetWindowDisplayAffinity(hwnd, WDA_MONITOR);
    return TRUE;
}

void disable_screenshotting() {
    EnumWindows(EnumWindowsProc, 0);
}
