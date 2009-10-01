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

int PrinterConnection::connect() {
    socket_descriptor = -1;
    int i;

    for (i = 0; i < timeout; i++) {
        struct addrinfo *res;
        struct addrinfo *addr;
        struct addrinfo base = { 0, PF_UNSPEC, SOCK_STREAM };
        int error_code = getaddrinfo(host->data(), "printer", &base, &res);

        /* Set an alarm to go off if the program has gone out to lunch. */
        alarm(SECONDS_PER_MIN);

        /* If getaddrinfo did not return an error code then we attempt to
         * connect to the printer and establish a socket.
         */
        if (!error_code) {
            for (addr = res; addr; addr = addr->ai_next) {
                socket_descriptor = socket(addr->ai_family, addr->ai_socktype,
                                           addr->ai_protocol);
                if (socket_descriptor >= 0) {
                    if (!::connect(socket_descriptor, addr->ai_addr,
                                 addr->ai_addrlen)) {
                        break;
                    } else {
                        close(socket_descriptor);
                        socket_descriptor = -1;
                    }
                }
            }
            freeaddrinfo(res);
        }
        if (socket_descriptor >= 0) {
            break;
        }

        /* Sleep for a second then try again. */
        sleep(1);
    }
    if (i >= timeout) {
        fprintf(stderr, "Cannot connect to %s\n", host);
        return -1;
    }
    /* Disable the timeout alarm. */
    alarm(0);
    /* Return the newly opened socket descriptor */
    return socket_descriptor;
}



/**
 *
 */
bool PrinterConnection::send(printer_job *pjob) {
    /* Open printer job language file. */
	char buf[102400];
	FILE* pjl_file = fopen(pjob->pjl_filename->data(), "r");
    if (!pjl_file) {
        perror(pjob->pjl_filename->data());
        return 1;
    }
	char localhost[HOSTNAME_NCHARS] = "";
    unsigned char lpdres;

    gethostname(localhost, sizeof(localhost));
    {
        char *d = strchr(localhost, '.');
        if (d) {
            *d = 0;
        }
    }

    /* Connect to the printer. */
    //socket_descriptor = connect();

    // talk to printer
    sprintf(buf, "\002%s\n", pjob->options->data());
    write(socket_descriptor, (char *)buf, strlen(buf));
    read(socket_descriptor, &lpdres, 1);
    if (lpdres) {
        fprintf (stderr, "Bad response from %s, %u\n", host, lpdres);
        //return false;
    }
    sprintf(buf, "H%s\n", localhost);
    sprintf(buf + strlen(buf) + 1, "P%s\n", pjob->user);
    sprintf(buf + strlen(buf) + 1, "J%s\n", pjob->title);
    sprintf(buf + strlen(buf) + 1, "ldfA%s%s\n", pjob->name, localhost);
    sprintf(buf + strlen(buf) + 1, "UdfA%s%s\n", pjob->name, localhost);
    sprintf(buf + strlen(buf) + 1, "N%s\n", pjob->title);
    sprintf(buf + strlen(buf) + 1, "\002%d cfA%s%s\n", (int)strlen(buf), pjob->name, localhost);
    write(socket_descriptor, buf + strlen(buf) + 1, strlen(buf + strlen(buf) + 1));
    read(socket_descriptor, &lpdres, 1);
    if (lpdres) {
        fprintf(stderr, "Bad response from %s, %u\n", host, lpdres);
        //return false;
    }
    write(socket_descriptor, (char *)buf, strlen(buf) + 1);
    read(socket_descriptor, &lpdres, 1);
    if (lpdres) {
        fprintf(stderr, "Bad response from %s, %u\n", host, lpdres);
        //return false;
    }
    {
        {
            struct stat file_stat;
            if (fstat(fileno(pjl_file), &file_stat)) {
                perror(buf);
                return false;
            }
            sprintf((char *) buf, "\003%u dfA%s%s\n", (int) file_stat.st_size, pjob->name, localhost);
        }
        write(socket_descriptor, (char *)buf, strlen(buf));
        read(socket_descriptor, &lpdres, 1);
        if (lpdres) {
            fprintf(stderr, "Bad response from %s, %u\n", host, lpdres);
            //return false;
        }
        {
            int l;
            while ((l = fread((char *)buf, 1, sizeof (buf), pjl_file)) > 0) {
                write(socket_descriptor, (char *)buf, l);
            }
        }
    }
    // dont wait for a response...
    close(socket_descriptor);
	return true;
}


