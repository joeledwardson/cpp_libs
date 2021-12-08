#include "basenet.h"

SockData::SockData(unsigned int _port,SocketType _typ,unsigned long _IPAddress)
	:typ( _typ ),
	IPAddress(_IPAddress),
	port(_port) {
	assert(port);

	assert( typ == SOCKET_HOST || typ == SOCKET_CLIENT );
	if( typ == SOCKET_CLIENT ) {
		assert( IPAddress  );
	}
	else if( typ == SOCKET_HOST ) {
		assert( IPAddress == 0);
	}
}

BaseNetworking::BaseNetworking(std::vector<SockData> socketList)
	:sockAddrSize(sizeof(sockaddr)),
	nSockets(socketList.size()) {

	assert( nSockets > 0 );

	sockets = new int[nSockets];
	socketTypeList = new SocketType[nSockets];

	ZeroMemory( sockets,sizeof(int)* nSockets );

#if PLATFORM == PLATFORM_WINDOWS
	WSADATA winsockDat;
	int rVal = WSAStartup( MAKEWORD( 2,2 ) , &winsockDat );
	if( rVal != 0 ) {
		SetSysErr();
		return;
	}
#endif

	for( unsigned int i = 0; i < nSockets; i++ ) {
		if( socketList[i].typ == SOCKET_CLIENT ) {
			InitClientSocket( i,socketList[i].IPAddress,socketList[i].port);
		}
		else if( socketList[i].typ == SOCKET_HOST ) {
			InitHostSocket( i,socketList[i].port );
		}
		else
			SetCritErr( "socket type not found");
	}

}
BaseNetworking::~BaseNetworking()
{
	for( UINT index = 0; index < nSockets; index++ ){
		if( sockets[index] ) {

			if( closesocket( sockets[index]))
				SetSysErr();
		}
	}

#if PLATFORM == PLATFORM_WINDOWS
	if( WSACleanup())
		SetSysErr();
#endif

	delete[] socketTypeList;
	delete[] sockets;


}
int BaseNetworking::SendData(UINT sock,int size, const char* buffer,const sockaddr* toAddr) const {
	int rVal;

	assert( sock < nSockets );
	assert( sockets[sock] );



	if( toAddr ) {
		assert( toAddr );
		assert( socketTypeList[sock] == SOCKET_HOST );
		rVal =  sendto(		sockets[sock],
							buffer,
							size ,
							0,
							toAddr,
							sockAddrSize);
	}
	else	{
		assert( !toAddr );
		assert( socketTypeList[sock] == SOCKET_CLIENT );
		rVal = send(		sockets[sock],
							buffer,
							size,
							0 );
	}

	if( rVal < 0 ) {
		SetSysErr();
		return 0;
	}

	return rVal;

}
unsigned int BaseNetworking::RecieveData( UINT sock,int size,char * recvBuf , sockaddr* fromAddr ) {
	assert(recvBuf );
	assert(size >= 0 );
	ZeroMemory( recvBuf,size);
	int rVal;

	assert( sock < nSockets );
	assert( sockets[sock] );


	if( fromAddr ) {
		assert( socketTypeList[sock] == SOCKET_HOST );
		assert( fromAddr );
		rVal = recvfrom(		sockets[sock],
								recvBuf,
								size,
								0,
								fromAddr,
								&sockAddrSize);

	}
	else {
		assert( socketTypeList[sock] == SOCKET_CLIENT );
		assert( !fromAddr );
		rVal = recv(		sockets[sock],
							recvBuf,
							size,
							0);
	}

	if( rVal == SOCKET_ERROR) {
		if( IsErrorWait() )
			return 0;
		else {
			SetSysErr();
			return 0;
		}
	}
	else
		return rVal;
}
void BaseNetworking::InitHostSocket(UINT sock,  UINT port) {
	assert( sock < nSockets );
	assert( !sockets[sock] );

	sockets[sock] = socket( AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if( sockets[sock] == INVALID_SOCKET ) {
		SetSysErr();
		return;
	}
	socketTypeList[sock] = SOCKET_HOST;

	sockaddr_in local;
	memset( &local,0,sockAddrSize);
	local.sin_family = AF_INET;
#if PLATFORM == PLATFORM_WINDOWS
	local.sin_addr.S_un.S_addr = htonl( INADDR_ANY );
#else
	local.sin_addr.s_addr = htonl( INADDR_ANY );
#endif
	local.sin_port = htons(port);

	if( bind( sockets[sock],(sockaddr*)&local,sockAddrSize)) {
		SetSysErr();
		return;
	}
#if PLATFORM == PLATFORM_WINDOWS
	DWORD nonBlocking = 1;
	if( ioctlsocket( sockets[sock],FIONBIO,&nonBlocking) == SOCKET_ERROR) {
		SetSysErr();
		return;
	}
#else
	if( fcntl( sockets[sock], F_SETFL,O_NONBLOCK)== SOCKET_ERROR) {
		SetSysErr();
		return;
	}
#endif
}
void BaseNetworking::InitClientSocket(UINT sock, unsigned long IPAddress, UINT port ) {
	assert( sock < nSockets );
	assert( !sockets[sock] );




	sockets[sock] = socket( AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if( sockets[sock] == INVALID_SOCKET ) {
		SetSysErr();
		return;
	}
	socketTypeList[sock] = SOCKET_CLIENT;


	sockaddr_in IPData;
	memset( &IPData,0,sockAddrSize);
#if PLATFORM == PLATFORM_WINDOWS
	IPData.sin_addr.S_un.S_addr =  IPAddress ;
#else
	IPData.sin_addr.s_addr = IPAddress;
#endif
	IPData.sin_family = AF_INET;
	IPData.sin_port = htons( port );

	if( connect(  sockets[sock],(sockaddr*)&IPData,sockAddrSize) == SOCKET_ERROR) {
		SetSysErr();
		return;
	}
#if PLATFORM == PLATFORM_WINDOWS
	DWORD nonBlocking = 1;
	if( ioctlsocket( sockets[sock],FIONBIO,&nonBlocking)== SOCKET_ERROR ) {
		SetSysErr();
		return;
	}
#else
	if(fcntl( sockets[sock], F_SETFL,O_NONBLOCK)== SOCKET_ERROR ) {
		SetSysErr();
		return;
	}
#endif
}
void BaseNetworking::DestroySocket( UINT sock ) {
	assert( sock < nSockets );
	assert( sockets[sock] );


	if( closesocket( sockets[sock] )== SOCKET_ERROR ) {
		SetSysErr();
		return;
	}
	sockets[sock] = 0;
}
void BaseNetworking::SetSysErr() const{
	SetCritErr( GetSysError() );
}
bool BaseNetworking::IsErrorWait() {
#if PLATFORM == PLATFORM_WINDOWS
	int error = GetLastError();
	if( error == WSAEWOULDBLOCK )
		return true;
#else
	if (errno == EAGAIN)
		return true;
#endif
	return false;
}

