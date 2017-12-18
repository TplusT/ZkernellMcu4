//-----------------------------------------------------------------------------//
//   Complete Code Solution                                                    //
//   (c) By SmartDevices 2005                                                  //
//   Queue manager: manejador de Colas                                         //
//   (c) PyV 2002   Jorge Contreras   V:0.0                                    //
//-----------------------------------------------------------------------------//
#ifndef INCLUDE_QUEUEMGR_H
#define INCLUDE_QUEUEMGR_H

#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------//
//          Internal Estructures : FIFO                                        //    
//-----------------------------------------------------------------------------//
typedef struct IntAsocQueue AsocQueue;

typedef struct IntElemQueue               // Item Structure
{   LONG   MaxSize              ;         // Max Size of The element
    LONG   ActualSize           ;         // Actual Size Of element
    CHAR * Element              ;         // Data of Element 
}   TElemQueue                  ;

typedef struct IntQueue                   // Queue Structure
{   HANDLE         OutSemph     ;         // Event Semaphor Indicate Elements remain in Queue
    CHAR           QueueName[MAX_PATH+1]; // Logical Queue Name

    SHORT          InElement    ;         // Actual Number of In Elements
    SHORT          OutElement   ;         // Actual Out Element
    SHORT          ActElements  ;         // Actual Elements in The queque 
    SHORT          MaxElement   ;         // max elements in Queue

    BOOL           AutoSize     ;         // Selected Autosize:

    SHORT          SectionAssign;         // Critical Section Assign

    AsocQueue    * JoiningAsocc ;         // If Has An Asocition Pending of this queue
    SHORT          NumberInAsocc;         // Set The Number Of This que Onto Asociation (Aceleration System) 
    TElemQueue   * Element      ;         // memory of Elements
}   TQueue                      ;

typedef struct IntMemoQueue               // Queue Structure
{   BOOL           IsServer     ;
    void        *  Queue        ;         // Client Side acces to queue  

    HANDLE         OutSemph     ;         // Event Semaphor Indicate Elements remain in Queue
    HANDLE         CritSem      ;         // Critical Event Semaphore   
    HANDLE         SharedMemo   ;         // Overal Shared memory 

    ULONG          InElement    ;         // Actual Number of In Elements
    ULONG          OutElement   ;         // Actual Out Element
    ULONG          ActElements  ;         // Actual Elements in The queque 
    ULONG          MaxElement   ;         // max elements in Queue

    ULONG          Size         ;         // Max Size of Elements   

}   MQueue                      ;

typedef struct IntMemoElemQueue           // Item Structure
{   ULONG  ActualSize           ;         // Actual Size Of element
}   MemoElemQueue               ;

//-----------------------------------------------------------------------------//
//          Internal Estructures : Queue Asociations                           //    
//-----------------------------------------------------------------------------//
typedef struct IntElemAsoc              // Item Structure
{   TQueue  * Queues            ;       // Point to an Array Of FIFO Queues 
    BOOL      QueueSet          ;       // Status Of the Queue
    BOOL      QueueWorking      ;       // Take This Queue In Accout In Semaphore Asociation  
}   ElemAsoc                    ;

struct IntAsocQueue
{   short    Size               ;       // Size of Asociations 
    short    SectionAssign      ;       // Critical Section Asigned to This resource
    HANDLE   GlobalSemph        ;       // Global Asociation Semaphore 
    ElemAsoc * Elem             ;       // One Element By Each Queue
}                               ;

//-----------------------------------------------------------------------------//
//          Exported Functions                                                 //
//-----------------------------------------------------------------------------//
#define QU_AUTOSIZE             0       // Size of Element will Be Automatic Calculate!!

//-----------------------------------------------------------------------------//
//          regular FIFO Queues                                                //
//-----------------------------------------------------------------------------//
void *  StartQueueV3       ( long size,short MaxElement,char * Name)           ;   // Create Queue(QU_AUTOSIZE), With elements Of This Size, With This max Of Elements And This Unique name in The Sistem
BOOL    EndQueueV3         ( TQueue * Queue )                                  ;   // Destroy a FIFO Queue 

//---------------- InputQueue ------------------------------------------------//
// Output:                                                                    //
// >0 Not Enough Size To Process Queue: return Size Needed (Head+Element)     //
//  0 All Ok                                                                  //
// -1 No Enought Queue (Queue Overflow) or other kind of errors               //
//----------------------------------------------------------------------------//
long    InputQueueV3       ( TQueue * Queue                                    ,   // Input Element in Queue
                                            void * Header  ,long Size2         ,   // Header of Element. Could Be 0 & Null if Not header Needed     
                                            void * Element ,long Size         );   // Element Data: It Not AutoQueue, Then It Cut to Actual Queue Elements Value

//---------------- OutputQueueBlocks -----------------------------------------//
// Input: Buffers Element1, real Buffer, element 2 , Max Size                 //
// If Sz == 0 Not Exist First Block ,and element could be NULL                //
// If Sz1z<=0 && Element2 == NULL Don't Extract Second Block of DATA !!       //
//                                                                            //
// Output:                                                                    //
// -2 Not Enough Size To Process Queue                                        //
// -1 Not More Elemnts Or Error                                               //
// >=0 Size Of Element                                                        //
//                                                                            //
// Sz =0, Sz1=0 return Size of element pending in queue                       //  
//---------------- OutputQueueBlocks -----------------------------------------//
#define OutQueueKeep         0      // Peek Data from Queue But Don't remove Element)
#define OutQueueRemove       1      // Remove element if Have Size In OutPut
#define OutQueueRemoveAlways 2      // Remove Element Anyway (Even if Error of Not Space To Hold Element)

#define OutputQueueV3(a,b,c) OutputQueueBloqsV3(a,NULL,(long)0,b,c,OutQueueRemoveAlways); 
long    OutputQueueBloqsV3 ( TQueue * Queue                                    ,   
                             void * Element1,long Size                         ,   
                             void * Element2,long Size2                        ,   
                             UCHAR  Behaviour                                 );

long    DataInQueueV3       ( TQueue * Queue);                                       // Check if Element in Queue. if yes, Then return Size Of element (Including Header+2nd Block) if not return -1
ULONG   GetActualQueueSizeV3( TQueue * Queue);                                       // get Actual QUEUE Sizes
BOOL    CleanQueueV3        ( TQueue * Queue);                                       // Empty Fifo Queue
void *  SemQueue            ( TQueue * Queue);                                       // get a Queue Semaphore (Indicate there are elements in Queue)

typedef BOOL (*ProcessQueueFunction)(void * Data,long  Size,void * UserEnviro     ); // Data Of Element, Size & UserEnviro
                                                                                     // if return TRUE Element remains in Queue
                                                                                     // if return FALSE Element Remove from Queue 
#define PROCESSQUEUEFUNCTIONV3(a) BOOL(a)(void * Data,long  Size,void * UserEnviro)

/*void ProcessQueueV3(TQueue * Queue,void * UserEnviro,ProcessQueueFunction Function); // This Enummerate all elements in Queueu (element By Element calling User)
                                                                                     // Using Previous Function*/
void ProcessQueueV4(TQueue * Queue,void * UserEnviro,ProcessQueueFunction Function); // This Enummerate all elements in Queueu (element By Element calling User)
                                                                                     // Using Previous Function

//-----------------------------------------------------------------------------//
//          Asociating Queues in Bloqs                                         //
//          That means That Some Queues Acts like a Bloq                       //
//          hving element in one of this Post Global Semaphore                 //
//-----------------------------------------------------------------------------//
AsocQueue *     CreateQueueAsociation(short Number);                               // Create an Asociation of This Number Of Queues
BOOL DestroyQueueAsociation(AsocQueue * Asociation);                               // Destroy This Asociation
BOOL AssociateQueue(AsocQueue * Asociation,TQueue * Queue,BOOL On);                // Set (TRUE) or Delete(FALSE) Queue from Asociation & Put The state of The asociation On Line: By default Asociation Is Disconected
BOOL ActivateAsoccQueue(AsocQueue * Asociation,TQueue * Queue,BOOL OnOff);         // Over One Asociation Queue, Activate to Take in account This queue
void * SemQueueAsociation(AsocQueue * Asociation)                        ;         // Global Asociation Semaphore

//-----------------------------------------------------------------------------//
//          Memory IPC Queues                                                  //
//-----------------------------------------------------------------------------//
void * StartMemoQueueV3  ( ULONG Size,ULONG MaxElement ,UCHAR * Name,BOOL IsServer);
BOOL   EndQueueMemoV3    ( MQueue * Queue )                        ;
LONG   InputQueueMemoV3  ( MQueue * Queue,void * Element,ULONG Size);
LONG   OutputQueueMemosV3( MQueue * Queue,void * Element,ULONG Size);
void * SemQueueMemoV3     (MQueue * Queue);
LONG   QueueCleanMemoV3   (MQueue * Queue);
#ifdef __cplusplus
}
#endif

#pragma pack()

#endif
