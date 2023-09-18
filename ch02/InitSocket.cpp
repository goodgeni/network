#include "../Common.h"

int main(int argc, char *argv[])
{
	// 소켓 생성
	// 괄호 안을 채워 보세요
	SOCKET sock = socket(    ,     ,    );
	if (sock == INVALID_SOCKET) err_quit("socket()");
	printf("[알림] 소켓 생성 성공\n");

	// 소켓 닫기
	close(sock);
	return 0;
}
