#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <ctime>
#include <chrono>
#include<windows.h>

#define TIME_PORT	27015

#define Qatar 2
#define CzechRepublic 1
#define USA -5
#define Germany 1



void answerClintRequest(char* recvBuff, char* sendBuff);
void getTime(char* sendBuff);
void GetTimeWithoutDate(char* sendBuff);
void GetTimeSinceEpoch(char* sendBuff);
void GetClientToServerDelayEstimation(char* sendBuff);
void MeasureRTT(char* sendBuff);
void GetTimeWithoutDateOrSeconds(char* sendBuff);
void GetYear(char* sendBuff);
void GetMonthAndDay(char* sendBuff);
void GetSecondsSinceBeginingOfMonth(char* sendBuff);
void GetWeekOfYear(char* sendBuff);
void GetDaylightSavings(char* sendBuff);
void GetTimeWithoutDateInCity(char* sendBuff, char city); 
void MeasureTimeLap(char* sendBuff);
tm* returnTimeLocal();
int getCityGTM(int city);
int summerOrWinter();


void main()
{
	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData;

	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}

	// Server side:
	// Create and bind a socket to an internet address.

	// After initialization, a SOCKET object is ready to be instantiated.

	// Create a SOCKET object called m_socket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							datagram sockets (SOCK_DGRAM), 
	//							and the UDP/IP protocol (IPPROTO_UDP).
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The "if" statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a server to communicate on a network, it must first bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
	// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigdned long (4 bytes) data type).
	// INADDR_ANY means to listen on all interfaces.
	// inet_addr (Internet address) is used to convert a string (char *) into unsigned int.
	// inet_ntoa (Internet address) is the reverse function (converts unsigned int to char *)
	// The IP address 127.0.0.1 is the host itself, it's actually a loop-back.
	serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order (which is big-endian).
	serverService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	// Waits for incoming requests from clients.

	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];
	// Get client's requests and answer them.
	// The recvfrom function receives a datagram and stores the source address.
	// The buffer for data to be received and its available size are 
	// returned by recvfrom. The fourth argument is an idicator 
	// specifying the way in which the call is made (0 for default).
	// The two last arguments are optional and will hold the details of the client for further communication. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
	cout << "Time Server: Wait for clients' requests.\n";

	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff[0] << "\" message.\n";

		// Answer client's request by the current time.
		
		// Get the current time.
		//time_t timer;
		//time(&timer);
		// Parse the current time to printable string.
		//strcpy(sendBuff, ctime(&timer));
		//sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string
		answerClintRequest(recvBuff, sendBuff);
		// Sends the answer to the client, using the client address gathered
		// by recvfrom. 
		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}

void answerClintRequest(char* recvBuff, char* sendBuff)
{
	char request = recvBuff[0];
	char city = recvBuff[1];

	switch (request)
	{
	case 'a':
		getTime(sendBuff);
		break;
	case 'b':
		GetTimeWithoutDate(sendBuff);
		break;
	case 'c':
		GetTimeSinceEpoch(sendBuff);
		break;
	case 'd':
		GetClientToServerDelayEstimation(sendBuff);
		break;
	case 'e':
		MeasureRTT(sendBuff);
		break;
	case 'f':
		GetTimeWithoutDateOrSeconds(sendBuff);
		break;
	case 'g':
		GetYear(sendBuff);
		break;
	case 'h':
		GetMonthAndDay(sendBuff);
		break;
	case 'i':
		GetSecondsSinceBeginingOfMonth(sendBuff);
		break;
	case 'j':
		GetWeekOfYear(sendBuff);
		break;
	case 'k':
		GetDaylightSavings(sendBuff);
		break;
	case 'l':
		GetTimeWithoutDateInCity(sendBuff, city);
		break;
	case 'm':
		MeasureTimeLap(sendBuff);
		break;
	}
}

void getTime(char* sendBuff)
{//1-a
	time_t timer;
	time(&timer);
	strcpy(sendBuff, ctime(&timer));
}

void GetTimeWithoutDate(char* sendBuff)
{//2-b
	strftime(sendBuff, 255, "Time Without Date: %H:%M:%S ", returnTimeLocal());
}

void GetTimeSinceEpoch(char* sendBuff)
{//3-c
	uint64_t timeSinceEpochseconds = std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();

	sprintf(sendBuff, "Time Since Epoch: %d", timeSinceEpochseconds);
}

void GetClientToServerDelayEstimation(char* sendBuff)
{//4-d
	getTime(sendBuff);
}

void MeasureRTT(char* sendBuff)
{//5-e
	getTime(sendBuff);
}

void GetTimeWithoutDateOrSeconds(char* sendBuff)
{//6-f
	strftime(sendBuff, 255, "Time Without Date Or Seconds %H:%M ", returnTimeLocal());
}

void GetYear(char* sendBuff)
{//7-g
	strftime(sendBuff, 255, "Current Year: %Y ", returnTimeLocal());
}

void GetMonthAndDay(char* sendBuff)
{//8-h
	strftime(sendBuff, 255, "Current Month And Day: %m , %d ", returnTimeLocal());
}

void GetSecondsSinceBeginingOfMonth(char* sendBuff)
{//9-i
	tm* time_local = returnTimeLocal();

	int daysInSec = time_local->tm_mday * 24 * 60 * 60;
	int hoursInSec = time_local->tm_hour * 60 * 60;
	int minitsInSec = time_local->tm_min * 60;
	int seconds = time_local->tm_sec;
	int sumOfSeconds = daysInSec + hoursInSec + minitsInSec + seconds;

	sprintf(sendBuff, "Seconds Since Begining Of Month : %d", sumOfSeconds);
}

void GetWeekOfYear(char* sendBuff)
{//10-j
	strftime(sendBuff, 255, "Week Of Year: %U", returnTimeLocal());
}

void GetDaylightSavings(char* sendBuff)
{//11-k
	sprintf(sendBuff, "1-For Daylight Saving, 0-For Night Saving. Answer: %d ", summerOrWinter());
}

void GetTimeWithoutDateInCity(char* sendBuff, char city)
{//12-l
	int daylightSavings = summerOrWinter();
	time_t local_time;
	time(&local_time);
	tm* tm_city = gmtime(&local_time);
	int cityGTM = getCityGTM(city);

	int hour = (tm_city->tm_hour + cityGTM + daylightSavings) % 24;
	int minute = tm_city->tm_min;

	if (cityGTM == 0)
	{
		sprintf(sendBuff, "The city you chose not on the list. UTC time: %d:%d", hour, minute);
	}
	else
		sprintf(sendBuff, "Time Without Date In City: %d:%d", hour, minute);
}

void MeasureTimeLap(char* sendBuff)
{//13-m
	
	static time_t timeLap = 0;
	if (timeLap == 0)
	{
		timeLap = time(nullptr);
		strcpy(sendBuff, "Start measure");
	}
	else
	{
		timeLap = time(nullptr) - timeLap;
		if (timeLap > 180)
		{
			sprintf(sendBuff, "too long");
		}
		else {
			
			sprintf(sendBuff, "The timeLap is: %d", timeLap);
			timeLap = 0; //reset
		}
	}
}

tm* returnTimeLocal()
{
	time_t local_time;
	time(&local_time);
	tm* time_local = localtime(&local_time);

	return time_local;
}

int getCityGTM(int city)
{
	int cityGTM;

	switch (city)
	{
	case '1':
		cityGTM = Qatar;
		break;
	case '2':
		cityGTM = CzechRepublic;
		break;
	case '3':
		cityGTM = USA;
		break;
	case '4':
		cityGTM = Germany;
		break;
	case '5':
		cityGTM = 0;
		break;
	}

	return cityGTM;
}

int summerOrWinter()
{
	tm* time_local = returnTimeLocal();
	return time_local->tm_isdst; //if summer it will be 1
}