// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <Windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include "CoreMacro.h"
#include "ThreadManager.h"
#include "Lock.h"
#include "Memory.h"
#include "Allocator.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include<WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout <<cause << " ErrorCode : " << errCode << endl;
}

int main()
{
	//윈속 초기화(ws2_32 라이브러리 초기화)
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	// level : 옵션을 해석하고 처리할 주체를 넣어줌
	// 소켓 코드 -> SOIL_SOCKET
	// IPv4 -> IPROTO_IP
	// TCP 프로토콜 -> IPROTO_TCP

	//SO_KEEPALIVE = 주기적으로 연결 상태 확인 여부 (TCP Only) UDP는 연결된게 아님
	//상대방이 소리소문없이 연결을 끊는 경우가 있음
	// 주기적으로 TCP 프로토콜 연결 상태 확인 -> 끊어진 연결 감지
	bool enable = true;
	::setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&enable, sizeof(enable) )
		;//각 옵션마다 받는 타입이 다름

	// SO_LINGER = 지연하다
	// 송신 버퍼에 있는 버퍼를 보낼 것인가? 날릴 것인가?
	// onoff = 0이면 closesocket()이면 바로 리턴, 아니면 linger초만큼 대기(default 0)
	// linger : 대기시간
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 5;
	::setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

	// Half-Close
	// SD_SEND : send 막는다
	// SD_RECEIVE : recv 막음
	// SD_BOTH : 둘다 막음
	//::shutdown(serverSocket, SD_SEND);//send만 안됨
	//상대방도 보낼게 없을때 따로 패킷 보내서 보고 종료시킬 수 있음
	
	// SO_SNDBUF = 송신 버퍼 크기
	// SO_RCVBUF = 수신 버퍼 크기

	int32 sendBufferSize;
	int32 optionLen = sizeof(sendBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufferSize, &optionLen);
	cout << "송신 버퍼 크기 : " << sendBufferSize << endl;

	int32 recvBufferSize;
	optionLen = sizeof(recvBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufferSize, &optionLen);
	cout << "수신 버퍼 크기 : " << recvBufferSize << endl;

	//SO_REUSEADDR
	// IP주소 및 port 재사용 , 이건 왜?
	// TCP에서 소켓을 만든다음 주소랑 bind해서 포트 번호도 바인딩 해줌
	// 이때 이미 주소가 사용되고 있을 수 도 있음
	{
		//이리하면 바로 재사용 가능
		bool enable = true;
		::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));
		//그냥 계속 이거 씀
	}


	// IPPROTO_TCP
	// TCP_NODELAY = Nagle 네이글 알고리즘 작동 여부
	// 데이터가 충분히 크면 보내고, 그렇지 않으면 데이터가 충분히 쌓일때까지 대기!
	// 장점 : 작은 패킷이 불필요하게 많이 생성되는 일 방지
	// 단점 : 반응 시간 손해
	{
		bool enable = true;//작동 안하게
		::setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable));
	}


	// 소켓 리소스 반환
	// send -> closesocket일 경우 받은 패킷을 어떻게 처리?
	::closesocket(serverSocket);
	// 윈속 종료
	::WSACleanup();
} 