#ifndef BOX_MAN_H
#define BOX_MAN_H
/* ======================================================================== */
#include <windows.h>
/* ======================================================================== */
#ifdef __cplusplus
extern "C" {
#endif
/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */
#ifndef UI16
#define SI08 char
#define SI16 short
#define SI32 int
#define SI64 long long
#define UI08 unsigned char
#define UI16 unsigned short
#define UI32 unsigned int
#define UI64 unsigned long long
#define FP32 float
#define FP64 double
#define VARG ...
typedef UI32 * PU32;
typedef SI32 * PS32;
typedef FP32 * PF32;
typedef FP64 * PF64;
typedef void * VPTR;
#if (defined(__x86_64__) || defined(_WIN64))
#define UImm UI64
#define SImm SI64
#else
#define UImm UI32
#define SImm SI32
#endif
#endif

/* ======================================================================== */
/* ======================================================================== */
#ifndef BM_CAL
#define BM_CAL
#define BM_EXTERN_C extern
/* ======================================================================== */
#ifdef __unix__
/* ======================================================================== */
	#define BM_CALL_DEF
	#define BM_PORT_DEF
/* ======================================================================== */
#else
/* #define WIN32_LEAN_AND_MEAN */
//#include <windows.h>
#define BM_CALL_DEF __stdcall

#if   defined(BM_DLL_EXPORT)
	#define BM_PORT_DEF BM_EXTERN_C __declspec(dllexport)
#elif defined(BM_DLL_IMPORT)
	#define BM_PORT_DEF BM_EXTERN_C __declspec(dllimport)
#else
	#define BM_PORT_DEF
#endif

#endif


#define CAL_CPTR  BM_PORT_DEF  char * BM_CALL_DEF
#define CAL_VPTR  BM_PORT_DEF  void * BM_CALL_DEF
#define CAL_VOID  BM_PORT_DEF  void   BM_CALL_DEF
#define CAL_UI08  BM_PORT_DEF  UI08   BM_CALL_DEF
#define CAL_SI32  BM_PORT_DEF  SI32   BM_CALL_DEF
#define CAL_SI64  BM_PORT_DEF  SI32   BM_CALL_DEF
#define CAL_UI32  BM_PORT_DEF  UI32   BM_CALL_DEF
#define CAL_UI64  BM_PORT_DEF  UI64   BM_CALL_DEF
#define CAL_FP32  BM_PORT_DEF  FP32   BM_CALL_DEF
#define CAL_FP64  BM_PORT_DEF  FP64   BM_CALL_DEF
#endif
/* ======================================================================== */
#define BM_Unused(x) ((void) x)
/* ======================================================================== */
int BM_main(int argc, char** argv);
/* ======================================================================== */
#ifdef __cplusplus
}
#endif
/* ======================================================================== */

#endif // header guard

