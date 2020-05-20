#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "Containers/Array.h"
#include "Math/XMathUtility.h"
#include "Containers/UnrealString.h"
#include "Misc/Parse.h"
//#include "Misc/ConfigCacheIni.h"

// video driver details
struct FGPUDriverInfo
{
	FGPUDriverInfo()
		: VendorId(0)
	{
	}

	// DirectX VendorId, 0 if not set, use functions below to set/get
	uint32 VendorId;
	// e.g. "NVIDIA GeForce GTX 680" or "AMD Radeon R9 200 / HD 7900 Series"
	FString DeviceDescription;
	// e.g. "NVIDIA" or "Advanced Micro Devices, Inc."
	FString ProviderName;
	// e.g. "15.200.1062.1004"(AMD)
	// e.g. "9.18.13.4788"(NVIDIA) first number is Windows version (e.g. 7:Vista, 6:XP, 4:Me, 9:Win8(1), 10:Win7), last 5 have the UserDriver version encoded
	// also called technical version number (https://wiki.mozilla.org/Blocklisting/Blocked_Graphics_Drivers)
	// TEXT("Unknown") if driver detection failed
	FString InternalDriverVersion;
	// e.g. "Catalyst 15.7.1"(AMD) or "Crimson 15.7.1"(AMD) or "347.88"(NVIDIA)
	// also called commercial version number (https://wiki.mozilla.org/Blocklisting/Blocked_Graphics_Drivers)
	FString UserDriverVersion;
	// e.g. 3-13-2015
	FString DriverDate;

	bool IsValid() const
	{
		return !DeviceDescription.IsEmpty()
			&& VendorId
			&& (InternalDriverVersion != TEXT("Unknown"))		// if driver detection code fails
			&& (InternalDriverVersion != TEXT(""));				// if running on non Windows platform we don't fill in the driver version, later we need to check for the OS as well.
	}

	// set VendorId
	void SetAMD() { VendorId = 0x1002; }
	// set VendorId
	void SetIntel() { VendorId = 0x8086; }
	// set VendorId
	void SetNVIDIA() { VendorId = 0x10DE; }
	// get VendorId
	bool IsAMD() const { return VendorId == 0x1002; }
	// get VendorId
	bool IsIntel() const { return VendorId == 0x8086; }
	// get VendorId
	bool IsNVIDIA() const { return VendorId == 0x10DE; }

	FString GetUnifiedDriverVersion() const
	{
		// we use the internal version, not the user version to avoid problem where the name was altered 
		const FString& FullVersion = InternalDriverVersion;

		if (IsNVIDIA())
		{
			// on the internal driver number: https://forums.geforce.com/default/topic/378546/confusion-over-driver-version-numbers/
			//   The first 7 shows u that is a Vista driver, 6 that is an XP and 4 that is Me
			// we don't care about the windows version so we don't look at the front part of the driver version
			// "9.18.13.4788" -> "347.88"
			// "10.18.13.4788" -> "347.88"
			// the following code works with the current numbering scheme, if needed we have to update that

			// we don't care about the windows version so we don't look at the front part of the driver version
			// e.g. 36.143
			FString RightPart = FullVersion.Right(6);

			// move the dot
			RightPart = RightPart.Replace(TEXT("."), TEXT(""));
			RightPart.InsertAt(3, TEXT("."));
			return RightPart;
		}
		else if (IsAMD())
		{
			// examples for AMD: "13.12" "15.101.1007" "13.351"
		}
		else if (IsIntel())
		{
		}
		return FullVersion;
	}
};