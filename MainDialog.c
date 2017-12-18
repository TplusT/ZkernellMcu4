//-----------------------------------------------------------------------------//
//		Dialog 2nd Thread Manager              			                       //
//      (c) By T+T 2010                                                        //
//      Jorge Contreras ramirez                                                //
//      Main Interface                                                         //
//-----------------------------------------------------------------------------//
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <Commctrl.h>

#include "resource.h"
#include "main.h"


typedef struct     ISrlDialogConfig
{   WCHAR    Port                      ;
    SrlCom * Bcd                       ;
}   SrlDialogConfig                    ;

//----------------------------------------------------------------------------------//
//                  Generate purpose Variables                                      //
//----------------------------------------------------------------------------------//
void GnrCenterWindow(HWND Window,HWND Parent)            // Centrar la ventana en la pantalla
{    RECT r,d;
     
     if (!GetWindowRect(Window            ,&r))         return       ;
     if (!GetWindowRect(Parent            ,&d))         return       ;
     SetWindowPos(Window,NULL,d.left  +((d.right -d.left)-(r.right -r.left))/2,
                              d.top   +((d.bottom-d.top )-(r.bottom-r.top ))/2,
                                0,0,SWP_NOSIZE | SWP_NOZORDER)       ;
}

//------------------------------ get private Profile wrappers ------------------------
DWORD PrgGetPrivateProfileString(WCHAR * lpAppName       ,
                                 WCHAR * lpKeyName       ,
                                 WCHAR * lpDefault       ,
                                 WCHAR * lpReturnedString,
                                 DWORD   nSize           ,
                                 WCHAR * lpFileName      )
{   WCHAR  WlpFileName[MAX_PATH+1]       ;
    WCHAR  JustName   [MAX_PATH+1]       ;
     SHORT c,l=(SHORT)wcslen(lpFileName) ;
    DWORD  r                             ;

    for(c=l-1;c>=0;c--) if (lpFileName[c]=='\\')                break;
    if (c)  c++                                                      ;
    swprintf(JustName,sizeof(JustName)/2-1,_T("Ext%s"),lpFileName+c );
    wcscpy_s(WlpFileName,sizeof(WlpFileName)/2-1,lpFileName)         ;
    WlpFileName[c]=0                                                 ;
    wcscat_s(WlpFileName,sizeof(WlpFileName)/2-1,JustName           );

    r=  GetPrivateProfileString(lpAppName,lpKeyName,_T("")           ,
                                lpReturnedString,nSize,WlpFileName ) ;
    if (r!=0)                                                return r;

    return GetPrivateProfileString(lpAppName,lpKeyName,lpDefault     ,
                                   lpReturnedString,nSize,lpFileName);
};

BOOL PrgSetPrivateProfileString(WCHAR * lpAppName       ,
                                WCHAR * lpKeyName       ,
                                WCHAR * Value           ,
                                WCHAR * lpFileName      )
{   return WritePrivateProfileString(lpAppName,lpKeyName,Value,lpFileName); };


UINT PrgGetPrivateProfileInt(WCHAR * lpAppName       ,
                             WCHAR * lpKeyName       ,
                             int nDefault            ,
                             WCHAR * lpFileName      )
{   WCHAR  WlpFileName[MAX_PATH+1]        ;
    WCHAR  JustName   [MAX_PATH+1]        ;
     SHORT c,l= (SHORT)wcslen(lpFileName) ;
    LONG   r                              ;

    for(c=l-1;c>=0;c--) if (lpFileName[c]=='\\')                break;
    if (c)  c++                                                      ;
    swprintf(JustName,sizeof(JustName)/2-1,_T("Ext%s"),lpFileName+c );
    wcscpy_s(WlpFileName,sizeof(WlpFileName)/2-1,lpFileName)         ;
    WlpFileName[c]=0                                                 ;
    wcscat_s(WlpFileName,sizeof(WlpFileName)/2-1,JustName           );

    r=  GetPrivateProfileInt(lpAppName,lpKeyName,-1                  ,
                             WlpFileName                            );
    if (r!=-1)                                               return r;

    return GetPrivateProfileInt(lpAppName,lpKeyName,nDefault         ,
                                lpFileName                          );
};

void GnrWinReadPositions(WCHAR * IniFile,WCHAR * Key,WCHAR * Adkey,SHORT * x,SHORT * y,SHORT * h,SHORT * w)
{    WCHAR     Compose[MAX_PATH]       ;
     WCHAR  IniCompose[MAX_PATH]       ;
     WCHAR  LstCompose[MAX_PATH];
     SHORT  c  = (SHORT)wcslen(IniFile);
     WCHAR  *  lasPos = NULL           ;
     SHORT     Vx,Vy,Vx1,Vy1                          ;

     wcscpy_s(IniCompose, sizeof(IniCompose) / 2 - 1, IniFile)             ;

     for(c--;c>=0;c--) {if (IniFile[c]=='\\')   {lasPos= IniCompose+c;break;}}
     if (lasPos!=NULL)   
     {   lasPos[0]=0;   
         swprintf_s(LstCompose,sizeof(LstCompose)/2-1,_T("%s\\POS_%s"),IniCompose,lasPos+1);
         lasPos[0] ='\\'                                                                ;
     }
    
     if  (Adkey!=NULL)
          swprintf(Compose,sizeof(Compose)/2-1,_T("%s_%s"),Key,Adkey     );
     else swprintf(Compose,sizeof(Compose)/2-1,_T("%s_%s"),Key,_T("NULL"));

     *x = PrgGetPrivateProfileInt(Compose,_T("XVpos")    ,0, LstCompose  );
     *y = PrgGetPrivateProfileInt(Compose,_T("YVpos")    ,0, LstCompose  );
     *h = PrgGetPrivateProfileInt(Compose,_T("HVpos")	 ,0, LstCompose  );
     *w = PrgGetPrivateProfileInt(Compose,_T("WVpos")	 ,0, LstCompose  );

     //-- Virtual Monitors ---------------------------------------
     Vy =    GetSystemMetrics (SM_YVIRTUALSCREEN );       // Top 
     Vx =    GetSystemMetrics (SM_XVIRTUALSCREEN );       // left 
     Vy1= Vy+GetSystemMetrics (SM_CYVIRTUALSCREEN);       // Righ
     Vx1= Vx+GetSystemMetrics (SM_CXVIRTUALSCREEN);       // Bottom 

     if (*x<Vx    ) *x=Vx                             ;
     if (*y<Vy    ) *y=Vy                             ;

     if (*x>Vx1-100) *x=Vx1-100                       ;
     if (*y>Vy1-100) *y=Vy1-100                       ;
}

void GnrWinWritePositions(WCHAR * IniFile,WCHAR * Key,WCHAR * Adkey,HWND hwnd,SHORT w,SHORT h)
{    RECT       r                      ;   
     WCHAR      WTemp[20]              ; // Used for short max 65535
     WCHAR    Compose[MAX_PATH]        ;
     WCHAR IniCompose[MAX_PATH]        ;
     WCHAR  LstCompose[MAX_PATH];
     SHORT  c  = (SHORT)wcslen(IniFile);
     WCHAR  *  lasPos = NULL           ;

     wcscpy_s(IniCompose, sizeof(IniCompose) / 2 - 1, IniFile);

     for (c--; c >= 0; c--) { if (IniFile[c] == '\\') { lasPos = IniCompose + c; break; } }
     if (lasPos != NULL)
     {
         lasPos[0] = 0;
         swprintf_s(LstCompose, sizeof(LstCompose) / 2 - 1, _T("%s\\POS_%s"), IniCompose, lasPos+1);
         lasPos[0] = '\\';
     }

     if  (Adkey!=NULL)
          swprintf(Compose,sizeof(Compose)/2-1,_T("%s_%s"),Key,Adkey     );
     else swprintf(Compose,sizeof(Compose)/2-1,_T("%s_%s"),Key,_T("NULL"));

    GetWindowRect(hwnd,&r)             ;
   
    wsprintf(WTemp,_T("%d"),r.left)    ;
    WritePrivateProfileString(Compose,_T("XVpos"),WTemp, LstCompose);

    wsprintf(WTemp,_T("%d"),r.top  )   ;
    WritePrivateProfileString(Compose,_T("YVpos"),WTemp, LstCompose);

    if (w==0) w=(short)(r.bottom-r.top);
    wsprintf(WTemp,_T("%d"),w)         ;
    WritePrivateProfileString(Compose,_T("WVpos"),WTemp, LstCompose);

    if (h==0) h=(short)(r.right-r.left);
    wsprintf(WTemp,_T("%d"),h)         ;
    WritePrivateProfileString(Compose,_T("HVpos"),WTemp, LstCompose);
}


//--------------------------------------------------------------------------------//
//                  Serial Dialog                                                 //
//--------------------------------------------------------------------------------//
BOOL CALLBACK DrvSrlCfg(HWND hwnd, UINT message, DWORD wParam, LONG lParam)
{
    HWND            Item                 ;
    USHORT          c                    ;
    WCHAR           WTemp[  MAX_PATH + 1];
    SrlDialogConfig * Config             ;

    Config = (SrlDialogConfig *)GetWindowLong(hwnd, GWL_USERDATA)                         ;

    switch (message)
    {
        case WM_INITDIALOG:
            GnrCenterWindow(hwnd, GetParent(hwnd))                                        ;
            Config = (SrlDialogConfig  *)lParam                                           ;
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)Config);
            Config = (SrlDialogConfig *)GetWindowLong(hwnd, GWL_USERDATA)                 ;
            if (Config == NULL)
            {   EndDialog(hwnd, 0); DestroyWindow(hwnd);                      return FALSE;}

            //--- Fill Data ---------------------------------------------------------------   
            swprintf(WTemp, sizeof(WTemp) / 2 - 1, _T("COM%c"), Config->Port)             ;
            SetWindowText(hwnd, WTemp)                                                    ;

            Item = GetDlgItem(hwnd, IDC_PORT)                                             ;
            for (c = 0; c<8; c++)
            {   swprintf(WTemp, sizeof(WTemp) / 2 - 1, _T("COM%u"), c + 1)                ;
                SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)WTemp)                         ;
                if (c==Config->Bcd->Port-1) SetWindowText(Item, WTemp)                    ;
            }                                                                             ;

            Item = GetDlgItem(hwnd, IDC_STOP)                                             ;
            SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)_T("1"))                           ;
            SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)_T("2"))                           ;
            _itow_s(Config->Bcd->StopBits, WTemp,sizeof(WTemp)/2-1,10)                    ;
            SetWindowText(Item, WTemp                                                    );

            Item = GetDlgItem(hwnd, IDC_DATA)                                             ;
            SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)_T("7"))                           ;
            SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)_T("8"))                           ;
            _itow_s(Config->Bcd->DataBits, WTemp,sizeof(WTemp) / 2 - 1, 10               );
            SetWindowText(Item, WTemp)                                                    ;

            Item = GetDlgItem(hwnd, IDC_PARITY)                                           ;
            SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)_T("N"))                           ;
            SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)_T("O"))                           ;
            SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)_T("E"))                           ;
            WTemp[0]= (WCHAR)Config->Bcd->Parity; WTemp[1]=0                              ;
            SetWindowText(Item, WTemp)                                                    ;

            Item = GetDlgItem(hwnd, IDC_SPEED)                                            ;
            SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)_T(   "9600"))                     ;
            SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)_T(  "19200"))                     ;
            SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)_T(  "38400"))                     ;
            SendMessage(Item, CB_ADDSTRING, 0, (LPARAM)_T(  "57600"))                     ;
            _itow_s(Config->Bcd->Speed, WTemp, sizeof(WTemp) / 2 - 1, 10)                 ;
            SetWindowText(Item, WTemp)                                                    ;                                        
                                                                               return TRUE;  // Dialog Gain Focus

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                Item = GetDlgItem(hwnd, IDC_SPEED)                                        ;
                GetWindowText(Item, WTemp, sizeof(WTemp) / 2 - 1)                         ;
                Config->Bcd->Speed = _wtoi(WTemp)                                         ;

                Item = GetDlgItem(hwnd, IDC_PARITY)                                       ;
                GetWindowText(Item, WTemp, sizeof(WTemp) / 2 - 1)                         ;
                Config->Bcd->Parity= (BYTE)WTemp[0]                                       ;

                Item = GetDlgItem(hwnd, IDC_DATA)                                         ;
                GetWindowText(Item, WTemp, sizeof(WTemp) / 2 - 1)                         ;
                Config->Bcd->DataBits= _wtoi(WTemp)                                       ;

                Item = GetDlgItem(hwnd, IDC_STOP)                                         ;
                GetWindowText(Item, WTemp, sizeof(WTemp) / 2 - 1)                         ;
                Config->Bcd->StopBits= _wtoi(WTemp)                                       ;

                Item = GetDlgItem(hwnd, IDC_PORT)                                         ;
                GetWindowText(Item, WTemp, sizeof(WTemp) / 2 - 1)                         ;
                Config->Bcd->Port    = _wtoi(WTemp+3)                                     ;

                EndDialog(hwnd, 1);                                           return FALSE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hwnd, 0);                                           return FALSE;
            }
            return FALSE;
        case WM_DESTROY:                                                             break;

    }
    return FALSE                                                                          ;
}

//-----------------------------------------------------------------------------//
//		Internal Core                                                          //
//-----------------------------------------------------------------------------//
BOOL CALLBACK MainDlgCore(HWND hwnd,UINT message,DWORD wParam,LONG lParam)
{    static NOTIFYICONDATA Icon      ;
     SrlDialogConfig   Config        ;
     GlobalApp       * GenEnv        ;
     WCHAR  ClassName[MAX_PATH]      ;
     WCHAR  WTemp    [MAX_PATH]      ;
     HWND   Item                     ;
     USHORT w1, h1                   ;
     WORD   w                        ;
      SHORT x,y                      ;
      LONG  i                        ;

    GenEnv = (GlobalApp *)GetWindowLong(hwnd,GWL_USERDATA)                               ;
    switch (message) 
    {   
        case WM_INITDIALOG:
             GenEnv =    (GlobalApp *)lParam                                             ;
             if (GenEnv==NULL)                
             {   DestroyWindow(hwnd);                                        return FALSE;}

             SetWindowLong(hwnd,GWL_USERDATA,(long)GenEnv)                               ;
             GenEnv = 
              (GlobalApp *)GetWindowLong(hwnd,GWL_USERDATA)                              ;
             if (GenEnv==NULL)                
             {   DestroyWindow(hwnd);                                        return FALSE;}
			 memset(&Icon,0,sizeof(Icon)   )                                             ;

             GnrWinReadPositions(_T(INI_FILE),_T("WinPos"),_T("Dialog") ,&x,&y,&w1,&h1)  ;
             if (x==0)
             {   GnrCenterWindow     (hwnd,GetDesktopWindow())                           ;
                 GnrWinWritePositions(_T(INI_FILE),_T("WinPos")                          ,
                                                   _T("Dialog")                          ,
                                                    hwnd,0,0           )                 ;
             }   else
             {   SetWindowPos(hwnd,NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);                }

             //-- Create icon from straight ---------------------------------------------- 
			 Icon.cbSize = sizeof(Icon)                                                  ;
             Icon.hWnd   = hwnd                                                          ;
             Icon.uID    = 1                                                             ;
             Icon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO                   ;
             Icon.uCallbackMessage = WM_USER+1                                           ;
             Icon.hIcon  =LoadIcon(GenEnv->Instance,MAKEINTRESOURCE(1))                  ;
             wcscpy_s(Icon.szInfoTitle ,sizeof(Icon.szInfoTitle)/2-1,TITTLE)             ;
             wcscpy_s(Icon.szInfo      ,sizeof(Icon.szInfo     )/2-1,TITTLE)             ;
             wcscpy_s(Icon.szTip       ,sizeof(Icon.szTip      )/2-1,TITTLE)             ;
             Shell_NotifyIcon(NIM_ADD   ,&Icon     )                                     ;
             SetTimer(hwnd,1,300,NULL              )                                     ; // Windows Timer: variuous Actions
             PostMessage(hwnd, MSG_POSCREATE,0,0)                                        ; // lanch Windows Client 
															                 return FALSE;

        case MSG_POSCREATE:
             SendToMainThreadL(GenEnv->InterThread,MSG_POSCREATE,0)                      ;  // Succesfully dialog Creation
                                                                             return FALSE;

        case WM_TIMER  : if (wParam!=1)                                      return FALSE; // If Icon Lost, Notify About It

             Icon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP                              ;
             Shell_NotifyIcon(NIM_MODIFY,&Icon );                            return FALSE; // Activate windows will Be The next One (return == FALSE)
                                                                             return FALSE;

		case WM_COMMAND   :                                 
             w = LOWORD(wParam)                                                          ;         

             if (w == SERIAL_COMA)
             {  Config.Port='A'                                                          ;    
                Config.Bcd=&GenEnv->Srl[0]                                               ;
                 x = DialogBoxParam(GenEnv->Instance                                     ,
                                    MAKEINTRESOURCE(IDD_SRL),hwnd                        ,
                                    (DLGPROC)DrvSrlCfg, (LPARAM)&Config)                 ;
                 if (x == 0)                                                 return FALSE;

                 //-- Configuration Has Accepted -----------------------------------------

                                                                                    break;
             }

             if (w == IDC_CLEAR  )
             {    SendDlgItemMessage(hwnd,IDC_LIST, LB_RESETCONTENT,0,0)                 ;
                                                                                    break;
             }
                                                                                    break;

        case MSG_PAINT_DLG   :  
             // Check Kind Of Control ----------------------------------------------------
             GetClassName(GetDlgItem(hwnd,(short)wParam),ClassName,sizeof(ClassName)/2-1);
             if (!wcscmp(ClassName,_T("ListBox")))
             {   if ( ((TCHAR*)lParam)[0]==0)
                         SendDlgItemMessage(hwnd, (short)wParam, LB_RESETCONTENT,0,    0);
                 else
                 {   SendDlgItemMessage(hwnd, (short)wParam,LB_ADDSTRING,0,lParam)       ;
                     i = SendDlgItemMessage(hwnd, (short)wParam, LB_GETCOUNT    ,0,    0);
                     if (i>1000)
                     {   SendDlgItemMessage(hwnd, (short)wParam, LB_DELETESTRING,0,    0);
                     i = SendDlgItemMessage(hwnd, (short)wParam, LB_GETCOUNT    ,0,    0);
                     }
                         SendDlgItemMessage(hwnd, (short)wParam, LB_SETTOPINDEX ,i-1,  0);
                 }
             }
             else  

            //-- Standard Paint --------------------------------------------------------
            {   Item = GetDlgItem(hwnd,(short)wParam)                                   ;
                GetWindowText(Item,ClassName,sizeof(ClassName)/2-1)                     ;
                if (wcscmp(ClassName,(WCHAR*)lParam))   
                {   SetWindowText(Item,(TCHAR*)lParam)                                  ;
                }
             }
             TermFreeMemory((void*)lParam)		                  ;                break;


        case WM_CLOSE     ://-- So Close has been Asked: Close all Other program --------

                        for(x=0;x<MAX_SRLS;x++)
                        {   swprintf(ClassName, sizeof(ClassName)/2-1,_T("COM%c"),'A'+x);
                        
                            swprintf(WTemp, sizeof(WTemp)/2-1,_T("%u;%u;%c%u%u")        ,
                                                             GenEnv->Srl[x].Port        ,
                                                             GenEnv->Srl[x].Speed       ,
                                                             GenEnv->Srl[x].Parity      ,
                                                             GenEnv->Srl[x].DataBits    ,
                                                             GenEnv->Srl[x].StopBits   );
                            PrgSetPrivateProfileString(_T("Default"), ClassName, WTemp  ,
                                                                           _T(INI_FILE));
                        }
                        GnrWinWritePositions(_T(INI_FILE),_T("WinPos")                  ,
                                                             _T("Dialog")               ,
                                                             hwnd,0,0                  );
                        SendToMainThreadL(GenEnv->InterThread,MSG_CLOSE ,0)             ;   
                        break;      

		case WM_DESTROY   :
                        Icon.cbSize = sizeof(Icon)                                      ;
						Icon.hWnd   = hwnd                                              ;
						Icon.uID    = 1                                                 ;
						Icon.uFlags = 0                                                 ;
						Shell_NotifyIcon(NIM_DELETE ,&Icon)                             ;
                        DestroyWindow(hwnd) ;
						PostQuitMessage(0)  ;	                                   break;
	}
    return FALSE                                                                        ;
}

static DWORD ThreadFunction(MtThreadStruct * ThreadData)
{   MSG   Msg                                            ;
    GlobalApp * GenEnv =(GlobalApp *)ThreadData->Userdata;

    GenEnv->MainDlg=CreateDialogParam(GenEnv->Instance,MAKEINTRESOURCE(IDD_MAIN)        ,
                         GetDesktopWindow(),(DLGPROC)MainDlgCore        ,(LPARAM)GenEnv);
	if (GenEnv->MainDlg==NULL)                                                 return -1;

    ThreadData->CreationOk = MTHREAD_OK                                                 ;
	while  (GetMessage( &Msg, NULL, 0, 0))   // Enter in regular Cycle 
    {       TranslateMessage(&Msg)                                                      ; 
            DispatchMessage (&Msg)                                                      ;   
    }                                                                           return 0;
}

//-----------------------------------------------------------------------------//
//		Creation / Destruction                                                 //
//-----------------------------------------------------------------------------//
BOOL CreateMainDlg( GlobalApp * GenEnv )
{	
    if (!MtThread((void*)GenEnv,ThreadFunction,&GenEnv->DlgTh,FALSE,TRUE       ,
                                                      MTHREAD_PRIORITY_REGULAR))
                                                                   return FALSE;
                                                                   return TRUE ;
}

BOOL SetPaintDlg(GlobalApp * GenEnv,WCHAR * Text,SHORT Control)
{	WCHAR * Message;
    ULONG   l      ;

	if (Text==NULL)										              return TRUE ;
    if (!TermGetMemory((char**)&Message,_tcslen(Text)*2+4,&l))  	  return FALSE;
    wcscpy_s(Message,l/2-1,Text)                                                  ;
	if (!PostMessage(GenEnv->MainDlg,MSG_PAINT_DLG,Control, (long)Message))
	{	TermFreeMemory((char**)Message)		                         ;return FALSE;}
																      return TRUE ;
}

void PostToDialog(GlobalApp * GenEnv,short Message,short User1,long User2)
{	 PostMessage(GenEnv->MainDlg,Message,User1,User2);                             }

void DestroyMainDlg(GlobalApp * GenEnv )
{   DWORD e;

    if (GenEnv->MainDlg==NULL)   return;
    PostToDialog(GenEnv,WM_DESTROY,0,0);
    MtDelThr(&GenEnv->DlgTh,1000,&e   );
}
