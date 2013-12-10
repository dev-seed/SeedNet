
// Queue
typedef std::queue<std::wstring>	WStringQueue;

// pool


// PacketHeader
typedef struct 
{
	// Basic
	WORD		DataSize;
	DWORD		DataCheckSum;
	WORD		InitialCode;

	union
	{
		DWORD		Protocol;

		struct  
		{
			BYTE	Type		:	1;			// Request/Response
			BYTE	Source		:	4;			// Source
			BYTE	Dest		:	4;			// Dest
			DWORD	Detail		:	23;			// Details. Define specific kind of protocol
		};
	};

}SeedPacketHeader;


// OverlappedEx
typedef struct  _sSeedOverlapped : public OVERLAPPED
{
	WSABUF	WSABuf;
	BYTE	IoOption;
}SeedOverlapped;