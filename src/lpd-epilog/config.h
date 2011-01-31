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

/*
 * Do we have the strXXX() functions?
 */

#define HAVE_STRCASECMP 1
#define HAVE_STRNCASECMP 1
#define HAVE_STRLCAT 1
#define HAVE_STRLCPY 1


/*
 * Do we have the geteuid() function?
 */

#define HAVE_GETEUID 1

/*
 * Do we have the (v)snprintf() functions?
 */

#define HAVE_SNPRINTF 1
#define HAVE_VSNPRINTF 1

/*
 * Which encryption libraries do we have?
 */

#define HAVE_CDSASSL 1

/*
 * What Security framework headers do we have?
 */

#define HAVE_AUTHORIZATION_H 1

/*
 * Do we have rresvport_af()?
 */

#define HAVE_RRESVPORT_AF 1




/*
 * Do we have getifaddrs()?
 */

#define HAVE_GETIFADDRS 1


/*
 * Do we have hstrerror()?
 */

#define HAVE_HSTRERROR 1


/*
 * Do we have the <sys/sockio.h> header file?
 */

#define HAVE_SYS_SOCKIO_H 1



/*
 * Do we have the GSSAPI support library (for Kerberos support)?
 */

#define HAVE_GSSAPI 1
#define HAVE_GSSAPI_H 1
#define HAVE_GSSAPI_GSSAPI_H 1
#define HAVE_GSSAPI_GSSAPI_GENERIC_H 1
#define HAVE_GSSAPI_GSSAPI_KRB5_H 1
#define HAVE_GSS_C_NT_HOSTBASED_SERVICE 1
#define HAVE_KRB5_H 1
/* #undef HAVE_HEIMDAL */

#endif /* !_CUPS_CONFIG_H_ */
