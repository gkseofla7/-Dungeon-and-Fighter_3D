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



int main()
{
	//윈속 초기화(ws2_32 라이브러리 초기화)
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;


	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	//socket(전화기) 오브젝트 만듬, af = 주소체계(IPv4, IPv6), Type은 TCP로 통신할지 UDP로 통신할지
	//즉 여기서는 IPv4, TCP사용
	//Socket은 그냥 번호인데 운영체제한테 이 소켓에 보내고 싶다고 말하면 알아서 보내줌
	if (listenSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();//어떤 이유로?
		cout << "Socket ErrorCode : " << errCode << endl;
		return 0;
	}
	

	//나의 주소는?
	SOCKADDR_IN serverAddr; //IPv4일때 사용
	::memset(&serverAddr, 0, sizeof(serverAddr));//0으로 초기화
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);// 너가 알아서 해줘
	serverAddr.sin_port = ::htons(7777);

	//안내원 폰 개통! 식당의 대표 번호
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		//번호(주소)를 내 핸드폰에 연결
	{
		int32 errCode = ::WSAGetLastError();//어떤 이유로?
		cout << "Bind ErrorCode : " << errCode << endl;
		return 0;
	}

	//영업 시작!
	if(::listen(listenSocket, 10)== SOCKET_ERROR)
		//10은 자리가 없을때 대기할 수 있는 자리, 대기열에 최대 한도, 자리가 없으면 보냄
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Listen ErrorCodoe : " << errCode << endl;
		return 0;
	}

	// ---------------------------


	while (true)
	{
		SOCKADDR_IN clientAddr;//나한테 연결한 클라이언트 주소
		::memset(&serverAddr, 0, sizeof(serverAddr));
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket= ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		//이때 클라이언트와 대화하는 소캣 나옴
		if (clientSocket == INVALID_SOCKET)
		{
			int32 errCode = ::WSAGetLastError();//어떤 이유로?
			cout << "Accept ErrorCode : " << errCode << endl;
			return 0;
		}

		//손님 입장
		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr,  ipAddress, sizeof(ipAddress));//문자열로 바꿔주는 함수
		cout << "Client Connected! IP = " << ipAddress << endl;


		//TODO
		while (true)
		{
			//sender에서 얼마 크기의 데이터를 보내줄지 모름
			char recvBuffer[1000];

			this_thread::sleep_for(1s);
			//과연 여러개의 데이터를 보내면 어떻게 출력이 될까(크기가 다다름)
			//여기서는 받을 크기를 1000으로 했으니 1000을 한번에 받음
			//문제는 합쳐져서 가서 원래 몇바이트끼리 합쳐진건지에 대한 정보를 모름
			//그 정보도 보내줘야됨;; 즉 바운더리 개념이 없음
			//만약 Hello를 보내는데 그 크기보다 작은 크기만 RecvBuffer가 남아있다면 그 작은 크기 정도만 잘려서 보냄
			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			//받을때 실제 받는 크기를 return
			if (recvLen <= 0)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Recv ErrorCodoe : " << errCode << endl;
				return 0;
			}
			cout << "Recv Data! Data = " << recvBuffer << endl;
			cout << "Recv Data! Len = " << recvLen << endl;

			//// 다시 역으로 보냄(echo)
			//int32 resultCode = ::send(clientSocket, recvBuffer, sizeof(recvBuffer), 0);//마지막 인자는 0
			//if (resultCode == SOCKET_ERROR)
			//{
			//	int32 errCode = ::WSAGetLastError();//어떤 이유로?
			//	cout << "Send ErrorCode : " << errCode << endl;
			//	return 0;
			//}
		}

	}
	// ---------------------------

	// 윈속 종료
	::WSACleanup();
} 