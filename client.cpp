/**
    klien: mengirimkan bilangan n ke server, menerima n + 1 dari server
    disarankan untuk membungkus address dan socket ke kelas tersendiri
*/

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
#include "client_modul.h"
using namespace std;

Client client;

void checkMsg();

int main()
{
    char opt[512];
    bool quit = false;
    thread t (checkMsg);
    t.detach();
    do {
        if (!client.is_logged_in())
        {
            cout << "$guest > ";
            cin.getline(opt,512);
            if(strcmp(opt,"login") == 0)
            {
                client.login();
            }
            else if (strcmp(opt,"signup") == 0)
            {
                client.signup();
            }
            else if (strcmp(opt,"exit") == 0)
            {
                quit = true;
            }
            else
            {
                cout << "Command invalid!" << endl;
            }
        }
        else
        {
            std::chrono::milliseconds dura( 200 );
            std::this_thread::sleep_for( dura );
            if (client.newMsg())
            {
                client.checkUnread();
            }
            cout << "$"<< client.getUser() <<" > ";
            cin.getline(opt,512);
            string query = opt;
            stringstream ss;
            if (strcmp(opt,"logout") == 0)
            {
                client.logout();
            }
            else if (query.substr(0,query.find(" ")).compare("create") == 0)
            {
                string group;
                ss << query;
                ss >> opt;
                ss >> group;
                client.createGroup(group);
            }
            else if (query.substr(0,query.find(" ")).compare("join") == 0)
            {
                string group;
                ss << query;
                ss >> opt;
                ss >> group;
                client.joinGroup(group);
            }
            else if (query.substr(0,query.find(" ")).compare("leave") == 0)
            {
                string group;
                ss << query;
                ss >> opt;
                ss >> group;
                client.leaveGroup(group);
            }
            else if (query.substr(0,query.find(" ")).compare("message") == 0)
            {
                string rcv;
                ss << query;
                ss >> opt;
                ss >> rcv;
                client.sendMsg(rcv);
            }
            else if (query.substr(0,query.find(" ")).compare("show") == 0)
            {
                string rcv;
                ss << query;
                ss >> opt;
                ss >> rcv;
                client.showMsg(rcv);
            }
            else
            {
                cout << "Command invalid!" << endl;
            }
        }
    } while (!quit);
    return 0;
}

void checkMsg()
{
    while (1)
    {
        if (client.is_logged_in())
        {
            client.checkMsg();
        }
        std::chrono::milliseconds dura( 100 );
        std::this_thread::sleep_for( dura );
    }
}
