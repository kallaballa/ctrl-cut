/*
 * printer_connection.c
 *
 *  Created on: 19.09.2009
 *      Author: amir
 */
#include <cctype>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "PrinterConnection.h"

using namespace std;
int socket_descriptor = -1;
string *host;
int timeout;
ClientSocket *clientSocket;
bool connected = false;

/**
 *
 * @param host The hostname or IP address of the printer to connect to.
 * @param timeout The number of seconds to wait before timing out on the
 * connect operation.
 */

PrinterConnection::PrinterConnection(string *host, int timeout) {
	this->host = host;
	this->timeout = timeout;
}

PrinterConnection::~PrinterConnection() {
}

/**
 * Connect to a printer.
 *
 * @param host The hostname or IP address of the printer to connect to.
 * @param timeout The number of seconds to wait before timing out on the
 * connect operation.
 * @return true if the connection was successfully established
 */

bool PrinterConnection::connect() {
	int i;

	for (i = 0; i < timeout; i++) {
		/* Set an alarm to go off if the program has gone out to lunch. */
		alarm(SECONDS_PER_MIN);

		try {
			clientSocket = new ClientSocket(*host, DEFAULT_PORT);
		} catch (SocketException& e) {
			cerr << "Exception caught on connection attempt:"
					<< e.description() << "\n";
			sleep(1);
		}
		break;
		/* Sleep for a second then try again. */

	}
	if (i >= timeout) {
		fprintf(stderr, "Cannot connect to %s\n", host);
		this->connected = false;
	} else {
		this->connected = true;
	}
	/* Disable the timeout alarm. */
	alarm(0);
	return this->connected;
}



/**
 *
 */
bool PrinterConnection::send(printer_job *pjob) {
	if (!connected)
		perror("No connection established.");
	string nl = "\n";
	char localhost[HOSTNAME_NCHARS] = "";
	int file_size;
	ifstream if_pjl(pjob->pjl_filename->data(), ios::in | ios::binary
			| ios::ate);

	gethostname(localhost, sizeof(localhost));
	{
		char *d = strchr(localhost, '.');
		if (d) {
			*d = 0;
		}
	}
	string str_localhost(localhost);
	// talk to printer

	string send_buffer;
	cout << pjob->options->length() << endl;
	if(pjob->options->length() < 2)
		send_buffer = "\002\n";
	else
		send_buffer = "\002" + *(pjob->options) + '\n';

	string recv_buffer;

	clientSocket->write(send_buffer.data(), send_buffer.length());
	clientSocket->receive(recv_buffer, 1);

	if (recv_buffer.data()[0]) {
		cerr << "Bad response from " << host->data() << ", "
				<< recv_buffer.data() << endl;
		//return false;
	}
	send_buffer = "H" + str_localhost + nl;
	/*+ "P" + *(pjob->user) + nl + "J"
			+ *(pjob->title) + nl + "ldfA" + *(pjob->name) + str_loclhost + nl
			+ "UdfA" + *(pjob->name) + str_loclhost + nl + "N" + *(pjob->title)
			+ nl;*/


	std::stringstream ss;
	ss <<  send_buffer.length();
	string tmp = '\002' + ss.str() + " cfA" + *(pjob->name)+ str_localhost + nl;

	clientSocket->write(tmp.data(), tmp.length()) ;
	clientSocket->receive(recv_buffer, 1);

	if (recv_buffer.data()[0]) {
		cerr << "Bad response from " << host->data() << ", "
				<< recv_buffer.data() << endl;
		//return false;
	}

	clientSocket->write(send_buffer.data(), send_buffer.length());
	clientSocket->receive(recv_buffer, 1);
	if (recv_buffer.data()[0]) {
		cerr << "Bad response from " << host->data() << ", "
				<< recv_buffer.data() << endl;
		//return false;
	}
	{
		{
			if (if_pjl.is_open()) {
				file_size = if_pjl.tellg();
			} else {
				cerr << "Error: file could not be opened" << endl;
				return false;
			}

			stringstream ss2;
			ss2 <<  file_size;
			string tmp = ss2.str();

			send_buffer = "\003" + tmp + " dfA" + *(pjob->name)
					+ localhost + nl;
		}

		clientSocket->write("\000", 1);
		clientSocket->write(send_buffer.data(), send_buffer.length());
		clientSocket->receive(recv_buffer, 1);

		if (recv_buffer.data()[0]) {
			cerr << "Bad response from " << host->data() << ", "
					<< recv_buffer.data() << endl;
			//return false;
		}
		{
			printf("sending file");
			char * memblock;
			if (if_pjl.is_open()) {
				memblock = new char[file_size];
				if_pjl.seekg(0, ios::beg);
				if_pjl.read(memblock, file_size);
				if_pjl.close();
				clientSocket->write(memblock, file_size);
			} else {
				cerr << "Error: file could not be opened" << endl;
				return false;
			}

			if_pjl.close();
		}
	}

	return true;
}


