//-----------------------------------------------------------------------------//
//   Complete Code Solution                                                    //
//   (c) By T&T 2011                                                           //
//   Log System                                                                //
//-----------------------------------------------------------------------------//
#ifndef INCLUDE_LOGMGR_H
#define INCLUDE_LOGMGR_H

#pragma pack(1)

#define LOGMGR_MAIN_APP		9

#define LOGMGR_UDP_LIB     10
#define LOGMGR_TCP_S_LIB   11
#define LOGMGR_TCP_C_LIB   12
#define LOGMGR_QUQ_LIB     13
#define LOGMGR_SRL_LIB     14
#define LOGMGR_THR_LIB     15
#define LOGMGR_MEM_LIB     16
#define LOGMGR_WDG_LIB     17
#define LOGMGR_DRV_LIB     18
#define LOGMGR_DBS_EXCEL   19
#define LOGMGR_DBS_SQL     20
#define LOGMGR_DBS_VMGR    21

#define LOGMGR_GENEARL_C   30

#define LOGMGR_USR_LIB   1000  // From this module Use by User

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------//
//                  Exported Functions                                         // 
//-----------------------------------------------------------------------------//
#define LOGMGR_MSG(a) void(a)(WCHAR * Message,short Length,unsigned short TypeOfLog,\
                                                           unsigned short Warnlevel,\
                                                           void * UserData         )
				

#define LOGMGR_MSG_PRINT(a) void(a)(WCHAR * Message,short Length,unsigned short TypeOfLog,\
                                                                 unsigned short Warnlevel,\
                                                                 void * UserData         ,\
									                             void * UserEnviro       )


LOGMGR_MSG(PrintToLog);                 // Main Funtion to Use Log 
void AttachLog(LOGMGR_MSG_PRINT(*DerivateLog),void * UserEnviro) ;
void DetachLog(void);

#ifdef __cplusplus
}
#endif

#pragma pack()

#endif
