CFILES=networking_utils.c game_utils.c
TCP_FILES=${CFILES} tcp/tcp_utils.c
UDP_FILES=${CFILES} udp/udp_utils.c

main: TCP UDP

TCP:
	gcc tcp/TCP_Client.c ${TCP_FILES} -o tcp_client
	gcc tcp/TCP_Server.c ${TCP_FILES} -o tcp_server
	
UDP:
	gcc udp/UDP_Client.c ${UDP_FILES} -o udp_client
	gcc udp/UDP_Server.c ${UDP_FILES} -o udp_server

clean:
	rm -rf tcp_client tcp_server udp_client udp_server
