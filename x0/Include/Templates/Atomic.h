#pragma once

#include "HAL/ThreadSafeCounter.h"
#include "HAL/ThreadSafeCounter64.h"
#include "Templates/IsIntegral.h"
#include "Templates/IsTrivial.h"
#include "Traits/IntType.h"

template <typename T>
struct TAtomicBase_Basic;

template <typename ElementType>
struct TAtomicBase_Mutex;

template <typename ElementType, typename DiffType>
struct TAtomicBase_Arithmetic;

template <typename ElementType>
struct TAtomicBase_Pointer;

template <typename ElementType>
struct TAtomicBase_Integral;