#ifndef __CROSS_THREAD_HEAD_FILE__
#define __CROSS_THREAD_HEAD_FILE__
#include "stdinc.h"


#ifdef WIN32
typedef DWORD THREAD_ID_t;

#define exitThread_c(x) _endthread()
#define detatchThread_c(x)

typedef HANDLE THREAD_HANDLE;
typedef DWORD THREAD_RETURN;
#else
typedef pthread_t THRAED_ID_t;

#define exitThread_c(x) pthread_exit(x);
#define detatchThread_c(x) pthread_detach(x);

typedef pthread_t THREAD_HANDLE;
typedef void * THREAD_RETURN;
#endif

typedef THREAD_RETURN (*THREAD_PROC)(void *);

#ifdef __cplusplus
extern "C" {
#endif

extern THREAD_ID_t currentThreadId_c();

extern int initThread_c(THREAD_HANDLE *hThread,THREAD_PROC proc,void *ThreadParam);

extern int joinThread_c(THREAD_HANDLE hThread);

#ifdef __cplusplus
};
#endif

#endif