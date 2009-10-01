/*
 * printer_connection.h
 *
 *  Created on: 19.09.2009
 *      Author: amir
 */

#ifndef PRINTER_CONNECTION_H_
#define PRINTER_CONNECTION_H

using namespace std;

/** Maximum allowable hostname characters. */
#define HOSTNAME_NCHARS (1024)

/** default port number for printer connections **/
#define DEFAULT_PORT (515)

/** Maximum wait before timing out on connecting to the printer (in seconds). */
#define PRINTER_MAX_WAIT (300)

/** Number of seconds per a minute. */
#define SECONDS_PER_MIN (60)


typedef struct
{
	/** The pjl file for the print. */
	string *pjl_filename;

	/** Job name for the print. */
	string *name;

	/** User name that submitted the print job. */
	string *user;

	/** Title for the job print. */
	string *title;

	/** Stub: Copies of the job print. */
	string *copies;

	/** Stub: Options for the job print. */
	string *options;
} printer_job;

class PrinterConnection {
private:
	int socket_descriptor;
	string *host;
	int timeout;
	ClientSocket *clientSocket;
	bool connected;

public:
	PrinterConnection(string *host, int timeout);
	virtual ~PrinterConnection();

	int connect();
	bool disconnect();
	bool send(printer_job *pjob);
};

#endif /* PRINTER_CONNECTION_H_ */
