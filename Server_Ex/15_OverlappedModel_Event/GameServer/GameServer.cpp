﻿// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
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
const int32 BUFSIZE = 1000;
struct Session//각각의 클라이언트를 담당
{
	SOCKET socket;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;

	WSAOVERLAPPED overlapped = {};
};

int main()
{
	//윈속 초기화(ws2_32 라이브러리 초기화)
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	// 블로킹(Blocking) 막음
	// accept -> 접속한 클라가 있을 때
	// connect -> 클라가 서버 접속 성공했을 때
	// send, sendto -> 요청한 데이터를 송신 버퍼에 복사했을 때
	// recv, recvfrom -> 수신 버퍼에 도착한 데이터가 있고, 이를 유저 레벨 버퍼에 복사했을 때
	
	// 논블로킹(Non-Blocking) 막지않음
	
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;
	//이리 논블로킹으로 설정
	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)//알아서 크기
		return 0;

	cout << "Accept" << endl;
	// Overlapped IO (비동기 + 논블로킹)
	// - Overlapped 함수를 건다 (WSARecv, WSASend)
	// - Overlapped 함수가 성공했는지 확인 후
	// -> 성공했으면 결과를 얻어서 처리
	// -> 실패했으면 사유를 확인
	 
	//char sendBuffer[100];
	//WSABUF wsaBuf[2];
	//wsaBuf[0].buf = sendBuffer;
	//wsaBuf[0].len = 100;

	//char sendBuffer2[100];
	//wsaBuf[1].buf = sendBuffer2;
	//wsaBuf[1].len = 100;
	// 1) 비동기 입출력 소켓
	// 2) WSABUF 배열의 시작 주소 + 개수
	// 3) 보내고/받은 바이트 수
	// 4) 상세 옵션인데 0
	// 5) WSAOVERLAPPED 구조체 주소값
	// 6) 입출력이 완료되면 OS가 호출할 콜백 함수
	// WSASend
	// WSARecv
	// 위에 함수들은 결국 비동기라 언제 실행할지 몰라서 WSAOVERLAPPED 구조체등을 막 건들면 안됨
	//

	//Overlapped 모델 (이벤트 기반)
	// - 비동기 입출력 지원하는 소켓 생성 + 통지 받기 위한 이벤트 객체 생성
	// - 비동기 입출력 함수 호출 (1에서 만든 이벤트 객체를 같이 넘겨줌, WSAOVERLAPPED구조체에!)
	// - 비동기 작업이 바로 완료되지 않으면, WSA_IO_PENDING 오류 코드
	// - 운영체제는 이벤트 객체를 signaled 상태로 만들어서 완료 상태를 알려줌
	// - WSAWaitForMultipleEvents 함수 호출해서 이벤트 객체의 signal 판별
	// - WSAGetOverlappedResult 호출해서 비동기 입출력 결과 확인 및 데이터 처리
	// 
	
	// 1) 비동기 소켓
	// 2) 넘겨준 overlapped 구조체
	// 3) 전송된 바이트 수
	// 4) 비동기 입출력 작업이 끝날때까지 대기할지?(사실 이미 WSAWaitForMultipleEvents에서 기다려줘서 false로)
	// 5) 비동기 입출력 작업 관련 부가 정보, 거의 사용 안함
	// WSAGetOverlappedResult

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket;
		while (true)
		{
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
				break;

			if (::WSAGetLastError() == WSAEWOULDBLOCK)//비동기라
				continue;

			//문제 있는 상황
			return 0;
		}

		Session session = Session{ clientSocket };
		WSAEVENT wsaEvent = ::WSACreateEvent();
		session.overlapped.hEvent = wsaEvent;

		cout << "Client Connected !" << endl;

		while (true)
		{
			WSABUF wsaBuf;//이건 날려줘도 상관없음
			wsaBuf.buf = session.recvBuffer;//이건 절대 건들면 안됨
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, nullptr) == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					//이벤트를 통해서 받음
					::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
					::WSAGetOverlappedResult(session.socket, &session.overlapped, &recvLen, FALSE, &flags);//뒤늦게 받음
				}
				else
				{
					// TODO : 문제있는 상황
					break;
				}
			}

			cout << "Data Recv Len = " << recvLen << endl;
		}

		::closesocket(session.socket);
		::WSACloseEvent(wsaEvent);
	}
	 
	// 윈속 종료
	::WSACleanup();
} 
