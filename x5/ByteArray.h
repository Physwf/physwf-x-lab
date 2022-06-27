#pragma once

#include <list>

namespace Net
{
	class ByteArray
	{
	public:
		ByteArray() : mLength(0) {}

		size_t Length() const { return mLength; }

		virtual BYTE* Head() const = 0;

		virtual size_t Write(void* Data, size_t DataSize) = 0;
		virtual size_t Read(void* OutBuffer, size_t BufferSize) = 0;

		virtual void* Alloc(size_t size) = 0;
		virtual void Free(size_t size) = 0;

		template <typename T>
		size_t Write(const T& Value)
		{
			return Write(&Value, sizeof(T));
		}
		template<typename T>
		size_t Read(T& Value)
		{
			return Read(&Value, sizeof(T));
		}

		void Clear() { mLength = 0; }
	protected:
		size_t mLength;
	};

	template <size_t Size>
	class FixedByteArray : public ByteArray
	{
	public:
		FixedByteArray() : mLength(0) {}
		~FixedByteArray() {}

		size_t Capacity() const { return Size; }

		virtual BYTE* Head() const { return mBuffer; }

		virtual size_t Write(void* Data, size_t DataSize);
		virtual size_t Read(void* OutBuffer, size_t BufferSize);

	private:
		BYTE mBuffer[Size];
	};

	class DynamicByteArray
	{
	public:
		DynamicByteArray() {}

		virtual size_t Write(void* Data, size_t DataSize);
		virtual size_t Read(void* OutBuffer, size_t BufferSize);

		virtual void* Alloc(size_t size);
		virtual void Free(size_t size);

	private:
		std::list<FixedByteArray<1024>*> mBuffer;
	};
}