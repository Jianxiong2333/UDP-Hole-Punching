#include <stdio.h>
#include <winsock2.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#define BUF_SIZE 100

/* 记录对端IP端口结构体 */
typedef struct {
	struct in_addr ip;
	int port;
}clientInfo;

int main(void) {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//对端信息结构体
	clientInfo info[2];
	//创建套接字
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	//绑定套接字
	sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
	servAddr.sin_family = PF_INET;  //使用IPv4地址
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
	servAddr.sin_port = htons(6623);  //端口
	bind(sock, (SOCKADDR*)& servAddr, sizeof(SOCKADDR));
	//接收客户端请求
	//SOCKADDR clntAddr;  //客户端地址信息
	sockaddr_in clntAddr;//客户端地址信息
	int nSize = sizeof(SOCKADDR);
	char buffer[BUF_SIZE];  //缓冲区

	while (1) {
		memset(&info, 0, sizeof(clientInfo) * 2);

		//接收A客户端
		recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*) & clntAddr, &nSize);
		memcpy(&info[0].ip, &clntAddr.sin_addr, sizeof(struct in_addr)); //复制IP
		info[0].port = clntAddr.sin_port;//复制端口
		printf("A 客户端 IP:%s \t端口:%d 已创建链接！\n", inet_ntoa(info[0].ip), ntohs(info[0].port));

		//此处A服务端进行链接以后，等待进行B服务端链接

		//接收B客户端
		recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*) & clntAddr, &nSize);
		memcpy(&info[1].ip, &clntAddr.sin_addr, sizeof(struct in_addr)); //复制IP
		info[1].port = clntAddr.sin_port;//复制端口
		printf("B 客户端 IP:%s \t端口:%d 已创建链接！\n", inet_ntoa(info[1].ip), ntohs(info[1].port));

		printf("A服务端传输就绪，准备发送数据\n");
		clntAddr.sin_addr = info[0].ip;
		clntAddr.sin_port = info[0].port;
		sendto(sock, (const char*)&info[1], sizeof(clientInfo), 0, (struct sockaddr*) & clntAddr, nSize);
		printf("A已发送\n");

		printf("B服务端传输就绪，准备发送数据\n");
		clntAddr.sin_addr = info[1].ip;
		clntAddr.sin_port = info[1].port;
		sendto(sock, (const char*)& info[0], sizeof(clientInfo), 0, (struct sockaddr*) & clntAddr, nSize);
		printf("B已发送\n");
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}