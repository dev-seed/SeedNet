#pragma once

class SeedSocket
{
public :
	SeedSocket();
	virtual ~SeedSocket();

	bool			Create();
	void			ShutDown();

	bool			Connect(const SOCKADDR &Addr);
	bool			Connect(WCHAR* Addr, u_short Port);
	bool			Disconnect();
	bool			Listen(WCHAR* IP, u_short Port);
	bool			Send(SeedPacketHeader &Header, LPBYTE pData);
	bool			Recv();

	bool			GetPeerName(OUT WCHAR *PeerAddr, u_short &PeerPort);

public :
	inline bool		Valid()
	{
		return (m_Socket != INVALID_SOCKET);
	}

protected :
	DWORD				m_ConnectionID;
	SOCKET				m_Socket;
};