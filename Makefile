all:
	g++ -o client client.cpp client_modul.cpp -std=c++11 -pthread -Wl,--no-as-needed
	g++ -o server server.cpp server_modul.cpp -std=c++11 -pthread -Wl,--no-as-needed