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
void Sort(STRNG* list) {
	STRNG* tmp;
	STRNG* a;
	LPWSTR t = L"";
	BOOL flag = TRUE;
	while (flag) {
		tmp = list;
		a = tmp->next;
		flag = FALSE;
		while (a) {
			if ((tmp->act) > (a->act)) {
				t = tmp->act;
				tmp->act = a->act;
				a->act = t;
				flag = TRUE;
			}
			tmp = tmp->next;
			a = a->next;
		}
	}
}