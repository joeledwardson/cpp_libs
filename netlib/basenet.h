#ifndef BASENET_H
#define BASENET_H

#include "stdlib/error.h"
#include "stdlib/platform.h"	//linux or windows
#include "timer.h"	//used in BNP::Timeout_Timer
#include <assert.h>	//use of assert() in function calls
#include <vector>	//vector used for list of contacts
#include <stdint.h> //all intenger types e.g. uint16_t_t
#include <string.h>	// memset()...

#if COMPILER == COMPILER_VS
	//all windows socket definitions
	#pragma comment(lib,"ws2_32.lib")
	#include <Winsock2.h>
	#include <Windows.h>
#elif PLATFORM==PLATFORM_LINUX
	#include <unistd.h> //close()
	#define closesocket close	//closesocket() is windows close() equivalent. saves preprocessor OS logic
	#include <sys/socket.h>	//sockaddr type
	#include <netdb.h>	//hostent type
	#include <fcntl.h>	//fcntl(), O_NONBLOCK etc
	#include <errno.h>	//errno, strerror() etc
	typedef sockaddr_in * PSOCKADDR_IN;
#endif

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET ~0
#endif

#ifndef SOCKET_ERROR
	#define SOCKET_ERROR -1
#endif

enum SocketType{
	SOCKET_HOST,
	SOCKET_CLIENT,
};
struct SockData {
	SockData(unsigned int _port,SocketType _typ,unsigned long _IPAddress=0);
	SocketType typ;
	unsigned long IPAddress;
	unsigned int port;
};



/*base networking class. used for game interaction for sending/recieving
data and controls address data for server. UDP NON BLOCKING SOCKETS ONLY */
class BaseNetworking
{
public:


	BaseNetworking(std::vector<SockData> socketList);

	virtual ~BaseNetworking();

	/* sends data (buffer param) to a speficied address and port (toAddr param)
	with NULL char appended. If toAddr is NULL, function uses server
	structure for sending. INCLUDE 4 byte GAME INDENTIFYING byteS
	returns > 0 on succes, -1 if failure.
	*/
	int SendData( UINT sock, int size,const char* buffer,const sockaddr* toAddr = NULL) const;
	/* sends data for a bound socket. CANNOT BE HOST SOCKET. */

	/* recieves data from specified location (fromAddr). if fromAddr is NULL, function uses server address structure.
	returns NULL if no data	recieved. sock = index of socket to be used in socket list
	returns number of bytes revieced if all goes well, 0 if nothing recieved, and -1 if error
	*/
	unsigned int RecieveData(UINT sock,int size,char * recvBuf, sockaddr* fromAddr = NULL );

private:
	void InitClientSocket(UINT sock, unsigned long IPAddress, UINT port );
	void InitHostSocket( UINT sock, UINT port );
	void DestroySocket( UINT sock );

	bool IsErrorWait();

	void SetSysErr() const;

#if PLATFORM == PLATFORM_WINDOWS
	int sockAddrSize;
#else
   	socklen_t sockAddrSize;
#endif

	const unsigned int nSockets;
	int * sockets;
	SocketType * socketTypeList;

};

#endif
