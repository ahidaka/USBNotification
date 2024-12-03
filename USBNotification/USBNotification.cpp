#include <windows.h>
#include <iostream>
#include <dbt.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DEVICECHANGE) {
        if (wParam == DBT_DEVICEARRIVAL) {
            DEV_BROADCAST_HDR* hdr = (DEV_BROADCAST_HDR*)lParam;
            if (hdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                DEV_BROADCAST_DEVICEINTERFACE* devInterface = (DEV_BROADCAST_DEVICEINTERFACE*)hdr;
                std::wcout << L"Device connected: " << devInterface->dbcc_name << std::endl;
            }
        }
        else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
            std::wcout << L"Device disconnected" << std::endl;
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    const wchar_t CLASS_NAME[] = L"USBDeviceListenerClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);
    HWND hWnd = CreateWindowEx(0, CLASS_NAME, L"USB Device Listener", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hInstance, nullptr);

    if (hWnd) {
        // Register for device notifications
        GUID InterfaceClassGuid = { 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } }; // USB device interface GUID
        DEV_BROADCAST_DEVICEINTERFACE NotificationFilter = { };
        NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
        NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        NotificationFilter.dbcc_classguid = InterfaceClassGuid;

        HDEVNOTIFY hDevNotify = RegisterDeviceNotification(hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
        if (!hDevNotify) {
            std::cerr << "Failed to register device notification." << std::endl;
            return 1;
        }

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        UnregisterDeviceNotification(hDevNotify);
    }
    else {
        std::cerr << "Failed to create window for device notifications." << std::endl;
        return 1;
    }

    return 0;
}
