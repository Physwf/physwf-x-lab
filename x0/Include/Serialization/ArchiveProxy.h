#pragma once

#include "CoreTypes.h"
//#include "Internationalization/TextNamespaceFwd.h"
#include "Serialization/Archive.h"
#include "Containers/UnrealString.h"


/**
* Base class for archive proxies.
*
* Archive proxies are archive types that modify the behavior of another archive type.
*/
class FArchiveProxy : public FArchive
{
public:
	/**
	* Creates and initializes a new instance of the archive proxy.
	*
	* @param InInnerArchive The inner archive to proxy.
	*/
	X0_API FArchiveProxy(FArchive& InInnerArchive);

	
	virtual void Serialize(void* V, int64 Length) override
	{
		InnerArchive.Serialize(V, Length);
	}

	virtual void SerializeBits(void* Bits, int64 LengthBits) override
	{
		InnerArchive.SerializeBits(Bits, LengthBits);
	}

	virtual void SerializeInt(uint32& Value, uint32 Max) override
	{
		InnerArchive.SerializeInt(Value, Max);
	}

	virtual void SerializeIntPacked(uint32& Value) override
	{
		InnerArchive.SerializeIntPacked(Value);
	}

	virtual void CountBytes(SIZE_T InNum, SIZE_T InMax) override
	{
		InnerArchive.CountBytes(InNum, InMax);
	}

	X0_API virtual FString GetArchiveName() const override;


#if USE_STABLE_LOCALIZATION_KEYS
	CORE_API virtual void SetLocalizationNamespace(const FString& InLocalizationNamespace) override;
	CORE_API virtual FString GetLocalizationNamespace() const override;
#endif // USE_STABLE_LOCALIZATION_KEYS

	virtual int64 Tell() override
	{
		return InnerArchive.Tell();
	}

	virtual int64 TotalSize() override
	{
		return InnerArchive.TotalSize();
	}

	virtual bool AtEnd() override
	{
		return InnerArchive.AtEnd();
	}

	virtual void Seek(int64 InPos) override
	{
		InnerArchive.Seek(InPos);
	}

	

	virtual void Flush() override
	{
		InnerArchive.Flush();
	}

	virtual bool Close() override
	{
		return InnerArchive.Close();
	}

	virtual bool GetError() override
	{
		return InnerArchive.GetError();
	}

	

	

	virtual bool UseToResolveEnumerators() const override
	{
		return InnerArchive.UseToResolveEnumerators();
	}

	virtual void FlushCache() override
	{
		InnerArchive.FlushCache();
	}



	virtual void SetFilterEditorOnly(bool InFilterEditorOnly) override
	{
		InnerArchive.SetFilterEditorOnly(InFilterEditorOnly);
	}

#if WITH_EDITOR
	virtual void PushDebugDataString(const FName& DebugData) override
	{
		InnerArchive.PushDebugDataString(DebugData);
	}
	virtual void PopDebugDataString() override
	{
		InnerArchive.PopDebugDataString();
	}
#endif

	/** Pushes editor-only marker to the stack of currently serialized properties */
	virtual FORCEINLINE void PushSerializedProperty(class UProperty* InProperty, const bool bIsEditorOnlyProperty)
	{
		InnerArchive.PushSerializedProperty(InProperty, bIsEditorOnlyProperty);
	}
	/** Pops editor-only marker from the stack of currently serialized properties */
	virtual FORCEINLINE void PopSerializedProperty(class UProperty* InProperty, const bool bIsEditorOnlyProperty)
	{
		InnerArchive.PopSerializedProperty(InProperty, bIsEditorOnlyProperty);
	}
#if WITH_EDITORONLY_DATA
	/** Returns true if the stack of currently serialized properties contains an editor-only property */
	virtual FORCEINLINE bool IsEditorOnlyPropertyOnTheStack() const
	{
		return InnerArchive.IsEditorOnlyPropertyOnTheStack();
	}
#endif

	virtual FORCEINLINE bool IsProxyOf(FArchive* InOther) const
	{
		return InOther == this || InOther == &InnerArchive || InnerArchive.IsProxyOf(InOther);
	}

	virtual FArchive* GetCacheableArchive() override
	{
		return InnerArchive.GetCacheableArchive();
	}

protected:

	/** Holds the archive that this archive is a proxy to. */
	FArchive & InnerArchive;
};
