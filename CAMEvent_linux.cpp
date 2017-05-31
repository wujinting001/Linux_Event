
#include "CAMEvent_linux.h" 


// --- CAMEvent -----------------------
CAMEvent::CAMEvent(BOOL fManualReset, BOOL fInitState)
{
    CAMEvent::m_hEvent.state = fInitState;
    CAMEvent::m_hEvent.manual_reset = fManualReset;
    //pthread_mutex_init(&m_hEvent.mutex, NULL);    
    //pthread_cond_init(&m_hEvent.cond, NULL);
}

CAMEvent::~CAMEvent()
{
    //pthread_cond_destroy(&m_hEvent.cond);
    //pthread_mutex_destroy(&m_hEvent.mutex);
}

void CAMEvent::Set()
{
    //pthread_mutex_lock(&m_hEvent.mutex);
    {
        std::unique_lock<std::mutex> lock(m_hEvent.mtx);        
        m_hEvent.state = true;
    }
    
    //pthread_mutex_unlock(&m_hEvent.mutex);
    
    if (m_hEvent.manual_reset)
    {
        //pthread_cond_broadcast(&m_hEvent.cond);
        m_hEvent.cond.notify_all();
    }
    else
    { 
        //pthread_cond_signal(&m_hEvent.cond);
        m_hEvent.cond.notify_one();
    }
}

BOOL CAMEvent::Wait(unsigned int dwTimeout) //milliseconds
{
    //pthread_mutex_lock(&m_hEvent.mutex);
    std::unique_lock<std::mutex> lock(m_hEvent.mtx);

    if(dwTimeout == INFINITE)
    {
        //while (!m_hEvent.state)    
        //{      
            //if (pthread_cond_wait(&m_hEvent.cond, &m_hEvent.mutex))
            //{   
                //pthread_mutex_unlock(&m_hEvent.mutex); 
                //return -1;
            //}   
        //}
        while(!m_hEvent.state)
        {
            m_hEvent.cond.wait(lock);
        }
    }
    else
    {
        //struct timespec ts;
        //if (clock_gettime(CLOCK_REALTIME, &ts ) == 0 )
        //{
            //ts.tv_sec  += dwTimeout/1000;
            //ts.tv_nsec += (dwTimeout%1000)*1000000;
            
            //ts.tv_sec += ts.tv_nsec/1000000000; //Nanoseconds [0 .. 999999999]
            //ts.tv_nsec = ts.tv_nsec%1000000000;
            //while (!m_hEvent.state) 
            //{
                //if(pthread_cond_timedwait(&m_hEvent.cond, &m_hEvent.mutex, &ts))
                //{
                    //pthread_mutex_unlock(&m_hEvent.mutex); 
                    //return -1;   
                //}
            //}

            while(!m_hEvent.state)
            {                
                std::cv_status status = std::cv_status::no_timeout;
                status = m_hEvent.cond.wait_for(lock, std::chrono::milliseconds(dwTimeout));
                if(status == std::cv_status::timeout)
                {
                    return 1;
                }
            }
        //}        
    }
    
    if (!m_hEvent.manual_reset) 
    {
        m_hEvent.state = false;
    }
    
    //pthread_mutex_unlock(&m_hEvent.mutex); 
    return 0;
}

void CAMEvent::Reset()
{
    //pthread_mutex_lock(&m_hEvent.mutex);
    std::unique_lock<std::mutex> lock(m_hEvent.mtx);
    m_hEvent.state = FALSE;
    //pthread_mutex_unlock(&m_hEvent.mutex);
}

BOOL CAMEvent::Check()
{
    return CAMEvent::Wait(0);
}

