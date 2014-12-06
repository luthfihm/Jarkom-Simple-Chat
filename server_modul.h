#ifndef SERVER_H
#define SERVER_H

#define BUFFER_SIZE 512

#include <cstdio>
#include <sys/types.h>   // tipe data penting untuk sys/socket.h dan netinet/in.h
#include <netinet/in.h>  // fungsi dan struct internet address
#include <sys/socket.h>  // fungsi dan struct socket API
#include <netdb.h>       // lookup domain/DNS hostname
#include <unistd.h>
#include <cstdlib>
#include <errno.h>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <ios>
#include <queue>
#include <chrono>
#include <thread>
#include "message.h"

using namespace std;

typedef char data[BUFFER_SIZE];

class Server
{
private:
	string active_user[100];
	queue<Message> active_chat[100];
public:
	Server();
	~Server();
	void start();
	void proses(int client_sock);
	bool login (string user,string pass);
	bool signup (string user,string pass);
	bool checkUser(string user);
	bool createGroup (string user,string group);
	bool joinGroup (string user,string group);
	bool leaveGroup (string user,string group);
	bool checkGroup(string group);
	bool isMember(string user,string group);
	void addMsg(Message m);
	void loadSavedMsg(string user);
	void log(string m);
	int getChannel(string user);
	void setChannel(string user);
};

#endif