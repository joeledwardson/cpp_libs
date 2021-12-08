#include "InputBox.h"


std::string caption, title, initEditTxt;
int maxInputLen;
char szReturnStr[INPUTBOX_MAXLEN+1];

INT_PTR CALLBACK InputBoxProc(  HWND hwndDlg, UINT32 uMsg, WPARAM wParam,LPARAM lParam )
{
	switch (uMsg) 
	{
		
		case WM_INITDIALOG:
			SetWindowText( hwndDlg,title.c_str());										//set title
			SetDlgItemText( hwndDlg,ID_IB_EDITTEXT,initEditTxt.c_str());				//set starting edit text
			SetDlgItemText( hwndDlg,ID_IB_DISPLAYTEXT,caption.c_str());					//set request string
			SendMessage( GetDlgItem( hwndDlg,ID_IB_EDITTEXT),EM_LIMITTEXT,(WPARAM)maxInputLen,NULL);	//limit edit text
			return true;
		case WM_COMMAND: 
			switch (LOWORD(wParam))  { 
				case IDOK:
					ZeroMemory(szReturnStr,INPUTBOX_MAXLEN+1);
					GetDlgItemText(hwndDlg,ID_IB_EDITTEXT,szReturnStr,maxInputLen);			//get edit text
					//dont want blank input
					if( szReturnStr[0] == '\0' ){
						MessageBox( hwndDlg,"you need to enter something!","error",NULL);
					}
					else {
						EndDialog( hwndDlg,1);							//end dialog
					}
					return TRUE; 
					
				case IDCANCEL:			//string is zero-memoried so the programmer knows the user cancelled.
					EndDialog(hwndDlg,0);
					return true;
			}
    } 
    return FALSE; 
}

bool CreateInputBox(HWND parentWin,const std::string& boxTitle,const std::string& boxCaption,std::string& editTxt,	int maxLen,std::string startingEditText ) {

	/* inputboxproc can only access global vars, so global variables are set the the parameters below, so that
		when the input box is created and WM_INITDIALOG is called it will refer to the correct variables. */
	caption = boxCaption;
	initEditTxt = startingEditText;
	title = boxTitle;
	maxInputLen  = maxLen;
	assert( maxInputLen >=1 && maxInputLen<=INPUTBOX_MAXLEN );

	//returns true if user enters something, false if cancels. See InputBoxProc
	int rVal = DialogBox( NULL,MAKEINTRESOURCE( ID_IB_WINDOW ),parentWin,(DLGPROC)InputBoxProc ); //create the dialog

	editTxt=szReturnStr;

	//returns true if user pressed ok and entered something, false if they cancelled.
	return rVal!=0;
}

