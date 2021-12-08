#include "packet.h"

StdVPacket::StdVPacket(const char * buf, unsigned int len ){
	msgBuf.resize(len);
	for( unsigned int i = 0;i<len;i++ )
		msgBuf[i]=buf[i];
}
StdVPacket::StdVPacket() {
}
StdVPacket::~StdVPacket() {
}
const char * StdVPacket::GetBuf() const {
	if( !msgBuf.size() )
		return NULL;
	else {
		return &msgBuf.front();
	}
}
void StdVPacket::Clear() {
	if( msgBuf.size() ) {
		memset( &msgBuf[0],0,sizeof(char)*msgBuf.size());
	}
}
PacketLen StdVPacket::len() const {
	return msgBuf.size();
}

StdOPacket::StdOPacket() {
}
StdOPacket::~StdOPacket() {
}
StdOPacket::StdOPacket( const char * buf, unsigned int len )
	:StdVPacket( buf, len ) {
}
void StdOPacket::AddData( const void * data, unsigned int datSize ) {
	if( !data )
		return ;
	if( !datSize )
		return ;
	msgBuf.resize( msgBuf.size() + datSize );	
	memcpy( &msgBuf.at( msgBuf.size() - datSize ), data, datSize );
}

StdIPacket::~StdIPacket() {
}
StdIPacket::StdIPacket( const char * buf, unsigned int len )
	:StdVPacket( buf, len ),
	place(0) {
}
void StdIPacket::PrcData( void * data,unsigned int datSize ) {
		
	if( msgBuf.size() < (place + datSize) ) {
		throw PacketInputErr();
		return;
	}
	memcpy( data, &msgBuf.at( place ), datSize );
	place += datSize;
	return;
}

template <typename T> 
StdOPacket& operator << ( StdOPacket& p, const T& item ) {
	p.AddData( &item, sizeof(T));
	return p;
}
template <typename T> 
StdIPacket& operator >> ( StdIPacket& p, T & item ) {	
	p.PrcData( &item, sizeof( T ) );
	return p;
}

template<>
StdOPacket& operator <<( StdOPacket& p, const std::string& item ) {
	p.AddData( item.c_str(), item.length() + 1);
	return p;
}
template<>
StdIPacket& operator >>( StdIPacket& p, std::string& item ) {
	unsigned int i = p.place;
	while( i < p.msgBuf.size() ) {
		if( p.msgBuf[i] == '\0' ) {
			//null terminator found
			item = std::string( (const char*)&p.msgBuf[p.place] );
			p.place = i + 1;
			return p;
		}
		i++;
	}
	//null terminator not found
	throw PacketInputErr();
	return p;
}

template <> 
StdOPacket& operator <<( StdOPacket& p,const uint16_t& item ) {
	uint16_t netItem = htons(item);
	p.AddData( &netItem, sizeof(item));
	return p;
	
}
template <> 
StdIPacket& operator >>( StdIPacket& p, uint16_t& item ) {
	uint16_t netItem;
	p.PrcData( &netItem, sizeof( uint16_t ) );
	item = ntohs( netItem );
	return p;
}

template <> 
StdOPacket& operator <<( StdOPacket& p, const uint32_t& item ) {
	uint32_t netItem = htonl(item);
	p.AddData( &netItem, sizeof(item));
	return p;
	
}
template <> 
StdIPacket& operator >>( StdIPacket& p, uint32_t& item ) {
	uint32_t netItem;
	p.PrcData( &netItem, sizeof( item ) );
	item = ntohl( netItem );
	return p;
}

template <> 
StdOPacket& operator <<( StdOPacket& p, const uint64_t& item ) {
	uint64_t netItem;
#if PLATFORM==PLATFORM_WINDOWS
	netItem= htonll(item);
#elif PLATFORM==PLATFORM_LINUX
	netItem=htobe64( item );
#endif
	p.AddData( &netItem, sizeof(item));
	return p;
	
}
template <> 
StdIPacket& operator >>( StdIPacket& p, uint64_t& item ) {
	uint64_t netItem;
	p.PrcData( &netItem, sizeof( item ) );
#if PLATFORM==PLATFORM_WINDOWS
	item = ntohll( netItem );
#elif PLATFORM==PLATFORM_LINUX
	item=be64toh( netItem );
#endif
	return p;
}

template
StdOPacket& operator << <uint8_t>( StdOPacket& p, const uint8_t& item );
template
StdIPacket& operator >> <uint8_t>( StdIPacket& p, uint8_t & item );

template
StdOPacket& operator << <bool>( StdOPacket& p, const bool& item );
template
StdIPacket& operator >> <bool>( StdIPacket& p, bool & item );

template<>
StdOPacket& operator <<( StdOPacket& p, const StdOPacket& add ) {
	p.AddData( add.GetBuf() , add.len() );
	return p;
}
