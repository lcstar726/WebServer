#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <memory.h>
#include <malloc.h>
#include <new>

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <dirent.h>
#include <fnmatch.h>
#include <time.h>
#include <sched.h>

#include <sys/times.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
//#include <sys/sem.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/epoll.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <signal.h>
#include <fcntl.h>
#include <setjmp.h>
#include <termio.h>
#include <limits>
#include <errno.h>

#include <poll.h>
//#include <aiocb.h>
//#include <aio.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <list>
#include <deque>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <iterator>

#include "GlobalMacros.h"
#include "DefaultConstants.h"

using namespace std;

typedef int             HANDLE;
typedef unsigned int    uint;
typedef unsigned short  ushort;
typedef unsigned long   ulong;

typedef uint8_t         uint8;
typedef uint16_t        uint16;
typedef uint32_t        uint32;
typedef uint64_t        uint64;

template <typename T>
struct NumericLimits
{
    static T Min(void) { return std::numeric_limits<T>::min(); }
    static T Max(void) { return std::numeric_limits<T>::max(); }
};

#endif
