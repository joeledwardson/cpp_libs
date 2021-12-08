#ifndef CLTBUTTONS_H
#define CLTBUTTONS_H
#include "client.h"
#include "stdbuttonids.h"

class ButtonProcess {
public:
	virtual bool ThisProcess( unsigned long buttonID, NumState state )=0;
	virtual void Process( unsigned long buttonID,MasterInterface& intf, NumState& newState )=0;
	virtual ~ButtonProcess();
};
class MMBtnProcess : public virtual ButtonProcess {
public:
	bool ThisProcess( unsigned long buttonID, NumState state );
protected:
	virtual bool ThisButton( unsigned long buttonID )=0;
};
class Btn_MM_Exit : public MMBtnProcess {
public:
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
private:
	bool ThisButton( unsigned long buttonID );
};
class Btn_MM_ChangeNM : public MMBtnProcess {
public:
	void Process(unsigned long buttonID, MasterInterface& intf , NumState& newState);
private:
	bool ThisButton( unsigned long buttonID );
};
class Btn_MM_SvrList : public MMBtnProcess {
public:
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
private:
	bool ThisButton( unsigned long buttonID );
};
class Btn_Chat : public ButtonProcess {
public:
	bool ThisProcess( unsigned long buttonID, NumState state );
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
};
class SvrListBtnProcess : public virtual ButtonProcess {
public:
	bool ThisProcess( unsigned long buttonID, NumState state );
protected:
	virtual bool ThisButton( unsigned long buttonID )=0;
};
class Btn_SvrLst_Host : public SvrListBtnProcess {
public:
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
private:
	bool ThisButton( unsigned long buttonID );
};
class Btn_SvrLst_Cancel : public SvrListBtnProcess {
public:
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
private:
	bool ThisButton( unsigned long buttonID );
};
class Btn_SvrLst_Join : public SvrListBtnProcess {
public:
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
private:
	bool ThisButton( unsigned long buttonID );
};
class Btn_SvrLst_SelSvr : public SvrListBtnProcess {
public:
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
private:
	bool ThisButton( unsigned long buttonID );
};
class Btn_Lob_Leave : public ButtonProcess {
public:
	bool ThisProcess( unsigned long buttonID, NumState state );
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
};
class Btn_Lob_Start : public ButtonProcess {
public:
	bool ThisProcess( unsigned long buttonID, NumState state );
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
};
class Btn_Lob_Promote : public ButtonProcess {
public:
	bool ThisProcess( unsigned long buttonID, NumState state );
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
};
class Btn_Lob_Kick : public ButtonProcess {
public:
	bool ThisProcess( unsigned long buttonID, NumState state );
	void Process( unsigned long buttonID, MasterInterface& intf, NumState& newState );
};


typedef std::vector< ButtonProcess* >BtnProcContainer;
BtnProcContainer StdBtnProcesses();



#endif