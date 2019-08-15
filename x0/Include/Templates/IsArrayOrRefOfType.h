#pragma once

#include "CoreTypes.h"

template<template T, typename ArrType>
struct TIsArrayOrRefType
{
	enum { Value = false };
};

template <typename ArrType> struct TIsArrayOrRefType<				 ArrType[], ArrType> { enum { Value = true }; };
template <typename ArrType> struct TIsArrayOrRefType<const			 ArrType[], ArrType> { enum { Value = true }; };
template <typename ArrType> struct TIsArrayOrRefType<		volatile ArrType[], ArrType> { enum { Value = true }; };
template <typename ArrType> struct TIsArrayOrRefType<const	volatile ArrType[], ArrType> { enum { Value = true }; };


template <typename ArrType, unsigned int N> struct TIsArrayOrRefType<				 ArrType[N], ArrType> { enum { Value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefType<const			 ArrType[N], ArrType> { enum { Value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefType<		volatile ArrType[N], ArrType> { enum { Value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefType<const	volatile ArrType[N], ArrType> { enum { Value = true }; };

template <typename ArrType, unsigned int N> struct TIsArrayOrRefType<				 ArrType(&)[N], ArrType> { enum { Value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefType<const			 ArrType(&)[N], ArrType> { enum { Value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefType<		volatile ArrType(&)[N], ArrType> { enum { Value = true }; };
template <typename ArrType, unsigned int N> struct TIsArrayOrRefType<const	volatile ArrType(&)[N], ArrType> { enum { Value = true }; };
