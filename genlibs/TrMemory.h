//-----------------------------------------------------------------------------//
//   Complete Code Solution                                                    //
//   (c) By SmartDevices 2005                                                  //
//   Memory Allocation System                                                  //
//   (c) PyV 2002   Jorge Contreras   V:0.0                                    //
//-----------------------------------------------------------------------------//
#ifndef INCLUDE_MEMORY_H
#define INCLUDE_MEMORY_H

#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif

//---- Memory round to Granularity Page ------------------------------
BOOL TermGetMemory                (char **Data,unsigned long memory,unsigned long * RealAlloc); // Ask About Memory, Data return handle
BOOL TermFreeMemory               (  void * memory     );               // Delete Previous Memory

//---- Memory round to Granularity Page using system global Memory ---
BOOL TermAbsoluteAbsoluteGetMemory(char **Data,unsigned long memory,unsigned long * RealAlloc);
BOOL TermAbsoluteFreeMemory       (  void * memory );

//---- Memory without Granularity Page -------------------------------
BOOL TermGetMemoryNoGgr           (char **Data, unsigned long memory);
BOOL TermFreeMemoryNoGgr          (void * memory   );

#ifdef __cplusplus
}
#endif

#pragma pack()

#endif