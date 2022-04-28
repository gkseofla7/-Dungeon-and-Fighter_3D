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

	// 블로킹(Blocking) 막음
	// accept -> 접속한 클라가 있을 때
	// connect -> 클라가 서버 접속 성공했을 때
	// send, sendto -> 요청한 데이터를 송신 버퍼에 복사햇을 때
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
	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);
	while (true)
	{
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			//블록킹 상태에선 기다렸다가 연결되면 빠져나와서 이때 문제가 생기면 문제가 맞는데
			//논블로킹에선 이때 꼭 문제가 있는게 아님
			//원래 블록했어야 했는데.. 너가 논블로킹으로 하라며
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;//데이터가 아직 오지 않은거
			//error
			break;
		}
		cout << "Client Connected!" << endl;

		// Recv
		while (true)
		{
			char recvBuffer[1000];
			int32 recvLen= ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
					continue;//데이터가 아직 오지 않은거

				break;


			}
			else if (recvLen == 0)
			{
				// 연결 끊김
				break;
			}

			cout << "Recv Data Len = " << recvLen << endl;


			// Send
			while (true)
			{
				//보내려는데 공간이 없어서 못보낼 수 도있고 다양한 상황
				if (::send(clientSocket, recvBuffer, recvLen, 0) == SOCKET_ERROR)
				{
					if (::WSAGetLastError() == WSAEWOULDBLOCK)
						continue;//데이터가 아직 오지 않은거
					//Error
					break;
				}
				cout << "Send Data! Len = " << recvLen << endl;
				break;
			}
		}
	}
	// 윈속 종료
	::WSACleanup();
} 