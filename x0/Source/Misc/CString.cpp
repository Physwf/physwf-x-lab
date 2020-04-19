#include "Misc/CString.h"
#include "Containers/StringConv.h"

// 4 lines of 64 chars each, plus a null terminator
template <>
X0_API const ANSICHAR TCStringSpcHelper<ANSICHAR>::SpcArray[MAX_SPACES + 1] =
"                                                                "
"                                                                "
"                                                                "
"                                                               ";

template <>
X0_API const WIDECHAR TCStringSpcHelper<WIDECHAR>::SpcArray[MAX_SPACES + 1] =
TEXT("                                                                ")
TEXT("                                                                ")
TEXT("                                                                ")
TEXT("                                                               ");

template <>
X0_API const ANSICHAR TCStringSpcHelper<ANSICHAR>::TabArray[MAX_TABS + 1] =
"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

template <>
X0_API const WIDECHAR TCStringSpcHelper<WIDECHAR>::TabArray[MAX_TABS + 1] =
TEXT("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t")
TEXT("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t")
TEXT("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t")
TEXT("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");


bool FToBoolHelper::FromCStringAnsi(const ANSICHAR* String)
{
	return FToBoolHelper::FromCStringWide(ANSI_TO_TCHAR(String));
}

bool FToBoolHelper::FromCStringWide(const WIDECHAR* String)
{
	if (
		FCStringWide::Stricmp(String, TEXT("True")) == 0
		|| FCStringWide::Stricmp(String, TEXT("Yes")) == 0
		|| FCStringWide::Stricmp(String, TEXT("On")) == 0
		/*|| FCStringWide::Stricmp(String, *(GTrue.ToString())) == 0*/
		/*|| FCStringWide::Stricmp(String, *(GYes.ToString())) == 0*/)
	{
		return true;
	}
	else if (
		FCStringWide::Stricmp(String, TEXT("False")) == 0
		|| FCStringWide::Stricmp(String, TEXT("No")) == 0
		|| FCStringWide::Stricmp(String, TEXT("Off")) == 0
		/*|| FCStringWide::Stricmp(String, *(GFalse.ToString())) == 0*/
		/*|| FCStringWide::Stricmp(String, *(GNo.ToString())) == 0*/)
	{
		return false;
	}
	else
	{
		return FCStringWide::Atoi(String) ? true : false;
	}
}