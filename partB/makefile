CFLAGS = -fsanitize=address,undefined -Wall -Werror
UTILFILES = receive.c send.c

main: UDP
	
UDP:
	gcc ${CFLAGS} ${UTILFILES} UDP_Client.c -o udp_client
	gcc ${CFLAGS} ${UTILFILES} UDP_Server.c -o udp_server

clean:
	rm -rf udp_client udp_server
