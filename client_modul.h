#ifndef CLIENT_H
#define CLIENT_H

#include <cstdio>
#include <sys/types.h>   // tipe data penting untuk sys/socket.h dan netinet/in.h
#include <netinet/in.h>  // fungsi dan struct internet address
#include <sys/socket.h>  // fungsi dan struct socket API
#include <netdb.h>       // lookup domain/DNS hostname
#include <unistd.h>
#include <cstdlib>
#include <errno.h>
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <ios>
#include "message.h"

#define BUFFER_SIZE 512

using namespace std;

typedef char data[BUFFER_SIZE];

class Client
{
private:
	string user;
	int channel;
public:
	Client();
	~Client();

	int createSocket();
	void send(data msg,data res);
	string getTime();
	bool is_logged_in();
	string getUser();
	void login();
	void logout();
	void signup();
	void createGroup(string group);
	void joinGroup(string group);
	void sendMsg(string rcv);
	void checkMsg();
	void saveMsg(Message m);
	void addUnread(string from);
	void checkUnread();
	void markAsRead(string from);
	void showMsg(string from);
	void leaveGroup(string group);
};
#endif