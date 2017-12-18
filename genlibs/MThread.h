//-----------------------------------------------------------------------------//
//   Complete Code Solution                                                    //
//   (c) By T&T 2009                                                           //               
//   Protected Threads & Semahores (Emulation Between DOS & windows)           //
//   resources over this module are "Global" because it protect Fatal Access   //
//   Between al resources & Systems                                            //
//-----------------------------------------------------------------------------//
#ifndef INCLUDE_MULTITHREAD_H
#define INCLUDE_MULTITHREAD_H

#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------//
//                  Global Defines                                             // 
//-----------------------------------------------------------------------------//
#define IS_UNKNOWN  -1                                        // Type Of Operating System Detected
#define IS_95_98_ME  0
#define IS_NT        1
#define IS_2000_XP   2
#define IS_VISTA     3
#define IS_W7        4
#define IS_CE42      5 
#define IS_DOS       6
#define IS_LINUX     7

#define PROTECT_MEMORY                        2               // Memory Protection 
#define PROTECT_THREAD                        4               // Protected section (of Fixed Ones)  

#define MT_MAX_VIOLATIONS_ALLOWED            30               // No More than 30 Vilations Allowed before critical error
#define THREAD_LIMIT_STOP                 12000               // in Ms, maximum Time To Stop a Thread
#define THREAD_MAX_CRITICAL                 230               // Maximum Number Of Critical Section Levels
#define THREAD_FREE_CRITICALS               210               // Of Those Criticals, What are Free Used !! Others is Assign Straight to kernell resources (20)

#define DDkPoll(a) void (a)(void * UserData,long ElapsedTime) // Pointer to poll System
typedef DDkPoll(DkPoll)                                     ;

//-----------------------------------------------------------------------------//
//                           Structures                                        // 
//-----------------------------------------------------------------------------//
typedef struct IMtActualDate        // Date/Time Estructure
{      
       unsigned short Year  ;
       unsigned short Month ; 
       unsigned short Day   ; 

       unsigned short Hour  ;
       unsigned short Minute;
       unsigned short Second;
       unsigned short Millis;       // Thousend of Seconds

}               MtActualDate;

//-----------------------------------------------------------------------------//
//                  general Functions                                          // 
//-----------------------------------------------------------------------------//
BOOL    MtGetName        (char * Name,short Sz               );                                 // Return In This Buffer machine's netbios name 
BOOL    MtStartPollSystem(DkPoll * Function,void * UserParams);                                 // Assign A Poll System Type ID
ULONG   MtGetStartTime( void )                                                     ;            // Get a machine Tick Count

void    MtGetmachineDate(MtActualDate * Date    )                                  ;            // Actual's Machine Local Date
ULONG   MtDifTime       (ULONG Time1,ULONG Time2)                                  ;            // Compute Diference Between two Times
void    MtWait          (ULONG Mils             )                                  ;            // Sleep Function

short   GetActualCount( void )                                                     ;            // Generate an Unique ID number
DWORD   GetProcessPid ( void )                                                     ;            // Get Actual Process ID

void    MtStartCounters( short * Counters  )                                       ;            // NOT IMPLEMENTED
void    MtRefrsCounters( short * Counters  )                                       ;            // NOT IMPLEMENTED

                                                                                                // Critical Error Based Upon Subsystems.        
void    MtCriticalError       ( UCHAR SubSystem )                                  ;            // Set A critical Error System
BOOL    MtIsCriticalError( void )                                                  ;            // This Semaphore is post if Critical Error is reached from User's or Other Kernell Modules
HANDLE  MtGetCriticalSem ( void )                                                  ;            // Get Critical Event Semaphore
void    MtResetCriticalError  ( UCHAR SubSystem );

unsigned long    MtGetSysteMemoryFree( void );													// Get memory values in Kb always
unsigned long    MtGetProcessMemoryInfo(ULONG ProcessId);
unsigned long MtGetProcessMemoryInfoByName(WCHAR * Name);


//-----------------------------------------------------------------------------//
//                  Critical Section Functions                                 // 
//-----------------------------------------------------------------------------//

                                                                                               // Critical Section Enter
                                                                                               // a= Number Of Critical Section 
                                                                                               // b= Time To retry Enter in critical Section
#ifdef  _WINDOWS
#define  StartProtect(a,b)          IntStartProtect((a),(b),__FILE__,(short)__LINE__,TRUE )   // False: Time Out reached
#endif

#ifdef  _WINCE
#define  StartProtect(a,b)          IntStartProtect((a),(b),__FILE__,__LINE__,TRUE         )   // False: Time Out reached
#endif

#ifdef  _DOS
#define  StartProtect(a,b)          IntStartProtect((a),(b),"DOS"   ,  0     ,TRUE         )   // False: Time Out reached
#endif

#ifdef  _EMULATOR
#define  StartProtect(a,b)          IntStartProtect((a),(b),"DOS"   ,  0     ,TRUE         )   // False: Time Out reached
#endif

#ifdef  _ARM_
#define  StartProtect(a,b)          IntStartProtect((a),(b),"DOS"   ,  0     ,TRUE         )   // False: Time Out reached
#endif


#ifdef _WINDOWS
#define  StartProtectNoRetry(a)     IntStartProtect((a),100,__FILE__,__LINE__,FALSE        )   // False: Time Out reached
#endif 

#ifdef _WINCE
#define  StartProtectNoRetry(a)     IntStartProtect((a),100,__FILE__,__LINE__,FALSE        )   // False: Time Out reached
#endif 

#ifdef _DOS
#define  StartProtectNoRetry(a)     IntStartProtect((a),100,"DOS"   ,  0     ,FALSE        )   // False: Time Out reached
#endif

#ifdef _EMULATOR
#define  StartProtectNoRetry(a)     IntStartProtect((a),100,"DOS"   ,  0     ,FALSE        )   // False: Time Out reached
#endif

#ifdef _ARM_
#define  StartProtectNoRetry(a)     IntStartProtect((a),100,"DOS"   ,  0     ,FALSE        )   // False: Time Out reached
#endif

BOOL    IntStartProtect( short Num,unsigned long milliseconds,char * File,short Line,
                                                                          BOOL Retry      );  // False: Time Out reached
void    EndProtect     ( short Num  );                                                        // Out From Critical Section

short   MtAssignPtrSection   ( void         );                                                // Allocate a Critical Section for Especific Operation
void    MtEndAssignProtection( short Number );                                                // return This Critical section To Sistem Pools
BOOL    MtCheckProtect( short Num );                                                          // Chck If section is already under protection

//-----------------------------------------------------------------------------//
//                  Semaphore Functions                                        // 
//-----------------------------------------------------------------------------//
void *  MtEvtSem(char * Name , BOOL Start,BOOL Manual);                                      // Create Event Semaphore (Named) or Unammed, Start value, Automatic rearming
BOOL    MtEndSem(void * Sem);                                                                // Destroy Semaphore
BOOL    MtSetSem(void * Sem);                                                                // post Semaphore
BOOL    MtRstSem(void * Sem);                                                                // reset Semaphore
short   MtWtSem (void * Sem,unsigned long  Time);                                            // Whait for Semaphore, Try before return Time milliseconds
short   MtWtMSem(           unsigned long  Time,unsigned short Number,...);                  // Wait on Multiple Semaphores
                                                                                             // Time: Timeout Value
                                                                                             // Number: Number of Semaphores   
                                                                                             // Then handles Or Semaphores or NULL if Semaphore is Not in Use
                                                                                             // Return -1: Error Critical
                                                                                             //         0: Timeout reached         
                                                                                             //         x: Number of Semaphore Posted

//-----------------------------------------------------------------------------//
//                  Thread Functions                                           // 
//-----------------------------------------------------------------------------//
typedef struct IMtThreadStruct
{       void * Thread       ;       // Thread Handle
        DWORD  ThrId        ;       // Thread ID
        void * SemThread    ;       // Semaphore to Start & Stop Thread
        void * Userdata     ;       // User Data to Send to Thread
        UCHAR  CreationOk   ;       // Thread Was Created OK (Signaling)
}       MtThreadStruct,MtThr;

#define MTHREAD_STARTING    0
#define MTHREAD_OK          1
#define MTHREAD_FAILED      2       // Thread Creation Failed

#define MTHREAD_PRIORITY_IDLE          0                    // level of Threads
#define MTHREAD_PRIORITY_LOW           1    
#define MTHREAD_PRIORITY_REGULAR       2    
#define MTHREAD_PRIORITY_HIGH          3
#define MTHREAD_PRIORITY_CRITICAL      4

#define MtThrFn(a) DWORD(a)(MtThreadStruct  * ThreadData)   // Thread Function
BOOL    MtWaitForThreadStart(MtThreadStruct * Thread);      // Active wait for Thread Starting
BOOL    MtThread   (void * UserParam         ,              // Create A Thread, And pass this User Params (Estruct)
                     MtThrFn(*ThreadFunction),              // Thread Function
                     MtThreadStruct * Thread ,              // A Struct of class Thread
                     BOOL StartStoped        ,              // Start Thread Stoped: Need a resume to Start working
                     BOOL Wait               ,              // Wait Until Thread Start OK, using semaphore of control 
                     UCHAR Level            );              // Thread Level

BOOL    MtWaitEndThr(MtThreadStruct * Thread  ,             // Waity until Thread will Stopped 
                                ULONG Timeout);
BOOL    MtDelThr    (MtThreadStruct * Thread  ,             // Destroy a Thread
                    ULONG Timeout,DWORD *Code);             // Wait This Timeout To Destroy, If not, Then Kill Thread & Return FALSE, If Return TRUE, Code contains User's exit Code

BOOL    MtEndThread(MtThreadStruct * Thread   ,             // Set and Exit Code To Thread
                    long ExitCode)            ;

BOOL    MtResumeThread(MtThreadStruct * Thread,
                       BOOL Action           );             // Start / Resume Thread (BOOL)

BOOL    MtSetThreadPriority(MtThreadStruct * Thread,        // Set Thread Priority
                            UCHAR Level            );

void    MtSetSignalThreadToFinih(MtThreadStruct * Thread);  // Signal Thread to finish
BOOL    MtIsThreadFinih(MtThreadStruct * Thread);           // Check from Main Thread if Thread already Finished or Not 

BOOL    MtIsSignalThreadFinih(MtThreadStruct * Thread);     // Thread Signaled to finish?
void *  MtGetTheadSem(MtThreadStruct * Thread);             // Get Thread Finish Semaphore
void *  MtGetTheadObj(MtThreadStruct * Thread);             // handle of Thread Using also as Semaphore

//-----------------------------------------------------------------------------//
//                  Miscellaneous Functions                                    // 
//-----------------------------------------------------------------------------//
BOOL    MtEjectVolume(WCHAR VolumeToDismount);              // Forcing ejecting of volume (USB include)   

#ifdef __cplusplus
}
#endif

#pragma pack()

#endif
