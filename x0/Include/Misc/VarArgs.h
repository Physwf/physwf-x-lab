#pragma once

#include "CoreTypes.h"

#ifdef VISUAL_ASSIST_HACK
#define VARARG_DECL(FuncRet, StaticFuncRet, Return, FunName, Pure, FmtType, ExtraDecl, ExtraCall ) FuncRet FuncName(ExtraDecl FmtType Fmt, ...)
#define VARARG_BODY(FuncRet, FuncName, FmtType, ExtraDecl) FuncRet FunName(ExtraDecl FmtType Fmt, ...)
#endif