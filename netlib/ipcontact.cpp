#include "networking.h"

BNP::IPContact::IPContact(const NetIPacket& request , unsigned int sockID, bool isHostContact,sockaddr * s )
	:socketID(sockID),
	timeout(TO_GENERAL),
	hostContact( isHostContact ) {
	if( s )
	{
		assert( hostContact );
		memcpy( &socketData, s, sizeof(sockaddr));
	}

	PacketCore core;
	request.GetCore(core);
	curHBID = core[byte_RQHB];
	curUPID = 0;
	slfUPID = 0;
}
BNP::IPContact::~IPContact()
{

}
int BNP::IPContact::IsPacketValid( byte cmType, PacketNumber packetID  ) {
	assert( cmType == CM_HEARTBEAT || cmType == CM_UPDATE_SND );
	PacketNumber nxtID;

	if( cmType == CM_HEARTBEAT )
		nxtID = PacketNumber( curHBID + 1);
	else if( cmType == CM_UPDATE_SND )
		nxtID = PacketNumber( curUPID + 1);

	int dif = packetID - nxtID;


	if( dif < 0 )
		dif += std::numeric_limits< uint8_t >::max();


	if( nxtID == packetID )
		return PV_VALID;
	else if( dif > 0 && dif < nPacketLag )
		return PV_FUTURE;
	else
		return PV_INVALID;
}
bool BNP::IPContact::ProcessRcvData( const NetIPacket& packet){
	PacketCore core;
	packet.GetCore(core);
	byte cmType = core[byte_CM];
	UpdateNumber updateID = core[byte_UP];
	PacketNumber hbID = core[byte_PN];

	if( cmType == CM_HEARTBEAT ){
		int pValid = IsPacketValid(CM_HEARTBEAT,hbID);
		if( pValid == PV_VALID || pValid == PV_FUTURE){
			//doesnt matter if future packet, all heartbeats the same
			curHBID++;
			timeout.ResetWatch();
		}
		return true;
	}
	else if(cmType == CM_UPDATE_RCV ){
		timeout.ResetWatch();
		int index = FindUpdate_UPID( updateID);
		if(  index >= 0  ){
			updateIDList.erase( updateIDList.begin() + index );
		}
		return true;
	}
	else if( cmType == CM_UPDATE_SND) {
		timeout.ResetWatch();
		int index = FindAcceptID( updateID );

		if( index < 0 ) {
			updateAcceptList.push_back(updateID);
		}
		
		//if ready to send,send acceptance
		int pValid = IsPacketValid( CM_UPDATE_SND,updateID);
		if( pValid == PV_VALID ) 	{
			int64_t curTs,sndTs,difTs;
			curTs=TimeStamp();
			sndTs=packet.GetTimeStamp();
			difTs=curTs-sndTs;
			NETVBSE<<"valid packet of ID "<<int(updateID)<<" recvd. curUPID now set to "<<int(byte(curUPID+1))<<", packet lag="<<difTs<<std::endl;
			//packet is a valid update
			curUPID++;
			return false;
		}
		else if( pValid == PV_FUTURE )	{
			NETVBSE<<"future packet received, cur ID="<<int(curUPID)<<"packet ID="<<int(updateID)<<std::endl;
			bool packInList = false;
			for( unsigned int i = 0; i < futureUpdates.size(); i++ ) {
				if( futureUpdates.at(i).GetBuf()[BNP::byte_PN] == updateID ) {
					packInList = true;
					break;
				}
			}
			if( !packInList )
				futureUpdates.push_back( packet );				
		}
		else {
			NETVBSE<<"duff packet received, current ID="<<int(curUPID)<<", packet ID="<<int(updateID)<<std::endl;
		}
		return true;
	}
	else{
		//packet not in any format to be processed here
		timeout.ResetWatch();
		return false;;
	}

}
const sockaddr * BNP::IPContact::ContactIPData() const {
	if( hostContact )
		return &socketData;
	else
		return NULL;
}
bool  BNP::IPContact::GetNxtUpdate( NetIPacket& packet ) {
	byte nxtUPID = byte( curUPID + 1 );
	for( uint32_t i = 0; i < futureUpdates.size(); i++ ){
		const char * buf = futureUpdates.at(i).GetBuf();
		//furutureupdates element must be erased before end of function
		byte ID = buf[ byte_UP ];
		if( ID == nxtUPID ){
			NETVBSE<<"returned ID "<<int(ID)<<" to system. curUPID now set to value"<<std::endl;
			curUPID = nxtUPID;
			packet = NetIPacket( futureUpdates.at(i) );
			futureUpdates.erase( futureUpdates.begin() + i );
			return true;
		}
	}
	return false;
}
unsigned int BNP::IPContact::GetNUpdateAcpt() {
	return updateAcceptList.size();
}
void  BNP::IPContact::ClearAcptList() {
	updateAcceptList.clear();
}
int  BNP::IPContact::FindAcceptID( UpdateNumber updateAcptID ) {
	return FindElement<UpdateNumber>(updateAcceptList,updateAcptID);
}
BNP::PacketNumber BNP::IPContact::GetUpdateAcptID( unsigned int IDindex ) {
	assert( IDindex < updateAcceptList.size() );
	return updateAcceptList[IDindex];
}
unsigned int  BNP::IPContact::GetNUpdateSnd() {
	return updateIDList.size();
}
int  BNP::IPContact::FindUpdate_RefID( UpdateReference ref) {
	for( unsigned int i=0;i<updateIDList.size();i++) {
		if( updateIDList[i].first==ref)
			return i;
	}
	return -1;
}
int  BNP::IPContact::FindUpdate_UPID( UpdateNumber updateID ) {
	for( unsigned int i=0;i<updateIDList.size();i++) {
		if( updateIDList[i].second==updateID)
			return i;
	}
	return -1;
}
BNP::UpdateReference  BNP::IPContact::GetUpdateRef( unsigned int refIndex ) {
	assert( refIndex < updateIDList.size() );
	return updateIDList[refIndex].first;
}
BNP::PacketNumber  BNP::IPContact::GetUpdateID( unsigned int IDIndex ) {
	assert( IDIndex < updateIDList.size() );
	return updateIDList[IDIndex].second;
}
bool  BNP::IPContact::HasTimedOut() {
	return timeout.HasTimedOut();
}
void  BNP::IPContact::AddUpdate( UpdateReference updateRef )
{
	slfUPID++;
	updateIDList.push_back( std::pair<UpdateReference,UpdateNumber>( updateRef, slfUPID) );
}
