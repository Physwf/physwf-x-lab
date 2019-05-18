// Undo any Windows defines.
#undef uint8
#undef uint16
#undef uint32
#undef int32
#undef float
#undef CDECL
#undef PF_MAX
#undef PlaySound
#undef DrawText
#undef CaptureStackBackTrace
#undef MemoryBarrier
#undef DeleteFile
#undef MoveFile
#undef CopyFile
#undef CreateDirectory
#undef GetCurrentTime
#undef SendMessage
#undef LoadString
#undef UpdateResource
#undef FindWindow
#undef GetObject
#undef GetEnvironmentVariable
#undef CreateFont
#undef CreateDesktop
#undef GetMessage
#undef GetCommandLine
#undef GetProp
#undef SetProp
#undef GetFileAttributes

// Restore any previously defined macros
#pragma pop_macro("MAX_uint8")
#pragma pop_macro("MAX_uint16")
#pragma pop_macro("MAX_uint32")
#pragma pop_macro("MAX_int32")
#pragma pop_macro("TEXT")