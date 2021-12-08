#include "client.h"
bool StateIsOnline( NumState state ) {
	return state < GameState::GAMESTATE_END||state==SERVERLIST;
}
bool StateIsD3d( NumState state ) {
	return state==COUNTDOWN||state==PLAYING;
}

MasterClient::MasterClient(HWND hwnd,ItemServer& iServ, KeyboardServer& _kServ,ULONG svrIP)
	:countdown(DEF_COUNDTOWN_SS,gfx),
	network(svrIP,GNP::PORT_SVR),
	isHost(false),
	doingOp(false),
	window(hwnd),
	gfx(hwnd),
	itemIntf(iServ),
	_iServ(iServ),
	kServ(_kServ),
	kbd(kServ){

	name=GenerateRandName();

	buttonLists.resize( BTNLST_NLISTS );
	buttonLists[BTNLSTIDX_MM]		=Buttons_MainMenu();
	buttonLists[BTNLSTIDX_SVRLST]	=Buttons_SvrList();
	buttonLists[BTNLSTIDX_LOBHST]	=Buttons_Lob_Host();
	buttonLists[BTNLSTIDX_LOBCLT]	=Buttons_Lob_Client();
	
}
MasterClient::~MasterClient() {
	for( unsigned int i=0;i<buttonLists.size();i++) {
		DelVec<Button>(buttonLists[i]);
	}
}
void MasterClient::InitSvrList(BNP::NetIPacket& p) {
	pLobby=new LobbyInt(_iServ,window,buttonLists[BTNLSTIDX_SVRLST]);
	pLobby->SetTitle("Server lobby");
	gameList.clear();
	gameint nServers;
	GNP::Game g;
	p>>nServers;
	for( unsigned int i=0;i<nServers;i++) {
		p>>g;
		gameList.push_back( g );
		pLobby->AddItem(g.lobName);
	}
	if( nServers==0 )
		pLobby->SetDataBox("no servers found");

}
void MasterClient::InitLobby( BNP::NetIPacket& p ) {
	BtnList btns=isHost?buttonLists[BTNLSTIDX_LOBHST]:buttonLists[BTNLSTIDX_LOBCLT];
	pLobby=new LobbyInt(_iServ,window,btns);
	GNP::Game g;
	p>>g;
	std::string nm;
	pLobby->SetTitle(g.lobName);
	for( unsigned int plr=0;plr<g.noOfPlayers;plr++ ) {
		p>>nm;
		pLobby->AddItem(nm);
	}
}

MasterInterface::MasterInterface( MasterClient& masterReference  )
	:net(masterReference.network),
	rf(masterReference) {
}
void MasterInterface::SetDataBox( const GNP::Game& g ) {
	std::ostringstream ss;
	ss<<"number of players: "<<g.noOfPlayers<<"\n"<<"max players: "<<g.maxPlayers<<"\nstate: "<<ToStr((GameState)g.state);
	GetLobby()->SetDataBox( ss.str() );
}
BNP::NetIPacket MasterInterface::GetRecvPacket() {
	return recvPacket;
}
void MasterInterface::SyncCountDn( uint8_t secs ) {
	assert( rf.state==COUNTDOWN );
	rf.countdown.Sync(secs,0);
}
Container_Game& MasterInterface::GetSvrList() {
	return rf.gameList;
}
void MasterInterface::PlayLobbySound( soundint soundIndex ) {
	assert( soundIndex <= SOUND_LASTSOUND);
	std::ostringstream sndFile;
	char charSound='0'+soundIndex;
	sndFile<<SOUND_LOCATION<<charSound<<".wav";
	PlaySound( sndFile.str().c_str(),NULL,SND_ASYNC);
}
bool MasterInterface::AmHost() {
	return rf.isHost;
}
void MasterInterface::ToHost() {
	rf.isHost=true;
	if( rf.state==LOBBY )
		GetLobby()->Change_Menu( rf.buttonLists[BTNLSTIDX_LOBHST] );
}
void MasterInterface::ToCLient() {
	rf.isHost=false;
	if( rf.state==LOBBY )
		GetLobby()->Change_Menu( rf.buttonLists[BTNLSTIDX_LOBCLT] );
}
bool MasterInterface::GetServerData(unsigned int sIndex,GNP::Game& svr ) {
	assert( rf.state==SERVERLIST);
	if( sIndex < rf.gameList.size() && sIndex >= 0) {
		svr= rf.gameList[sIndex];
		return true;
	}
	else
		return false;
}
void MasterInterface::SetRecvPacket( const BNP::NetIPacket& packet ) {
	recvPacket = packet;
}
D3DGraphicsClient MasterInterface::Graphics() {
	return D3DGraphicsClient(rf.gfx);
}
void MasterInterface::ChangeName( const std::string& newName ) {
	rf.name=newName;
	if( rf.state==MAINMENU) {
		((MenuWBox*)(rf.pMenu))->ChangeExtText("name: "+newName);
	}
}
HWND MasterInterface::GetWind() {
	return rf.window;
}
LobbyInt * MasterInterface::GetLobby() {
	assert( rf.pLobby );
	return rf.pLobby;
}
bool MasterInterface::OpInProg() {
	return rf.doingOp;
}
void MasterInterface::InitOp() {
	rf.doingOp=true;
}
void MasterInterface::OpStopped() {
	rf.doingOp=false;
}
std::string MasterInterface::name() {
	return rf.name;
}
