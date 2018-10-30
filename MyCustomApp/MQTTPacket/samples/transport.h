#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen);
int transport_getdata(unsigned char* buf, int count);
int transport_getdatanb(void *sck, unsigned char* buf, int count);
int transport_open(char* host, int port);
int transport_close(int sock);

#endif
