#include "client_modul.h"

Client::Client()
{
	user = "";
}

Client::~Client()
{
	
}

int Client::createSocket()
{
	int port = 9000;
    int sock;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // buka socket TCP (SOCK_STREAM) dengan alamat IPv4 dan protocol IP
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0){
        close(sock);
        printf("Cannot open socket\n");
        exit(1);
    }

    // buat address server
    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        fprintf(stderr,"Host not found\n");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    // connect ke server, jika error keluar
    if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) exit(1);
    return sock;
}

void Client::send(data msg,data res)
{
	int sock = createSocket();
	bool success = false;
    int len = write(sock,msg,BUFFER_SIZE);
    if (len >= 0){
        len = read(sock,res,BUFFER_SIZE);
        if (len >= 0){
            
        }
    }
    close(sock);
}

bool Client::is_logged_in()
{
	return (user.compare("") != 0);
}

string Client::getUser()
{
	return user;
}

void Client::login()
{
	string name;
	string pass;
	data input,output;
	cout << "Name     : "; getline(cin,name);
	cout << "Password : "; getline(cin,pass);
	sprintf(input,"%d %s %s",1,name.c_str(),pass.c_str());
	send(input,output);
	if (strcmp(output,"invalid")!=0)
	{
		stringstream ss;
		ss << output;
		ss >> channel;
		cout << endl <<"Login success!" << endl << endl;
		user = name;
	}
	else
	{
		cout << endl << "Invalid username or password" << endl << endl;
	}
}

void Client::logout()
{
	data input,output;
	sprintf(input,"%d %d",0,channel);
	send(input,output);
	user.clear();
}

void Client::signup()
{
	string name;
	string pass;
	data input,output;
	cout << "Name     : "; getline(cin,name);
	cout << "Password : "; getline(cin,pass);
	sprintf(input,"%d %s %s",2,name.c_str(),pass.c_str());
	send(input,output);
	if (strcmp(output,"invalid")!=0)
	{
		stringstream ss;
		ss << output;
		ss >> channel;
		cout << endl << "Signup success!" << endl << endl;
		user = name;
	}
	else
	{
		cout << endl << "Username exist!" << endl << endl;
	}
}

void Client::createGroup(string group)
{
	data input,output;
	sprintf(input,"%d %s %s",3,user.c_str(),group.c_str());
	send(input,output);
	if (strcmp(output,"invalid")!=0)
	{
		cout << endl << "Group "<< group <<" created." << endl << endl;
	}
	else
	{
		cout << endl << group << " already exist." << endl << endl;
	}
}

void Client::joinGroup(string group)
{
	data input,output;
	sprintf(input,"%d %s %s",4,user.c_str(),group.c_str());
	send(input,output);
	if (strcmp(output,"valid")==0)
	{
		cout << endl << "Joining group "<< group <<"." << endl << endl;
	}
	else if (strcmp(output,"already")==0)
	{
		cout << endl << "You are already member of "<< group <<"." << endl << endl;
	}
	else
	{
		cout << endl << group << " doesn't exist." << endl << endl;
	}
}

void Client::leaveGroup(string group)
{
	data input,output;
	sprintf(input,"%d %s %s",5,user.c_str(),group.c_str());
	send(input,output);
	if (strcmp(output,"valid")==0)
	{
		cout << endl << "Leaving group "<< group <<"." << endl << endl;
	}
	else if (strcmp(output,"notmember")==0)
	{
		cout << endl << "You aren't member of "<< group <<"." << endl << endl;
	}
	else
	{
		cout << endl << group << " doesn't exist." << endl << endl;
	}
}