#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/sysinfo.h>

#include "hdr/msgHeader.h"

#define SIZE sizeof(struct sockaddr_in)
#define MAX_SIZE 2048

void *recvMsg(void *parms);
int sendMsg2(char *msg, int size, struct sockaddr* sock, int sock_len);


int main()
{
	pthread_t tid;
	if (pthread_create(&tid, NULL, recvMsg, NULL))
	{
		printf("thread create fail\n");
	}

	while (1)
		sleep(1);
}

void *recvMsg(void *parms)
{
	struct sockaddr_in mySock = {AF_INET, htons(PORT_SERVER_R), INADDR_ANY};
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	int ret = bind(sockfd, (struct sockaddr *)&mySock, sizeof(mySock));
	if (ret < 0) 
	{
		extern int errno;
		printf("ret(%d), errno(%d, %s)\n", ret, errno, strerror(errno));
	}

	struct sockaddr_in sock;
	int sock_len;
	char msg[MAX_SIZE];
	while (1) 
	{
		memset(msg, 0, MAX_SIZE);
		int ret = recvfrom(sockfd, &msg, MAX_SIZE, 0, (struct sockaddr*)&sock, (socklen_t*)&sock_len);
		if (ret < 0) 
		{
			extern int errno;
			printf("ret(%d), errno(%d, %s)\n", ret, errno, strerror(errno));
		} 
		else 
		{
			printf("%d msg received\n", ret);
			struct msgHeader hdr = *(struct msgHeader *)msg;
			Read(hdr);
			switch(hdr.cmdId)
			{
				case reqGetTime:
					{
						printf("Get Time\n");

						struct msgTime mTime;
						mTime.hdr.cmdId = rspGetTime;
						mTime.data.time = time(NULL);
						printf("time : %ld\n", mTime.data.time);
						mTime.data.retValue = 0;
						Write(mTime);

						sock.sin_port = htons(PORT_CLIENT_R);
						sendMsg2((char*)&mTime, sizeof(mTime), (struct sockaddr*) &sock, sock_len);
						break;
					}
				case reqSysInfo:
					{
						printf("Sys Info\n");
						struct sysinfo sys;
						int ret = sysinfo(&sys);

						struct msgSysInfo mSysInfo;
						mSysInfo.hdr.cmdId = rspSysInfo;
						gethostname(mSysInfo.data.hostname, sizeof(mSysInfo.data.hostname));
						printf("hostname : %s\n", mSysInfo.data.hostname);

						mSysInfo.data.totalram = sys.totalram;
						mSysInfo.data.freeram = sys.freeram;
						printf("total ram : %ld\nfree ram : %ld\n", sys.totalram, sys.freeram);

						mSysInfo.data.retValue = ret;
						Write(mSysInfo);

						sock.sin_port = htons(PORT_CLIENT_R);
						sendMsg2((char*)&mSysInfo, sizeof(mSysInfo), (struct sockaddr*)&sock, sock_len);
						break;
					}
			}
		}
	}
	close(sockfd);
}


int sendMsg2(char *msg, int size, struct sockaddr* sock, int sock_len)
{
	struct sockaddr_in mySock = {AF_INET, htons(PORT_SERVER_S), INADDR_ANY};
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	int ret = bind(sockfd, (struct sockaddr*)&mySock, sizeof(mySock));
	if (ret < 0) 
	{
		extern int errno;
		printf("ret(%d), errno(%d, %s)\n", ret, errno, strerror(errno));
	}

	sendto(sockfd, msg, size, 0, sock, sock_len);
	close(sockfd);
	return 0;
}
