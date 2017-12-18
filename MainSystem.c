//-----------------------------------------------------------------------------//
//                                                                             // 
//		C18 System Emulator                     			                   //
//      (c) By PyV 2017                                                        //
//      Jorge Contreras ramirez                                                //
//                                                                             // 
//-----------------------------------------------------------------------------//
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <share.h>
#include <Tlhelp32.h>

#include "resource.h"
#include "main.h"
#include "compiler.h"

//-----------------------------------------------------------------------------//
//		Global Printer                                      		           //
//-----------------------------------------------------------------------------//
LOGMGR_MSG_PRINT(MsgPrint)
{   GlobalApp * GenEnv = (GlobalApp *)UserEnviro;

    UserData=(void *)IDC_LIST                       ;    
    SetPaintDlg(GenEnv,Message,(SHORT)UserData)     ;
}

//-----------------------------------------------------------------------------//
//		IProcess Comunication                               		           //
//-----------------------------------------------------------------------------//
DVRCommandSendFunctionL(SendToMainThreadL)
{   DrvCommnandSend S;

    S.SubSystem     =DRV_COMMAND_FROM_USER               ;
    S.Command       =command                             ;
	*((LONG*)S.Data)=Data                                ;
    InputQueueV3(Queue,NULL,0,&S,sizeof(DrvCommnandSend));
}

DVRCommandSendFunctionA(SendToMainThreadA)
{   DrvCommnandSend S;

    S.SubSystem     =DRV_COMMAND_FROM_USER               ;
    S.Command       =command                             ;
	strcpy_s(S.Data,sizeof(S.Data)-1,Data)               ;
    InputQueueV3(Queue,NULL,0,&S,sizeof(DrvCommnandSend));
}

//-----------------------------------------------------------------------------//
//                                                                             //
//                                                                             //
//		Main Loop 													           //
//                                                                             //
//                                                                             //
//-----------------------------------------------------------------------------// 
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine    ,  int nCmdShow         )
{   GlobalApp       GenEnv                             ;  // Full Structure Program
    DrvCommnandSend Cs                                 ;  // Inter Program Element
    SYSTEMTIME      Time                               ;
    DWORD           AntTime,ActualTime,ElapsedTime     ;
     WORD           Timeout                            ;
      LONG          r                                  ;
    USHORT          c                                  ;
    WCHAR           Message [MAX_PATH+1]               ;
    WCHAR           Wtemp   [MAX_PATH+1]               ;
    BYTE            LastError                          ;
    DWORD           LastGetError                       ;
     BOOL           FirstView = TRUE                   ;

    GenEnv.Instance   =hInstance			          		        ;
    LastError         = 0                                           ;  
    LastGetError      = 0                                           ;

    //--------------------------------------------------------------//
    //		Ini Values                                              //
    //--------------------------------------------------------------//
    memset(&GenEnv,0,sizeof(GenEnv))                                ;
    GenEnv.Instance    =hInstance			          		        ;
    for (r = 0; r<MAX_SRLS; r++)
    {   swprintf(Wtemp, sizeof(Wtemp)/2-1,_T("COM%c"),'A' +  r     );
        c=(USHORT)PrgGetPrivateProfileString(_T("Default"), Wtemp   ,
                                 _T("1,9600,N81")                   ,
                                 Message,sizeof(Message)/2-1        ,
                                 _T(INI_FILE))                      ;
        if (!c)          
        { PrintToLog(_T("COM A Invalid"), 0, 0, 0, &GenEnv);return-1;
        }
        SerialFormatStart(Message,&GenEnv.Srl[r]                   );                      
    }                                                               ;

    //--------------------------------------------------------------//
    //		Queue Comunication to From Dialog                       //
    //--------------------------------------------------------------//
    GenEnv.InterThread=(TQueue*)
    StartQueueV3(sizeof(DrvCommnandSend), MAIN_TH_MAX_COMMANDS      ,
                                         "InterThread"             );
    if (GenEnv.InterThread==NULL)                          
    {   PrintToLog(_T("InterThread QUEUE Invalid")                  ,
                                      0, 0, 0, &GenEnv);   return -1;
    }

    //--------------------------------------------------------------//
    //		Comunication Dialog                                     //
    //--------------------------------------------------------------//
    if (!CreateMainDlg(&GenEnv))
    {   PrintToLog(_T("Can't Create main Dialog"),  0, 0, 0, &GenEnv);
        EndQueueV3(GenEnv.InterThread    );                 return -1;
    }

    //--------------------------------------------------------------//
    //		Attach to LOG Start                                     //
    //--------------------------------------------------------------//
    AttachLog(MsgPrint, &GenEnv)                                     ; // Now have Dialog To Present Errors

    //--------------------------------------------------------------//
    //		Start "up"                                              //
    //--------------------------------------------------------------//
    Timeout             = SET_CLK_TIMEOUT                           ;
    AntTime             = MtGetStartTime()                          ;
    GenEnv.DialogView   = FALSE                                     ;

    do
    {
        if (!GenEnv.DialogView)                       goto SEMAPHORE;
        
        //----------------------------------------------------------//
        //		Check Elapsed Time                                  //
        //----------------------------------------------------------//
        ActualTime   =  MtGetStartTime()                            ;
		ElapsedTime  =  MtDifTime(AntTime,ActualTime)               ;
		AntTime		 =  ActualTime							        ;

		//----------------------------------------------------------//
		//		Global Timeout                                      //
		//----------------------------------------------------------//
        Timeout     -=(WORD)ElapsedTime                             ;
        if (Timeout<=0)
        {   Timeout       = SET_CLK_TIMEOUT-100                     ;
            GetLocalTime(&Time)                                     ;
            swprintf(Message,sizeof(Message)/2-1                    ,
                                  _T("%2u:%02u.%02u")               ,
                                  Time.wHour                        ,
                                  Time.wMinute                      ,
                                  Time.wSecond                     );
			SetPaintDlg(&GenEnv,Message,IDC_TIME                   );

            if (FirstView)
            {   PrintToLog(_T("Start App"), 0, 0, 0, &GenEnv)       ;
                swprintf(Message,sizeof(Message)/2-1                ,
                                           _T("%ub"),sizeof(1224))  ;
                SetPaintDlg(&GenEnv,Message,IDC_SIZEPIC            );
                FirstView = FALSE                                   ;
            }
        }

        //---------------------------------------------------------//
		//		Semaphore Waiting                                  //
		//---------------------------------------------------------//
        SEMAPHORE:
        r = MtWtMSem(MAIN_STANDAR_TICK,1                            ,
                     SemQueue        (GenEnv.InterThread)    )      ;
        if (r==-1)                                                    // Retry Due To Error of System
        {   Sleep(1000);
            PrintToLog(_T("Critical "),0,0,0,&GenEnv         )      ;
            continue                                                ;
        }  

        //---------------------------------------------------------//
        //           Dialog <> Interprocess                        //
        //---------------------------------------------------------//
        if (r==1)        // Inter Process Comunication System 
        {   r=OutputQueueV3(GenEnv.InterThread,&Cs,sizeof(Cs))      ;
            if (r<=0)                                       continue; // No Element in Queue or Element Size = 0
            if (Cs.SubSystem!=DRV_COMMAND_FROM_USER)        continue; // Only IProcess SubSystem

            if (Cs.Command  == MSG_POSCREATE)
            {   GenEnv.DialogView   = TRUE;                 continue;}
            if (Cs.Command  ==MSG_CLOSE)                       break;
        }
       
    }   while(1)                                                    ;

	PrintToLog(_T("End App"),0,0,0,&GenEnv                  )       ;
    
    DestroyMainDlg(&GenEnv)                                         ;
    if (GenEnv.InterThread    !=NULL) EndQueueV3(GenEnv.InterThread);
    DetachLog()                                                     ;
	return 0												        ;
}   
