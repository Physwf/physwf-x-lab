#pragma once

#include "CoreTypes.h"

#ifdef VISUAL_ASSIST_HACK
#define VARARG_DECL(FuncRet, StaticFuncRet, Return, FunName, Pure, FmtType, ExtraDecl, ExtraCall ) FuncRet FuncName(ExtraDecl FmtType Fmt, ...)
#define VARARG_BODY(FuncRet, FuncName, FmtType, ExtraDecl) FuncRet FunName(ExtraDecl FmtType Fmt, ...)
#endif

#define GET_VARARGS(msg,msgsize,len,lastarg,fmt) \
	{ \
		va_list ap; \
		va_start(ap, lastarg); \
		FCString::GetVarArgs(msg,msgsize,len,fmt,ap); \
	}

#define GET_VARARGS_WIDE(msg,msgsize,len,lastarg,fmt) \
	{ \
		va_list ap; \
		va_start(ap, lastarg); \
		FCStringWide::GetVarArgs(msg,msgsize,len,fmt,ap); \
	}

#define GET_VARARGS_ANSI(msg,msgsize,len,lastarg,fmt) \
	{ \
		va_list ap; \
		va_start(ap, lastarg); \
		FCStringAnsi::GetVarArgs(msg,msgsize,len,fmt,ap); \
	}

#define GET_VARARGS_RESULT(msg,msgsize,len,lastarg,fmt,result) \
	{ \
		va_list ap; \
		va_start(ap, lastarg); \
		FCString::GetVarArgs(msg,msgsize,len,fmt,ap); \
		if(result >= msgsize)\
		{\
			result = -1;\
		}\
	}

#define GET_VARARGS_RESULT_WIDE(msg,msgsize,len,lastarg,fmt,result) \
	{ \
		va_list ap; \
		va_start(ap, lastarg); \
		FCStringWide::GetVarArgs(msg,msgsize,len,fmt,ap); \
		if(result >= msgsize)\
		{\
			result = -1;\
		}\
	}

#define GET_VARARGS_RESULT_ANSI(msg,msgsize,len,lastarg,fmt,result) \
	{ \
		va_list ap; \
		va_start(ap, lastarg); \
		FCStringAnsi::GetVarArgs(msg,msgsize,len,fmt,ap); \
		if(result >= msgsize)\
		{\
			result = -1;\
		}\
	}

#define VARARG_EXTRA(...) __VAR_ARGS__,
#define VARARG_NONE
#define VARARG_PURE =0