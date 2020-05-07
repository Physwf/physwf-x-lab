#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
//#include "UObject/NameTypes.h"
#include "Templates/SharedPointer.h"
//#include "UObject/WeakObjectPtrTemplates.h"
#include "Delegates/MulticastDelegateBase.h"
#include "Delegates/IntegerSequence.h"

// This suffix is appended to all header exported delegates
#define HEADER_GENERATED_DELEGATE_SIGNATURE_SUFFIX TEXT("__DelegateSignature")

/** Helper macro that enables passing comma-separated arguments as a single macro parameter */
#define FUNC_CONCAT( ... ) __VA_ARGS__

/** Declare the user's delegate object */
// NOTE: The last parameter is variadic and is used as the 'template args' for this delegate's classes (__VA_ARGS__)
#define FUNC_DECLARE_DELEGATE( DelegateName, ... ) \
	typedef TBaseDelegate<__VA_ARGS__> DelegateName;

/** Declare the user's multicast delegate object */
// NOTE: The last parameter is variadic and is used as the 'template args' for this delegate's classes (__VA_ARGS__)
#define FUNC_DECLARE_MULTICAST_DELEGATE( MulticastDelegateName, ... ) \
	typedef TMulticastDelegate<__VA_ARGS__> MulticastDelegateName;

#define FUNC_DECLARE_EVENT( OwningType, EventName, ... ) \
	class EventName : public TBaseMulticastDelegate<__VA_ARGS__> \
	{ \
		friend class OwningType; \
	};

/** Declare user's dynamic delegate, with wrapper proxy method for executing the delegate */
#define FUNC_DECLARE_DYNAMIC_DELEGATE( TWeakPtr, DynamicDelegateName, ExecFunction, FuncParamList, FuncParamPassThru, ... ) \
	class DynamicDelegateName : public TBaseDynamicDelegate<TWeakPtr, __VA_ARGS__> \
	{ \
	public: \
		/** Default constructor */ \
		DynamicDelegateName() \
		{ \
		} \
		\
		/** Construction from an FScriptDelegate must be explicit.  This is really only used by UObject system internals. */ \
		explicit DynamicDelegateName( const TScriptDelegate<>& InScriptDelegate ) \
			: TBaseDynamicDelegate<TWeakPtr, __VA_ARGS__>( InScriptDelegate ) \
		{ \
		} \
		\
		/** Execute the delegate.  If the function pointer is not valid, an error will occur. */ \
		inline void Execute( FuncParamList ) const \
		{ \
			/* Verify that the user object is still valid.  We only have a weak reference to it. */ \
			checkSlow( IsBound() ); \
			ExecFunction( FuncParamPassThru ); \
		} \
		/** Execute the delegate, but only if the function pointer is still valid */ \
		inline bool ExecuteIfBound( FuncParamList ) const \
		{ \
			if( IsBound() ) \
			{ \
				ExecFunction( FuncParamPassThru ); \
				return true; \
			} \
			return false; \
		} \
	};

#define FUNC_DECLARE_DYNAMIC_DELEGATE_RETVAL(TWeakPtr, DynamicDelegateName, ExecFunction, RetValType, FuncParamList, FuncParamPassThru, ...) \
	class DynamicDelegateName : public TBaseDynamicDelegate<TWeakPtr, __VA_ARGS__> \
	{ \
	public: \
		/** Default constructor */ \
		DynamicDelegateName() \
		{ \
		} \
		\
		/** Construction from an FScriptDelegate must be explicit.  This is really only used by UObject system internals. */ \
		explicit DynamicDelegateName( const TScriptDelegate<>& InScriptDelegate ) \
			: TBaseDynamicDelegate<TWeakPtr, __VA_ARGS__>( InScriptDelegate ) \
		{ \
		} \
		\
		/** Execute the delegate.  If the function pointer is not valid, an error will occur. */ \
		inline RetValType Execute( FuncParamList ) const \
		{ \
			/* Verify that the user object is still valid.  We only have a weak reference to it. */ \
			checkSlow( IsBound() ); \
			return ExecFunction( FuncParamPassThru ); \
		} \
	};


/** Declare user's dynamic multi-cast delegate, with wrapper proxy method for executing the delegate */
#define FUNC_DECLARE_DYNAMIC_MULTICAST_DELEGATE(TWeakPtr, DynamicMulticastDelegateName, ExecFunction, FuncParamList, FuncParamPassThru, ...) \
class DynamicMulticastDelegateName : public TBaseDynamicMulticastDelegate<TWeakPtr, __VA_ARGS__> \
	{ \
	public: \
		/** Default constructor */ \
		DynamicMulticastDelegateName() \
		{ \
		} \
		\
		/** Construction from an FMulticastScriptDelegate must be explicit.  This is really only used by UObject system internals. */ \
		explicit DynamicMulticastDelegateName( const TMulticastScriptDelegate<>& InMulticastScriptDelegate ) \
			: TBaseDynamicMulticastDelegate<TWeakPtr, __VA_ARGS__>( InMulticastScriptDelegate ) \
		{ \
		} \
		\
		/** Broadcasts this delegate to all bound objects, except to those that may have expired */ \
		void Broadcast( FuncParamList ) const \
		{ \
			ExecFunction( FuncParamPassThru ); \
		} \
	};


// Simple macro chain to concatenate code text
#define FUNC_COMBINE_ACTUAL( A, B ) A##B
#define FUNC_COMBINE( A, B ) FUNC_COMBINE_ACTUAL( A, B )



#define ENABLE_STATIC_FUNCTION_FNAMES (PLATFORM_COMPILER_CLANG && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 5)))

#if ENABLE_STATIC_FUNCTION_FNAMES


namespace NStrAfterLastDoubleColon_Private
{
	template <typename T>
	struct TTypedImpl
	{
		// Dummy bool parameter needed so that we can ensure the last specialization is partial
		template <bool Dummy, T... Chars>
		struct Inner;

		template <typename SecondMatch, T... Chars>
		struct Inner2
		{
			typedef typename TTypedImpl<T>::template Inner<true, Chars...>::Type Type;
		};

		template <T... Chars>
		struct Inner2<TIntegerSequence<T>, Chars...>
		{
			typedef TIntegerSequence<T, Chars...> Type;
		};

		template <bool Dummy, T Char, T... Chars>
		struct Inner<Dummy, Char, Chars...>
		{
			typedef typename Inner<Dummy, Chars...>::Type Type;
		};

		template <bool Dummy, T... Chars>
		struct Inner<Dummy, ':', ':', Chars...>
		{
			typedef typename Inner2<typename Inner<Dummy, Chars...>::Type, Chars...>::Type Type;
		};

		// Without the dummy bool parameter, this would not compile, as full specializations
		// of nested class templates is not allowed.
		template <bool Dummy>
		struct Inner<Dummy>
		{
			typedef TIntegerSequence<T> Type;
		};
	};


	template <typename IntSeq>
	struct TStaticFNameFromCharSequence;

	template <typename T, T... Chars>
	struct TStaticFNameFromCharSequence<TIntegerSequence<T, Chars...>>
	{
		static FName Get()
		{
			static FName Result = Create();
			return Result;
		}

	private:
		static FName Create()
		{
			const T Str[sizeof...(Chars) + 1] = { Chars..., 0 };
			return Str;
		}
	};

	/**
	 * Metafunction which evaluates to a TIntegerSequence of chars containing only the function name.
	 *
	 * Example:
	 * TStrAfterLastDoubleColon<TEXT("&SomeClass::SomeNestedClass::SomeFunc")_intseq>::Type == TIntegerSequence<TCHAR, 'S', 'o', 'm', 'e', 'F', 'u', 'n', 'c'>
	 */
	template <typename T>
	struct TStrAfterLastDoubleColon;

	template <typename T, T... Chars>
	struct TStrAfterLastDoubleColon<TIntegerSequence<T, Chars...>>
	{
		typedef typename NStrAfterLastDoubleColon_Private::TTypedImpl<T>::template Inner<true, Chars...>::Type Type;
	};
}

/**
 * Custom literal operator which converts a string into a TIntegerSequence of chars.
 */
template <typename T, T... Chars>
FORCEINLINE constexpr TIntegerSequence<T, Chars...> operator""_intseq()
{
	return {};
}

#define STATIC_FUNCTION_FNAME(str) NStrAfterLastDoubleColon_Private::TStaticFNameFromCharSequence<typename NStrAfterLastDoubleColon_Private::TStrAfterLastDoubleColon<decltype(PREPROCESSOR_JOIN(str, _intseq))>::Type>::Get()

#else

#define STATIC_FUNCTION_FNAME(str) UE4Delegates_Private::GetTrimmedMemberFunctionName(str)

#endif


// Helper macro for calling BindDynamic() on dynamic delegates.  Automatically generates the function name string.
#define BindDynamic( UserObject, FuncName ) __Internal_BindDynamic( UserObject, FuncName, STATIC_FUNCTION_FNAME( TEXT( #FuncName ) ) )

// Helper macro for calling AddDynamic() on dynamic multi-cast delegates.  Automatically generates the function name string.
#define AddDynamic( UserObject, FuncName ) __Internal_AddDynamic( UserObject, FuncName, STATIC_FUNCTION_FNAME( TEXT( #FuncName ) ) )

// Helper macro for calling AddUniqueDynamic() on dynamic multi-cast delegates.  Automatically generates the function name string.
#define AddUniqueDynamic( UserObject, FuncName ) __Internal_AddUniqueDynamic( UserObject, FuncName, STATIC_FUNCTION_FNAME( TEXT( #FuncName ) ) )

// Helper macro for calling RemoveDynamic() on dynamic multi-cast delegates.  Automatically generates the function name string.
#define RemoveDynamic( UserObject, FuncName ) __Internal_RemoveDynamic( UserObject, FuncName, STATIC_FUNCTION_FNAME( TEXT( #FuncName ) ) )

// Helper macro for calling IsAlreadyBound() on dynamic multi-cast delegates.  Automatically generates the function name string.
#define IsAlreadyBound( UserObject, FuncName ) __Internal_IsAlreadyBound( UserObject, FuncName, STATIC_FUNCTION_FNAME( TEXT( #FuncName ) ) )


namespace UE4Delegates_Private
{
	/**
	 * Returns the root function name from a string representing a member function pointer.
	 * Note: this function only returns a pointer to the substring and doesn't create a new string.
	 *
	 * @param  InMacroFunctionName  The string containing the member function name.
	 * @return An FName of the member function name.
	 */
	inline FName GetTrimmedMemberFunctionName(const TCHAR* InMacroFunctionName)
	{
		// We strip off the class prefix and just return the function name by itself.
		check(InMacroFunctionName);
		const TCHAR* Result = FCString::Strrstr(InMacroFunctionName, TEXT("::"));
		checkf(Result && Result[2] != (TCHAR)'0', TEXT("'%s' does not look like a member function"), InMacroFunctionName);
		return FName(Result + 2);
	}
}


/*********************************************************************************************************************/

// We define this as a guard to prevent DelegateSignatureImpl.inl being included outside of this file
#define __Delegate_h__
#define FUNC_INCLUDING_INLINE_IMPL

#if !UE_BUILD_DOCS
#include "Delegates/DelegateInstanceInterface.h"
#include "Delegates/DelegateInstancesImpl.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Delegates/DelegateCombinations.h"
#endif

// No longer allowed to include DelegateSignatureImpl.inl
#undef FUNC_INCLUDING_INLINE_IMPL
#undef __Delegate_h__

/*********************************************************************************************************************/

#define DECLARE_DERIVED_EVENT( OwningType, BaseTypeEvent, EventName ) \
	class EventName : public BaseTypeEvent { friend class OwningType; };


// Undefine temporary macros
#undef FUNC_COMBINE_ACTUAL
#undef FUNC_COMBINE

// Simple delegate used by various utilities such as timers
DECLARE_DELEGATE(FSimpleDelegate);
DECLARE_MULTICAST_DELEGATE(FSimpleMulticastDelegate);