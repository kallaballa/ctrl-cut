/*
 * printer_connection.h
 *
 *  Created on: 19.09.2009
 *      Author: amir
 */

#ifndef PRINTER_CONNECTION_H_
#define PRINTER_CONNECTION_H

/** Maximum allowable hostname characters. */
#define HOSTNAME_NCHARS (1024)

/** Maximum wait before timing out on connecting to the printer (in seconds). */
#define PRINTER_MAX_WAIT (300)

/** Number of seconds per a minute. */
#define SECONDS_PER_MIN (60)


typedef struct
{
	/** The pjl file for the print. */
	FILE *pjl_file;

	/** Job name for the print. */
	char *name;

	/** User name that submitted the print job. */
	char *user;

	/** Title for the job print. */
	char *title;

	/** Stub: Copies of the job print. */
	char *copies;

	/** Stub: Options for the job print. */
	char *options;
} printer_job;

int printer_connect(__const char *host, __const int timeout);
bool printer_disconnect(int socket_descriptor);
bool printer_send(__const char *host, printer_job *job);


#endif /* PRINTER_CONNECTION_H_ */
