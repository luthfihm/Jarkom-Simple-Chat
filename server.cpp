/**
	server: menerima bilangan n dari klien, mengembalikan n + 1 ke klien
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
#include "server_modul.h"


int main(){
	Server server;
	server.start();
	return 0;
}
