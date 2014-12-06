#include "server_modul.h"

Server::Server()
{

}
Server::~Server()
{

}
void Server::start()
{
	int sock, client_sock;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int port = 9000, len, n;
	
	// buka socket TCP (SOCK_STREAM) dengan alamat IPv4 dan protocol IP
	if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0){
		close(sock);
		printf("Cannot open socket\n");
		exit(1);
	}
	
	// ganti opsi socket agar dapat di-reuse addressnya
	int yes = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("Cannot set option\n");
		exit(1);
	}
	
	// buat address untuk server
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY; // semua address pada network interface
	serv_addr.sin_port = htons(9000); // port 9000
	
	// bind server ke address
	if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		close(sock);
		printf("Cannot bind socket\n");
		exit(1);
	}
	
	listen(sock,5); // listen ke maksimal 5 koneksi klien
	log("Server starts");
	while(1){
		// terima koneksi dari klien
		clilen = sizeof(cli_addr);
		client_sock = accept(sock, (struct sockaddr *) &cli_addr, &clilen);
		
		//proses(client_sock);
		thread t (&Server::proses,this,client_sock);

		t.detach();
	}
	close(sock);
}
void Server::proses(int client_sock)
{
	data input;
	data output;

	int len = read(client_sock, input, 512);
	if (len >= 0){ // jika pembacaan berhasil
		stringstream ss;
		ss.str(input);
		int opt;
		ss >> opt;
		string user;
		string pass;
		string group;
		int channel;
		Message m;
		string tmp;
		switch(opt)
		{
			case 0 :
				ss >> channel;
				user = active_user[channel];
				active_user[channel].clear();
				log (user+" logged out");
				break;
			case 1 :
				ss >> user;
				ss >> pass;
				if (login(user,pass))
				{
					setChannel(user);
					sprintf(output, "%d",getChannel(user));
					loadSavedMsg(user);
					log(user+" logged in");
				}
				else
				{
					sprintf(output, "invalid");
				}
				break;
			case 2 :
				ss >> user;
				ss >> pass;
				if (signup(user,pass))
				{
					setChannel(user);
					sprintf(output, "%d",getChannel(user));
					log(user+" logged in");
				}
				else
				{
					sprintf(output, "invalid");
				}
				break;
			case 3 :
				ss >> user;
				ss >> group;
				if (createGroup(user,group))
				{
					sprintf(output, "valid");
					log(user+" created "+group);
				}
				else
				{
					sprintf(output, "invalid");
				}
				break;
			case 4 :
				ss >> user;
				ss >> group;
				if (checkGroup(group))
				{
					if (joinGroup(user,group))
					{
						sprintf(output, "valid");
						log (user+" joined "+group);
					}
					else
					{
						sprintf(output, "already");
					}
				}
				else
				{
					sprintf(output, "invalid");
				}
				break;
			case 5 :
				ss >> user;
				ss >> group;
				if (checkGroup(group))
				{
					if (leaveGroup(user,group))
					{
						sprintf(output, "valid");
						log (user+" left "+group);
					}
					else
					{
						sprintf(output, "notmember");
					}
				}
				else
				{
					sprintf(output, "invalid");
				}
				break;
			case 6 :
				ss >> user;
				ss >> group;
				if (checkUser(group))
				{
					sprintf(output, "valid");
				}
				else if(checkGroup(group))
				{
					if (isMember(user,group))
					{
						sprintf(output, "valid");
					}
					else
					{
						sprintf(output, "notmember");
					}
				}
				else
				{
					sprintf(output, "invalid");
				}
				break;
			case 7 :
				getline(ss,tmp);
				getline(ss,m.from);
				getline(ss,m.to);
				getline(ss,m.time);
				getline(ss,m.content);
				addMsg(m);
				log (m.from+" messages "+m.to);
				sprintf(output, "sent");
				break;
			case 8 :
				ss >> user;
				channel = getChannel(user);
				if (active_chat[channel].empty())
				{
					sprintf(output, "empty");
				}
				else
				{
					sprintf(output, "%s\n%s\n%s\n%s",active_chat[channel].front().from.c_str(),active_chat[channel].front().to.c_str(),active_chat[channel].front().time.c_str(),active_chat[channel].front().content.c_str());
					active_chat[channel].pop();
				}
				break;
			default :
				sprintf(output, "invalid");
				break;
		}
		write(client_sock,output,512); // tulis ke klien
	}
	// tutup koneksi klien
	close(client_sock);
	std::chrono::milliseconds dura( 50 );
    std::this_thread::sleep_for( dura );
}

bool Server::login (string user,string pass)
{
	ifstream fs ("data_server/user.txt");
	stringstream ss;
	string line;
	bool valid = false;
	while ((!fs.eof())&&(!valid))
	{
		string username;
		string password;
		fs >> username;
		fs >> password;
		if ((user.compare(username) == 0)&&(pass.compare(password) == 0))
		{
			valid = true;
		}
	}
	fs.close();
	return valid;
}

bool Server::signup (string user,string pass)
{
	bool valid = false;
	if ((!checkUser(user)) && (!checkGroup(user)))
	{
		ofstream fo ("data_server/user.txt",ios_base::app | ios_base::out);
		fo << user << " " << pass << endl;
		fo.close();
		valid = true;
	}
	return valid;
}

bool Server::checkUser(string user)
{
	ifstream fs ("data_server/user.txt");
	bool exist = false;
	while ((!fs.eof())&&(!exist))
	{
		string username;
		string password;
		fs >> username;
		fs >> password;
		if ((user.compare(username) == 0))
		{
			exist = true;
		}
	}
	fs.close();
	return exist;
}

bool Server::createGroup (string user,string group)
{
	if ((!checkUser(group)) && (!checkGroup(group)))
	{
		char filename[30];
		sprintf(filename,"data_server/group/%s.txt",group.c_str());
		ofstream fo (filename,ios_base::app | ios_base::out);
		fo << user;
		fo.close();
		return true;
	}
	else
	{
		return false;
	}
}

bool Server::joinGroup (string user,string group)
{
	char filename[30];
	sprintf(filename,"data_server/group/%s.txt",group.c_str());
	ifstream fs (filename);
	bool exist = false;
	bool valid = false;
	while ((!fs.eof())&&(!exist))
	{
		string username;
		fs >> username;
		if ((user.compare(username) == 0))
		{
			exist = true;
		}
	}
	fs.close();
	if (!exist)
	{
		ofstream fo (filename,ios_base::app | ios_base::out);
		fo << endl << user;
		fo.close();
		valid = true;
	}
	return valid;
}

bool Server::leaveGroup (string user,string group)
{
	char filename[30];
	sprintf(filename,"data_server/group/%s.txt",group.c_str());
	ifstream fs (filename);
	bool valid = false;
	stringstream ss;
	int n = 0;
	while(!fs.eof())
	{
		string s;
		fs >> s;
		if (s.compare(user) != 0)
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
		else
		{
			valid = true;
		}
	}

	if (valid)
	{
		if (n > 0)
		{
			ofstream fo (filename);
			fo << ss.str() ;
			fo.close();
		}
		else
		{
			valid = (remove(filename) == 0);
		}
	}

	return valid;
}

bool Server::checkGroup(string group)
{
	char filename[30];
	sprintf(filename,"data_server/group/%s.txt",group.c_str());
	if (ifstream(filename))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Server::isMember(string user,string group)
{
	char filename[30];
	sprintf(filename,"data_server/group/%s.txt",group.c_str());
	ifstream fs (filename);
	bool exist = false;
	while ((!fs.eof())&&(!exist))
	{
		string username;
		fs >> username;
		if ((user.compare(username) == 0))
		{
			exist = true;
		}
	}
	fs.close();
	return exist;
}

void Server::addMsg(Message m)
{
	if (checkUser(m.to))
	{
		int channel = getChannel(m.to);
		if (channel != -1)
		{
			active_chat[channel].push(m);
		}
		else
		{
			char filename[50];
			sprintf(filename,"data_server/saved/%s.txt",m.to.c_str());
			ofstream fo (filename,ios_base::app | ios_base::out);
			fo << m.from << endl;
			fo << m.to << endl;
			fo << m.time << endl;
			fo << m.content << endl;
			fo.close();
		}
	}
	else if (checkGroup(m.to))
	{
		char groupfile[30];
		sprintf(groupfile,"data_server/group/%s.txt",m.to.c_str());
		ifstream fs (groupfile);
		while (!fs.eof())
		{
			string user;
			fs >> user;
			if (user.compare(m.from) != 0)
			{
				int channel = getChannel(user);
				if (channel != -1)
				{
					active_chat[channel].push(m);
				}
				else
				{
					char filename[50];
					sprintf(filename,"data_server/saved/%s.txt",user.c_str());
					ofstream fo (filename,ios_base::app | ios_base::out);
					fo << m.from << endl;
					fo << m.to << endl;
					fo << m.time << endl;
					fo << m.content << endl;
					fo.close();
				}
			}
		}
	}
}

void Server::loadSavedMsg(string user)
{
	int channel = getChannel(user);
	char filename[50];
	sprintf(filename,"data_server/saved/%s.txt",user.c_str());
	ifstream fs (filename);
	if (fs.is_open())
	{
		while (!fs.eof())
		{
			Message m;
			string tmp;
			getline(fs,tmp);
			if (!tmp.empty())
			{
				m.from = tmp;
				getline(fs,m.to);
				getline(fs,m.time);
				getline(fs,m.content);

				active_chat[channel].push(m);
			}
		}
		fs.close();

		int r = remove(filename);
	}
}


void Server::log(string m)
{
	// current date/time based on current system
   	time_t now = time(0);
   	struct tm * timeinfo;
   	char dt[30];

   	time (&now);
  	timeinfo = localtime (&now);

   	strftime (dt,30,"%F %T",timeinfo);

   	char filename[50];
	sprintf(filename,"data_server/server_log.txt");
	ofstream fo (filename,ios_base::app | ios_base::out);
	fo << "[" << dt <<"] " << m << endl;
	cout << "[" << dt <<"] " << m << endl;
	fo.close();
}

int Server::getChannel(string user)
{
	bool found = false;
	int i = 0;
	while((!found)&&(i<100))
	{
		if (active_user[i].compare(user) == 0)
		{
			found = true;
		}
		else
		{
			i++;
		}
	}
	if (found)
	{
		return i;
	}
	else
	{
		return -1;
	}
}
void Server::setChannel(string user)
{
	bool found = false;
	int i = 0;
	while((!found)&&(i<100))
	{
		if (active_user[i].empty())
		{
			active_user[i] = user;
			found = true;
		}
		else
		{
			i++;
		}
	}
}