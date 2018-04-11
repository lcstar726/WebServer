#ifndef DEFAULT_CONSTANTS_H
#define DEFAULT_CONSTANTS_H

// Included just keep compilers that see #pragma directive first
// happy.
//#include /**/ "ace/config-all.h"

#if !defined (LACKS_PRAGMA_ONCE)
# pragma once
#endif /* LACKS_PRAGMA_ONCE */

// For _POSIX_TIMER_MAX
//#include "ace/os_include/os_limits.h"
#if ! defined (howmany)
#  define howmany(x, y)   (((x)+((y)-1))/(y))
#endif /* howmany */

#if !defined (NAME_MAX)
#  if defined (MAXNAMLEN)
#    define NAME_MAX MAXNAMLEN
#  elif defined (MAXNAMELEN)
#    define NAME_MAX MAXNAMELEN
#  elif defined (FILENAME_MAX)
#    define NAME_MAX FILENAME_MAX
#  elif defined (_MAX_FNAME)
#    define NAME_MAX _MAX_FNAME
#  else /* _MAX_FNAME */
#    define NAME_MAX 256
#  endif /* MAXNAMLEN */
#endif /* !NAME_MAX */

#if !defined (MAXNAMELEN)
#  define MAXNAMELEN NAME_MAX
#endif /* MAXNAMELEN */

#if !defined (HOST_NAME_MAX)
#  define HOST_NAME_MAX 256
#endif /* !HOST_NAME_MAX */

// Note that we are using PATH_MAX instead of _POSIX_PATH_MAX, since
// _POSIX_PATH_MAX is the *minimun* maximum value for PATH_MAX and is
// defined by POSIX as 256.
#if !defined (PATH_MAX)
#  if defined (_MAX_PATH)
#    define PATH_MAX _MAX_PATH
#  elif defined (MAX_PATH)
#    define PATH_MAX MAX_PATH
#  else /* !_MAX_PATH */
#    define PATH_MAX 1024
#  endif /* _MAX_PATH */
#endif /* !PATH_MAX */

// Leaving this for backward compatibility, but PATH_MAX should always be
// used directly.
#if !defined (MAXPATHLEN)
#  define MAXPATHLEN  PATH_MAX
#endif /* !MAXPATHLEN */

// This is defined by XOPEN to be a minimum of 16.  POSIX.1g
// also defines this value.  platform-specific config.h can
// override this if need be.
#if !defined (IOV_MAX)
#  define IOV_MAX 16
#endif /* IOV_MAX */

#if !defined (ACE_IOV_MAX)
#  define ACE_IOV_MAX IOV_MAX
#endif /* ACE_IOV_MAX */

#if defined (ACE_VXWORKS) && (ACE_VXWORKS <= 0x670) && !defined (__RTP__)
#  if defined (PIPE_BUF) && (PIPE_BUF == -1)
#    undef PIPE_BUF
#  endif
#endif /* ACE_VXWORKS */

#if !defined (PIPE_BUF)
#  define PIPE_BUF 5120
#endif /* PIPE_BUF */

#if defined (ACE_HAS_POSIX_REALTIME_SIGNALS)
  // = Giving unique ACE scoped names for some important
  // RTSignal-Related constants. Becuase sometimes, different
  // platforms use different names for these constants.

  // Number of realtime signals provided in the system.
  // _POSIX_RTSIG_MAX is the upper limit on the number of real time
  // signals supported in a posix-4 compliant system.
#  if defined (_POSIX_RTSIG_MAX)
#    define ACE_RTSIG_MAX _POSIX_RTSIG_MAX
#  else /* not _POSIX_RTSIG_MAX */
  // POSIX-4 compilant system has to provide atleast 8 RT signals.
  // @@ Make sure the platform does *not* define this constant with
  // some other name. If yes, use that instead of 8.
#     define ACE_RTSIG_MAX 8
#  endif /* _POSIX_RTSIG_MAX */
#endif /* ACE_HAS_POSIX_REALTIME_SIGNALS */

  // The maximum number of concurrent timers per process.
# if !defined (_POSIX_TIMER_MAX)
#   define _POSIX_TIMER_MAX 44
# endif /* _POSIX_TIMER_MAX */

// Define the default constants for ACE.  Many of these are used for
// the ACE tests and applications.  You can change these values by
// defining the macros in your config.h file.
# if !defined (DEFAULT_CLOSE_ALL_HANDLES)
#   define DEFAULT_CLOSE_ALL_HANDLES true
# endif /* DEFAULT_CLOSE_ALL_HANDLES */

// The maximum length for a fully qualified Internet name.
# if !defined(MAX_FULLY_QUALIFIED_NAME_LEN)
#   define MAX_FULLY_QUALIFIED_NAME_LEN 256
# endif /* MAX_FULLY_QUALIFIED_NAME_LEN */

#if !defined (DEFAULT_PAGEFILE_POOL_BASE)
#define DEFAULT_PAGEFILE_POOL_BASE (void *) 0
#endif /* DEFAULT_PAGEFILE_POOL_BASE */

#if !defined (DEFAULT_PAGEFILE_POOL_SIZE)
#define DEFAULT_PAGEFILE_POOL_SIZE (size_t) 0x01000000
#endif /* DEFAULT_PAGEFILE_POOL_SIZE */

#if !defined (DEFAULT_PAGEFILE_POOL_CHUNK)
#define DEFAULT_PAGEFILE_POOL_CHUNK (size_t) 0x00010000
#endif /* DEFAULT_PAGEFILE_POOL_CHUNK */

#if !defined (DEFAULT_PAGEFILE_POOL_NAME)
#define DEFAULT_PAGEFILE_POOL_NAME TEXT ("Default_Pagefile_Memory_Pool")
#endif /* DEFAULT_PAGEFILE_POOL_NAME */

#if !defined (DEFAULT_MESSAGE_BLOCK_PRIORITY)
#define DEFAULT_MESSAGE_BLOCK_PRIORITY 0
#endif /* DEFAULT_MESSAGE_BLOCK_PRIORITY */

#if !defined (DEFAULT_SERVICE_REPOSITORY_SIZE)
#define DEFAULT_SERVICE_REPOSITORY_SIZE 1024
#endif /* DEFAULT_SERVICE_REPOSITORY_SIZE */

#if !defined (DEFAULT_SERVICE_GESTALT_SIZE)
#define DEFAULT_SERVICE_GESTALT_SIZE 1024
#endif /* DEFAULT_SERVICE_GESTALT_SIZE */

#if !defined (REACTOR_NOTIFICATION_ARRAY_SIZE)
#define REACTOR_NOTIFICATION_ARRAY_SIZE 1024
#endif /* REACTOR_NOTIFICATION_ARRAY_SIZE */

# if !defined (DEFAULT_TIMEOUT)
#   define DEFAULT_TIMEOUT 5
# endif /* DEFAULT_TIMEOUT */

# if !defined (DEFAULT_BACKLOG)
#   define DEFAULT_BACKLOG 5
# endif /* DEFAULT_BACKLOG */

# if !defined (DEFAULT_ASYNCH_BACKLOG)
#   define DEFAULT_ASYNCH_BACKLOG 5
# endif /* DEFAULT_ASYNCH_BACKLOG */

# if !defined (DEFAULT_THREADS)
#   define DEFAULT_THREADS 1
# endif /* DEFAULT_THREADS */

// The following 3 defines are used in the IP multicast and broadcast tests.
# if !defined (DEFAULT_BROADCAST_PORT)
#   define DEFAULT_BROADCAST_PORT 20000
# endif /* DEFAULT_BROADCAST_PORT */

# if !defined (DEFAULT_MULTICAST_PORT)
#   define DEFAULT_MULTICAST_PORT 20001
# endif /* DEFAULT_MULTICAST_PORT */

# if !defined (DEFAULT_MULTICAST_ADDR)
// This address MUST be within the range for host group addresses:
// 224.0.0.0 to 239.255.255.255.
#   define DEFAULT_MULTICAST_ADDR "224.9.9.2"
# endif /* DEFAULT_MULTICAST_ADDR */

# if defined (HAS_IPV6)
# if !defined (DEFAULT_MULTICASTV6_ADDR)
// This address should be within the range for site-local addresses:
// ff05::0/16 .
#   define DEFAULT_MULTICASTV6_ADDR "ff05:0::ff01:1"
# endif /* DEFAULT_MULTICASTV6_ADDR */
# endif

// Default port number for HTTP.
# if !defined (DEFAULT_HTTP_SERVER_PORT)
#   define DEFAULT_HTTP_SERVER_PORT 80
# endif /* DEFAULT_HTTP_SERVER_PORT */

// Used in many IPC_SAP tests
# if !defined (DEFAULT_SERVER_PORT)
#   define DEFAULT_SERVER_PORT 20002
# endif /* DEFAULT_SERVER_PORT */

# if !defined (DEFAULT_HTTP_PORT)
#   define DEFAULT_HTTP_PORT 80
# endif /* DEFAULT_HTTP_PORT */

# if !defined (DEFAULT_MAX_SOCKET_BUFSIZ)
#   define DEFAULT_MAX_SOCKET_BUFSIZ 65536
# endif /* DEFAULT_MAX_SOCKET_BUFSIZ */

# if !defined (DEFAULT_SERVER_PORT_STR)
#   define DEFAULT_SERVER_PORT_STR TEXT("20002")
# endif /* DEFAULT_SERVER_PORT_STR */

// Used for the Service_Directory test
# if !defined (DEFAULT_SERVICE_PORT)
#   define DEFAULT_SERVICE_PORT 20003
# endif /* DEFAULT_SERVICE_PORT */

// Used for the Thread_Spawn test
# if !defined (DEFAULT_THR_PORT    )
#   define DEFAULT_THR_PORT 20004
# endif /* DEFAULT_THR_PORT */

// Used for <SOCK_Connect::connect> tests
# if !defined (DEFAULT_LOCAL_PORT)
#   define DEFAULT_LOCAL_PORT 20005
# endif /* DEFAULT_LOCAL_PORT */

// Used for Connector tests
# if !defined (DEFAULT_LOCAL_PORT_STR)
#   define DEFAULT_LOCAL_PORT_STR "20005"
# endif /* DEFAULT_LOCAL_PORT_STR */

// Used for the name server.
# if !defined (DEFAULT_NAME_SERVER_PORT)
#   define DEFAULT_NAME_SERVER_PORT 20006
# endif /* DEFAULT_NAME_SERVER_PORT */

# if !defined (DEFAULT_NAME_SERVER_PORT_STR)
#   define DEFAULT_NAME_SERVER_PORT_STR "20006"
# endif /* DEFAULT_NAME_SERVER_PORT_STR */

// Used for the token server.
# if !defined (DEFAULT_TOKEN_SERVER_PORT)
#   define DEFAULT_TOKEN_SERVER_PORT 20007
# endif /* DEFAULT_TOKEN_SERVER_PORT */

# if !defined (DEFAULT_TOKEN_SERVER_PORT_STR)
#   define DEFAULT_TOKEN_SERVER_PORT_STR "20007"
# endif /* DEFAULT_TOKEN_SERVER_PORT_STR */

// Used for the logging server.
# if !defined (DEFAULT_LOGGING_SERVER_PORT)
#   define DEFAULT_LOGGING_SERVER_PORT 20008
# endif /* DEFAULT_LOGGING_SERVER_PORT */

# if !defined (DEFAULT_LOGGING_SERVER_PORT_STR)
#   define DEFAULT_LOGGING_SERVER_PORT_STR "20008"
# endif /* DEFAULT_LOGGING_SERVER_PORT_STR */

// Used for the logging server.
# if !defined (DEFAULT_THR_LOGGING_SERVER_PORT)
#   define DEFAULT_THR_LOGGING_SERVER_PORT 20008
# endif /* DEFAULT_THR_LOGGING_SERVER_PORT */

# if !defined (DEFAULT_THR_LOGGING_SERVER_PORT_STR)
#   define DEFAULT_THR_LOGGING_SERVER_PORT_STR "20008"
# endif /* DEFAULT_THR_LOGGING_SERVER_PORT_STR */

// Used for the time server.
# if !defined (DEFAULT_TIME_SERVER_PORT)
#   define DEFAULT_TIME_SERVER_PORT 20009
# endif /* DEFAULT_TIME_SERVER_PORT */

# if !defined (DEFAULT_TIME_SERVER_PORT_STR)
#   define DEFAULT_TIME_SERVER_PORT_STR "20009"
# endif /* DEFAULT_TIME_SERVER_PORT_STR */

# if !defined (DEFAULT_TIME_SERVER_STR)
#   define DEFAULT_TIME_SERVER_STR "TS_TIME"
# endif /* DEFAULT_TIME_SERVER_STR */

// Used by the FIFO tests
# if !defined (DEFAULT_RENDEZVOUS)
#   if defined (HAS_STREAM_PIPES)
#     define DEFAULT_RENDEZVOUS TEXT("/tmp/fifo.ace")
#   else
#     define DEFAULT_RENDEZVOUS TEXT("localhost:20010")
#   endif /* HAS_STREAM_PIPES */
# endif /* DEFAULT_RENDEZVOUS */

// Used for the UNIX syslog logging interface to Log_Msg.
# ifndef DEFAULT_SYSLOG_FACILITY
# define DEFAULT_SYSLOG_FACILITY LOG_USER
# endif /* DEFAULT_SYSLOG_FACILITY */

# if !defined (HAS_STREAM_LOG_MSG_IPC)
#   if defined (HAS_STREAM_PIPES)
#     define HAS_STREAM_LOG_MSG_IPC 1
#   else
#     define HAS_STREAM_LOG_MSG_IPC 0
#   endif /* HAS_STREAM_PIPES */
# endif /* !HAS_STREAM_LOG_MSG_IPC */

# if !defined (DEFAULT_LOGGER_KEY)
#   if (HAS_STREAM_LOG_MSG_IPC == 1)
#     define DEFAULT_LOGGER_KEY TEXT ("/tmp/server_daemon")
#   else
#     define DEFAULT_LOGGER_KEY TEXT ("localhost:20012")
#   endif /* HAS_STREAM_LOG_MSG_IPC==1 */
# endif /* DEFAULT_LOGGER_KEY */

// The way to specify the local host for loopback IP. This is usually
// "localhost" but it may need changing on some platforms.
# if !defined (LOCALHOST)
#   define LOCALHOST TEXT ("localhost")
# endif

// This specification for an IPv6 localhost should work on all platforms
// supporting IPv6
# if defined (HAS_IPV6)
#   if !defined (IPV6_LOCALHOST)
#     define IPV6_LOCALHOST TEXT ("::1")
#   endif /* IPV6_LOCALHOST*/
#endif /* HAS_IPV6 */

// This specification for an IPv6 ANY address should work on all platforms
// supporting IPv6
# if defined (HAS_IPV6)
#   if !defined (IPV6_ANY)
#     define IPV6_ANY TEXT ("::")
#   endif /* IPV6_ANY*/
#endif /* HAS_IPV6 */

# if !defined (DEFAULT_SERVER_HOST)
#   if defined (HAS_IPV6)
#     define DEFAULT_SERVER_HOST IPV6_LOCALHOST
#   else /*HAS_IPV6*/
#     define DEFAULT_SERVER_HOST LOCALHOST
#   endif /*HAS_IPV6*/
# endif /* DEFAULT_SERVER_HOST */

// Default shared memory key
# if !defined (DEFAULT_SHM_KEY)
#   define DEFAULT_SHM_KEY 1234
# endif /* DEFAULT_SHM_KEY */

// Default address for shared memory mapped files and SYSV shared memory
// (defaults to 64 M).
# if !defined (DEFAULT_BASE_ADDR)
#   define DEFAULT_BASE_ADDR ((char *) (64 * 1024 * 1024))
# endif /* DEFAULT_BASE_ADDR */

// Default segment size used by SYSV shared memory (128 K)
# if !defined (DEFAULT_SEGMENT_SIZE)
#   define DEFAULT_SEGMENT_SIZE 1024 * 128
# endif /* DEFAULT_SEGMENT_SIZE */

// Maximum number of SYSV shared memory segments
// (does anyone know how to figure out the right values?!)
# if !defined (DEFAULT_MAX_SEGMENTS)
#   define DEFAULT_MAX_SEGMENTS 6
# endif /* DEFAULT_MAX_SEGMENTS */

// Name of the map that's stored in shared memory.
# if !defined (NAME_SERVER_MAP)
#   define NAME_SERVER_MAP "Name Server Map"
# endif /* NAME_SERVER_MAP */

// Default file permissions.
# if !defined (DEFAULT_FILE_PERMS)
#   if defined (VXWORKS)
#     define DEFAULT_FILE_PERMS (S_IRUSR | S_IWUSR| S_IRGRP| S_IROTH)
#   else
#     define DEFAULT_FILE_PERMS 0644
#   endif /* VXWORKS */
# endif /* DEFAULT_FILE_PERMS */

// Default directory permissions.
# if !defined (DEFAULT_DIR_PERMS)
#   define DEFAULT_DIR_PERMS 0755
# endif /* DEFAULT_DIR_PERMS */

# if !defined (DEFAULT_TIMEPROBE_TABLE_SIZE)
#   define DEFAULT_TIMEPROBE_TABLE_SIZE 8 * 1024
# endif /* DEFAULT_TIMEPROBE_TABLE_SIZE */

// Default size of the ACE Map_Manager.
# if !defined (DEFAULT_MAP_SIZE)
#   define DEFAULT_MAP_SIZE 1024
# endif /* DEFAULT_MAP_SIZE */

# if defined (DEFAULT_MAP_SIZE) && (DEFAULT_MAP_SIZE == 0)
#  error DEFAULT_MAP_SIZE should not be zero
# endif /* DEFAULT_MAP_SIZE */

// Defaults for ACE Timer Wheel
# if !defined (DEFAULT_TIMER_WHEEL_SIZE)
#   define DEFAULT_TIMER_WHEEL_SIZE 1024
# endif /* DEFAULT_TIMER_WHEEL_SIZE */

# if !defined (DEFAULT_TIMER_WHEEL_RESOLUTION)
#   define DEFAULT_TIMER_WHEEL_RESOLUTION 100
# endif /* DEFAULT_TIMER_WHEEL_RESOLUTION */

// Default size for ACE Timer Hash table
# if !defined (DEFAULT_TIMER_HASH_TABLE_SIZE)
#   define DEFAULT_TIMER_HASH_TABLE_SIZE 1024
# endif /* DEFAULT_TIMER_HASH_TABLE_SIZE */

// Defaults for the ACE Free List
# if !defined (DEFAULT_FREE_LIST_PREALLOC)
#   define DEFAULT_FREE_LIST_PREALLOC 0
# endif /* DEFAULT_FREE_LIST_PREALLOC */

# if !defined (DEFAULT_FREE_LIST_LWM)
#   define DEFAULT_FREE_LIST_LWM 0
# endif /* DEFAULT_FREE_LIST_LWM */

# if !defined (DEFAULT_FREE_LIST_HWM)
#   define DEFAULT_FREE_LIST_HWM 25000
# endif /* DEFAULT_FREE_LIST_HWM */

# if !defined (DEFAULT_FREE_LIST_INC)
#   define DEFAULT_FREE_LIST_INC 100
# endif /* DEFAULT_FREE_LIST_INC */

# if !defined (UNIQUE_NAME_LEN)
#   define UNIQUE_NAME_LEN 100
# endif /* UNIQUE_NAME_LEN */

# if !defined (MAX_DGRAM_SIZE)
   // This is just a guess.  8k is the normal limit on
   // most machines because that's what NFS expects.
#   define MAX_DGRAM_SIZE 8192
# endif /* MAX_DGRAM_SIZE */

# if !defined (DEFAULT_ARGV_BUFSIZ)
#   define DEFAULT_ARGV_BUFSIZ 1024 * 4
# endif /* DEFAULT_ARGV_BUFSIZ */

// A free list which create more elements when there aren't enough
// elements.
# define FREE_LIST_WITH_POOL 1

// A simple free list which doen't allocate/deallocate elements.
# define PURE_FREE_LIST 2

# if defined (WIN32)

// This is necessary to work around bugs with Win32 non-blocking
// connects...
#   if !defined (NON_BLOCKING_BUG_DELAY)
#     define NON_BLOCKING_BUG_DELAY 35000
#   endif /* NON_BLOCKING_BUG_DELAY */
# endif /*WIN32*/

// Max size of an ACE Log Record data buffer.  This can be reset in
// the config.h file if you'd like to increase or decrease the size.
# if !defined (MAXLOGMSGLEN)
#   define MAXLOGMSGLEN 4 * 1024
# endif /* MAXLOGMSGLEN */

// Max size of an ACE Token.
# define MAXTOKENNAMELEN 40

// Max size of an ACE Token client ID.
# define MAXCLIENTIDLEN MAXHOSTNAMELEN + 20

/// Max udp packet size
#if !defined (MAX_UDP_PACKET_SIZE)
#define MAX_UDP_PACKET_SIZE 65507
#endif

/**
 * @name Default values to control CDR classes memory allocation strategies
 */
//@{

/// Control the initial size of all CDR buffers, application
/// developers may want to optimize this value to fit their request
/// size
#if !defined (DEFAULT_CDR_BUFSIZE)
#  define DEFAULT_CDR_BUFSIZE 512
#endif /* DEFAULT_CDR_BUFSIZE */

#if (DEFAULT_CDR_BUFSIZE == 0)
# error: DEFAULT_CDR_BUFSIZE should be bigger then 0
#endif

/// Stop exponential growth of CDR buffers to avoid overallocation
#if !defined (DEFAULT_CDR_EXP_GROWTH_MAX)
#  define DEFAULT_CDR_EXP_GROWTH_MAX 65536
#endif /* DEFAULT_CDR_EXP_GROWTH_MAX */

/// Control CDR buffer growth after maximum exponential growth is
/// reached
#if !defined (DEFAULT_CDR_LINEAR_GROWTH_CHUNK)
#  define DEFAULT_CDR_LINEAR_GROWTH_CHUNK 65536
#endif /* DEFAULT_CDR_LINEAR_GROWTH_CHUNK */
//@}

/// Control the zero-copy optimizations for octet sequences
/**
 * Large octet sequences can be sent without any copies by chaining
 * them in the list of message blocks that represent a single CDR
 * stream.  However, if the octet sequence is too small the zero copy
 * optimizations actually hurt performance.  Octet sequences smaller
 * than this value will be copied.
 */
#if !defined (DEFAULT_CDR_MEMCPY_TRADEOFF)
#define DEFAULT_CDR_MEMCPY_TRADEOFF 256
#endif /* DEFAULT_CDR_MEMCPY_TRADEOFF */

#if defined (WIN32)
   // Define the pathname separator characters for Win32 (ugh).
#  define DIRECTORY_SEPARATOR_STR_A "\\"
#  define DIRECTORY_SEPARATOR_CHAR_A '\\'
#else
   // Define the pathname separator characters for UNIX.
#  define DIRECTORY_SEPARATOR_STR_A "/"
#  define DIRECTORY_SEPARATOR_CHAR_A '/'
#endif /* WIN32 */

// Define the Wide character and normal versions of some of the string macros
#if defined (HAS_WCHAR)
#  define DIRECTORY_SEPARATOR_STR_W TEXT_WIDE(DIRECTORY_SEPARATOR_STR_A)
#  define DIRECTORY_SEPARATOR_CHAR_W TEXT_WIDE(DIRECTORY_SEPARATOR_CHAR_A)
#endif /* HAS_WCHAR */

#define DIRECTORY_SEPARATOR_STR TEXT (DIRECTORY_SEPARATOR_STR_A)
#define DIRECTORY_SEPARATOR_CHAR TEXT (DIRECTORY_SEPARATOR_CHAR_A)

#if !defined (DEFAULT_THREAD_PRIORITY)
#  define DEFAULT_THREAD_PRIORITY (-0x7fffffffL - 1L)
#endif /* DEFAULT_THREAD_PRIORITY */

#if !defined (DEFAULT_THREAD_STACKSIZE)
#  define DEFAULT_THREAD_STACKSIZE 0
#endif /* DEFAULT_THREAD_STACKSIZE */

#if !defined (MAX_DEFAULT_PORT)
#  define MAX_DEFAULT_PORT 65535
#endif /* MAX_DEFAULT_PORT */

// Default number of Event_Handlers supported by
// Timer_Heap.
#if !defined (DEFAULT_TIMERS) && defined (_POSIX_TIMER_MAX)
#  define DEFAULT_TIMERS _POSIX_TIMER_MAX
#endif /* DEFAULT_TIMERS */

#if !defined (DEFAULT_TIMERS) || (defined (DEFAULT_TIMERS) && (DEFAULT_TIMERS == 0))
#error DEFAULT_TIMERS should be defined and not be zero
#endif /* DEFAULT_TIMERS */

#if defined (WIN32)
#  define PLATFORM_A "Win32"
#  define PLATFORM_EXE_SUFFIX_A ".exe"
#elif defined (VXWORKS)
#  define PLATFORM_A "VxWorks"
#  if defined (__RTP__)
#    define PLATFORM_EXE_SUFFIX_A ".vxe"
#  else
#    define PLATFORM_EXE_SUFFIX_A ".out"
#  endif
#else /* !WIN32 && !VXWORKS */
#  define PLATFORM_A "UNIX"
#  define PLATFORM_EXE_SUFFIX_A ""
#endif /* WIN32 */

// Define the Wide character and normal versions of some of the string macros
#if defined (HAS_WCHAR)
#  define PLATFORM_W TEXT_WIDE(PLATFORM_A)
#  define PLATFORM_EXE_SUFFIX_W TEXT_WIDE(PLATFORM_EXE_SUFFIX_A)
#endif /* HAS_WCHAR */

#define PLATFORM TEXT (PLATFORM_A)
#define PLATFORM_EXE_SUFFIX TEXT (PLATFORM_EXE_SUFFIX_A)

#if defined (WIN32)
#  define LD_SEARCH_PATH TEXT ("PATH")
#  define LD_SEARCH_PATH_SEPARATOR_STR TEXT (";")
#  define DLL_SUFFIX TEXT (".dll")
#  if !defined (DLL_PREFIX)
#    define DLL_PREFIX TEXT ("")
#  endif /* !DLL_PREFIX */
#else /* !WIN32 */
#  if !defined (LD_SEARCH_PATH)
#    define LD_SEARCH_PATH TEXT ("LD_LIBRARY_PATH")
#  endif /* LD_SEARCH_PATH */
#  if !defined (LD_SEARCH_PATH_SEPARATOR_STR)
#    define LD_SEARCH_PATH_SEPARATOR_STR TEXT (":")
#  endif /* LD_SEARCH_PATH_SEPARATOR_STR */
#endif /* WIN32 */

#if !defined (DLL_SUFFIX)
#  define DLL_SUFFIX TEXT (".so")
#endif /* DLL_SUFFIX */

#if !defined (DLL_PREFIX)
#  define DLL_PREFIX TEXT ("lib")
#endif /* DLL_PREFIX */

#if defined (WIN32)
// Used for dynamic linking
#   if !defined (DEFAULT_SVC_CONF)
#     if (USES_CLASSIC_SVC_CONF == 1)
#       define DEFAULT_SVC_CONF TEXT (".\\svc.conf")
#     else
#       define DEFAULT_SVC_CONF TEXT (".\\svc.conf.xml")
#     endif /* USES_CLASSIC_SVC_CONF ==1 */
#   endif /* DEFAULT_SVC_CONF */
#endif /* WIN32 */

 // Used for dynamic linking.
#if !defined (DEFAULT_SVC_CONF)
#  if (USES_CLASSIC_SVC_CONF == 1)
#    define DEFAULT_SVC_CONF TEXT ("./svc.conf")
#  else
#    define DEFAULT_SVC_CONF TEXT ("./svc.conf.xml")
#  endif /* USES_CLASSIC_SVC_CONF ==1 */
#endif /* DEFAULT_SVC_CONF */

#if !defined (LOGGER_KEY)
#  define LOGGER_KEY TEXT ("/tmp/server_daemon")
#endif /* LOGGER_KEY */

// Theses defines are used by the ACE Name Server.
#if !defined (DEFAULT_LOCALNAME_A)
#  define DEFAULT_LOCALNAME_A "localnames"
#endif /* DEFAULT_LOCALNAME_A */
#if !defined (DEFAULT_GLOBALNAME_A)
#  define DEFAULT_GLOBALNAME_A "globalnames"
#endif /* DEFAULT_GLOBALNAME_A */

#if defined (HAS_WCHAR)
#  define DEFAULT_LOCALNAME_W TEXT_WIDE(DEFAULT_LOCALNAME_A)
#  define DEFAULT_GLOBALNAME_W TEXT_WIDE(DEFAULT_GLOBALNAME_A)
#endif /* HAS_WCHAR */

#define DEFAULT_LOCALNAME TEXT (DEFAULT_LOCALNAME_A)
#define DEFAULT_GLOBALNAME TEXT (DEFAULT_GLOBALNAME_A)

#if !defined (DEFAULT_OPEN_PERMS)
#  define DEFAULT_OPEN_PERMS DEFAULT_FILE_PERMS
#endif  /* DEFAULT_OPEN_PERMS */

#if !defined (DEFAULT_RW_PROCESS_MUTEX_PERMS)
# if defined (WIN32)
#  define DEFAULT_RW_PROCESS_MUTEX_PERMS DEFAULT_OPEN_PERMS
# else
#  define DEFAULT_RW_PROCESS_MUTEX_PERMS (S_IRUSR | S_IWUSR)
# endif /* WIN32 */
#endif /* DEFAULT_RW_PROCESS_MUTEX_PERMS */

# if defined (WIN32)
    // The "null" device on Win32.
#   define DEV_NULL "nul"
#   define SYSCALL_FAILED 0xFFFFFFFF
# else /* !WIN32 */
    // The "null" device on UNIX.
#   define DEV_NULL "/dev/null"
#   define SYSCALL_FAILED -1
# endif /* WIN32 */

//#include /**/ "ace/post.h"
#endif /*DEFAULT_CONSTANTS_H*/
