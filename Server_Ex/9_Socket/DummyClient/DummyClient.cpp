#include "pch.h"
#include <iostream>

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

	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM,0);
	//socket(전화기) 오브젝트 만듬, af = 주소체계(IPv4, IPv6), Type은 TCP로 통신할지 UDP로 통신할지
	//즉 여기서는 IPv4, TCP사용
	//Socket은 그냥 번호인데 운영체제한테 이 소켓에 보내고 싶다고 말하면 알아서 보내줌
	if (clientSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();//어떤 이유로?
		cout << "Socket ErrorCode : " << errCode << endl;
		return 0;
	}
	
	//이제 식당 번호를 알아야됨
	// 연결할 목적지는? (IP주소 * Port) -> XX아파트 YY호
	SOCKADDR_IN serverAddr; //IPv4일때 사용
	::memset(&serverAddr, 0, sizeof(serverAddr));//0으로 초기화
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");//1바이트씩 끊어서 표현한걸 변환함
	//위에껀 구식
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	//127.0.0.1 주소는 스스로에게 주소를 다시 보내는 느낌, 즉 우리컴퓨터, 
	// 만약 실제 온라인 게임이면 실제 온라인 서버 주소를 넣어줌
	serverAddr.sin_port = ::htons(7777);//임의의 번호(포트번호)
	//일부 포트 번호가 예약 돼 있음 80 :: HTTP
	//Host to network short(htons) 이걸 왜 불러줌?
	// Little-Endian vs Big-Endian 때문, 네트워크는 빅엔디안이라 이걸 바꿔줌
	//즉 호스트에서 네트워크 방식으로 바꿔줌

	//이제 전화를 검
	//어떤 핸드폰으로, 대상
	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();//어떤 이유로?
		cout << "Connect ErrorCode : " << errCode << endl;
		return 0;
	}

	// -----------------
	// 연결 성공! 이제부터 데이터 송수신 가능!

	cout << "Connected To Server!" << endl;

	while (true)
	{
		//send와 recv함수는 블록킹 함수, send
		char sendBuffer[100] = "Hello Wolrd";
		for (int32 i = 0; i < 10; i++)
		{
			int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
			//마지막 인자는 0
			if (resultCode == SOCKET_ERROR)
			{
				int32 errCode = ::WSAGetLastError();//어떤 이유로?
				cout << "Send ErrorCode : " << errCode << endl;
				return 0;
			}
		}
		
		cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;


		//char recvBuffer[1000];

		//int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		////받을때 실제 받는 크기를 return
		//if (recvLen <= 0)
		//{
		//	int32 errCode = ::WSAGetLastError();
		//	cout << "Recv ErrorCodoe : " << errCode << endl;
		//	return 0;
		//}
		//cout << "Recv Data! Data = " << recvBuffer << endl;
		//cout << "Recv Data! Len = " << recvLen << endl;
		this_thread::sleep_for(1s);
	}




	// 소켓 리소스 반환
	::closesocket(clientSocket);

	// 윈속 종료
	::WSACleanup();
}

//상대방이 받지도 않았는데 send했을때 왜 받았다고 뜨는지!
/*OS레벨에 SendBuffer, RecvBuffer가 별도로 존재하는데(커널버퍼) 유저 입장에선 그냥 SendBuffer에 보내면 그게 
* Send success, 즉 OS 입장에선 못보냄..
* SendBuffer에 꽉차있을때나 send가 block
* 서버와 클라이언트 사이에 send, recieve를 사용하면 이런 문제 발생;; 그럼?
* 네트워크 코드때문에 클라이언트가 먹통이됨. Blocking 방식의 send, recieve 사용 X
* 
*/

/*TCP vs UDP
* TCP는 안전, 전화 연결 방식
* - 연결현 서비스 : 상대방이 전화를 받아야지만 연결 가능,연결을 위해 할당되는 논리적인 경로가 있음
* - 전송 순서가 보장됨, 즉 여보세요 안녕하세요 이리 말하면 말한 순서대로 전달
* - 신뢰성 굿! 분실되면 책임지고 다시 전송
* - 흐름 혼잡 제어 : 상대방이 물건을 주고 받을 상황이 아니라면 기다려줌, 즉 속도 조절  
* - 고려할 것이 많으니 속도 bad
* - 데이터 경계 없음, 즉 Hello, Wolrd 이리 보내면 Hello Wo, lrd 이리 받을 수 있음
* UDP는 위험한데 빠름, 이메일 전송 방식
* - 비연결형 서비스 : 연결 개념이 없음
* - 전송 순서 보장되지 않음
* - 경계의 개념이 있음
* - 신뢰성 Bad, 분실에 대한 책임 없음
* - 상대방 입장 고려 안하고 막보냄
* - 속도 good
* - 데이터 경계 있음, Hello, World 보내면 World, Hello 이리 받을 수 있음
* 
*/