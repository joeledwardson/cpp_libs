#include "networking.h"

const BNP::ContPos BNP::IPComm_StdClient::contactPos = BNP::ContPos(0,0);

BNP::IPComm_StdClient::IPComm_StdClient( unsigned long ipAddr, unsigned short port ) 
	:IPComm(1, GetSockList(ipAddr,port)) {
}
BNP::IPComm_StdClient::~IPComm_StdClient() {
}
void BNP::IPComm_StdClient::Std_SndData() {
	IPComm::Std_SndData();
}
int BNP::IPComm_StdClient::Std_RcvData(NetIPacket& packet,bool& prc) {
	return IPComm::Std_RcvData( contactPos,packet,prc);
}
bool BNP::IPComm_StdClient::HasTimedOut() {
	return IPComm::HasTimedOut(contactPos);
}
int BNP::IPComm_StdClient::SendPacket(  NetOPacket& packet )  {
	return IPComm::SendPacket( *GetContact(contactPos),packet);
}
void BNP::IPComm_StdClient::AddUpdate( int updateType, const StdOPacket& packet , bool forceSend) {
	return IPComm::AddUpdate( updateType,packet,forceSend);
}
void BNP::IPComm_StdClient::InitCurUpdate() {
	IPComm::InitCurUpdate();
}
bool BNP::IPComm_StdClient::Connect( byte& denyReason, const StdOPacket& usrOut, NetIPacket& in  ) {
	return IPComm::Connect( 0,denyReason,usrOut,in);
}
std::vector<SockData> BNP::IPComm_StdClient::GetSockList(unsigned long ipAddr, unsigned short port) {
	std::vector<SockData>sckList;
	sckList.push_back( SockData(port,SOCKET_CLIENT,ipAddr));
	return sckList;
}
