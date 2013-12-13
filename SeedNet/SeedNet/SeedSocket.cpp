#include "stdafx.h"

SeedSocket::SeedSocket()
	: m_Socket(INVALID_SOCKET),
	m_ConnectionID(0)
{
}

SeedSocket::~SeedSocket()
{

}

bool SeedSocket::Create()
{
	if (m_Socket != INVALID_SOCKET)
	{
		return false;
	}

	m_Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (m_Socket == INVALID_SOCKET)
	{
		return false;
	}

	// Async Mode
	u_long command = 1;
	if (ioctlsocket(m_Socket, FIONBIO, &command) == SOCKET_ERROR)
	{
		ShutDown();
		return false;
	}

	return true;
}

void SeedSocket::ShutDown()
{
	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
	m_ConnectionID = 0;
}

bool SeedSocket::Connect( const SOCKADDR &Addr )
{
	if (Valid())
	{
		if (WSAConnect(m_Socket, &Addr, sizeof(SOCKADDR_IN), nullptr, nullptr, nullptr, nullptr) == SOCKET_ERROR)
		{
			ShutDown();
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool SeedSocket::Connect( WCHAR* Addr, u_short Port )
{
	if (Valid())
	{
		char transferedAddr[LENGTH_ADDRESS] = {0,};
		WideCharToMultiByte(CP_ACP, 0, Addr, -1, transferedAddr, LENGTH_ADDRESS, nullptr, nullptr);

		SOCKADDR_IN sockAddr;
		ZeroMemory(&sockAddr, sizeof(SOCKADDR_IN));

		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port	= Port;
		sockAddr.sin_addr.s_addr = inet_addr(transferedAddr);

		if (sockAddr.sin_addr.s_addr == INADDR_NONE)
		{
			HOSTENT *pHost = gethostbyname(transferedAddr);
			if (pHost != nullptr)
			{
				sockAddr.sin_addr.s_addr = ((LPIN_ADDR)pHost->h_addr)->s_addr;
			}
			else
			{
				ShutDown();
				return false;
			}
		}

		return Connect(*((SOCKADDR*)&sockAddr));
	}
	else
	{
		return false;
	}
}

bool SeedSocket::Disconnect()
{
	if (Valid())
	{
		shutdown(m_Socket, SD_BOTH);
	}
	return true;
}

bool SeedSocket::Listen( WCHAR* IP, u_short Port )
{
	if (Valid())
	{
		char transferdAddr[LENGTH_ADDRESS] = {0,};
		WideCharToMultiByte(CP_ACP, 0, IP, -1, transferdAddr, LENGTH_ADDRESS, nullptr ,nullptr);

		SOCKADDR_IN sockAddr;
		ZeroMemory(&sockAddr, sizeof(SOCKADDR_IN));

		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port	= Port;
		sockAddr.sin_addr.s_addr = inet_addr(transferdAddr);

		if (::bind(m_Socket, (LPSOCKADDR)&sockAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			ShutDown();
			return false;
		}
		if (listen(m_Socket, SOMAXCONN) == SOCKET_ERROR)
		{
			ShutDown();
			return false;
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool SeedSocket::Send( SeedPacketHeader &Header, LPBYTE pData )
{
	if (Valid())
	{
		// Data
		//LPBYTE	pTempData = new BYTE[SIZE_DEFAULTPACKETDATA];
		LPBYTE pTempData = (LPBYTE)PacketPool::malloc();
		ZeroMemory(pTempData, SIZE_DEFAULTPACKETDATA);
		
		CopyMemory(pTempData, &Header, sizeof(SeedPacketHeader));

		if (Header.DataSize != 0 && pData != nullptr)
		{
			CopyMemory(pTempData + sizeof(SeedPacketHeader), pData, Header.DataSize);
		}

		// Overlapped
		//SeedOverlapped *pTempOverlapped = new SeedOverlapped;
		SeedOverlapped *pTempOverlapped = (SeedOverlapped *)OverlappedPool::malloc();
		ZeroMemory(pTempOverlapped, sizeof(SeedOverlapped));

		pTempOverlapped->IoOption = IoSend;
		pTempOverlapped->WSABuf.len = sizeof(SeedPacketHeader) + Header.DataSize;
		pTempOverlapped->WSABuf.buf = (LPSTR)pTempData;

		DWORD Flag = 0, ByteTransfer = 0;

		if (WSASend(m_Socket, &(pTempOverlapped->WSABuf), 1, &ByteTransfer, Flag, pTempOverlapped, nullptr) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				Disconnect();

				return false;
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool SeedSocket::Recv()
{
	if (Valid())
	{
		// Data
		//LPBYTE pTempData = new BYTE[SIZE_DEFAULTPACKETDATA];
		LPBYTE pTempData = (LPBYTE)PacketPool::malloc();
		ZeroMemory(pTempData, SIZE_DEFAULTPACKETDATA);

		// Overlapped
		//SeedOverlapped *pTempOverlapped = new SeedOverlapped;
		SeedOverlapped *pTempOverlapped = (SeedOverlapped *)OverlappedPool::malloc();
		ZeroMemory(pTempOverlapped, sizeof(SeedOverlapped));

		pTempOverlapped->IoOption	= IoRecv;
		pTempOverlapped->WSABuf.buf	= (LPSTR)pTempData;
		pTempOverlapped->WSABuf.len	= SIZE_DEFAULTPACKETDATA;

		DWORD Flag = 0, ByteTransfer = 0;
		if (WSARecv(m_Socket, &(pTempOverlapped->WSABuf), 1, &ByteTransfer, &Flag, pTempOverlapped, nullptr) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool SeedSocket::GetPeerName( OUT WCHAR *PeerAddr, u_short &PeerPort )
{
	if (Valid())
	{
		SOCKADDR_IN sockAddr;
		ZeroMemory(&sockAddr, sizeof(SOCKADDR_IN));
		int addrLen = sizeof(SOCKADDR_IN);

		if (getpeername(m_Socket, (SOCKADDR*)&sockAddr, &addrLen) == 0)
		{
			char peerAddr[LENGTH_ADDRESS] = {0,};
			CopyMemory(peerAddr, inet_ntoa(sockAddr.sin_addr), LENGTH_ADDRESS);

			MultiByteToWideChar(CP_ACP, 0, peerAddr, -1, PeerAddr, LENGTH_ADDRESS);
			PeerPort = ntohs(sockAddr.sin_port);

			return true;
		}
	}

	return false;
}