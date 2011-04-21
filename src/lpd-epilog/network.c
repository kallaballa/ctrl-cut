/*
 * "$Id: network.c 8896 2009-11-20 01:27:57Z mike $"
 *
 *   Common network APIs for the Common UNIX Printing System (CUPS).
 *
 *   Copyright 2007-2009 by Apple Inc.
 *   Copyright 2006-2007 by Easy Software Products, all rights reserved.
 *
 *   These coded instructions, statements, and computer programs are the
 *   property of Apple Inc. and are protected by Federal copyright
 *   law.  Distribution and use rights are outlined in the file "LICENSE.txt"
 *   "LICENSE" which should have been included with this file.  If this
 *   file is missing or damaged, see the license at "http://www.cups.org/".
 *
 *   This file is subject to the Apple OS-Developed Software exception.
 *
 * Contents:
 *
 *   backendCheckSideChannel() - Check the side-channel for pending requests.
 *   backendNetworkSideCB()    - Handle common network side-channel commands.
 */

/*
 * Include necessary headers.
 */

#include "backend-private.h"
#include <limits.h>
#ifdef __hpux
#  include <sys/time.h>
#else
#  include <sys/select.h>
#endif /* __hpux */


/*
 * 'backendCheckSideChannel()' - Check the side-channel for pending requests.
 */


void
backendCheckSideChannel(
    int         snmp_fd,		/* I - SNMP socket */
    http_addr_t *addr)			/* I - Address of device */
{
  fd_set	input;			/* Select input set */
  struct timeval timeout;		/* Select timeout */


  FD_ZERO(&input);
  FD_SET(CUPS_SC_FD, &input);

  timeout.tv_sec = timeout.tv_usec = 0;

  if (select(CUPS_SC_FD + 1, &input, NULL, NULL, &timeout) > 0)
    backendNetworkSideCB(-1, -1, snmp_fd, addr, 0);
}


/*
 * 'backendNetworkSideCB()' - Handle common network side-channel commands.
 */

int					/* O - -1 on error, 0 on success */
backendNetworkSideCB(
    int         print_fd,		/* I - Print file or -1 */
    int         device_fd,		/* I - Device file or -1 */
    int         snmp_fd,		/* I - SNMP socket */
    http_addr_t *addr,			/* I - Address of device */
    int         use_bc)			/* I - Use back-channel data? */
{
  cups_sc_command_t	command;	/* Request command */
  cups_sc_status_t	status;		/* Request/response status */
  char			data[2048];	/* Request/response data */
  int			datalen;	/* Request/response data size */
  const char		*device_id;	/* 1284DEVICEID env var */


  datalen = sizeof(data);

  if (cupsSideChannelRead(&command, &status, data, &datalen, 1.0))
    return (-1);

  switch (command)
  {
    case CUPS_SC_CMD_DRAIN_OUTPUT :
       /*
        * Our sockets disable the Nagle algorithm and data is sent immediately.
	*/

        if (device_fd < 0)
	  status = CUPS_SC_STATUS_NOT_IMPLEMENTED;
	else if (backendDrainOutput(print_fd, device_fd))
	  status = CUPS_SC_STATUS_IO_ERROR;
	else 
          status = CUPS_SC_STATUS_OK;

	datalen = 0;
        break;

    case CUPS_SC_CMD_GET_BIDI :
	status  = CUPS_SC_STATUS_OK;
        data[0] = use_bc;
        datalen = 1;
        break;

    default :
        status  = CUPS_SC_STATUS_NOT_IMPLEMENTED;
	datalen = 0;
	break;
  }

  return (cupsSideChannelWrite(command, status, data, datalen, 1.0));
}


/*
 * End of "$Id: network.c 8896 2009-11-20 01:27:57Z mike $".
 */
