/*
 * "$Id: string.h 8635 2009-05-14 21:18:35Z mike $"
 *
 *   String definitions for the Common UNIX Printing System (CUPS).
 *
 *   Copyright 2007-2009 by Apple Inc.
 *   Copyright 1997-2006 by Easy Software Products.
 *
 *   These coded instructions, statements, and computer programs are the
 *   property of Apple Inc. and are protected by Federal copyright
 *   law.  Distribution and use rights are outlined in the file "LICENSE.txt"
 *   which should have been included with this file.  If this file is
 *   file is missing or damaged, see the license at "http://www.cups.org/".
 *
 *   This file is subject to the Apple OS-Developed Software exception.
 */

#ifndef _CUPS_STRING_H_
#  define _CUPS_STRING_H_

/*
 * Include necessary headers...
 */

#  include <stdio.h>
#  include <stdlib.h>
#  include <stdarg.h>
#  include <ctype.h>
#  include <locale.h>

#  include <config.h>

#    include <string.h>

#    include <strings.h>


/*
 * Stuff for WIN32 and OS/2...
 */

#  if defined(WIN32) || defined(__EMX__)
#    define strcasecmp	_stricmp
#    define strncasecmp	_strnicmp
#  endif /* WIN32 || __EMX__ */


/*
 * C++ magic...
 */

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */


/*
 * String pool structures...
 */

#  define _CUPS_STR_GUARD	0x12344321

typedef struct _cups_sp_item_s		/**** String Pool Item ****/
{
#  ifdef DEBUG_GUARDS
  unsigned int	guard;			/* Guard word */
#  endif /* DEBUG_GUARDS */
  unsigned int	ref_count;		/* Reference count */
  char		str[1];			/* String */
} _cups_sp_item_t;


/*
 * Prototypes...
 */

extern void	_cups_strcpy(char *dst, const char *src);

#  ifndef HAVE_STRLCAT
extern size_t _cups_strlcat(char *, const char *, size_t);
#    define strlcat _cups_strlcat
#  endif /* !HAVE_STRLCAT */

#  ifndef HAVE_STRLCPY
extern size_t _cups_strlcpy(char *, const char *, size_t);
#    define strlcpy _cups_strlcpy
#  endif /* !HAVE_STRLCPY */

/*
 * String pool functions...
 */

extern char	*_cupsStrAlloc(const char *s);
extern void	_cupsStrFlush(void);
extern void	_cupsStrFree(const char *s);
extern char	*_cupsStrRetain(const char *s);
extern size_t	_cupsStrStatistics(size_t *alloc_bytes, size_t *total_bytes);


/*
 * Floating point number functions...
 */

extern char	*_cupsStrFormatd(char *buf, char *bufend, double number,
		                 struct lconv *loc);
extern double	_cupsStrScand(const char *buf, char **bufptr,
		              struct lconv *loc);


/*
 * C++ magic...
 */

#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif /* !_CUPS_STRING_H_ */

/*
 * End of "$Id: string.h 8635 2009-05-14 21:18:35Z mike $".
 */
