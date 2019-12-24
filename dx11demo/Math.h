#pragma once

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

struct Vector4
{
	float X, Y, Z, W;
};

struct Vector
{
	float X, Y, Z;
};

struct Vector2
{
	float X, Y;
};

struct LinearColor
{
	float R, G, B, A;
};

struct Color
{
	uint8 R, G, B, A;
};