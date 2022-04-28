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
const int32 BUFSIZE = 1000;
struct Session//각각의 클라이언트를 담당
{
	SOCKET socket;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
	int32 sendBytes = 0;
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
	
	// Select 모델 = (select 함수가 핵심이 되는)
	// 클라에서는 select 모델 그래도 사용
	// 소켓 함수 호출이 성공할 시점을 미리 알 수 있음
	// 문제 상황) 
	// 수신버퍼에 데이터가 없는데, read 한다거나!
	// 수신버퍼가 꽉 찼는데, write 한다거나!
	// - 블로킹 소켓(에 적용할 때) : 조건이 만족되지 않아서 블로킹되는 상황 예방
	// - 논블로킹 소켓 : 조건이 만족되지 않아서 불필요하게 반복 체크하는 상황을 예방
	
	// socket set(소켓 묶음)
	// 1) 읽기[ ] 쓰기[ ] 예외[OOB] 관찰 대상 등록
	// Out of Band는 Send() 마지막 인자 MSG_OOB로 보내는 특별한 데이터
	// 받는 쪽에서도 recv OOB 세팅을 해야 읽을 수 있음, 걍 없다고 생각
	// 2) select(readSet, writeSet, exceptSet); 꼭 다 채울 필요 없음 -> 관찰 시작
	// 3) 적어도 하나의 소켓이 준비되면 리턴 -> 낙오자는 알아서 제거됨
	// 예로 읽기 셋에 1, 2, 3소켓이 있는데 2번 소켓에 데이터가 들어와서 수신버퍼가 찼다고 하면
	// 바로 select가 반환해서 준비된 개수를 반환 (즉 2번만 준비 됐으니 1개 반환)
	// 그리고 set 안에 다 삭제됨, 이제 2번만 확인
	// 4) 남은 소켓 체크해서 진행

	// fd_set set;
	// FD_SERO : 비운다
	// ex) FD_ZERO(set)
	// FD)SET : 소켓 s를 넣는다
	// ex) FD_SET(s, &set);
	// FD_CLR : 소켓 s를 제거
	// ex) FD_CLR(s, &set);
	// FD_ISSET : 소켓 s가 set에 들어있으면 0이 아닌 값을 리턴한다
	
	vector<Session> sessions;
	sessions.reserve(100);//충분한 크기로 잡아줌
	
	fd_set reads;
	fd_set writes;

	while (true)
	{
		//소켓 셋 초기화
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		// ListenSocket 등록
		FD_SET(listenSocket, &reads);//accept할게 있는지 확인

		// 소켓 등록
		for (Session& s : sessions)
		{
			
			if (s.recvBytes <= s.sendBytes)
				FD_SET(s.socket, &reads);
			else
				FD_SET(s.socket, &writes);
		}
		// [옵션] 마지막 timeout 인자 설정 가능, 대기 시간 설정 가능
		//인자가 없으면 하나라도 준비 될때 까지 기다림
		//timeval timeout;
		//timeout.tv_sec;
		//timeout.tv_usec;
		int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);//처음껀 일단 0
		if (retVal == SOCKET_ERROR)
			break;

		//Listner 소켓 체크
		if (FD_ISSET(listenSocket, &reads))//클라이언트에서 요청이 있음
		{
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected" << endl;
				sessions.push_back(Session{ clientSocket });//생성자 문법..?

			}

		}

		// 나머지 소켓 체크
		for (Session& s : sessions)
		{
			//Read
			if (FD_ISSET(s.socket, &reads))
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen <= 0)
					// TODO : sessions 제거
					
					continue;
				
				s.recvBytes = recvLen;
			}


			//Write
			if (FD_ISSET(s.socket, &writes))
			{
				//블로킹 모드 ->  모든 데이터 다 보냄
				//논 블로킹 모드 -> 일부만 보낼 수가 있음 (상대방 수신 버퍼 상황에 따라)
				//10바이트를 받아서 이전에 5바이트만 보낸 상황이라면! 나머지 5바이트를 보내기 위해서
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				//근데 웬만하면 보내라는사이즈로 보냄
				if (sendLen == SOCKET_ERROR)
				{
					// TODO : session 제거
					continue;
					
				}

				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}
			}
		}
	}
	// 윈속 종료
	::WSACleanup();
} 