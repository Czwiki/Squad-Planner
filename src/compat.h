/**
 * @file compat.h
 * @brief Platform compatibility header for cross-platform builds.
 *
 * Provides compatibility macros and includes to ensure the project
 * compiles cleanly on both POSIX systems (Linux, macOS) and Windows (MSVC).
 *
 * Known Windows build issues addressed:
 * 1. <unistd.h> does not exist on MSVC – conditionally included.
 * 2. strdup() is POSIX, not C99 – MSVC provides _strdup() instead.
 * 3. Makefile uses 'rm' – Windows uses 'del'; see Makefile comments.
 */

#ifndef COMPAT_H
#define COMPAT_H

/*
 * MSVC does not provide <unistd.h>. Only include it on POSIX systems.
 * On Windows, the functions we use (sleep, isatty, etc.) have
 * alternatives in <io.h> and <process.h> if needed in the future.
 */
#if !defined(_MSC_VER)
#include <unistd.h>
#endif

/*
 * strdup() is defined by POSIX (IEEE Std 1003.1) but is not part of
 * the C99 or C11 standards. MSVC provides _strdup() as a replacement.
 * This macro ensures strdup() works transparently on all platforms.
 */
#if defined(_MSC_VER)
#define strdup _strdup
#endif

#endif /* COMPAT_H */
