#pragma comment(lib, "ws2_32")
#include<iostream>
#include<iomanip>
#include <string>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <WinSock2.h>	//	library for Windows socket
#include "SerialClass.h" // library for Arduino(using Serial.cpp/SerialClass.h)
#include <thread>	// Thread for image proccessing
using namespace std;
using std::thread;

#define port 2204	// port num 2204

const double pi = 3.1415926535897932384626433832795028841971693993751;

double completed[10], cost = 0;
const int user = 5;   // target point number
int* course; // path array global variable
int coursenum = 0;   // array location of course

// Variables for Socket
// To connect scoket connection by using Wi-Fi(port 2204)
WSADATA     wsaData;
SOCKET      hServSock, hClntSock;
SOCKADDR_IN servAddr, clntAddr;
int szClntAddr;

string snddata;   // string to send Client

struct TargetPt {   // struct to send (user point number, Latitude, Longitude) to Client(Pixhawk-Pi)
	int usernum;    // user number requesting while 3 min(target point number)
	double x;   // x = Latitude, y = Longitude
	double y;
};

// prototype to calculate distance between each point
double toRad(double degree);      // degree to radian
double calculateDistance(double lat1, double long1, double lat2, double long2);   // to calculate GPS data

// prototype about TSP Algorithm
void takeInput(int points, double cordinates[user][user]);
double least(int c, int points, double cordinates[user][user]);
void mincost(int city, int points, double cordinates[user][user], int* course);

// prototype about socket
string makemsg(string snddata, struct TargetPt targetdata[user]);
void ErrorHandling(const char* message);

// prototype for thread
void socket_serial_thread_outdoor();	
void socket_serial_thread_indoor();

int main(int argc, char* argv[])
{
	thread image_thread;

	cout << fixed;
	cout.precision(6);
	
	TargetPt targetdata[user] = { {0, 35.132833, 129.106215 }, {1, 35.133118, 129.105832 }, {2, 35.133139, 129.106001 },
{3, 35.133150, 129.106168 }, {4, 35.133168, 129.106331 } };


	// base point and allocated target point(Drone zone) GPS point  

	int i, j;
	for (i = 0; i < user; i++) {
		cout << "Target[" << i+1 << "] : (" << targetdata[i].x << ", " << targetdata[i].y << ")\n";
	}

	double dist[user][user];	// To save distance calculating result from Android Client

	for (i = 0; i < user; i++) {      // make 2d array by using calculateDistance
		for (j = 0; j < user; j++) {
			if (i == j) dist[i][j] = 0;
			else {
				dist[i][j] = calculateDistance(targetdata[i].x, targetdata[i].y,
					targetdata[j].x, targetdata[j].y);
			}
		}
	}

	course = new int[user + 1];      // course dynamic allocation

	// TSP algorithm start
	takeInput(user, dist);

	cout << "\n\nThe Path is:\n";
	mincost(0, user, dist, course); //passing 0 because starting vertex

	cout << "\n\nMinimum cost is " << cost << "m\n";

	snddata = makemsg(snddata, targetdata);

	cout << "\nMessage : " << snddata << "\n";      // test

	thread t1(socket_serial_thread_outdoor);		// thread 1 for outdoor

	t1.join();	// wait until thread is finished

	//thread t2(socket_serial_thread_indoor);		// thread 2 for indoor

	//t2.join();	// wait until thread is finished
	
	return 0;
}
double toRad(double degree) {
	return degree / 180 * pi;
}

// Indoor virture location(x, y) lat = Latitude(x), long = longitude(y)
double calculateDistance(double lat1, double long1, double lat2, double long2) {
	double dist;
	dist = sin(toRad(lat1)) * sin(toRad(lat2)) + cos(toRad(lat1)) * cos(toRad(lat2)) * cos(toRad(long2 - long1));
	dist = acos(dist);
	//   dist = (6371 * pi * dist) / 180;
	//   got dist in radian, no need to change back to degree and convert to rad again.
	dist = 6371000 * dist;
	return dist;
}

void takeInput(int points, double cordinates[user][user])      // points : service requesting user number, cordinates : TSP 2dimension array by using point
{
	int i, j;
	int n = points;

	cout << "\nthe number of points : " << points << "\n";

	cout << "\nTSP Cost Matrix\n";
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			cout.setf(ios::right);
			cout << setw(8) << cordinates[i][j] << "m\t";
		}
		cout << "\n";

		completed[i] = 0;
	}

}

double least(int c, int points, double cordinates[user][user])
{
	int i, nc = 999;
	int min = 999;
	double kmin;
	int n = points;

	for (i = 0; i < n; i++)
	{
		if ((cordinates[c][i] != 0) && (completed[i] == 0))
			if (cordinates[c][i] + cordinates[i][c] < min)
			{
				min = cordinates[i][c] + cordinates[c][i];
				kmin = cordinates[c][i];
				nc = i;
			}
	}

	if (min != 999)
		cost += kmin;

	return nc;
}

void mincost(int city, int points, double cordinates[user][user], int* course)
{
	int i, ncity;

	completed[city] = 1;

	cout << city + 1 << "--->";

	course[coursenum] = city;
	coursenum++;

	ncity = least(city, points, cordinates);

	if (ncity == 999)
	{
		ncity = 0;
		cout << ncity + 1;
		cost += cordinates[city][ncity];

		return;
	}

	mincost(ncity, points, cordinates, course);
}

string makemsg(string snddata, struct TargetPt targetdata[user]) {
	int i, j;

	for (i = 0; i < user; i++) {
		for (j = 0; j < user; j++) {
			if (course[i] == (targetdata + j)->usernum) {
				snddata.append(to_string((targetdata + j)->x));
				snddata.append("/");
				snddata.append(to_string((targetdata + j)->y));
				snddata.append("/");
				break;
			}
		}
	}

	snddata.append(to_string((targetdata + 0)->x));
	snddata.append("/");
	snddata.append(to_string((targetdata + 0)->y));
	snddata.append("/");

	return snddata;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void socket_serial_thread_outdoor() {

	/* To connect Ground pannel by USB Serial port
	printf("\n\nWaiting Pannel\n\n");
	Serial* SP = new Serial("\\\\.\\COM10");
	if (SP->IsConnected())
		cout << "Visiting Pannel connected\n\n";
	else cout << "Connect Failed\n\n";*/


	const char* msg = snddata.c_str();      // convert snddata to char type message
	char point[BUFSIZ] = "\0";		// message(visiting point or return back to base point(arrive)) from Client(Pixhawk-Pi)

	// Windows Socket initialization, before using Socket it should be initialized
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// make TCP Socket
	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error!");

	// information about Socket
	memset(&servAddr, 0, sizeof(servAddr));

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);  // using current PC IP address
	servAddr.sin_port = htons(port);        // port number 2204

	// bind
	if (::bind(hServSock, (SOCKADDR*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error!");

	// wait client
	if (listen(hServSock, 5) == SOCKET_ERROR)    // wating for connection during 5sec
		ErrorHandling("listen() error!");

	cout << "\n\nWaiting Client...\n";

	// Connection accept
	szClntAddr = sizeof(clntAddr);
	hClntSock = accept(hServSock, (SOCKADDR*)& clntAddr, &szClntAddr);   // communication with Client

	if (hClntSock == INVALID_SOCKET)
		ErrorHandling("accept() error!");

	cout << "\nConecct Successfully!!\n";

	send(hClntSock, msg, BUFSIZ, 0);

	cout << "Send Message : " << msg << "\n\n";

	
	//get visiting target number information from Client
	cout << "Message Receives ...\n";
	while (1) {

		recv(hClntSock, point, BUFSIZ, 0);

		if (strcmp(point, "arrive") == 0) {		// finish Socket when Server get arrive message
			printf("Close Clinet Connection..\n");
			break;
		}
		else cout << "Message from Drone : " << point << "\n";	// about Arduino
		// send current Drone visiting point number to arduino
		//SP->WriteData(point, 1);
	}
	

	cout << "\nDrone Mission Complete\n";
	closesocket(hClntSock);
	closesocket(hServSock);
	WSACleanup();
	// end of Socket connection
	cout << "\nDisconnect!! Bye~!\n";
}

void socket_serial_thread_indoor() {

	const char* msg = snddata.c_str();      // convert snddata to char type message
	char point[BUFSIZ] = "\0";		// message(visiting point or return back to base point(arrive)) from Client(Pixhawk-Pi)

	// Windows Socket initialization, before using Socket it should be initialized
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// make TCP Socket
	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error!");

	// information about Socket
	memset(&servAddr, 0, sizeof(servAddr));

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);  // using current PC IP address
	servAddr.sin_port = htons(port);        // port number 2204

	// bind
	if (::bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error!");

	// wait client
	if (listen(hServSock, 5) == SOCKET_ERROR)    // wating for connection during 5sec
		ErrorHandling("listen() error!");

	cout << "\n\nWaiting Client...\n";

	// Connection accept
	szClntAddr = sizeof(clntAddr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);   // communication with Client

	if (hClntSock == INVALID_SOCKET)
		ErrorHandling("accept() error!");

	cout << "\nConecct Successfully!!\n";

	send(hClntSock, msg, BUFSIZ, 0);

	cout << "Send Message : " << msg << "\n\n";
	closesocket(hClntSock);
	closesocket(hServSock);
	WSACleanup();
	cout << "Disconnect\n";
}
