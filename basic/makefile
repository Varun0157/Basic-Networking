CFILES=utils.c

main: TCP UDP

TCP:
	gcc TCP_Client.c ${CFILES} -o tcp_client
	gcc TCP_Server.c ${CFILES} -o tcp_server
	
UDP:
	gcc UDP_Client.c ${CFILES} -o udp_client
	gcc UDP_Server.c ${CFILES} -o udp_server

clean:
	rm -rf tcp_client tcp_server udp_client udp_server
