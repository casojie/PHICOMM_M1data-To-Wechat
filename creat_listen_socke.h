#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<string.h>
#include "errexit.h"

#ifndef INADDR_NONE
#define INADDR_NONE   0xffffffff
#endif
int errexit(const char *format,...);

int creat_listen_socke(const char* _port,const char* _ip,int max_listen)
{
	
	int socke = socket(AF_INET,SOCK_STREAM,0);
	if(socke<0)
	{
		errexit("can't creat source socke (creat_listen_socke.c/14)\n");
		exit(1);
	}
	
	struct sockaddr_in local;
	memset(&local,0,sizeof(local));
    local.sin_family = AF_INET;
    if((local.sin_port = htons((unsigned short)atoi(_port))) == 0)
           errexit("can't get \"%s\" service entry\n",_port);
    if((local.sin_addr.s_addr = inet_addr(_ip)) == INADDR_NONE)
           errexit("can't get \"%s\" host entry\n",_ip);

	if(bind(socke,(struct sockaddr*)&local,sizeof(local))>0)
	{
		errexit("can't bind socket and sockaddr (creat_listen_socke.c/26)\n");
		exit(1);
	}
	
	if(listen(socke,max_listen)<0)
	{
		errexit("can't listen socke(creat_listen_socke.c/32)\n");
		exit(1);
	}
	
	return socke;
}