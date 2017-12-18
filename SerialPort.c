//-----------------------------------------------------------------------------//
//                                                                             // 
//		C18 System Emulator / Serial Port as PIC emulation                     //
//      (c) By PyV 2017                                                        //
//      Jorge Contreras ramirez                                                //
//                                                                             // 
//-----------------------------------------------------------------------------//
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <io.h>

#include "main.h"

//-----------------------------------------------------------------------------//
//                                                                             //
//  Serial port Routines  (phisical Layers)                                    //
//                                                                             //
//-----------------------------------------------------------------------------//
void SerialFormatStart(WCHAR * Data, SrlCom * Bcd)
{   WCHAR * Start              ;

    Bcd->Port     = 1          ;
    Bcd->Speed    = 9600       ;
    Bcd->Parity   ='N'         ;
    Bcd->DataBits = 8          ;
    Bcd->StopBits = 1          ;

    Start = wcschr(Data,   ';' );if (Start==NULL)    return; Start[0]=0; Bcd->Port =_wtoi(Data);Data=Start+1;
    Start = wcschr(Data,   ';' );if (Start==NULL)    return; Start[0]=0; Bcd->Speed=_wtoi(Data);Data=Start+1;
    switch(Data[0])
    {   case 'N':Bcd->Parity='N';break                      ;
        case 'O':Bcd->Parity='O';break                      ;
        default :Bcd->Parity='E';break                      ;
    }

    if (++Data==0)                                    return;
    switch (Data[0])
    {   case'7':Bcd->DataBits=  7 ; break;
        default:Bcd->DataBits=  8 ; break;
    }

    if (++Data == 0)                                  return;
    switch (Data[0])
    {
        case'1':Bcd->StopBits =  1 ;break;
        default:Bcd->StopBits =  2 ;break;
    }
}

HANDLE SerialStart(SrlCom * Bcd)
{
    DWORD          Err         ;
    DCB            dcb         ;
    HANDLE         handle      ;
    COMMTIMEOUTS   Tm          ;
    WCHAR          PortName[10];

    // Create name of port 1-9   ----------------------------------------------
    swprintf_s(PortName, sizeof(PortName) / 2 - 1,_T("COM%u:"), Bcd->Port)    ;
    
    // Try To Open a Port Device ----------------------------------------------
    handle = CreateFile(PortName,GENERIC_READ | GENERIC_WRITE,0,NULL          ,
                        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL          )   ;
    if (handle == INVALID_HANDLE_VALUE)				       return (HANDLE)NULL;

    // Check For Health of This Device ----------------------------------------
    if (!GetCommState(handle, &dcb)) 
    {   Err = GetLastError(); CloseHandle(handle);         return (HANDLE)NULL;}

    // Now Configuring Port Uppond Parameters ---------------------------------
    dcb.fBinary         = TRUE                                                ;  // Always Transfers will be binary Bassed
    dcb.fParity         = FALSE                                               ;  // Never Chek parity So, don't Create Parity Erros
    dcb.fOutxDsrFlow    = FALSE                                               ;  // Always Tx Even Dsr State
    dcb.fDsrSensitivity = FALSE                                               ;  // Always Tx Even Dsr State
    dcb.fOutxCtsFlow    = FALSE                                               ;  // Always Tx Even Cts False
    dcb.fOutX           = FALSE                                               ;  // Non Xonn/ Xoff Protocol
    dcb.fInX            = FALSE                                               ;
    dcb.fNull           = FALSE                                               ;  // Not Null Bytes Discarded
    dcb.fRtsControl     = RTS_CONTROL_DISABLE                                 ;  // This for 485 Behaviour
    dcb.fDtrControl     = DTR_CONTROL_DISABLE                                 ;
    dcb.fAbortOnError   = FALSE                                               ; // never Stop Rx/tx: Not Neccesary ClearCom state

    //-- Speed & Other Params -------------------------------------------------
    dcb.ByteSize        = Bcd->DataBits                                       ;
    switch(Bcd->StopBits)
    {      case'1':    dcb.StopBits = ONESTOPBIT ;  break                     ;
           case'2':
           default:    dcb.StopBits = TWOSTOPBITS;  break                     ;
    }
    switch (dcb.Parity  )
    {
           case'N':    dcb.StopBits = NOPARITY  ;  break                      ;
           case'O':    dcb.StopBits = ODDPARITY ;  break                      ;
           default:    dcb.StopBits = EVENPARITY;  break                      ;
    }
    dcb.BaudRate        = Bcd->Speed                                          ;
    if (!SetCommState(handle,&dcb)) 
    {   Err = GetLastError(); CloseHandle(handle);         return (HANDLE)NULL;}

    //-- Internal Buffers -----------------------------------------------------
    if (!SetupComm(handle, SRL_COMA_RX_BUFFER,0))
    {   Err = GetLastError(); CloseHandle(handle);         return (HANDLE)NULL;}

    //-- Timeouts -------------------------------------------------------------
    if (!GetCommTimeouts(handle, &Tm))
    {   Err = GetLastError(); CloseHandle(handle);         return (HANDLE)NULL;}

    //-- Timeouts manager -------------------------------------------------------
    //-- Acording to microsoft: 
    //-- Tm.ReadIntervalTimeout = 0xFFFFFFFF and other part to 0, Read will be 
    //--          return inmediatly with the bytes already rx or 0 if no Bytes
    Tm.ReadIntervalTimeout = 0xFFFFFFFF                                        ;             
    Tm.ReadTotalTimeoutMultiplier   = 0                                        ;
    Tm.ReadTotalTimeoutConstant     = 0                                        ;
    Tm.WriteTotalTimeoutMultiplier  = 0                                        ;
    Tm.WriteTotalTimeoutConstant    = 0                                        ;
    if (!SetCommTimeouts(handle, &Tm))
    {   Err = GetLastError(); CloseHandle(handle);         return (HANDLE)NULL;}
    
    // if All Was OK, Clean Port Errors Before begin To Rx ----------------------
    if (!ClearCommError(handle, &Err, NULL))
    {   Err = GetLastError(); CloseHandle(handle);         return (HANDLE)NULL;}
    return handle                                                             ;
}

void SerialEnd(HANDLE handle)
{
    if (handle == NULL || handle == INVALID_HANDLE_VALUE)                return;

    //--- Purge Any Pending Operation -----------------------------------------
    PurgeComm(handle,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR| PURGE_RXCLEAR) ;
    CancelIo (handle)                                                          ; // cancel Any pending Operations !!
    EscapeCommFunction(handle, CLRDTR | CLRRTS)                                ; // Clear RTS & DTR To Setop Any Modem Comunication
    Sleep(150)                                                                 ; // Time to close The modem 
    CloseHandle(handle)                                                        ; // Free resources 
}

BOOL SerialSend(HANDLE handle,BYTE * Buffer,WORD Size)
{   DWORD BytesWrite;

    if (handle == NULL || handle == INVALID_HANDLE_VALUE)       return TRUE    ;
    if (!WriteFile(handle, Buffer, Size, &BytesWrite, NULL))    return FALSE   ;
    if (Size!= BytesWrite)                                      return FALSE   ;
                                                                return TRUE    ;
}

static WORD SerialReceive(HANDLE handle, BYTE * Buffer, WORD MaxSize)
{   DWORD   BytesRead;
    COMSTAT Stat     ;
    
    if (handle == NULL || handle == INVALID_HANDLE_VALUE               ) return 0xFFF1   ; // Error in received 
    if (!ClearCommError(handle, NULL, &Stat)                           ) return 0xFFF1   ; // Error in received 
    if (Stat.cbInQue == 0                                              ) return 0xFFF2   ; // No Data 
    if (Stat.cbInQue > MaxSize) Stat.cbInQue = MaxSize                                   ; // Truncation 
    if (!ReadFile(handle, Buffer, (WORD)Stat.cbInQue, &BytesRead, NULL)) return 0xFFF1   ; // Error in received 
                                                                    return (WORD)BytesRead; // return Number of characters
}
 
BYTE SerialWaitForData(HANDLE handle,BYTE * Data,DWORD Timeout)
{   WORD  StatRx  ;
    DWORD PrevTime;

    PrevTime = MtGetStartTime()                                                                ;
    while (1)
    {
        if (Timeout && MtDifTime(PrevTime, MtGetStartTime()) > Timeout)      return SRL_NO_DATA; // Timeout 

        StatRx = SerialReceive(handle, Data, 1)                                                ;
        if (StatRx == 0xFFF2)                                                return SRL_NO_DATA; // Timeout 
        {   if (!Timeout)                                   
            Sleep(10)                         ;                                        continue;
        }                                                                                        // No data on Queue 
        if (StatRx == 0xFFF1)                                                return SRL_ERROR  ; // Error On Port
                                                                             return SRL_OK     ; // There are Data 
    }
}

BOOL SerialFlushPort(HANDLE handle)
{   WORD  StatRx ;
    BYTE  Data[1];

    while (1)
    {   StatRx = SerialReceive(handle, Data, 1)                                                ;
        if (StatRx == 0xFFF2)                                                return SRL_OK     ; // No data on Queue 
        if (StatRx == 0xFFF1)                                                return 0xFF       ; // Error On Port
    }
}
