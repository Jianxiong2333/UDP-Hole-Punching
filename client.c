#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll
#define BUF_SIZE 512

/*对端IP和端口结构体*/
typedef struct {
	struct in_addr ip;
	int port;
}clientInfo;

int main() {
	//对端数据结构体
	clientInfo info;

	//初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//创建套接字
	SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
	//服务器地址信息
	sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	servAddr.sin_port = htons(6623);

	//不断获取用户输入并发送给服务器，然后接受服务器数据
	sockaddr fromAddr;
	int addrLen = sizeof(fromAddr);

	//刷新结构体
	memset(&info, 0, sizeof(info));
	char severText[BUF_SIZE] = "服务器你好！";
	char buffer[BUF_SIZE] = "你好，对方客户端！";
	char other[BUF_SIZE] = { 0 };//对端数据
	//getchar();
	sendto(sock, severText, strlen(severText), 0, (struct sockaddr*) & servAddr, sizeof(servAddr));

	printf("已向服务器 %s 发包\n", inet_ntoa(servAddr.sin_addr));
	recvfrom(sock, (char*)& info, sizeof(clientInfo), 0, &fromAddr, &addrLen);
	printf("收到响应 IP:%s \t端口:%d \n", inet_ntoa(info.ip), ntohs(info.port));

	/*将服务器方转为对方端口*/
	servAddr.sin_addr = info.ip;
	servAddr.sin_port = info.port;

	//预先发送一次数据，保障对方消息顺利通过我方NAT设备
	sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*) & servAddr, sizeof(struct sockaddr_in));

	while (1)
	{
		char other[BUF_SIZE] = { 0 };//对端数据
		sockaddr_in otherInfo;//客户端地址信息
		int otherInfoSize = sizeof(SOCKADDR);
		struct in_addr ip;

		//向对端发送数据
		int textByte = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*) & servAddr, sizeof(struct sockaddr_in));
		printf("发送字节 %d \n", textByte);

		//接收对端数据
		int strLen = recvfrom(sock, other, sizeof(other), 0, (struct sockaddr*) & otherInfo, &otherInfoSize);
		memcpy(&ip, &otherInfo.sin_addr, sizeof(struct in_addr)); //复制IP
		printf("收到 IP:%s \t端口:%d 消息：%s\n", inet_ntoa(ip), ntohs(otherInfo.sin_port), other);

		/*将服务器方转为对方端口,这一步主要是防止中途端口变动*/
		servAddr.sin_addr.s_addr = inet_addr(inet_ntoa(ip));
		servAddr.sin_port = htons(ntohs(otherInfo.sin_port));

		Sleep(5000);
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}
