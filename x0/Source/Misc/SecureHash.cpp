#include "Misc/SecureHash.h"
//#include "HAL/FileManager.h"
//#include "Misc/Paths.h"

uint32 GetTypeHash(FSHAHash const& InKey)
{
	return FCrc::MemCrc32(InKey.Hash, sizeof(InKey.Hash));
}