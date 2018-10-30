/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/
#include "cbm_consts.h"
#include "app2soc_struct.h"
#include "soc_api.h"
#include "DtcntSrvGprot.h"
#include "mmi_frm_events_gprot.h"
#include "mmi_frm_mem_gprot.h"
#include "mmi_frm_nvram_gprot.h"
#include "cbm_api.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "socket.h"
/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
#if !defined(SOCKET_ERROR)
	/** error in socket operation */
	#define SOCKET_ERROR -1
#endif

#define INVALID_SOCKET SOCKET_ERROR

static int mysock = INVALID_SOCKET;

//static  mqtt_socket_func socket_func;
void transport_init()
{
	//socket_func.open = mmi_socket_send;
	//socket_func.init_flag = 1;
}

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = 0;
//	rc = write(sock, buf, buflen);
	return rc;
}


int transport_getdata(unsigned char* buf, int count)
{
	int rc = -1;
	//int rc = recv(mysock, buf, count, 0);
	//printf("received %d bytes count %d\n", rc, (int)count);
	return rc;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	int sock = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
	/* this call will return after the timeout set on initialization if no bytes;
	   in your system you will use whatever you use to get whichever outstanding
	   bytes your socket equivalent has ready to be extracted right now, if any,
	   or return immediately */
	int rc = -1;	
	//rc = recv(sock, buf, count, 0);	
	if (rc == -1) {
		/* check error conditions from your system here, and return -1 */
		return 0;
	}
	return rc;
}

/**
²»ÐèÒª
*/
int transport_open(char* addr, int port)
{
	return -1;
}

int transport_close(int sock)
{
	int rc;
	//soc_close(socket_id);  
	//rc = shutdown(sock, SHUT_WR);
	//rc = recv(sock, NULL, (size_t)0, 0);
	//rc = close(sock);

	return rc;
}
