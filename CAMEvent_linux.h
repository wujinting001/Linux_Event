#ifndef _CAMEVENT_LINUX_H_
#define _CAMEVENT_LINUX_H_

#include <stdio.h>
#include <ctime>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
//#include <pthread.h>
#include "types.h"

#define INFINITE -1

typedef struct  
{
    bool state;
    bool manual_reset;
    //pthread_mutex_t mutex;
    //pthread_cond_t cond;
	std::mutex mtx;
	std::condition_variable cond;
}event_t;

class CAMEvent
{
    // make copy constructor and assignment operator inaccessible

    CAMEvent(const CAMEvent &refEvent);
    CAMEvent &operator=(const CAMEvent &refEvent);

protected:
    event_t m_hEvent;
public:
    CAMEvent(BOOL fManualReset = FALSE, BOOL fInitState = FALSE);
    ~CAMEvent();

    // Cast to HANDLE - we don't support this as an lvalue
    //operator HANDLE () const { return &m_hEvent; };

    void Set();
    BOOL Wait(unsigned int dwTimeout = INFINITE);
    void Reset();
    BOOL Check();
};

// wrapper for whatever critical section we have
class CCritSec {

    // make copy constructor and assignment operator inaccessible

    CCritSec(const CCritSec &refCritSec);
    CCritSec &operator=(const CCritSec &refCritSec);

	//CRITICAL_SECTION m_CritSec;
    //pthread_mutex_t m_CritSec;
    std::mutex m_CritSec;

#ifdef DEBUG
public:
    DWORD   m_currentOwner;
    DWORD   m_lockCount;
    BOOL    m_fTrace;        // Trace this one
public:
    CCritSec();
    ~CCritSec();
    void Lock();
    void Unlock();
#else

public:
    CCritSec() {
		//pthread_mutexattr_t mattr;		
		//pthread_mutexattr_init(&mattr);
		//pthread_mutex_init(&m_CritSec, &mattr);
        //InitializeCriticalSection(&m_CritSec);
    };

    ~CCritSec() {
        //DeleteCriticalSection(&m_CritSec);
		//pthread_mutex_destroy(&m_CritSec);
    };

    void Lock() {
        //EnterCriticalSection(&m_CritSec);
		//pthread_mutex_lock(&m_CritSec);
		m_CritSec.lock();

    };

    void Unlock() {
        //LeaveCriticalSection(&m_CritSec);
		//pthread_mutex_unlock(&m_CritSec);
		m_CritSec.unlock();
    };
#endif
};

// locks a critical section, and unlocks it automatically
// when the lock goes out of scope
class CAutoLock {

    // make copy constructor and assignment operator inaccessible

    CAutoLock(const CAutoLock &refAutoLock);
    CAutoLock &operator=(const CAutoLock &refAutoLock);

protected:
    CCritSec * m_pLock;

public:
    CAutoLock(CCritSec * plock)
    {
        m_pLock = plock;
        m_pLock->Lock();
    };

    ~CAutoLock() {
        m_pLock->Unlock();
    };
};

#endif
