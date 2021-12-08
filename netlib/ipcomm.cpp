#include "networking.h"

BNP::IPComm::IPComm(unsigned int numberOfLists, std::vector<SockData> socketList)
:  	networker(socketList),
	nLists( numberOfLists ),
	UPTimer( BNP::TO_SENDMILLI ),
	HBTimer( BNP::TO_HEARTBEAT ),
	curUpListID(0),
	requestID(0) {
	contactLists.resize(numberOfLists );
}
BNP::IPComm::~IPComm() {
	for( unsigned int i=0;i<contactLists.size();i++) {
		for( unsigned int y=0;y<contactLists[i].size();y++)
			delete contactLists[i][y];
	}
	for( unsigned int i=0;i<updateList.size();i++) {
		delete updateList[i];
	}
}
BNP::IPComm::ReceievePacket::ReceievePacket()
	:NetIPacket() {
		msgBuf.resize( RCV_MAX_SIZE, 0 );
}
char * BNP::IPComm::ReceievePacket::GetInBuf() {
	return (char*)&msgBuf[0];
}
int BNP::IPComm::ReceiveData( unsigned int socketIndex, ReceievePacket& in,sockaddr*src ) {
	unsigned int r = networker.RecieveData( socketIndex,in.len(),in.GetInBuf(),src);
	if( r == 0 )
		return 0;
	else {
		if( r < byte_START ) {
			AddErr("packet smaller than required core size receieved");	
			return -1;
		}
		else
			return r;
	}
}
BNP::IPContact * BNP::IPComm::GetContact( ContPos pos ) const {
	assert( ContactInRange(pos));
	return contactLists[ pos.lIndex ][ pos.cIndex ];
}
bool BNP::IPComm::ContactInRange( ContPos pos ) const {
	if( pos.lIndex < contactLists.size() ) {
		if( pos.cIndex < contactLists[ pos.lIndex ].size() )
			return true;
	}
	return false;
}
void BNP::IPComm::MoveContact( ContPos source, ListPos dstL ) {
	assert( ContactInRange(source));
	assert( dstL < contactLists.size() );
	contactLists[dstL].push_back( contactLists[source.lIndex][source.cIndex] );
	contactLists[source.lIndex].erase( contactLists[source.lIndex].begin() + source.cIndex );
}
void BNP::IPComm::AddList() {
	nLists++;
	std::vector< IPContact* > a;
	contactLists.push_back( a );
}
void BNP::IPComm::RmList( ListPos listIndex ) {
	assert( listIndex < nLists );
	for( unsigned int c = 0; c < contactLists[listIndex].size(); c++ ) {
		delete contactLists[listIndex][c];
	}

	contactLists.erase( contactLists.begin() + listIndex );
	nLists--;
}
void BNP::IPComm::Std_SndData() {

	for( UINT u = 0; u < updateList.size(); ) {
		bool updateSent = true;
		UpdateReference updateRef = updateList[u]->updateID;
		for( UINT l = 0; l < nLists; l++ ) {
			if( !IsUpdateSent( contactLists[l],	updateRef))
				updateSent = false;
		}

		if( updateSent ) {
			delete updateList[u];
			updateList.erase( updateList.begin() + u );
		}
		else
			u++;
	}
		
	for( UINT l = 0; l < nLists; l++ ) {
		SendUpdates(contactLists[l],true);
	}	
	
	if( HBTimer.HasTimedOut() ) {
		heartbeat.Update();
		for( UINT l = 0; l < nLists; l++ )
			SendHeartBeats( contactLists[l] );
	}

	if( UPTimer.HasTimedOut() ) {
		for( UINT l = 0; l < nLists; l++ ) {
			SendUpdates( contactLists[l],false );
			SendUpdateAccepts( contactLists[l] );
		}
	}
}
int BNP::IPComm::Std_RcvData(ContPos pos,  NetIPacket& packet,bool& prc) {
	assert( ContactInRange(pos) );
	
	IPContact& p = *GetContact(pos);
	inputPacket.Clear();

	int r = ReceiveData( p.socketID,inputPacket );
	prc = true;

	if( r<0 )
		return r;
	if( r == 0 )
		if( p.GetNxtUpdate( packet ) ) {	//try and find stored update from previously
			prc = false;
			return packet.len();

		}
		else
			return 0;
	else {//r > 0
		packet = NetIPacket(inputPacket.GetBuf(),r);
		prc =  p.ProcessRcvData( packet );
		if( prc ) {
			packet.Clear();
			return -1;
		}
		return packet.len();
	}
}
void BNP::IPComm::AddContact_Client( ListPos listIndex, const NetIPacket& request, unsigned int sockIndex )
{
	assert( listIndex < nLists );
	IPContact * c = new IPContact( request,sockIndex,false );
	contactLists[listIndex].push_back(c);
}
void BNP::IPComm::RemoveContact( ContPos pos ) {
	assert( ContactInRange( pos ));

	delete GetContact( pos );
	contactLists[ pos.lIndex ].erase( contactLists[pos.lIndex].begin() + pos.cIndex );
}
bool BNP::IPComm::HasTimedOut( ContPos pos ) {
	assert( ContactInRange( pos ) );
	return GetContact( pos )->HasTimedOut();
}
bool BNP::IPComm::IsUpdateSent( std::vector<IPContact*>& list ,UpdateReference ref ) {
		
	for(unsigned  int playerIndex = 0; playerIndex < list.size(); playerIndex++ ){
		IPContact& cur = *list[playerIndex];
		if( cur.FindUpdate_RefID( ref) >= 0 ){
			return false;
		}
	}
	return true;
}
BNP::NetOPacket BNP::IPComm::FormRequest() {
	requestID++;

	PacketCore msg;
	ZeroCore(msg);
	msg[ byte_CM] = CM_NORMREQUEST;
	msg[ byte_RQID] = requestID;
	msg[ byte_RQHB] = heartbeat.GetPacketID();
	
	return NetOPacket( msg );

}
void BNP::IPComm::AddUpdate( int updateType, const StdOPacket& packet , bool forceSend ) {
	curUpListID++;
	BNP::Update * u = new BNP::Update( curUpListID, updateType,packet,forceSend );
	updateList.push_back(u );
}
void BNP::IPComm::InitCurUpdate( ){
	for( UINT i = 0; i < nLists; i++ ){
		InitCurUpdate( i );
	}

}
void BNP::IPComm::InitCurUpdate( ListPos listIndex ) {
	
	assert( listIndex < nLists );	//accidentally entered a too big value to the list index?

	//loops through all players adding current update ID to their list of required updates
	for(unsigned int pIndex = 0; pIndex < contactLists[listIndex].size(); pIndex++ ){
		InitCurUpdate( ContPos(listIndex,pIndex));
	}
}
void BNP::IPComm::InitCurUpdate( ContPos pos ) {
	assert( updateList.size() );
	assert( ContactInRange(pos ));
	GetContact( pos )->AddUpdate( curUpListID );
}
int BNP::IPComm::SendPacket(unsigned int socketIndex,  NetOPacket& packet, const sockaddr * to )  {
	packet.SetTimeStamp();
	return networker.SendData( socketIndex,packet.len(),packet.GetBuf(),to );
}
int BNP::IPComm::SendPacket( const IPContact& contact ,  NetOPacket& packet )  {
	return SendPacket(	contact.socketID,packet,contact.ContactIPData() );
}
int BNP::IPComm::SendPacket( const IPContact& contact, const PacketCore c )  {
	NetOPacket p( c );
	return SendPacket( contact,p);
}
void BNP::IPComm::SendHeartBeats(  std::vector<IPContact*>& list   ) {
	for(unsigned int contact = 0; contact < list.size(); contact++ ) {
		SendPacket( *list[contact],heartbeat.GetCore());
	}
}
void BNP::IPComm::SendUpdates( std::vector<IPContact*>& list ,bool forceSend) {

	
	for( UINT u = 0; u < updateList.size(); u++ ){
		UpdateReference refID = updateList[u]->updateID;
		if( updateList[u]->forceSend != forceSend )
			continue;	//forceSend does not match update List element

		for( unsigned int cIndex=0; cIndex < list.size(); cIndex++ ){
			IPContact& cur = *list[cIndex];
						
			int updateIndex = cur.FindUpdate_RefID( refID );
			if( updateIndex >= 0 ) {
				UpdateNumber upID = cur.GetUpdateID( updateIndex );
				SendPacket( cur,updateList[u]->GetUpdate( upID ));
			}
		}
	}
}
void BNP::IPComm::SendUpdateAccepts( std::vector<IPContact*>& list ){

	PacketCore core;
	ZeroCore(core);
	core[ BNP::byte_CM ] = BNP::CM_UPDATE_RCV;
	char& updateID = core[ BNP::byte_PN ];
	
	for( unsigned int cIndex=0 ; cIndex < list.size(); cIndex++ ){
		IPContact& cur = *list[cIndex];
		for(UINT updateIndex = 0; updateIndex < cur.GetNUpdateAcpt(); updateIndex++ ) {
			updateID = cur.GetUpdateAcptID( updateIndex );
			SendPacket( cur, core );  
		}
		cur.ClearAcptList();
	}
}
bool BNP::IPComm::Connect(unsigned int socketIndex,byte& denyReason, const StdOPacket& usrOut, NetIPacket& in   ) {


	NetOPacket fullOut( FormRequest( ) );
	fullOut << (StdOPacket&)usrOut;

	SendPacket( socketIndex,fullOut);
	
	Timeout_Timer timer( TO_CLIENT_CONNECT );

	in = BNP::NetIPacket();	//set length to 0

	while( !timer.HasTimedOut() ) {
		int len = ReceiveData( socketIndex, inputPacket   );
		if( len <= 0 )
			continue;
		//len > 0			
		
		PacketCore core;
		inputPacket.GetCore( core );

		if(requestID != core[BNP::byte_RQID] )
			continue;
		//request ID = received ID		
			
		if( core[BNP::byte_CM] == BNP::CM_REPLY_SUCCESS ) {
			in = NetIPacket( inputPacket.GetBuf(),len);
			return true;
		}
		else if( core[BNP::byte_CM] == BNP::CM_REPLY_FAIL ) {
			denyReason = core[BNP::byte_RQDR];
			return false;
		}
		
	}

	return 0;

}



