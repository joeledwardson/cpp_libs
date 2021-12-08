
#include "Networking.h"

std::vector< BNP::Format >formatList;


void BNP::AddFormat( PacketLen (*func)(char* buf,char* element ),char c )
{
	formatList.push_back( Format(func,c));
}


ULONG BNP::GetIpFromHostName( const char * hostName )
{
	
	sockaddr_in theIp;

	hostent * theHost;

	theHost =  	gethostbyname( hostName );

	if( theHost )
	{
		memcpy( (void*) &theIp.sin_addr,theHost->h_addr_list[0],theHost->h_length );
#if PLATFORM == PLATFORM_WINDOWS
		return theIp.sin_addr.S_un.S_addr;
#else
		return theIp.sin_addr.s_addr;
#endif
	}
	else
	{
		return 0;
	}
	
}
#if PLATFORM == PLATFORM_WINDOWS
bool BNP::GetExternalIP(ULONG& externalIP)
{

	

	struct IPWebsite
	{
		//szUniquePreIPText - CAN appear more than once. first appearance must directly preceed IP address string
		//afterIPChar - character directly proceeding ip address string
		char  * szUniquePreIPText, * afterIPChar;
		//szUrl - url of site to download
		wchar_t * szURL;
		IPWebsite( wchar_t * szURL, char * szUniquePreIPText, char * afterIPChar )
		{
			this->szURL = szURL;
			this->szUniquePreIPText = szUniquePreIPText;
			this->afterIPChar = afterIPChar;
			assert( strlen(afterIPChar) == 1 );
		}
		bool GetIPFromSite(ULONG& ipAddress)
		{
			char szPath[256];		//path of temp file contaning site HTML
			wchar_t wszPath[256];	//path in widestring form
			GetTempPath(256, wszPath);	//store temporary path
			lstrcatW(wszPath, L"my_ip.txt");	//add file name to path
			wcstombs( szPath,wszPath,lstrlenW(wszPath)+1);	//convert to ANSII string

			//function URLDOwnloadToFIle() if succeeds stores HTML in given path
			if( URLDownloadToFile( 0,szURL,wszPath,0,0 ) == S_OK )
			{
				const int len = 3000;
				char szRead[len];		//buffer to store html
				ZeroMemory(szRead,len );

				char * Token;

				FILE * file = fopen( szPath,"r" );	//open file in read mode
				if( file )
				{
					int preLen = strlen(szUniquePreIPText);	//length of pre string

					fread(szRead,len,1,file );	//move HTML into read buffer

					fclose(file);	//close file
					remove(szPath);	//delete temp file containing html

					//function finds first appearance of pre-string in szRead and returns pointer to store in Token
					Token = strstr(szRead,szUniquePreIPText);	
					if( !Token )
						return false;

					Token = Token + preLen;	//token is incremented to after the pre-string (at the place of the ip address)
					//strtok() splits string where afterIPChar is found. therefore it should return a pointer to just the IP address
					Token = strtok( Token,afterIPChar );	
					if( !Token )
						return false;

					ULONG tempIP = inet_addr( Token );
					if( tempIP == INADDR_NONE || tempIP == 0 )	//if string is not a valid IP address then function has failed
						return false;
					else
					{
						ipAddress = tempIP;
						return true;
					}
				
				}
			}

			return false;

		}
	};


	// pointer to list of sites. 
	IPWebsite * sites;
	const int nSites = 3;
	sites = (IPWebsite*) malloc( sizeof( IPWebsite ) * nSites );

	sites[0] = IPWebsite(		L"http://checkip.dyndns.org/Current IP Check.htm",
								": ",
								"<");
	sites[1] = IPWebsite(		L"http://www.myip.ru/",
								"<TR><TD bgcolor=white align=center valign=middle>",
								"<" );
	sites[2] = IPWebsite(		L"http://www.whatsmyip.us/",
								"class=\"ip\" onclick=\"copyClip()\" onmouseover=\"copyClip()\">\n",
								"<" );
	
	bool success = false;
	//loop through each site and try to get IP address
	for( int index = 0; index < nSites; index++ )
	{
		if( sites[index].GetIPFromSite( externalIP ))
		{
			success = true;
			break;	
			
		}
	}
	//free sites pointer
	free( sites );
	return success;
}
#endif
uint32_t BNP::Pack_Packet(char * buf, const char * format,...)
{
	uint16_t d;
	const char * s;
	
	UINT i;

	UINT len = 0;
	char * p = buf;

	va_list l;
	va_start( l,format);


	for( const char * t = format; *t != '\0'; t++ )
	{
		char b = *t;
		bool isCaps = 0 != isupper( b );
		b = tolower( b );
		assert( strchr( BNP::StdFormats,b));
		int varSize;
		switch (b)
		{
		case 'd':
			{
				varSize = sizeof(uint16_t);
				d = (uint16_t) va_arg( l, int);
				if( !isCaps )
					d = htons( d );
				memcpy( p, &d, varSize );
			}
			break;
		case 's':
			{
				s = va_arg( l,const char*);
				varSize = strlen(s)+1;
				strcpy( p, s );
			}
			break;
		case 'i':
			{
				i = va_arg( l, UINT );
				if( !isCaps )
					i = htonl( i );
				varSize = sizeof(UINT );
				memcpy( p, &i, varSize );
			}
			break;
		case 'c':
			{
				varSize = sizeof(char);
				*p = (char)va_arg( l, int );
			}
			break;
		default:
			break;
		}
		len += varSize;
		p += varSize;
	}
	return len;
}
uint32_t BNP::UnpackPacket( const char * buf, const char * format, ... )
{
	uint32_t len = 0;
	uint16_t * d;
	char * s;
	UINT * i;
	
	const char * p = buf;

	va_list l;
	va_start( l,format);

	for( const char * t = format; *t != '\0'; t++ )
	{
		char b = *t;
		bool isCaps = 0 != isupper( b );
		b = tolower( b );

		assert( strchr( BNP::StdFormats,b));
		int varSize;
		switch (b)
		{
		case 'd':
			varSize = sizeof(uint16_t);
			d = va_arg( l,uint16_t*);
			memcpy( d, p , varSize );
			if( !isCaps )
				*d = ntohs( *d);
			break;
		case 's':
			{
				s = va_arg( l,char*);
				strcpy( s, p );
				varSize = strlen(s)+1;
			}
			break;
		case 'i':
			varSize = sizeof( UINT );
			i = va_arg( l, UINT*);
			memcpy( i, p, varSize );
			if( !isCaps )
				*i = ntohl( *i );
			break;
		case 'c':
			varSize = sizeof(char);
			s = va_arg( l, char*);
			*s = *p;
		default:
			break;
		}
		len += varSize;
		p += varSize;
	}
	return len;
}

BNP::Timeout_Timer::Timeout_Timer( NID_BIG theTimeout )
	:timeout(theTimeout)
{

}
bool BNP::Timeout_Timer::HasTimedOut()
{
	if( timer.GetTimeMilli() < timeout)
		return false;
	else
	{
		timer.ResetWatch();
		return true;
	}
}
void BNP::Timeout_Timer::ResetWatch()
{
	timer.ResetWatch();
}


byte BNP::HeartBeat::GetPacketID()
{
	return packetID;
}
void BNP::HeartBeat::Update()
{
	packetID++;
	sendBuffer[BNP::byte_PN] = packetID;
}
BNP::HeartBeat::HeartBeat( )
{
	memset(sendBuffer,'\0',size);
	packetID = 0;
	sendBuffer[BNP::byte_CM] = BNP::CM_HEARTBEAT;
}
const char * BNP::HeartBeat::GetPacket()
{
	return sendBuffer;
}

BNP::Update::Update( byte& IDOfUpdate,NID updateType  ,const char * szExData, UINT len, bool isForceSend)
	:updateID( ++IDOfUpdate),
	 packLen(len + byte_START),
	forceSend( isForceSend )
{

	buf = new char[packLen];
	buf[byte_CM] = CM_UPDATE_SND;
	buf[byte_UP] = 0;	//this is set when GetUpdate() is called
	buf[byte_UT] = updateType;
	if( szExData )
		memcpy( buf + byte_START,szExData,len);
}
BNP::Update::Update(  byte& IDOfUpdate, const char * completeBuf, uint32_t len, bool isForceSend  )
	:packLen(len),
	updateID( ++IDOfUpdate ),
	forceSend( isForceSend )
{
	buf = new char[packLen];
	memcpy( buf, completeBuf, len );
}
BNP::Update::~Update()
{
	if( buf )
		delete[] buf;
}
const char * BNP::Update::GetUpdate(byte upID)
{
	buf[BNP::byte_UP] = upID;
	return buf;
}
BNP::Update& BNP::Update::operator=( const Update& u )
{
	memcpy( this,&u,sizeof(BNP::Update));
	
	BNP::Update& o = (BNP::Update&)u;
	o.buf = NULL;
	
	return *this;
}
BNP::Update::Update( const Update& u )
	:updateID(u.updateID),
	packLen( u.packLen ),
	forceSend( u.forceSend ),
	buf(u.buf)

{
	Update& o = (Update&)u;
	o.buf = NULL;
}

BNP::RcvUpdate::RcvUpdate( const char * data, PacketLen pLen )
	:len(pLen)
{
	buf = new char[pLen];
	memcpy( buf,data,pLen);
}
BNP::RcvUpdate::RcvUpdate( const RcvUpdate& s )
	:len(s.len)
{
	buf = new char[s.len];
	memcpy( buf,s.buf,s.len);
}
BNP::RcvUpdate& BNP::RcvUpdate::operator=( const BNP::RcvUpdate& u )
{
	memcpy( (void*)&len, &u.len,sizeof(len));
	delete[] this->buf;
	this->buf = new char[len];
	memcpy( buf,u.buf,len);
	return *this;
}


BNP::IPComm::IPComm(UINT numberOfLists, int nConnections)
: UPTimer( BNP::TO_SENDMILLI ),
  HBTimer( BNP::TO_HEARTBEAT ),
  curUpListID(0),
  heartbeat( ),
  networker(nConnections),
  nLists( numberOfLists ),
  requestID(0)
{
	contactLists.resize(numberOfLists );
}
BNP::IPComm::~IPComm()
{

}
void BNP::IPComm::MoveContact( uint32_t srcL, uint32_t srcP, uint32_t dstL )
{
	assert( srcL < contactLists.size() );
	assert( srcP < contactLists.at(srcL).size() );
	contactLists.at( dstL ).push_back( contactLists.at(srcL).at(srcP) );
	contactLists.at( srcL ).erase( contactLists.at( srcL ).begin() + srcP );
}
void BNP::IPComm::AddList()
{
	nLists++;
	std::vector< IPContact > a;
	contactLists.push_back( a );
}
void BNP::IPComm::RmList( uint32_t listIndex )
{
	assert( listIndex < nLists );
	contactLists.erase( contactLists.begin() + listIndex );
	nLists--;
}
bool BNP::IPComm::Std_SndData()
{

	for( UINT u = 0; u < updateList.size(); u++ )
	{
		bool updateSent = true;
		for( UINT i = 0; i < nLists; i++ )
		{
			if( !IsUpdateSent( contactLists.at(i),	u))
				updateSent = false;
		}

		if( updateSent )
		{
			updateList.erase( updateList.begin() + u );
		}
	}
	
	
	for( UINT i = 0; i < nLists; i++ )
	{
		if( !SendUpdates(contactLists.at(i),true))
			return false;	
	}	

	
	if( HBTimer.HasTimedOut() )
	{
		heartbeat.Update();
		for( UINT i = 0; i < nLists; i++ )
		{
			if( !SendHeartBeats( contactLists.at(i)))
				return false;		
		}
	}
	if( UPTimer.HasTimedOut() )
	{
		for( UINT i = 0; i < nLists; i++ )
		{
			if( !SendUpdates( contactLists.at(i),false))
				return false;
			if( !SendUpdateAccepts( contactLists.at(i)))
				return false;
		}
	}
	return true;
}
int BNP::IPComm::Std_RcvData(uint32_t lIndex, uint32_t pIndex, bool& processed,char * rcvBuf, int rcvLen )
{
	assert( lIndex < contactLists.size() );
	assert( pIndex < contactLists[lIndex].size() );

	IPContact& p = contactLists[lIndex].at( pIndex );

	int r = networker.RecieveData( p.socketID,rcvLen,rcvBuf,NULL );
	
	processed = false; //default value

	if( r < 0 )
		return r;	//error
	else if( r == 0 )
		return p.GetNxtUpdate( rcvBuf );	//try and find stored update from previously
		//returns len if found. 0 if not
	else
	{
		processed =  p.ProcessRcvData( rcvBuf,r);
	}
	return r;

	
}
void BNP::IPComm::AddContact_Client( UINT listIndex, const  char * recvBuf, int sockIndex )
{
	assert( listIndex < nLists );
	contactLists[listIndex].push_back( IPContact( recvBuf,sockIndex,false));
}
void BNP::IPComm::RemoveContact( UINT listIndex, UINT contactIndex )
{
	assert( listIndex < nLists);
	std::vector<IPContact>& curList = contactLists[listIndex];
	assert( contactIndex < curList.size() );
	curList.erase( curList.begin() + contactIndex );

}
bool BNP::IPComm::HasTimedOut( UINT listIndex, UINT contactIndex )
{
	assert( listIndex < nLists);
	std::vector<IPContact>& curList = contactLists[listIndex];
	assert( contactIndex < curList.size() );
	return curList[contactIndex].timeout.HasTimedOut();

}
bool BNP::IPComm::IsUpdateSent( std::vector< IPContact >& list , UINT uIndex )
{


	for(unsigned  int playerIndex = 0; playerIndex < list.size(); playerIndex++ )
	{
		IPContact& cur = list.at(playerIndex );
		byte refUpID = updateList.at(uIndex).updateID;

		if(cur.FindUpdate(refUpID, IPContact::UpdateList_Snd,false ) > -1 )
		{
			return false;
		}
	}

	return true;
}
void BNP::IPComm::FormRequest( char * szMessage )
{
	requestID++;

	szMessage[BNP::byte_CM] = CM_NORMREQUEST;
	szMessage[BNP::byte_RQID] = requestID;
	szMessage[BNP::byte_RQHB] = heartbeat.GetPacketID();

}
void BNP::IPComm::AddUpdate( int updateType,const char * msg, UINT len , bool forceSend)
{
	updateList.push_back( BNP::Update( curUpListID, updateType,msg,len,forceSend ));
}
void BNP::IPComm::InitCurUpdate( )
{
	
	for( UINT i = 0; i < nLists; i++ )
	{
		InitCurUpdate( i );
	}

}
void BNP::IPComm::InitCurUpdate( UINT listIndex )
{
	assert( updateList.size() ); //make sure there is a current update to start with

	assert( nLists > listIndex);	//accidentally entered a too big value to the list index?

	
	//loops through all players adding current update ID to their list of required updates
	for(unsigned int pIndex = 0; pIndex < contactLists.at(listIndex).size(); pIndex++ )
	{
		InitCurUpdate( listIndex,pIndex);
	}
	


}
void BNP::IPComm::InitCurUpdate( UINT listIndex, UINT playerIndex )
{
	assert( nLists > listIndex);//make sure there is a current update to start with
	assert( contactLists[listIndex].size() > playerIndex );//accidentally entered a too big value to the list index?
									
	//add update ID
	contactLists[listIndex].at(playerIndex).AddUpdate( curUpListID );

}
int BNP::IPComm::SendPacket( UINT listIndex, UINT contactIndex , char * szBuffer, UINT len )
{
	assert( listIndex < nLists);
	std::vector<IPContact>& curList = contactLists[listIndex];
	assert( contactIndex < curList.size() );
	return networker.SendData(	curList[contactIndex].socketID,
								len,szBuffer,
								curList[contactIndex].ContactIPData() );
}
bool BNP::IPComm::SendHeartBeats(  std::vector< IPContact >& list  )
{
	for(UINT contact = 0; contact < list.size(); contact++ )
	{
		if(	networker.SendData(	list[contact].socketID,
								HeartBeat::size,
								heartbeat.GetPacket( ),
								list[contact].ContactIPData() )
								< 0 )
		{
			return false;
		}
	}
	return true;
}
bool BNP::IPComm::SendUpdates( std::vector< IPContact >& list , bool forceSend)
{
	for( UINT c = 0; c < list.size(); c++)
	{
		IPContact& cur = list[c];
		for( UINT u = 0; u < updateList.size(); u++ )
		{
			if( updateList[u].forceSend != forceSend )
				continue;	//forceSend does not match update List element

			byte refID = updateList.at(u).updateID;
			int actUPIndex;
			byte actID;

			actUPIndex = cur.FindUpdate( refID,IPContact::UpdateList_Snd,false );

			if( actUPIndex < 0 )
				continue;

			actID = cur.GetUpdateID( IPContact::UpdateList_Snd,actUPIndex,true);

			if( networker.SendData( cur.socketID,
									updateList[u].packLen,
									updateList[u].GetUpdate(actID),
									cur.ContactIPData()	)
									< 0 )
			{
				return false;
			}
			
		}
	}
	return true;
}
bool BNP::IPComm::SendUpdateAccepts( std::vector< IPContact >& list )
{
	const int len = BNP::byte_START;
	char sendingBuf[len];
	memset(sendingBuf,'\0',len);
	sendingBuf[ BNP::byte_CM ] = BNP::CM_UPDATE_RCV;

	for( UINT contactIndex = 0; contactIndex < list.size(); contactIndex++ )
	{
		IPContact& cur( list.at(contactIndex) );
		for(UINT updateIndex = 0; updateIndex < cur.GetNUpdates( IPContact::UpdateList_Acpt); updateIndex++ )
		{
			byte upID = cur.GetUpdateID( IPContact::UpdateList_Acpt,updateIndex);

			sendingBuf[ BNP::byte_PN ] = upID;

			if( networker.SendData( cur.socketID,
									len,
									sendingBuf,
									cur.ContactIPData() )
									< 0 )
			{
				return false;
			}
		}
		cur.ClearAcptList();
	}
	return true;
}
int BNP::IPComm::Connect( bool& success,char& denyReason, int socketIndex, const char * rqstBuf, int rqstBufLen,int rcvBufLen, char * rcvBuf  )
{


	int rqstFullLen = rqstBufLen + byte_START;
	char * rqstFullBuf = new char[rqstFullLen];
	FormRequest( rqstFullBuf );
	memcpy( rqstFullBuf + byte_START,rqstBuf,rqstBufLen*sizeof(char));

	networker.SendData(socketIndex,rqstFullLen,rqstFullBuf);

	

	Timer timer;

	success = false;

	while( timer.GetTimeMilli() < TO_CLIENT_CONNECT )
	{
		int len = networker.RecieveData( socketIndex,rcvBufLen,rcvBuf);
		if( len < 0 )
		{
			delete[] rqstFullBuf;
			return -1;
		}
		else if( len > 0 )
		{
			
			if( rqstFullBuf[BNP::byte_RQID] == rcvBuf[BNP::byte_RQID] )
			{
				if( rcvBuf[BNP::byte_CM] == BNP::CM_REPLY_SUCCESS )
				{
					delete[] rqstFullBuf;
					success = true;
					return len;
				}
				else if( rcvBuf[BNP::byte_CM] == BNP::CM_REPLY_FAIL )
				{
					delete[] rqstFullBuf;
					denyReason = rcvBuf[BNP::byte_RQDR];
					return len;
				}
			}
		}

	}

	delete[] rqstFullBuf;
	return 0;

}

BNP::IPComm_HostType::IPComm_HostType(UINT nLists, int nConnections)
	:IPComm(nLists, nConnections )
{}
BNP::IPComm_HostType::~IPComm_HostType()
{

}
PacketLen BNP::IPComm_HostType::FormReplyFailure( char * szBuffer,  const char * rqstBuf,char denyReason )
{
	szBuffer[byte_CM]		= BNP::CM_REPLY_FAIL;
	szBuffer[byte_RQID]	= rqstBuf[BNP::byte_RQID];
	szBuffer[byte_RQDR]	= denyReason;
	return byte_START;
	
}
PacketLen BNP::IPComm_HostType::FormReplySuccess( char * szBuffer, const char * rqstBuf )
{
	szBuffer[byte_CM]		= BNP::CM_REPLY_SUCCESS;
	szBuffer[byte_RQID]	= rqstBuf[BNP::byte_RQID];
	szBuffer[byte_RQHB]	= heartbeat.GetPacketID();
	return byte_START;
}
bool BNP::IPComm_HostType::FindContact( uint32_t& lIndex, uint32_t& pIndex, sockaddr * IPData )
{

	for( lIndex = 0; lIndex < contactLists.size(); lIndex++ )
	{
		for(  pIndex = 0; pIndex < contactLists.at(lIndex).size(); pIndex++ )
		{
			if( !memcmp(IPData, contactLists.at(lIndex).at(pIndex).ContactIPData(),sizeof(sockaddr)))
			{
				return true;
			}
		}
	}
	return false;

}
void BNP::IPComm_HostType::AddContact_Host( 	UINT listIndex, const  char * recvBuf, int sockID, sockaddr socketData )
{
	assert( listIndex < nLists );
	contactLists[listIndex].push_back( IPContact( recvBuf,sockID,true,&socketData));
}
int BNP::IPComm_HostType::SendPacket( char * buffer, int len, sockaddr socketData, int sockIndex )
{
	return networker.SendData( sockIndex,len,buffer,&socketData);
}

BNP::IPContact& BNP::IPContact::operator=( const IPContact& c )
{
	this->curHBID = c.curHBID;
	this->curUPID = c.curUPID;
	this->socketData = c.socketData;
	this->slfUPID = c.slfUPID;
	
	memcpy( (void*)&this->hostContact , &c.hostContact, sizeof( this->hostContact ) );
	memcpy( &this->timeout, &c.timeout, sizeof( c.timeout ));
	memcpy( (void*)&this->socketID,&c.socketID,sizeof(c.socketID));
	
	this->updateIDList = c.updateIDList;
	this->updateAcceptList = c.updateAcceptList;
	this->futureUpdates = c.futureUpdates;

	return * this;
}
BNP::IPContact::IPContact(const  char * recvBuf, int sockID, bool isHostContact,sockaddr * s )
	:socketID(sockID),
	timeout(TO_GENERAL),
	hostContact( isHostContact )
{
	if( s )
	{
		assert( hostContact );
		memcpy( &socketData, s, sizeof(sockaddr));
	}

	int place = 2;
	curHBID = recvBuf[place];
	curUPID = 0;
	slfUPID = 0;
}
BNP::IPContact::~IPContact()
{

}
int BNP::IPContact::IsPacketValid( byte cmType, byte packetNumber )
{
	assert( cmType == CM_HEARTBEAT || cmType == CM_UPDATE_SND );
	byte nxtID;

	if( cmType == CM_HEARTBEAT )
		nxtID = byte( curHBID + 1);
	else if( cmType == CM_UPDATE_SND )
		nxtID = byte ( curUPID + 1);

	byte dif = nxtID - packetNumber;

	if( nxtID == packetNumber )
		return 1;
	else if( dif < nPacketLag && dif  > 0 )
		return -1;
	else
		return 0;
}
bool BNP::IPContact::ProcessRcvData( char * recvBuf , int len)
{


	byte cmType = recvBuf[byte_CM];
	byte packetNumber = recvBuf[byte_PN];

	if( cmType == CM_HEARTBEAT )
	{
		if( IsPacketValid(CM_HEARTBEAT,packetNumber))
		{
			//doesnt matter if future packet, all heartbeats the same
			curHBID++;
			timeout.ResetWatch();
		}
		return true;

	}
	else if(cmType == CM_UPDATE_RCV )
	{
		timeout.ResetWatch();

		int updateIndex = FindUpdate( packetNumber, UpdateList_Snd,true );
		if( updateIndex >= 0 )
		{
			updateIDList.erase( updateIDList.begin() + updateIndex );
		}

		return true;
	}
	else if( cmType == CM_UPDATE_SND)
	{
		if( FindUpdate( packetNumber,UpdateList_Acpt ) < 0 )
		{
			timeout.ResetWatch();
			updateAcceptList.push_back(packetNumber);
		}

		
		//if ready to send,send acceptance
		int pValid = IsPacketValid( CM_UPDATE_SND,packetNumber);
		if( pValid == 1 )
		{
			//packet is a valid update
			curUPID++;
			return false;;
		}

		if( pValid == -1 )
		{
			futureUpdates.push_back( RcvUpdate(recvBuf,len ));
		}
		return true;
	}
	else
	{
		//packet not in any format to be processed here
		timeout.ResetWatch();
		return false;;
	}

}
int BNP::IPContact::FindUpdate( byte updateID, UpdateList_Typ listType , bool isActual )
{

	assert( listType == UpdateList_Snd ||  listType == UpdateList_Acpt );
	if( listType == UpdateList_Snd )
	{
		if( isActual )
		{
			for( unsigned int i = 0; i < updateIDList.size(); i++ )
			{
				//first is reference to update struct ID
				if( updateIDList.at(i).second  == updateID )
					return i;
			}
		}
		else
		{
			for( unsigned int i = 0; i < updateIDList.size(); i++ )
			{
				//first is reference to update struct ID
				if( updateIDList.at(i).first  == updateID )
					return i;
			}
		}
	}
	else if( listType == UpdateList_Acpt )
	{
		for( unsigned int i = 0; i < updateAcceptList.size(); i++ )
		{
			if( updateAcceptList.at(i) == updateID )
				return i;
		}
	}
	return -1;
}
const sockaddr * BNP::IPContact::ContactIPData()
{
	if( hostContact )
		return &socketData;
	else
		return NULL;
}
uint32_t  BNP::IPContact::GetNxtUpdate( char * recvBuf )
{
	byte nxtUPID = byte( curUPID + 1 );
	for( uint32_t i = 0; i < futureUpdates.size(); i++ )
	{
		RcvUpdate up(futureUpdates.at(i));	//create local element
		//furutureupdates element must be erased before end of function
		byte ID = up.buf[ byte_UP ];
		if( ID == nxtUPID )
		{
			memcpy( recvBuf, up.buf , up.len);
			futureUpdates.erase( futureUpdates.begin() + i );
			return up.len;
		}
	}
	return 0;
}
uint32_t BNP::IPContact::GetNUpdates( UpdateList_Typ upType )
{
	assert( upType == UpdateList_Snd ||  upType == UpdateList_Acpt );
	if( upType == UpdateList_Snd )
		return updateIDList.size();
	else if( upType == UpdateList_Acpt )
		return updateAcceptList.size();
	return 0;	//remove warning not all control paths return...
}
byte BNP::IPContact::GetUpdateID( UpdateList_Typ upType, uint32_t index, bool isActual  )
{
	assert( upType == UpdateList_Snd ||  upType == UpdateList_Acpt );
	if( upType == UpdateList_Snd )
	{
		assert( index < updateIDList.size() );
		if( isActual )
			return updateIDList.at(index).second;
		else
			return updateIDList.at(index).first;
	}
	else if( upType == UpdateList_Acpt )
	{
		assert( index < updateAcceptList.size() );
		return updateAcceptList.at(index);
	}
	return 0; //remove warning
}

BaseNetworking::BaseNetworking(uint16_t numberOfSockets)
	:sockAddrSize(sizeof(sockaddr)),
	nSockets(numberOfSockets)
{
	

	sockets = new int[nSockets];
	socketTypeList = new SocketType[nSockets];

	ZeroMemory( sockets,sizeof(int)* nSockets );

#if PLATFORM == PLATFORM_WINDOWS
	WSADATA winsockDat;
	int rVal = WSAStartup( MAKEWORD( 2,2 ) , &winsockDat );
	if( rVal != 0 )
	{
		_exit(1);
	}
#endif



}
BaseNetworking::~BaseNetworking()
{
	for( UINT index = 0; index < nSockets; index++ )
	{
		if( sockets[index] )
		{

			if( closesocket( sockets[index]))
				_exit(1);
		}
	}

#if PLATFORM == PLATFORM_WINDOWS
	if( WSACleanup())
		_exit(1);
#endif

	delete[] socketTypeList;
	delete[] sockets;



}
int BaseNetworking::SendData(UINT sock,int size, const char* buffer,const sockaddr* toAddr)
{
	int rVal;

	assert( sockets[sock] );
	assert( sock >= 0 && sock < nSockets );


	if( toAddr )
	{
		assert( socketTypeList[sock] == SOCKET_HOST );
		rVal =  sendto(		sockets[sock],
							buffer,
							size ,
							0,
							toAddr,
							sockAddrSize);
	}
	else
	{
		assert( socketTypeList[sock] == SOCKET_CLIENT );
		rVal = send(		sockets[sock],
							buffer,
							size,
							0 );
	}

	if( rVal < 0 )
	{
		SetSysErr();
		return -1;
	}
	else
	{
		assert(rVal == size );
	}

	return rVal;


}
void BaseNetworking::SetSysErr()
{
#if PLATFORM==PLATFORM_WINDOWS
	sysErr = WSAGetLastError();
#else
	sysErr = errno;
#endif
}
int BaseNetworking::RecieveData( UINT sock,int size,char * recvBuf , sockaddr* fromAddr )
{
	int rVal;

	assert( sockets[sock] );
	assert( sock >= 0 && sock < nSockets );

	if( fromAddr )
	{
		assert( socketTypeList[sock] == SOCKET_HOST );
		rVal = recvfrom(		sockets[sock],
								recvBuf,
								size,
								0,
								fromAddr,
								&sockAddrSize);

	}
	else
	{
		assert( socketTypeList[sock] == SOCKET_CLIENT );
		rVal = recv(		sockets[sock],
							recvBuf,
							size,
							0);
	}

	if( rVal == SOCKET_ERROR)
	{
		if( IsErrorWait() )
			return 0;
		else
		{
			SetSysErr();
			return -1;
		}
	}
	else 
		return rVal;
}
bool BaseNetworking::InitHostSocket(UINT sock,  UINT port)
{
	assert( sock < nSockets );
	assert( !sockets[sock] );

	sockets[sock] = socket( AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if( sockets[sock] == INVALID_SOCKET )
	{
		SetSysErr();
		return false;
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

	if( bind( sockets[sock],(sockaddr*)&local,sockAddrSize))
	{
		SetSysErr();
		return false;
	}
#if PLATFORM == PLATFORM_WINDOWS
	DWORD nonBlocking = 1;
	if( ioctlsocket( sockets[sock],FIONBIO,&nonBlocking) == SOCKET_ERROR)
	{
		SetSysErr();
		return false;
	}
#else
	if( fcntl( sockets[sock], F_SETFL,O_NONBLOCK)== SOCKET_ERROR)
	{	
		SetSysErr();
		return false;
	}
#endif
	return true;
}
bool BaseNetworking::InitClientSocket(UINT sock, unsigned long IPAddress, UINT port )
{
	assert( !sockets[sock] );
	assert( sock < nSockets );



	sockets[sock] = socket( AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if( sockets[sock] == INVALID_SOCKET )
	{
	
		return false;
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

	if( connect(  sockets[sock],(sockaddr*)&IPData,sockAddrSize) == SOCKET_ERROR)
	{
		return false;
	}
#if PLATFORM == PLATFORM_WINDOWS
	DWORD nonBlocking = 1;
	if( ioctlsocket( sockets[sock],FIONBIO,&nonBlocking)== SOCKET_ERROR )
	{
		return false;
	}
#else
	if(fcntl( sockets[sock], F_SETFL,O_NONBLOCK)== SOCKET_ERROR )
	{
		SetSysErr();
		return false;
	}
#endif

	return true;
}
bool BaseNetworking::DestroySocket( UINT sock )
{
	assert( sock < nSockets );
	assert( sockets[sock] );


	if( closesocket( sockets[sock] )== SOCKET_ERROR )
	{
		return false;
	}
	sockets[sock] = 0;

	return true;
}
bool BaseNetworking::IsErrorWait()
{
#if PLATFORM == PLATFORM_WINDOWS
	int error = WSAGetLastError();
	if( error == WSAEWOULDBLOCK )
		return true;
#else
	if (errno == EAGAIN)
		return true;
#endif
	return false;
}
void BaseNetworking::SetCustomErr( std::string err )
{
	customError = err;
}
std::string BaseNetworking::GetError()
{
	std::string err;

	if( customError.length() )
	{
		err = customError;
		customError.clear();
	}
	else
	{
	
		err =  _GetSysError();
	}

	return err;
}
std::wstring BaseNetworking::WGetError()
{
	std::wstring ws;
	std::string s;
	s = GetError();
	ws.assign( s.begin(), s.end() );
	return ws;
}
std::string BaseNetworking::_GetSysError()
{

	std::string err;

#if PLATFORM==PLATFORM_WINDOWS



	wchar_t* msg;
	HMODULE lib = NULL;
	FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					0,
					(LPCVOID)lib,
					sysErr,
					0, // language ID
					(LPTSTR)&msg,
					0, // size ignored
					NULL); // arglist

	int len = lstrlenW(msg)+1;
	char * szMsg = new char[len];
	ZeroMemory(szMsg,len);
	wcstombs( szMsg,msg,len);
	err =  std::string(szMsg);
	delete[] szMsg;
#else
	err = std::string( strerror( errno ) );
#endif

	return err;


}
