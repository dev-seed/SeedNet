
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

#define SIZE_DEFAULTPACKETHEADER	sizeof(SeedPacketHeader)

// OverlappedEx
typedef struct  _sSeedOverlapped : public OVERLAPPED
{
	WSABUF	WSABuf;
	BYTE	IoOption;
}SeedOverlapped;

#define SIZE_DEFAULTOVERLAPPEDEX	sizeof(SeedOverlapped)

// Tag
class SeedPacket;
typedef struct 
{
	BYTE Data[SIZE_DEFAULTPACKETDATA*2];
}SeedBuffer;

// Queue
typedef std::queue<std::wstring>					WStringQueue;

// pool
typedef boost::singleton_pool<SeedPacket, SIZE_DEFAULTPACKETHEADER+SIZE_DEFAULTPACKETDATA>		PacketPool;
typedef boost::singleton_pool<SeedOverlapped, SIZE_DEFAULTOVERLAPPEDEX>							OverlappedPool;
typedef boost::singleton_pool<SeedBuffer, sizeof(SeedBuffer)>									BufferPool;

