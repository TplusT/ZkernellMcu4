//-----------------------------------------------------------------------------//
//		Main C18 (Emulator)                 			                       //
//      (c) By PyV 2017                                                        //
//      Jorge Contreras ramirez                                                //
//                                                                             //
//      Internal Define to Emulate PIC                                         //
//                                                                             //
//-----------------------------------------------------------------------------//
#ifndef __GENERIC_PICS_DEFS_H_
#define __GENERIC_PICS_DEFS_H_


#define START_INTERNAL_PIC_MEMO  typedef struct IPicMemo {
#define END_INTERNAL_PIC_MEMO    } PicMemo;
#define __EXTENSION


#define ROM_STRING  ROM BYTE *
#pragma pack(1)

#define PROTECTED
#define PRIVATE     static
#define ROM_STRING  ROM BYTE *

#undef  TRUE
#define TRUE                        1u

#undef  FALSE
#define FALSE                       0u

   // Variable Types simulation of PIC
#define auto 
#define overlay
#define volatile

typedef unsigned char            BYTE;
typedef   signed char           SBYTE;
typedef unsigned short           WORD;
typedef   signed short     	    SWORD;
typedef unsigned long           DWORD;
typedef   signed long          SDWORD;
typedef unsigned long long      QWORD;     // avpoid to use at PIC 18 

typedef union
{
    BYTE   Val                       ;
    struct __PACKED
    {
        __EXTENSION BYTE b0       : 1;
        __EXTENSION BYTE b1       : 1;
        __EXTENSION BYTE b2       : 1;
        __EXTENSION BYTE b3       : 1;
        __EXTENSION BYTE b4       : 1;
        __EXTENSION BYTE b5       : 1;
        __EXTENSION BYTE b6       : 1;
        __EXTENSION BYTE b7       : 1;
    } bits;
}   BYTE_VAL, BYTE_BITS              ;

typedef union
{
    WORD Val                         ;
    BYTE v[2]                        ;
    struct
    {
        BYTE LB                      ;
        BYTE HB                      ;
    }   byte                         ;
    struct
    {
        __EXTENSION BYTE b0  :      1;
        __EXTENSION BYTE b1  :      1;
        __EXTENSION BYTE b2  :      1;
        __EXTENSION BYTE b3  :      1;
        __EXTENSION BYTE b4  :      1;
        __EXTENSION BYTE b5  :      1;
        __EXTENSION BYTE b6  :      1;
        __EXTENSION BYTE b7  :      1;
        __EXTENSION BYTE b8  :      1;
        __EXTENSION BYTE b9  :      1;
        __EXTENSION BYTE b10 :      1;
        __EXTENSION BYTE b11 :      1;
        __EXTENSION BYTE b12 :      1;
        __EXTENSION BYTE b13 :      1;
        __EXTENSION BYTE b14 :      1;
        __EXTENSION BYTE b15 :      1;
    }   bits                         ;
}   WORD_VAL, WORD_BITS              ;

typedef union
{
    DWORD Val                        ;
    WORD  w[2]                       ;
    BYTE  v[4]                       ;   
    struct
    {   WORD LW                      ;
        WORD HW                      ;
    } word;
    struct
    {   BYTE LB                      ;
        BYTE HB                      ;
        BYTE UB                      ;
        BYTE MB                      ;
    }   byte                         ;
    struct
    {
        WORD_VAL low                 ;
        WORD_VAL high                ;
    }   wordUnion                    ;
    struct 
    {
        __EXTENSION BYTE b0  :      1;
        __EXTENSION BYTE b1  :      1;
        __EXTENSION BYTE b2  :      1;
        __EXTENSION BYTE b3  :      1;
        __EXTENSION BYTE b4  :      1;
        __EXTENSION BYTE b5  :      1;
        __EXTENSION BYTE b6  :      1;
        __EXTENSION BYTE b7  :      1;
        __EXTENSION BYTE b8  :      1;
        __EXTENSION BYTE b9  :      1;
        __EXTENSION BYTE b10 :      1;
        __EXTENSION BYTE b11 :      1;
        __EXTENSION BYTE b12 :      1;
        __EXTENSION BYTE b13 :      1;
        __EXTENSION BYTE b14 :      1;
        __EXTENSION BYTE b15 :      1;
        __EXTENSION BYTE b16 :      1;
        __EXTENSION BYTE b17 :      1;
        __EXTENSION BYTE b18 :      1;
        __EXTENSION BYTE b19 :      1;
        __EXTENSION BYTE b20 :      1;
        __EXTENSION BYTE b21 :      1;
        __EXTENSION BYTE b22 :      1;
        __EXTENSION BYTE b23 :      1;
        __EXTENSION BYTE b24 :      1;
        __EXTENSION BYTE b25 :      1;
        __EXTENSION BYTE b26 :      1;
        __EXTENSION BYTE b27 :      1;
        __EXTENSION BYTE b28 :      1;
        __EXTENSION BYTE b29 :      1;
        __EXTENSION BYTE b30 :      1;
        __EXTENSION BYTE b31 :      1;
    }  bits                          ;
} DWORD_VAL                          ;

typedef union
{
    QWORD Val                        ;
    DWORD d[2]                       ;
    WORD  w[4]                       ;
    BYTE  v[8]                       ;
    struct
    {   DWORD LD                     ;
        DWORD HD                     ;
    }  dword                         ;
    struct
    {   WORD  LW                     ;
        WORD  HW                     ;
        WORD  UW                     ;
        WORD  MW                     ;
    }   word                         ;
    struct
    {
        __EXTENSION BYTE b0  :      1;
        __EXTENSION BYTE b1  :      1;
        __EXTENSION BYTE b2  :      1;
        __EXTENSION BYTE b3  :      1;
        __EXTENSION BYTE b4  :      1;
        __EXTENSION BYTE b5  :      1;
        __EXTENSION BYTE b6  :      1;
        __EXTENSION BYTE b7  :      1;
        __EXTENSION BYTE b8  :      1;
        __EXTENSION BYTE b9  :      1;
        __EXTENSION BYTE b10 :      1;
        __EXTENSION BYTE b11 :      1;
        __EXTENSION BYTE b12 :      1;
        __EXTENSION BYTE b13 :      1;
        __EXTENSION BYTE b14 :      1;
        __EXTENSION BYTE b15 :      1;
        __EXTENSION BYTE b16 :      1;
        __EXTENSION BYTE b17 :      1;
        __EXTENSION BYTE b18 :      1;
        __EXTENSION BYTE b19 :      1;
        __EXTENSION BYTE b20 :      1;
        __EXTENSION BYTE b21 :      1;
        __EXTENSION BYTE b22 :      1;
        __EXTENSION BYTE b23 :      1;
        __EXTENSION BYTE b24 :      1;
        __EXTENSION BYTE b25 :      1;
        __EXTENSION BYTE b26 :      1;
        __EXTENSION BYTE b27 :      1;
        __EXTENSION BYTE b28 :      1;
        __EXTENSION BYTE b29 :      1;
        __EXTENSION BYTE b30 :      1;
        __EXTENSION BYTE b31 :      1;
        __EXTENSION BYTE b32 :      1;
        __EXTENSION BYTE b33 :      1;
        __EXTENSION BYTE b34 :      1;
        __EXTENSION BYTE b35 :      1;
        __EXTENSION BYTE b36 :      1;
        __EXTENSION BYTE b37 :      1;
        __EXTENSION BYTE b38 :      1;
        __EXTENSION BYTE b39 :      1;
        __EXTENSION BYTE b40 :      1;
        __EXTENSION BYTE b41 :      1;
        __EXTENSION BYTE b42 :      1;
        __EXTENSION BYTE b43 :      1;
        __EXTENSION BYTE b44 :      1;
        __EXTENSION BYTE b45 :      1;
        __EXTENSION BYTE b46 :      1;
        __EXTENSION BYTE b47 :      1;
        __EXTENSION BYTE b48 :      1;
        __EXTENSION BYTE b49 :      1;
        __EXTENSION BYTE b50 :      1;
        __EXTENSION BYTE b51 :      1;
        __EXTENSION BYTE b52 :      1;
        __EXTENSION BYTE b53 :      1;
        __EXTENSION BYTE b54 :      1;
        __EXTENSION BYTE b55 :      1;
        __EXTENSION BYTE b56 :      1;
        __EXTENSION BYTE b57 :      1;
        __EXTENSION BYTE b58 :      1;
        __EXTENSION BYTE b59 :      1;
        __EXTENSION BYTE b60 :      1;
        __EXTENSION BYTE b61 :      1;
        __EXTENSION BYTE b62 :      1;
        __EXTENSION BYTE b63 :      1;
    }   bits                         ;
}   QWORD_VAL                   ;

#pragma pack()
#endif __GENERIC_PICS_DEFS_H_
