#include "gamenet.h"

GNP::Game::Game()
	:maxPlayers(DEF_MAXPLAYERS),
	noOfPlayers(DEF_NPLAYERS),
	state(DEF_STATE)
{}
std::string GNP::ToStr( const GNP::REPLY_TYPES& denyReason ) {
	switch (denyReason)
	{
	case GNP::FAIL_INGAME:
		return "instance is in game";
	case GNP::FAIL_FULL:
		return "server is full";
	case GNP::FAIL_NAMETAKEN:
		return "name taken";
	case GNP::FAIL_NOTFOUND:
		return "instance not found";
	case GNP::FAIL_NODATA:
		return "no instance data found";
	case GNP::REPLY_TYPES_END:
	default:
		SetCritErr( "parsing deny reason to string found, out of range");
		return "";
	}
}
std::vector<SockData> GNP::SvrSockList() {
	std::vector<SockData> sck;
	sck.push_back( SockData( PORT_SVR,SOCKET_HOST ));
	return sck;
}
GNP::ChatPacket::ChatPacket( )
	:soundIndex( 0 ) {
}
//by default - only allowed to process input ChatPackets, client not allowed to packet them
template<> 
StdIPacket& operator>>( StdIPacket& p, GNP::ChatPacket& c ) {
	p >> c.soundIndex >> c.chatMsg;
	return p;
}

template<>
StdOPacket& operator<<( StdOPacket& p, const GNP::Game& h ) {
	p << h.lobName << h.maxPlayers << h.noOfPlayers << h.state;
	return p;
}
template<>
StdIPacket& operator>>( StdIPacket& p, GNP::Game& h ) {
	p >> h.lobName >> h.maxPlayers >> h.noOfPlayers >> h.state;
	return p;
}


