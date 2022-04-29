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
	
	// WSAEventSelect = (WSAEventSelect 함수가 핵심이 되는)
	// 소켓과 관련된 네트워크 이벤트를 [이벤트 객체]를 통해 감지
	// 응답에 관계 없이 바로 다음 동작이 실행되는 것,나중에 콜백이 실행되는 것

	// 이벤트 객체 관련 함수들
	// 생성 : WSACreateEvent (수동 리셋 Manual-Reset + Non-Signaled 상태 시작)
	// 삭제 : WSACloseEvent
	// 신호 상태 감지 : WSAWaitForMultipleEvents
	// 구체적인 네트워크 이벤트 알아내기 : WSAEnumNetworkEvents
	//

	// 소켓 <-> 이벤트객체 연동
	// WSAEventSelect(socket, event, networkEvents);//어떤 이벤트를 관찰하고 싶은지
	// - 관심있는 네트워크 이벤트
	// FD_ACCEPT : 접속한 클라가 있음 accept
	// FD_READ : 데이터 수신 가능 recv, recvfrom
	// FD_WRITE : 데이터 송신 가능 send, sendto
	// FD_CLOSE : 상대가 접속 종료
	// FD_CONNECT : 통신을 위한 연결 절차 완료
	// FD_OOB
	//

	// 주의 사항
	// WSAEventSelect 함수를 호출하면, 해당 소켓은 자동으로 넌블로킹 모드 전환
	// accept() 함수가 리턴하는 소켓은 listensocket과 동일한 속성을 갖는다
	// - 따라서 clientSocket은 FD_READ, FD_WRITE 등을 다시 등록 필요
	// 드물게 WSAEWOULDBLOC 오류가 뜰 수 있으니 예외 처리 필요
	// 중요)
	// - 이벤트 발생 시, 적절한 소켓 함수 호출해야함
	// 아니면 다음 번에는 동일 네트워크 이벤트가 발생 X(예를 들어 FD_READ를 관찰했고 관찰하고 있는애가 
	// 완료되었다고 통제가 오면 반드시 recv를 해줘야 다음턴에 FD_READ를 또 해줄 수 있음
	//  ex) FD_READ 이벤트 떴으면 recv() 호출해야 하고, 안하면 FD_READ 두번 다시 발생 안함
	// select의 경우 무언가 하나라도 나올때 까지 기다리는애인데 WSAEventSelect의 경우에는
	// 그냥 등록만하고 생기면 이벤트 발생시킴
	//
	//
	// 1) count, event
	// 2) waiitAll : 모두 기다림? 하나만 완료 되어도 OK?
	// 3) timeout : 타임아웃
	// 4) 지금은 false
	// return : 완료된 첫번째 인덱스
	// WSAWaitForMultipleEvents 에서 받음


	// 나온 해당 이벤트가 무슨 소켓인지 모름, 왜 이벤트가 시그널이 됐는지 모르는거 아닌가..?
	// 1) socket
	// 2) eventObject : socket 과 연동된 이벤트 객체 핸들을 넘겨주면 , 이벤트 객체를  자동으로 non-signaled로 바꿔줌
	// 3) networkEvent : 네트워크 이벤트 / 오류 정보가 저장
	//WSAEnumNetworkEvents

	

	vector<WSAEVENT> wsaEvents;//각 소켓마다
	vector<Session> sessions;
	sessions.reserve(100);//충분한 크기로 잡아줌
	

	WSAEVENT listenEvent = ::WSACreateEvent();
	wsaEvents.push_back(listenEvent);
	sessions.push_back(Session{ listenSocket });//클라이언트에서 보낸 소켓은 아니지만 wsaEvents랑 맞춰주려고
	if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
		return 0;


	while (true)
	{
		int32 index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);//False라 하나라도 가능하면 나옴

		
		if (index == WSA_WAIT_FAILED)
			continue;
		index -= WSA_WAIT_EVENT_0;//왜인진 모르는데 더해져서 나와서

		//::WSAResetEvent(wsaEvents[index]); 이건 그냥 밑에서 자동으로 같이 해줌, 즉 시그널 상태에서 논시그널상태로 바꿔줌
		//FD_ACCEPT 때문인지 FD_CLOSE때문인지 무슨이유 때문에 EVENT가 발생했는지 몰라서
		WSANETWORKEVENTS networkEvents;
		if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents)==SOCKET_ERROR)//결과물이 networkEvents에 채워짐
			continue;

		// Listener 소켓 체크
		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			// Error-Check
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
				continue;

			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected" << endl;
				WSAEVENT clientEvent = ::WSACreateEvent();
				wsaEvents.push_back(clientEvent);
				sessions.push_back(Session{ clientSocket });//클라이언트에서 보낸 소켓은 아니지만 wsaEvents랑 맞춰주려고
				if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
					return 0;
			}
		}

		// Client Session 소켓 체크
		if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
		{
			// Error-Check
			if ((networkEvents.lNetworkEvents & FD_READ )&&(networkEvents.iErrorCode[FD_READ_BIT] != 0))
				continue;
			// Error-Check
			if ((networkEvents.lNetworkEvents & FD_WRITE) && (networkEvents.iErrorCode[FD_WRITE_BIT] != 0))
				continue;


			Session& s = sessions[index];

			// Read
			if (s.recvBytes == 0)
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				cout << "Recv Data = " << recvLen << endl;
				if (recvLen == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)//WSAEWOULDBLOCK인경우는 심각한경우가 아님
				{
					// TODO : Remove Session
					continue;
				}

				s.recvBytes = recvLen;
				cout << "Recv Data = " << recvLen << endl;
				cout << "Recv Data = " << s.recvBuffer << endl;
			}

			// Writie
			if (s.recvBytes > s.sendBytes)
			{
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					// TODO : Remove Session
					continue;
				}
				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.sendBytes = 0;
					s.recvBytes = 0;
				}

				cout << "Send Data = " << sendLen << endl;
			}
		}

		// FD_CLOSE 처리
		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			//TODO : Remove Socket
		}
	}
	// 윈속 종료
	::WSACleanup();
} 


/*블로킹 논블로킹 동기화 비동기화
* 
*/