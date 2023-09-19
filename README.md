# Spellbound
Spellbound is a C2 (Command and Control) framework meant for creating a botnet. Spellbound is created in python and includes a payload generator (spellgen) to send to your target. Spellbound is capable of handling around 65000 bots at a time (depending on the machine).

![1670504818823](https://user-images.githubusercontent.com/88616338/221417940-74bbffa4-9a20-49d2-8f93-36de576d167e.jpg)

# Components

## Spellgen
Spellgen is a payload generator which offers 3 different kinds of payloads:
- Base64 encoded powershell command
- Staged MSFvenom payload
- Spellshell (Custom payload with post-exploitation modules)

To get the help menu for spellgen, you can use the command ```python spellgen.py```
```
Usage: spellgen.py <payload> <IP> <PORT>

        payload: spellshell
                Fully specialized reverse shell with keylogging and persistance capabilities
                Command: whataretheywriting- start writing keystrokes to a file
                Command: persist- add your reverse shell to the startup registry
                Command: spawn- spawn another instance of the reverse shell
                Arguments: <C2C IP> <C2C Port>

        payload: powershell
                Base64 encoded fileless powershell reverse shell
                Arguments: <C2C IP> <C2C Port>

        payload: msf
                Standard msfvenom generated reverse shell staged payload
                Arguments: <STAGER IP> <STAGER Port>
                Dependencies: sudo apt-get install gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 wine64
```

Generating a payload with spellgen is an easy task where you just have to specify the payload type, C2 IP and port.

#### Powershell Payload
The powershell payload is a simple reverse shell encoded in base64 just as an obfuscation tactic. This payload can be run on the victim PC by executing it through powershell or cmd.

#### MSFvenom Payload
This payload is a bit tricky as it is a staged payload. The stager server can be found in spellgen by the name ```spellstager.py```. First, the stager needs to be run with the following command:
```python spellstager.py <IP> <STAGER_PORT> <C2_PORT>```
Then, the payload needs to be generated with the following command:
```python spellgen.py msf <STAGER IP> <STAGER PORT>```
The payload can be executed on the victim PC, which will connect to the stager and share a key, through Diffie Hellman key exchange, which will encrypt the actual reverse shell paylod during transit.

#### Spellshell
Spellshell is a self-developed backdoor with some in-built post-exploitation modules:
- pesistance through the ```persist``` command
- keylogging through the ```whataretheywriting``` command
- nested backdoor creation through the ```spawn``` command

## Spellbound
Spellbound is the actual C2 server, to which all bots connect. This is devloped in python and provides a beautiful web interface with the help of the Flask micro-framework. 

#### Setting it up
- ```git clone https://github.com/dingavinga1/spellbound/```
- ```cd spellbound```
- ```pip install -r requirements.txt```
- ```python app.py```
<br/><br/>
After the above steps, open up localhost in your browser, provide the IP for your listening interface and the port to listen on and your C2 will be ready to accept dem bots! 

# Collaborators 
- [Muhammad Huzaifa](https://github.com/huzaifi0604/)
- [Aisha Irfan](https://github.com/aishairfan/)

# Disclaimer
<b>This program is meant only for educational purposes and red-teaming practices. The authors will not be responsible for any misuse of this program. This program is free for personal and commercial use, provided that due credit is given to this repository and the authors.</b>
