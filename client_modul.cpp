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

string Client::getTime()
{
	time_t now = time(0);
   	struct tm * timeinfo;
   	stringstream ss;
   	char dt[30];

   	time (&now);
  	timeinfo = localtime (&now);

   	strftime (dt,30,"[%F %T]",timeinfo);
   	ss << dt;
   	return ss.str();
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

void Client::sendMsg(string rcv)
{
	data input,output;
	sprintf(input,"%d %s %s",6,user.c_str(),rcv.c_str());
	send(input,output);
	if (strcmp(output,"valid")==0)
	{
		Message m;
		cout << "Message :" << endl;
		getline(cin,m.content);
		m.from = user;
		m.to = rcv;
		m.time = getTime();
		bzero(input,512);
		bzero(output,512);
		sprintf(input,"%d\n%s\n%s\n%s\n%s",7,m.from.c_str(),m.to.c_str(),m.time.c_str(),m.content.c_str());
		send(input,output);
		if (strcmp(output,"sent")==0)
		{
			cout << endl << "Message sent." << endl << endl;
			char filename[50];
			sprintf(filename,"data_client/%s_%s.txt",user.c_str(),m.to.c_str());
			ofstream fo (filename,ios_base::app | ios_base::out);
			fo << m.time << " " << user << " : " << m.content << endl;
			fo.close();
		}
	}
	else if (strcmp(output,"notmember")==0)
	{
		cout << endl << "You aren't member of "<< rcv <<"." << endl << endl;
	}
	else
	{
		cout << endl << rcv <<" doesn't exist." << endl << endl;
	}
}

void Client::checkMsg()
{
	data input,output;
	sprintf(input,"%d %s",8,user.c_str());
	send(input,output);
	if (strcmp(output,"empty")!=0)
	{
		do {
			stringstream ss;
			ss.str(output);
			Message m;
			getline(ss,m.from);
			getline(ss,m.to);
			getline(ss,m.time);
			getline(ss,m.content);

			saveMsg(m);

			bzero(input,512);
			bzero(output,512);
			sprintf(input,"%d %s",8,user.c_str());
			send(input,output);
		} while (strcmp(output,"empty")!=0);
	}

	checkUnread();

}

void Client::saveMsg(Message m)
{
	if (m.to.compare(user) == 0)
	{
		char filename[50];
		sprintf(filename,"data_client/%s_%s.txt",user.c_str(),m.from.c_str());
		bool exist = false;
		ifstream fs (filename);
		stringstream ss;
		ss << "";
		if (fs.is_open())
		{
			while (!fs.eof())
			{
				string tmp;
				getline(fs,tmp);
				if (!tmp.empty())
				{
					ss << tmp << endl;
				}
				if (tmp.compare("----- New Message(s) -----") == 0)
				{
					exist = true;
				}
			}
			fs.close();
		}
		ofstream fo (filename);
		fo << ss.str();
		if (!exist)
		{
			fo << "----- New Message(s) -----" << endl;
		}
		fo << m.time << " " << m.from << " : " << m.content << endl;
		fo.close();
		addUnread(m.from);
	}
	else
	{
		char filename[50];
		sprintf(filename,"data_client/%s_%s.txt",user.c_str(),m.to.c_str());
		bool exist = false;
		ifstream fs (filename);
		stringstream ss;
		ss << "";
		if (fs.is_open())
		{
			while (!fs.eof())
			{
				string tmp;
				getline(fs,tmp);
				if (!tmp.empty())
				{
					ss << tmp << endl;
				}
				if (tmp.compare("----- New Message(s) -----") == 0)
				{
					exist = true;
				}
			}
			fs.close();
		}
		ofstream fo (filename);
		fo << ss.str();
		if (!exist)
		{
			fo << "----- New Message(s) -----" << endl;
		}
		fo << m.time << " " << m.from << " : " << m.content << endl;
		fo.close();
		addUnread(m.to);
	}
}

void Client::showMsg(string from)
{
	char filename[50];
	stringstream ss;
	sprintf(filename,"data_client/%s_%s.txt",user.c_str(),from.c_str());
	if (ifstream(filename))
	{
		ifstream fs (filename);
		while (!fs.eof())
		{
			string s;
			getline(fs,s);
			cout << s << endl;
			if ((s.compare("----- New Message(s) -----") != 0)&&(!s.empty()))
			{
				ss << s << endl;
			}
		}
		fs.close();
		ofstream fo (filename);
		fo << ss.str();
		fo.close();
		markAsRead(from);
	}
	else
	{
		cout << endl << "Chat not found!" << endl << endl;
	}
}

void Client::addUnread(string from)
{
	char filename[50];
	sprintf(filename,"data_client/%s_unread.txt",user.c_str());
	if (ifstream(filename))
	{
		bool exist = false;
		ifstream fs (filename);
		while ((!fs.eof())&&(!exist))
		{
			string rcv;
			fs >> rcv;
			if ((from.compare(rcv) == 0))
			{
				exist = true;
			}
		}
		fs.close();
		if (!exist)
		{
			ofstream fo (filename,ios_base::app | ios_base::out);
			fo << endl << from;
			fo.close();
		}
	}
	else
	{
		ofstream fo (filename);
		fo << from ;
		fo.close();
	}
}

void Client::checkUnread()
{
	char filename[50];
	sprintf(filename,"data_client/%s_unread.txt",user.c_str());
	if (ifstream(filename))
	{
		ifstream fs (filename);
		int n = 0;
		stringstream ss;
		while(!fs.eof())
		{
			string tmp;
			fs >> tmp;
			ss << tmp << endl;
			n++;
		}
		fs.close();
		cout << endl << "New message(s) from ";
		if (n > 1)
		{
			string tmp;
			ss >> tmp;
			cout << tmp;
			for (int i=2;i<=n;i++)
			{
				ss >> tmp;
				if (i==n)
				{
					cout << " and " << tmp << "." << endl << endl;
				}
				else if (i == 2)
				{
					cout << ", " << tmp << ",";
				}
				else
				{
					cout << " " << tmp << ",";	
				}
			}
		}
		else
		{
			string tmp;
			ss >> tmp;
			cout << tmp << "." << endl << endl;
		}
	}
}

void Client::markAsRead(string from)
{
	char filename[50];
	sprintf(filename,"data_client/%s_unread.txt",user.c_str());
	ifstream fs (filename);
	stringstream ss;
	int n = 0;
	if (fs.is_open())
	{
		while(!fs.eof())
		{
			string s;
			fs >> s;
			if (s.compare(from) != 0)
			{
				if (n > 0)
				{
					ss << endl << s;
				}
				else
				{
					ss << s;
				}
				n++;
			}
		}

		if (n > 0)
		{
			ofstream fo (filename);
			fo << ss.str() ;
			fo.close();
		}
		else
		{
			int r = remove(filename);
		}
	}
}