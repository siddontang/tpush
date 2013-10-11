#include "threadutil.h"

#include <time.h>

namespace tpush
{

    void Condition::wait(int timeout)
    {
        if(timeout <= 0)
        {
            return wait();    
        }

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout;

        MutexGuard mg(m_mutex);
        pthread_cond_timedwait(&m_cond, m_mutex.get(), &ts);
    }
    
}
