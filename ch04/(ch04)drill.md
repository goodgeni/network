# 4장 연습문제

## 문제1
### TCPServer.cpp 예제를 수정하여 시작할 때 명령행 인수로 포트 번호를 입력할 수 있도록 수정하시오. 또한 TCPClient.cpp 예제를 수정하여 시작할 때 명령행 인수로 접속할 서버의 IPv4 주소 외에 포트 번호도 입력할 수 있도록 수정하시오.

    ./TCPServer 9000   //  ./a.out argv[1] argv[2] ... argv[n]
    
## 문제2
### TCPServer, Client 예제를 수정하여 받은 데이터를 영문자열로 간주하고 모두 대문자로 바꾼 후 다시 클라이언트에 보내도록 수정하시오.

    #include <ctype.h>
    int tolower(int C); //대문자를 소문자로 변환
    int toupper(int c); //소문자를 대문자로 변환
    
## 문제3
### TCPServer, Client 예제를 수정하여 클라이언트에서 32비트 정수 두개를 입력받아 보내면 서버에서 두 수의 합을 계산하여 숫자 형태로 리턴하도록 수정하시오.

## 문제4
### TCPServer, Client 예제를 수정하여 클라이언트가 접속하면 서버에서 현재 날짜와 시각을 문자열 형태로 리턴하도록 수정하시오.

    // 현재 날짜와 시각을 문자열 형태로 리턴한다.
    #include <time.h>
    void getCurrentDateTime(char res[])
    {
	    time_t t;
	    time(&t);

	    struct tm *p;
	    p = localtime(&t);
	    sprintf(res, "%d년 %d월 %d일 %d시 %d분 %d초", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    }







