#include "text.h"

/*
 * Autores:
 *
 * Barata Gonzalez, Jorge
 * Galan Galiano, Cristian
 *
 * */


unsigned int _ping;
unsigned int _swap;
unsigned int _hash;
unsigned int _check;
unsigned int _stat;
/*
 * PRIMER PARAMETRO DE ENTRADA: Equivale a socket de entrada.
 * SEGUNDO DE SALIDA: Equivale a socket de salida.
 * TERCERO DE RCPGEN: No se usa.
 * RETURN: RETORNO DEL VALOR DIRECTO AL CLIENTE
 *
 **/


/* PING
 *
 * PARAMETRO: Ip, port.
 * RETORNO:
 * RETURN: Char ack.
 **/
int f_ping_1_svc(parametro2 *parametro, char *b, struct svc_req* req){
	//Recibe el ip y port
	char* ip = parametro->ip;
	int port = parametro->port;

	char ack = 'a';
	*b = ack;

	//Se imprime por pantalla
	fprintf(stderr, "s> %s:%i ping\n", ip, port);
	fprintf(stderr, "\n");

	//Se suma la llamada
	_ping++;

	return 1;
}

/* SWAP
 *
 * PARAMETRO: Ip, port, longitud, cadena.
 * RETORNO: letrasCambiadas, cadena.
 * RETURN: ¿?.
 **/
int f_swap_1_svc(parametro4 *parametro, retorno2 *retorno, struct svc_req* req){

	//Recibe la longitud del texto, la ip y el puerto
	unsigned int longitud = parametro->longitud;
	char* ip = parametro->ip;
	int port = parametro->port;

	//Se imprime por pantalla
	fprintf(stderr, "s>%s:%i init swap %u\n", ip, port, longitud);

	//Recibe la cadena
	char* copia = calloc(longitud, sizeof(char));
	copia = parametro->cadena;

	//Intercambia los valores de la cadena
	int i=0;
	unsigned int letrasCambiadas = 0;

	for(i=0; i< longitud; i++){
		if(copia[i] >= 'A' && copia[i] <= 'Z') {
			copia[i] = copia[i] + 32;
			/* resta a c el valor ascii de A */
			letrasCambiadas++;
		}else if(copia[i] >= 'a' && copia[i] <= 'z') {
			copia[i] = copia[i] - 32;
			/* resta a c el valor ascii de a */
			letrasCambiadas++;
		}
	}
/*
	//Envia la cantidad de letras cambiadas
	retorno->letrasCambiadas = letrasCambiadas;

	//Envia la nueva copia de la cadena
	retorno->cadena = calloc(longitud, sizeof(char));
	retorno->cadena = copia;

	//Se imprime por pantalla
	fprintf(stderr, "s> %s:%i swap = %u\n", ip, port, letrasCambiadas);
	fprintf(stderr, "\n");

	//Se suma la llamada
	_swap++;*/

	return 1;
}

/* HASH
 *
 * PARAMETRO: Ip, port, longitud, cadena.
 * RETORNO:
 * RETURN: unsigned int hash.
 **/
int f_hash_1_svc(parametro4 *parametro, unsigned int *b, struct svc_req* req){
	//Recibe la longitud del texto, la ip y el puerto
	unsigned int longitud = parametro->longitud;
	char* ip = parametro->ip;
	int port = parametro->port;

	//Se imprime por pantalla
	fprintf(stderr, "s> %s:%i init hash %u\n", ip, port, longitud);

	//Recibe la cadena
	char* copia = calloc(longitud, sizeof(char));
	copia = parametro->cadena;

	//Obtiene la funcion hash
	int i=0;
	unsigned int hash = 0;
	for(i=0; i< longitud; i++){
		hash = (hash + copia[i]) % 1000000000;
	}

	//Se imprime por pantalla
	fprintf(stderr, "s> %s:%i hash = %u\n", ip, port, hash);
	fprintf(stderr, "\n");

	//Envia el hash
	*b = hash;

	//Se suma la llamada
	_hash++;

	return 1;
}

/* CHECK
 *
 * PARAMETRO: Ip, port, longitud, cadena, hash.
 * RETORNO:
 * RETURN: Char correcto.
 **/
int f_check_1_svc(parametro5 *parametro, char *b, struct svc_req* req){
	//Recibe la longitud del texto, la ip, el puerto y el hash
	unsigned int longitud = parametro->longitud;
	char* ip = parametro->ip;
	int port = parametro->port;
	unsigned int hash = parametro->hash;

	//Se imprime por pantalla
	fprintf(stderr, "%s> s:%i init check %i %u\n", ip, port, longitud, hash);

	//Recibe la cadena
	char* copia = calloc(longitud, sizeof(char));
	copia = parametro->cadena;

	//Obtiene la funcion hash
	int i=0;
	unsigned int uhash = 0;
	for(i=0; i< longitud; i++){
		uhash = (uhash + copia[i]) % 1000000000;
	}
	//Comprueba si es correcta
	char correcto = 0;
	if(hash == uhash)
		correcto = 1;

	//Se imprime por pantalla
	if(correcto == 1)
		fprintf(stderr, "s> %s:%i check = OK\n", ip, port);
	else
		fprintf(stderr, "s> %s:%i check = FAIL\n", ip, port);
	fprintf(stderr, "\n");

	//Envia si es correcto el hash
	*b = correcto;

	//Se suma la llamada
	_check++;

	return 1;
}

/* STAT
 *
 * PARAMETRO:
 * RETORNO: Ping, swap, hash, check, stat
 * RETURN: ¿?.
 **/
int f_stat_1_svc(parametro2 *parametro, retorno5 *retorno, struct svc_req* req){
	//Recibe el ip y port
	char* ip = parametro->ip;
	int port = parametro->port;

	//Se imprime por pantalla
	fprintf(stderr, "s> %s:%i init stat\n", ip, port);

	//Envia ping
	retorno->ping = _ping;

	//Envia swap
	retorno->swap = _swap;

	//Envia hash
	retorno->hash = _hash;

	//Envia check
	retorno->check = _check;

	//Envia stat
	retorno->stat = _stat;

	//Se imprime por pantalla
	fprintf(stderr, "s> %s:%i stat = %u %u %u %u %u\n", ip, port, _ping, _swap, _hash, _check, _stat);
	fprintf(stderr, "\n");

	//Se suma la llamada
	_stat++;

	return 1;
}

int f_quit_1_svc(void *a, int *quit,struct svc_req* req){
	return 1;
}

int servicioprog_1_freeresult(SVCXPRT *svc, xdrproc_t xdr, caddr_t caddr){
	xdr_free(xdr,caddr);

	return 1;
}
