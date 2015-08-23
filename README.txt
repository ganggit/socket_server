1. source files:
server.cpp: implemented with select async, port 9999

client.cpp: client, connect to port 9999

2. compile:
g++ -g server.cpp -o server
g++ -g client.cpp -o client

3. runing and testing:
for server: ./server
for client: ./client 127.0.0.1 9999

4. any comments are welcome

5. multithread.cpp: a simple multiple threading code to do summation
