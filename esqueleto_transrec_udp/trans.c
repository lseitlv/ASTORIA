/* Copyright (C) 2006 PRAV - Pesquisa em Redes de Alta Velocidade
 *                    NTVD - N�cleo de TV Digital
 * http://www.ufrgs.br/ntvd
 *
 *  O objetivo deste programa � apresentar a base da estrutura de programa��o com sockets
 *  atrav�s de UDP
 *
 * Cli.c: Esqueleto de cliente UDP. 
 * Argumentos: -h <IP destino> -p <porta>
 *
 * Desenvolvido para sistemas UNIX Like (Linux, FreeBSD, NetBSD...) e Windows
 *		Maiko de Andrade
 *		Valter Roesler
*/

#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SOCKET	int

#define PORTA_SRV 2023 // porta TCP do servidor
#define STR_IPSERVIDOR "127.0.0.1"

int main(int argc, char **argv){
	struct sockaddr_in peer;
	SOCKET s;
	int peerlen;
 
	// Cria o socket na familia AF_INET (Internet) e do tipo UDP (SOCK_DGRAM)
	if((s = socket(AF_INET, SOCK_DGRAM,0)) >= 0) {

		// Cria a estrutura com quem vai conversar 
		peer.sin_family = AF_INET;
		peer.sin_port = htons(PORTA_SRV);
		peer.sin_addr.s_addr = inet_addr(STR_IPSERVER); 
		peerlen = sizeof(peer);
	
		// Envia pacotes Hello e aguarda resposta
		strcpy(buffer,"Hello");
		sendto(s, buffer, sizeof(buffer), 0, (struct sockaddr *)&peer, peerlen);
		close(s);
	}
}
