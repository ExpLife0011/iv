#ifndef IV_PLATFORM_H_
#define IV_PLATFORM_H_

#if (defined(__unix__) || defined(unix)) && !defined(USG)
#include <sys/param.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#ifndef IV_OS_WIN
#define IV_OS_WIN 1
#define NOMINMAX
#endif  // IV_OS_WIN
#elif defined(__APPLE__) || defined(__darwin__)
#ifndef IV_OS_MACOSX
#define IV_OS_MACOSX 1
#endif  // IV_OS_MACOSX
#elif defined(sun) || defined(__sun)
#ifndef IV_OS_SOLARIS
#define IV_OS_SOLARIS 1
#endif  // IV_OS_SOLARIS
#elif defined(__linux__)
#ifndef IV_OS_LINUX
#define IV_OS_LINUX 1
#endif  // IV_OS_LINUX
#elif defined(BSD)
#ifndef IV_OS_BSD
#define IV_OS_BSD 1
#endif  // IV_OS_BSD
#elif defined(__CYGWIN__)
#ifndef IV_OS_CYGWIN
#define IV_OS_CYGWIN 1
#endif  // IV_OS_CYGWIN
#endif

// compiler
#if defined(__GNUC__)
#if defined(__GNU_PATCHLEVEL__)
#define IV_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNU_PATCHLEVEL__)  // NOLINT
#else
#define IV_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#endif
#elif defined(_MSC_VER)
#define IV_COMPILER_MSVC _MSC_VER
#define IV_COMPILER_MSVC_10 (_MSC_VER >= 1600)
#elif defined(__clang__)
#define IV_COMPILER_CLANG __clang__
#elif defined(__ICL) || defined(__ICC)
#if defined(__ICL)
#define IV_COMPILER_ICC __ICL
#else
#define IV_COMPILER_ICC __ICC
#endif
#endif

// snprintf
#if defined(IV_COMPILER_MSVC)
#define snprintf _snprintf_s
#endif

// 64bit system or not
#if defined(__LP64__)
#define IV_64
#elif defined(_WIN64)
#define IV_64
#endif

// CPU
#if defined(__x86_64__) || defined(_WIN64)
#define IV_CPU_X64
#elif defined(__i386__)
#define IV_CPU_IA32
#elif defined(__arm__)
#define IV_CPU_ARM
#else
#define IV_CPU_OTHERS
#endif

// ENABLE_JIT
// http://homepage1.nifty.com/herumi/diary/1109.html#7
#if not +0
#define IV_DISABLE_JIT
// #error "operator names used. use -fno-operator-names"
#else
#if defined(IV_CPU_X64) && (defined(IV_OS_MACOSX) || defined(IV_OS_LINUX) || defined(IV_OS_BSD))
#if defined(IV_COMPILER_CLANG) || (defined(IV_COMPILER_GCC) && IV_COMPILER_GCC >= 40000)
#define IV_ENABLE_JIT
#endif
#endif
#endif

#if !defined(IV_ENABLE_JIT)
#define IV_DISABLE_JIT
#endif

#endif  // IV_PLATFORM_H_
