/* config.h.  Generated from config.h.in by configure.  */
/*
 * "$Id: config.h.in 9259 2010-08-13 04:11:46Z mike $"
 *
 *   Configuration file for CUPS.
 *
 *   Copyright 2007-2010 by Apple Inc.
 *   Copyright 1997-2007 by Easy Software Products.
 *
 *   These coded instructions, statements, and computer programs are the
 *   property of Apple Inc. and are protected by Federal copyright
 *   law.  Distribution and use rights are outlined in the file "LICENSE.txt"
 *   which should have been included with this file.  If this file is
 *   file is missing or damaged, see the license at "http://www.cups.org/".
 */

#ifndef _CUPS_CONFIG_H_
#define _CUPS_CONFIG_H_

#  define CUPS_LLFMT	"%lld"
#  define CUPS_LLCAST	(long long)

/*
 * Do we have the strXXX() functions?
 */

#ifdef __APPLE__
#define HAVE_STRLCAT 1
#define HAVE_STRLCPY 1
#endif

#endif /* !_CUPS_CONFIG_H_ */

/*
 * End of "$Id: config.h.in 9259 2010-08-13 04:11:46Z mike $".
 */
