#ifndef StdPacket_H
#define StdPacket_H

#include "stdlib/platform.h" //platform windows or linux
#include <vector>	//std::vector
#include "stdlib/error.h"	//SetError
#include <stdint.h>	//uint16_t
#if PLATFORM == PLATFORM_LINUX
#include <arpa/inet.h>	//htons,ntohs
#include <endian.h>
#elif PLATFORM == PLATFORM_WINDOWS
#include <winsock2.h> //htons,ntohs
#endif
#include <cstring>	//memcpy
#include <assert.h>

class StdOPacket;
class StdIPacket;

template <typename T> 
StdOPacket& operator <<( StdOPacket& p, const T& item );
template <typename T> 
StdIPacket& operator >>( StdIPacket& p, T& item );

typedef uint32_t 			PacketLen;

class PacketInputErr {};

class StdVPacket {
public:	

	StdVPacket();
	StdVPacket( const char * buf, unsigned int len );
	virtual ~StdVPacket();
	const char * GetBuf() const;
	void Clear();	//clears buffer
	PacketLen len() const;
protected:
	std::vector<char> msgBuf;
};

class StdOPacket : public StdVPacket {
	template <typename T> 
	friend StdOPacket& operator<<(StdOPacket& p,const T& item);
public:
	StdOPacket( const char * buf, unsigned int len );
	StdOPacket();
	virtual ~StdOPacket();
protected:
	void AddData( const void * data, unsigned int datSize );
};

class StdIPacket : public StdVPacket {
	template <typename T> 
	friend StdIPacket& operator >>( StdIPacket& p, T& item );
public:
	StdIPacket( const char * buf, unsigned int len );
	virtual ~StdIPacket();
protected:
	void PrcData( void * data,unsigned int datSize ); 
	unsigned int place;
};


#endif
