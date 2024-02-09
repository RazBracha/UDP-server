#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>

#define TIME_PORT	27015

char getUserRequst();
void printMenu();
void printCityMemu();
float calculatTimeDelay(float* sendTime, float* recvTime);

void main()
{

	// Initialize Winsock (Windows Sockets).

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return;
	}

	// Client side:
	// Create a socket and connect to an internet address.

	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a client to communicate on a network, it must connect to a server.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	// Send and receive data.
	char sendBuff[2];
	int bytesRecv = 0;
	int bytesSent = 0;
	char recvBuff[255];
	float delay;
	float RTT;

	while (true)
	{
		printMenu();
		sendBuff[0] = getUserRequst();

		if (sendBuff[0] == '0')
		{
			cout << "bye bye!";
			break;
		}
		else if (sendBuff[0] == 'd')
		{
			float sendReqTime[100];
			float recvReqTime[100];

			for (int i = 0; i < 100; i++)
			{
				bytesSent = sendto(connSocket, sendBuff, 1, 0, (const sockaddr*)&server, sizeof(server));
				sendReqTime[i] = GetTickCount();
			}

			for (int i = 0; i < 100; i++)
			{
				bytesRecv = recv(connSocket, recvBuff, 255, 0);
				recvReqTime[i] = GetTickCount();
			}

			delay = calculatTimeDelay(sendReqTime, recvReqTime);
			cout << "Client To Server Delay " << delay << endl;
		}
		else if (sendBuff[0] == 'e')
		{
			float sendReqTime[100];
			float recvReqTime[100];

			for (int i = 0; i < 100; i++)
			{
				bytesSent = sendto(connSocket, sendBuff, 1, 0, (const sockaddr*)&server, sizeof(server));
				sendReqTime[i] = GetTickCount();

				bytesRecv = recv(connSocket, recvBuff, 255, 0);
				recvReqTime[i] = GetTickCount();
			}

			RTT = calculatTimeDelay(sendReqTime, recvReqTime);
			cout << "Round Trip Time: " << RTT << endl;

		}
		else {

			// Asks the server what's the currnet time.
			// The send function sends data on a connected socket.
			// The buffer to be sent and its size are needed.
			// The fourth argument is an idicator specifying the way in which the call is made (0 for default).
			// The two last arguments hold the details of the server to communicate with. 
			// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
			if (sendBuff[0] == 'l')
			{
				printCityMemu();
				cin >> sendBuff[1];
				bytesSent = sendto(connSocket, sendBuff, 2, 0, (const sockaddr*)&server, sizeof(server));
			}
			else 
				bytesSent = sendto(connSocket, sendBuff, 1, 0, (const sockaddr*)&server, sizeof(server));
			if (SOCKET_ERROR == bytesSent)
			{
				cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
				closesocket(connSocket);
				WSACleanup();
				return;
			}
			cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff[0] << "\" message.\n";

			// Gets the server's answer using simple recieve (no need to hold the server's address).
			bytesRecv = recv(connSocket, recvBuff, 255, 0);
			if (SOCKET_ERROR == bytesRecv)
			{
				cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
				closesocket(connSocket);
				WSACleanup();
				return;
			}

			recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
			cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

		}

	}
	// Closing connections and Winsock.
	cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);

	system("pause");
}

float calculatTimeDelay(float* sendTime, float* recvTime)
{
	float delay = 0;
	float avg = 0;

	for (int i = 0; i < 100; i++)
	{
		delay = recvTime[i] - sendTime[i];
		avg += delay;
	}

	avg = avg / 100;

	return avg;
}

void printMenu()
{
	cout << "Please choose your requst:" << endl;
	cout << "0. Exit" << endl;
	cout << "a. GetTime" << endl;
	cout << "b. GetTimeWithoutDate" << endl;
	cout << "c. GetTimeSinceEpoch" << endl;
	cout << "d. GetClientToServerDelayEstimation" << endl;
	cout << "e. MeasureRTT" << endl;
	cout << "f. GetTimeWithoutDateOrSeconds" << endl;
	cout << "g. GetYear" << endl;
	cout << "h. GetMonthAndDay" << endl;
	cout << "i. GetSecondsSinceBeginingOfMonth" << endl;
	cout << "j. GetWeekOfYear" << endl;
	cout << "k. GetDaylightSavings" << endl;
	cout << "l. GetTimeWithoutDateInCity" << endl;
	cout << "m. MeasureTimeLap" << endl;

}

char getUserRequst()
{
	char userRequest;
	cin >> userRequest;

	while ((userRequest < 'a' || userRequest >'n') && userRequest != '0')
	{
		cout << "Invalid input. please enter valid requst" << endl;
		printMenu();
		cin >> userRequest;
	}

	return userRequest;
}

void printCityMemu()
{
	cout << "1. Doha (Qatar)" << endl;
	cout << "2. Prague (Czech Republic)" << endl;
	cout << "3. New York (USA)" << endl;
	cout << "4. Berlin (Germany)" << endl;
	cout << "5. other" << endl;
}