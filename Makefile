
run_server:
	lsof -t -i:8989 | xargs kill -9; gcc -pthread server.c -o server.exe && ./server.exe

run_client:
	gcc client.c -o client.exe && ./client.exe
