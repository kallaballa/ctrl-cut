/*
 * "$Id: http-private.h 9104 2010-04-12 04:03:53Z mike $"
 *
 *   Private HTTP definitions for the Common UNIX Printing System (CUPS).
 *
 *   Copyright 2007-2008 by Apple Inc.
 *   Copyright 1997-2007 by Easy Software Products, all rights reserved.
 *
 *   These coded instructions, statements, and computer programs are the
 *   property of Apple Inc. and are protected by Federal copyright
 *   law.  Distribution and use rights are outlined in the file "LICENSE.txt"
 *   which should have been included with this file.  If this file is
 *   file is missing or damaged, see the license at "http://www.cups.org/".
 *
 *   This file is subject to the Apple OS-Developed Software exception.
 */

#ifndef _CUPS_HTTP_PRIVATE_H_
#  define _CUPS_HTTP_PRIVATE_H_

/*
 * Include necessary headers...
 */

#  include <stdlib.h>
#  include <config.h>

#  ifdef __sun
#    include <sys/select.h>
#  endif /* __sun */

#  include <limits.h>
#  ifdef WIN32
#    include <io.h>
#    include <winsock2.h>
#  else
#    include <unistd.h>
#    include <fcntl.h>
#    include <sys/socket.h>
#    define closesocket(f) close(f)
#  endif /* WIN32 */

#  if defined(__sgi) || (defined(__APPLE__) && !defined(_SOCKLEN_T))
/*
 * IRIX and MacOS X 10.2.x do not define socklen_t, and in fact use an int instead of
 * unsigned type for length values...
 */

typedef int socklen_t;
#  endif /* __sgi || (__APPLE__ && !_SOCKLEN_T) */

#  include <cups/http.h>
#  include "md5.h"
#  include "ipp-private.h"

struct _http_s				/**** HTTP connection structure. ****/
{
  int			fd;		/* File descriptor for this socket */
  int			blocking;	/* To block or not to block */
  int			error;		/* Last error on read */
  time_t		activity;	/* Time since last read/write */
  http_state_t		state;		/* State of client */
  http_status_t		status;		/* Status of last request */
  http_version_t	version;	/* Protocol version */
  http_keepalive_t	keep_alive;	/* Keep-alive supported? */
  struct sockaddr_in	_hostaddr;	/* Address of connected host @deprecated@ */
  char			hostname[HTTP_MAX_HOST],
  					/* Name of connected host */
			fields[HTTP_FIELD_MAX][HTTP_MAX_VALUE];
					/* Field values */
  char			*data;		/* Pointer to data buffer */
  http_encoding_t	data_encoding;	/* Chunked or not */
  int			_data_remaining;/* Number of bytes left @deprecated@ */
  int			used;		/* Number of bytes used in buffer */
  char			buffer[HTTP_MAX_BUFFER];
					/* Buffer for incoming data */
  int			auth_type;	/* Authentication in use */
  _cups_md5_state_t	md5_state;	/* MD5 state */
  char			nonce[HTTP_MAX_VALUE];
					/* Nonce value */
  int			nonce_count;	/* Nonce count */
  void			*tls;		/* TLS state information */
  http_encryption_t	encryption;	/* Encryption requirements */
  /**** New in CUPS 1.1.19 ****/
  fd_set		*input_set;	/* select() set for httpWait() @deprecated@ */
  http_status_t		expect;		/* Expect: header @since CUPS 1.1.19@ */
  char			*cookie;	/* Cookie value(s) @since CUPS 1.1.19@ */
  /**** New in CUPS 1.1.20 ****/
  char			_authstring[HTTP_MAX_VALUE],
					/* Current Authentication value. @deprecated@ */
			userpass[HTTP_MAX_VALUE];
					/* Username:password string @since CUPS 1.1.20@ */
  int			digest_tries;	/* Number of tries for digest auth @since CUPS 1.1.20@ */
  /**** New in CUPS 1.2 ****/
  off_t			data_remaining;	/* Number of bytes left @since CUPS 1.2@ */
  http_addr_t		*hostaddr;	/* Current host address and port @since CUPS 1.2@ */
  http_addrlist_t	*addrlist;	/* List of valid addresses @since CUPS 1.2@ */
  char			wbuffer[HTTP_MAX_BUFFER];
					/* Buffer for outgoing data */
  int			wused;		/* Write buffer bytes used @since CUPS 1.2@ */
  /**** New in CUPS 1.3 ****/
  char			*field_authorization;
					/* Authorization field @since CUPS 1.3@ */
  char			*authstring;	/* Current authorization field @since CUPS 1.3 */
};


/*
 * Prototypes...
 */

extern int		_httpAddrPort(http_addr_t *addr);
extern http_t		*_httpCreate(const char *host, int port,
			             http_encryption_t encryption);
extern char		*_httpEncodeURI(char *dst, const char *src,
			                size_t dstsize);
extern const char	*_httpResolveURI(const char *uri, char *resolved_uri,
			                 size_t resolved_size, int log);
extern int		_httpWait(http_t *http, int msec, int usessl);
#endif /* !_CUPS_HTTP_PRIVATE_H_ */

/*
 * End of "$Id: http-private.h 9104 2010-04-12 04:03:53Z mike $".
 */
