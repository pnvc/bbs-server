<h1>It's not a full program, just a part</h1>
I just tried to understand some things about socket, actually upload/download mechanism
<br/>
May be later I shall finaly these programs (client and server parts)
<br/>
Very dirty code, I know it, many parts of the code could be wrapped as separate functions
Many things have been omitted (some times snprintf, some times sprintf, calloc, malloc and other...)
<br/>
Here is simple bbs server with client.
Server starts as deamon process which can be killed by USR1 signal or HUP signal for reconfiguration after change server.fconf file.
If you do HUP signal -> all active connetions will be closed.
<br/>
Simple protocol for messages, all commands that is avalable, server will send you.
<br/>
<h3>Install:<h5>
1. 	You need Linux
<br/>
2. 	You can run make_compiling... file as script for the build
	Or you can run cmake_building... file as script that is used cmake for building
	<b>Server and client has own files for that and they are inside the root and ./client directories respectively</b>
	Both these files are making new directory bin_server/client and there will be exe file and clean copies of base and config files.
<br/>
3. Run ./bin_server/exe and server will be executed as deamon, all errors and info will be within the /var/log/syslog file as default (IP, PORT, PID and other...)
<br/>
4. Run ./client/bin_client/exe IP PORT, where IP and PORT - ip and port of the server.
