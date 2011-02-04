#ifndef _CUPS_CONFIG_H_
#define _CUPS_CONFIG_H_

/*
 * Do we have the long long type?
 */

#define HAVE_LONG_LONG 1

#ifdef HAVE_LONG_LONG
#  define CUPS_LLFMT	"%lld"
#  define CUPS_LLCAST	(long long)
#else
#  define CUPS_LLFMT	"%ld"
#  define CUPS_LLCAST	(long)
#endif /* HAVE_LONG_LONG */

// These functions are only implemented on Mac OS X (and other BSD systems?)
#ifdef __APPLE__
#define HAVE_STRLCAT 1
#define HAVE_STRLCPY 1
#endif

#endif /* !_CUPS_CONFIG_H_ */
