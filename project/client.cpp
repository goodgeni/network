#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <unistd.h>

#include "hdr/msgHeader.h"

#define SIZE sizeof(struct socketaddr_in)
#define MAX_SIZE 2048

void *recvMsg(void *parms);
int sendMsg(char *msg, int size, char *ip, int port);

int main(int argc, char **argv)
{
	pthread_t tid;
	if (pthread_create(&tid, NULL, recvMsg, NULL)) 
	{
		printf("thread create fail\n");
	}

	int choice = 0;
	printf("==== Choice ====\n");
	printf("1. Get Time     \n");
	printf("2. Get Sys Info \n");
	printf("================\n");
	printf("Choice : ");

	while(1) {
		scanf("%d", &choice);

		switch(choice) {
			case 1:
				// reqGetTime
				struct msgTime mTime;
				mTime.hdr.cmdId = reqGetTime;
				Write(mTime);
				sendMsg((char*)&mTime, sizeof(mTime), (char*)"127.0.0.1", PORT_SERVER_R);
				break;
			case 2:
				// reqSysInfo
				struct msgSysInfo mSysInfo;
				mSysInfo.hdr.cmdId = reqSysInfo;
				Write(mSysInfo);
				sendMsg((char*)&mSysInfo, sizeof(mSysInfo), (char*)"127.0.0.1", PORT_SERVER_R);
				break;
			default:
				printf("Invalid choice\n");
				break;
		}
	}
}

void *recvMsg(void *parms)
{
	struct sockaddr_in mySock = {AF_INET, htons(PORT_CLIENT_R), INADDR_ANY};
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	int ret = bind(sockfd, (struct sockaddr*)&mySock, sizeof(mySock));
	if (ret < 0) 
	{
		extern int errno;
		printf("ret (%d), errno(%d, %s)\n", ret, errno, strerror(errno));
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
				case rspGetTime:
					{
						printf("\n=== Time ===\n");
						struct dataTime data = *(struct dataTime *)(msg+sizeof(struct msgHeader));
						Read(data);
						printf("rawTime : %ld\n", data.time);

						struct tm* timeInfo;
						timeInfo = localtime(&data.time);
						int year = timeInfo->tm_year + 1900;
						int month = timeInfo->tm_mon + 1;
						int day = timeInfo->tm_mday;
						int hour = timeInfo->tm_hour;
						int min = timeInfo->tm_min;
						int sec = timeInfo->tm_sec;
						printf("timeInfo : %d년 %d월 %d일 %d시 %d분 %d초\n", year, month, day, hour, min, sec);
						printf("===================\n");
						printf("\n");
						break;
					}
				case rspSysInfo:
					{
						printf("\n=== Sys Info ===\n");
						struct dataSysInfo data = *(struct dataSysInfo *)(msg+sizeof(struct msgHeader));
						Read(data);
						printf("host name : %s\n", data.hostname);
						printf("total ram : %ld\n", data.totalram);
						printf("free ram : %ld\n", data.freeram);
						printf("===================\n");
						printf("\n");
						break;
					}
				default:
					break;
			}
		}
	}
	close(sockfd);
}

int sendMsg(char *msg, int size, char *ip, int port)
{
	struct sockaddr_in mySock = {AF_INET, htons(PORT_CLIENT_S), INADDR_ANY};
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	int ret = bind(sockfd, (struct sockaddr*)&mySock, sizeof(mySock));
	if (ret < 0) 
	{
		extern int errno;
		printf("ret(%d), errno(%d, %s)\n", ret, errno, strerror(errno));
	}

	struct sockaddr_in sock = {AF_INET, htons(port), INADDR_ANY};
	sock.sin_addr.s_addr = inet_addr(ip);

	sendto(sockfd, msg, size, 0, (struct sockaddr*)&sock, sizeof(sock));
	close(sockfd);
	return 0;
}
