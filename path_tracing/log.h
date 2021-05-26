#pragma once

#include <stdio.h>
#include <cstdarg>
#include <mutex>

#define X_LOG(Format,...) XLOG(Format, __VA_ARGS__)

std::mutex lk;

inline void XLOG(const char* format, ...)
{
	char buffer[1024] = { 0 };
	va_list v_list;
	va_start(v_list, format);
	vsprintf_s(buffer, format, v_list);
	va_end(v_list);
	extern void OutputDebug(const char* Format);
	{
		std::lock_guard<std::mutex> guard(lk);
		OutputDebug(buffer);
	}
}
