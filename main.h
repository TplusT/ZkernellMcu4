//-----------------------------------------------------------------------------//
//		Main C18 (Emulator)                 			                       //
//      (c) By PyV 2017                                                        //
//      Jorge Contreras ramirez                                                //
//-----------------------------------------------------------------------------//
#include "TrMemory.h"
#include "MThread.h"
#include "QueueMgr.h"
#include "LogMgr.h"

#define TITTLE            _T("C18Emulator") 
#define INI_FILE          ".\\C18Emulator.ini"

//-----------------------------------------------------------------------------//
//		Messages on Windows                     		                       //
//-----------------------------------------------------------------------------//
#define MSG_PAINT_DLG     WM_USER+2
#define MSG_POSCREATE     WM_USER+3  
#define MSG_CLOSE         WM_USER+5

//-----------------------------------------------------------------------------//
//		Maximum Defines                         		                       //
//-----------------------------------------------------------------------------//
#define DRV_COMMAND_FROM_USER  100  // From General Libraries 

#define SET_CLK_TIMEOUT       1000  // Timeout every 1000s (for wathdog and other things) 
#define MAIN_STANDAR_TICK      100  // Tick On windows for real behaviour 
#define MAIN_TH_MAX_COMMANDS   100  // max  Command in Main Queue 
#define SRL_COMA_RX_BUFFER      50  // Buffer For ComA (Rx)

#define MAX_SRLS                 4  // Max Serial Ports

//-----------------------------------------------------------------------------//
//		Structures                              		                       //
//-----------------------------------------------------------------------------//
#pragma pack(2)                      //   Equal to PIC 

typedef struct ISrlCom
{   HANDLE          SrlHandle     ; // Com handle
    BYTE            Port          ; // port 
    WORD            Speed         ; // Speed
    BYTE            Parity        ; // parity
    BYTE            DataBits      ; // Data Bits
    BYTE            StopBits      ; // Stop Bits 
}   SrlCom                        ;

typedef struct IGlobalApp            // Global Aplication Structure
{   TQueue *     InterThread      ;  // InterThread Queue
    HINSTANCE    Instance         ;
    HWND         MainDlg          ;  // View Dialog
    MtThreadStruct  DlgTh         ;  // Thread for InterProcess
    BOOL         DialogView       ;  // Dialog Is Viewed Or Not

    SrlCom       Srl[MAX_SRLS]    ;  // port(s) To Open
}   GlobalApp                     ; 

typedef struct
{                                   // Interprocess Structure  
    UCHAR          SubSystem      ; // System Comming From       
    USHORT         Command        ; // Command to Perform (Depend Of System)  
    void    *      Ptr            ; // Free Pointer  
    BOOL           DialogView     ; // Dialog already Viewed
    UCHAR          Data[MAX_PATH] ; // parameters Onto This queue 
}   DrvCommnandSend               ;

#pragma pack()

//-----------------------------------------------------------------------------//
//		Main C18 & MainDialog (Just Messages)   		                       //
//-----------------------------------------------------------------------------//
#define DVRCommandSendFunctionL(a) void (a)(TQueue * Queue,USHORT command,ULONG   Data)  // Send Data to Main (Long ) from Thread
#define DVRCommandSendFunctionA(a) void (a)(TQueue * Queue,USHORT command,UCHAR * Data)  // Send Data to Main (Ascii) from Thread
DVRCommandSendFunctionL(SendToMainThreadL);
DVRCommandSendFunctionA(SendToMainThreadA);

BOOL SetPaintDlg        (GlobalApp * GenEnv,WCHAR * Text,SHORT Control               );      //  Send Data to specific control Over Dialog
BOOL CreateMainDlg      (GlobalApp * GenEnv                                          );
void DestroyMainDlg     (GlobalApp * GenEnv                                          );

UINT  PrgGetPrivateProfileInt   (WCHAR * lpAppName,WCHAR * lpKeyName,int      nDefault,WCHAR * lpFileName                                       );      // Get Data from Ini 
DWORD PrgGetPrivateProfileString(WCHAR * lpAppName,WCHAR * lpKeyName,WCHAR * lpDefault,WCHAR * lpReturnedString,DWORD   nSize,WCHAR * lpFileName);

//-----------------------------------------------------------------------------//
//		Serial Port layer                       		                       //
//-----------------------------------------------------------------------------//
#define SRL_NO_DATA       0x00
#define SRL_ERROR         0xFF
#define SRL_OK            0xF1

void   SerialFormatStart(WCHAR * Data,SrlCom * Bcd)                            ;                        // Format Starting value 
HANDLE SerialStart(SrlCom * Bcd )                                              ;                        // Start Serial Layer 
void   SerialEnd  (HANDLE handle)                                              ;                        // End Serial Layer  
BOOL   SerialSend (HANDLE handle, BYTE * Buffer, WORD Size)                    ;                        // Blocking Send 
BYTE   SerialWaitForData(HANDLE handle, BYTE * Data, DWORD Timeout)            ;                        // Non Blocking Received 

#ifdef __cplusplus
}
#endif

