#pragma once

struct XGenericPlatformTypes;

struct XUnixPlatformTypes : public XGenericPlatformTypes
{
	typedef __SIZE_TYPE__ SIZE_T;
};

typedef XUnixPlatformTypes XPlatformTypes;