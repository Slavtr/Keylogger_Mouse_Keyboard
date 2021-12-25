#pragma once
#include <stdio.h>
#include <Windows.h>
#include <time.h>

struct MouseLog {
	POINT mousePoint;
	WPARAM mouseButton;
	struct MouseLog* next;
	struct MouseLog* prev;
};
typedef struct MouseLog MLOG;
struct KeyboardLog {
	LPWSTR KeyString;
	struct KeyboardLog* next;
	struct KeyboardLog* prev;
};
typedef struct KeyboardLog KLOG;
struct MinuteLog {
	time_t time;
	MLOG* mlog;
	KLOG* klog;
	struct MinuteLog* next;
	struct MinuteLog* prev;
};
typedef struct MinuteLog MAINLOG;
struct STRNG {
	LPWSTR act;
	int count;
	struct STRNG* next;
	struct STRNG* prev;
};
typedef struct STRNG STRNG;