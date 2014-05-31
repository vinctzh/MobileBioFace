

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Fri Jan 11 21:24:19 2013
 */
/* Compiler settings for win32\mwcomutil.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IMWUtil,0xC47EA90E,0x56D1,0x11d5,0xB1,0x59,0x00,0xD0,0xB7,0xBA,0x75,0x44);


MIDL_DEFINE_GUID(IID, LIBID_MWComUtil,0x020FA1A8,0xE948,0x443F,0xB9,0xE8,0x00,0xDB,0x0B,0x19,0x44,0xB1);


MIDL_DEFINE_GUID(CLSID, CLSID_MWField,0x64A8E8FD,0x94DA,0x4BC2,0xAF,0x9E,0x5A,0x42,0x45,0x49,0xE4,0xC2);


MIDL_DEFINE_GUID(CLSID, CLSID_MWStruct,0xE680886A,0x5344,0x4407,0x8E,0xCA,0x0D,0x6D,0x8B,0xEC,0xEB,0x7A);


MIDL_DEFINE_GUID(CLSID, CLSID_MWComplex,0xF3B2EC8E,0x6410,0x4511,0x9B,0x93,0x01,0x2D,0xA5,0x89,0xB9,0x7D);


MIDL_DEFINE_GUID(CLSID, CLSID_MWSparse,0xFE2D0E49,0xF0C7,0x48BE,0x8C,0x27,0x71,0xB4,0xFF,0x5D,0x77,0x1F);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArg,0x198AE7FB,0xB411,0x476D,0x99,0xF2,0x98,0x03,0x8B,0x8E,0x31,0x51);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArrayFormatFlags,0x57B420AA,0xB615,0x4230,0xB2,0xA1,0xAC,0x2A,0x37,0x01,0xC8,0xA9);


MIDL_DEFINE_GUID(CLSID, CLSID_MWDataConversionFlags,0xDB1CB632,0xF22D,0x416F,0x80,0x06,0x8C,0x05,0x58,0x23,0x8C,0x09);


MIDL_DEFINE_GUID(CLSID, CLSID_MWUtil,0x75A729B2,0x5A32,0x4E54,0xBF,0xFF,0xB0,0x2C,0x31,0x4A,0x19,0xD8);


MIDL_DEFINE_GUID(CLSID, CLSID_MWFlags,0x1DD332D9,0x4627,0x4322,0x95,0xCC,0x92,0x05,0x04,0x3E,0xE6,0xE1);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



