#include "InputBox.h"
#if COMPILER == COMPILER_VS && PLATFORM == PLATFORM_WINDOWS

const jchar * gCaption            	;
const jchar * gRequest            	;
const jchar * gStartingEditText   	;
int gMaxLen					;
jchar * gReturnString		;

INT_PTR CALLBACK InputBoxProc(  HWND hwndDlg, UINT32 uMsg, WPARAM wParam,LPARAM lParam )
{
	switch (uMsg) 
	{
		
		case WM_INITDIALOG:
			SetWindowText( hwndDlg,gCaption);										//set title
			SetDlgItemText( hwndDlg,ID_IB_EDITTEXT,gStartingEditText);				//set starting edit text
			SetDlgItemText( hwndDlg,ID_IB_DISPLAYTEXT,gRequest);					//set request string
			SendMessage( GetDlgItem( hwndDlg,ID_IB_EDITTEXT),EM_LIMITTEXT,(WPARAM)gMaxLen,NULL);	//limit edit text
			return true;
		case WM_COMMAND: 
			switch (LOWORD(wParam)) 
			{ 
				
				case IDOK:
					GetDlgItemText(hwndDlg,ID_IB_EDITTEXT,gReturnString,gMaxLen);			//get edit text
					//dont want blank input
					if( *gReturnString == NULL )
					{
#ifdef UNICODE
						MessageBox( hwndDlg,L"you need to enter something!",L"error",NULL);
#else
						MessageBox( hwndDlg,"you need to enter something!","error",NULL);
#endif
					}
					else
					{
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

bool CreateInputBox( HWND parentWin,char * returnString, const jchar * caption, const jchar * request,const jchar * startingEditText, int maxLen )
{
	assert(maxLen >= 1 && maxLen < 50 );

	memset( returnString,'\0',maxLen+1);
	
	/* inputboxproc can only access global vars, so global variables are set the the parameters below, so that
		when the input box is created and WM_INITDIALOG is called it will refer to the correct variables. */
	gCaption = caption;
	gRequest = request;
	gStartingEditText = startingEditText;
	gMaxLen = maxLen;
#ifdef UNICODE
	gReturnString = new wchar_t[maxLen+1];
#else
	gReturnString = returnString;
#endif
	//returns true if user enters something, false if cancels. See InputBoxProc
	int rVal = DialogBox( NULL,MAKEINTRESOURCE( ID_IB_WINDOW ),parentWin,(DLGPROC)InputBoxProc ); //create the dialog
	
#ifdef UNICODE
	wcstombs( returnString,gReturnString,maxLen);
#endif

	//returns true if user pressed ok and entered something, false if they cancelled.
	return !!rVal;
}

#endif