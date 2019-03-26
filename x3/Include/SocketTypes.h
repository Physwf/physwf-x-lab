#pragma once

enum ESocketType
{
	SOCKTYPE_Unkown,
	SOCKTYPE_Datagram,
	SOCKTYPE_Sreaming,
};

namespace ESocketWaitConditions
{
	/**
	* Enumerates socket wait conditions.
	*/
	enum Type
	{
		/**
		* Wait until data is available for reading.
		*/
		WaitForRead,

		/**
		* Wait until data can be written.
		*/
		WaitForWrite,

		/**
		* Wait until data is available for reading or can be written.
		*/
		WaitForReadOrWrite
	};
}