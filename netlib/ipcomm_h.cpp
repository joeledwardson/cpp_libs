#include "networking.h"

BNP::IPComm_HostType::IPComm_HostType(unsigned int numberOfLists, std::vector<SockData> socketList)
	:IPComm(numberOfLists, socketList )
{}
BNP::IPComm_HostType::~IPComm_HostType()
{

}
unsigned int BNP::IPComm_HostType::GetNLists() {
	return nLists;
}
BNP::NetOPacket BNP::IPComm_HostType::FormReplyFailure(	NetIPacket& request, char denyReason ){ 
	PacketCore msgCore;
	request.GetCore(msgCore);	//gets byte at index byte_RQID - id of request
	msgCore[byte_CM]	= BNP::CM_REPLY_FAIL;
	msgCore[byte_RQDR]	= denyReason;
	return NetOPacket( msgCore );
	
}
BNP::NetOPacket BNP::IPComm_HostType::FormReplySuccess( NetIPacket& request) {
	PacketCore msgCore;
	request.GetCore( msgCore );
	msgCore[byte_CM]	= BNP::CM_REPLY_SUCCESS;
	msgCore[byte_RQHB]	= heartbeat.GetPacketID();
	return NetOPacket( msgCore );
}
bool BNP::IPComm_HostType::FindContact(ContPos& pos, sockaddr * IPData ) {

	for( pos.lIndex = 0; pos.lIndex < contactLists.size(); pos.lIndex++ ){
		for(  pos.cIndex = 0; pos.cIndex < contactLists[pos.lIndex].size(); pos.cIndex++ ){
			if( !memcmp(IPData, contactLists[pos.lIndex][pos.cIndex]->ContactIPData(),sizeof(sockaddr))){
				return true;
			}
		}
	}
	return false;

}
void BNP::IPComm_HostType::AddContact_Host( 	UINT listIndex, const NetIPacket& request, unsigned int sockID,sockaddr socketData )
{
	assert( listIndex < nLists );
	IPContact * c = new IPContact( request,sockID,true,&socketData );
	contactLists[listIndex].push_back( c );
}
int BNP::IPComm_HostType::RecvData( ContPos& pos,unsigned int sock,  bool& isConnected, NetIPacket& packet ) {
	int r = ReceiveData( sock,inputPacket,&sData );
	isConnected = false;
	packet = NetIPacket();	//wipe

	if( r < 0 )
		return -1;
	if(r == 0 ) {
		for( pos.lIndex = 0; pos.lIndex < nLists; pos.lIndex++ ) {
			for( pos.cIndex = 0; pos.cIndex < contactLists[pos.lIndex].size(); pos.cIndex++ ) {
				if( GetContact( pos )->GetNxtUpdate( packet )) {
					isConnected = true;
					return packet.len();
				}
			}
		}
		return 0;
	}
	else { //r>0
		isConnected	= FindContact( pos,&sData );
		packet = NetIPacket( inputPacket.GetBuf(),r);
		if( isConnected ) {
			if( GetContact( pos )->ProcessRcvData( packet )) {
				packet.Clear();
				return -1;
			}
		}
		return packet.len();
	}
	
}
sockaddr BNP::IPComm_HostType::GetSockAddr() {
	return sData;
}

