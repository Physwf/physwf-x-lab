#pragma once
/*
* Data Models--------------------------------------------------
	C / C++ base type	LP64(Unix / Linux)	LLP64(Windows)	ILP32
	int, short, char	¡Ü32					¡Ü32				¡Ü32
	Long				64					32				32
	void * (pointer)	64					64				32
	long long			64					64				64
	SP Float			32					32				32
	DP Float			64					64				64
*/

// Unsigned base types.
typedef unsigned char 		uint8;		// 8-bit  unsigned.
typedef unsigned short int	uint16;		// 16-bit unsigned.
typedef unsigned int		uint32;		// 32-bit unsigned.
typedef unsigned long long	uint64;		// 64-bit unsigned.

// Signed base types.
typedef	signed char			int8;		// 8-bit  signed.
typedef signed short int	int16;		// 16-bit signed.
typedef signed int	 		int32;		// 32-bit signed.
typedef signed long long	int64;		// 64-bit signed.