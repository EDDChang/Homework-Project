all: server.c
	gcc server2.c -o write_server
	gcc server2.c -D READ_SERVER -o read_server

clean:
	rm -f read_server write_server
