#include "util.h"

namespace eleven{
    pid_t getThreadId(){
        return syscall(SYS_gettid);
    }
}