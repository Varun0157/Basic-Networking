CFILES=utils.c
TCP_FILES=${CFILES} tcp/tcp_utils.c
UDP_FILES=${CFILES} udp_files.c

main: TCP

TCP:
	gcc tcp/TCP_Client.c ${TCP_FILES} -o tcp_client
	gcc tcp/TCP_Server.c ${TCP_FILES} -o tcp_server
	
UDP:
	gcc UDP_Client.c ${CFILES} -o udp_client
	gcc UDP_Server.c ${CFILES} -o udp_server

clean:
	rm -rf tcp_client tcp_server udp_client udp_server
