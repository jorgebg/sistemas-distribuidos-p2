#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <ctype.h>

#include <wordexp.h>

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
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

/**
 * Funciones
 * */

void recibir(int serverConnected, char* copia, unsigned int longitud);
char* obtenerIpServer(char* server);

/**
 * Atributos Globales:
 * */

int debug = 0;
char* ipLocal;
int port;
CLIENT *_client;


void usage(char *program_name) {
	printf("Usage: %s [-d] -s <server>\n", program_name);
}

/* LLAMADA A METODOS REMOTOS
 * PRIMER PARAMETRO DE SALIDA: Equivale a socket de salida.
 * SEGUNDO DE ENTRADA: Equivale a socket de entrada.
 * TERCERO: ClLIENTE.
 * RETURN: 1 se ha terminado la llamada del procedimiento
 *
 **/

/* PING
 *
 * PARAMETRO:
 * RETORNO:
 * RETURN: Char ack.
 **/
void f_ping(){
	if (debug)
		printf("PING\n");
	
	// Write code here
	struct timespec ini, end;
	parametro2* parametro = malloc(sizeof(*parametro));
	parametro->ip = ipLocal;
	parametro->port = 111;

	//ack
	char ack;

	//Tiempo
	clock_gettime(CLOCK_REALTIME, &ini);

	//Llamada a metodo remoto. Recibe el ack
	f_ping_1(parametro, &ack, _client);

	//Tiempo
	clock_gettime( CLOCK_REALTIME, &end);

	fprintf(stderr, "%f s\n", (float)(1.0*(1.0*end.tv_nsec - ini.tv_nsec*1.0)*1e-9 + 1.0*end.tv_sec - 1.0*ini.tv_sec));

	//Se suma la llamada
	_ping++;
}

/* SWAP
 *
 * PARAMETRO: Ip, port, longitud, cadena.
 * RETORNO: letrasCambiadas, cadena.
 * RETURN: ¿?.
 **/

void f_swap(char *src, char *dst){
	if (debug)
		printf("SWAP <SRC=%s> <DST=%s>\n", src, dst);
	
	// Write code here
	parametro4* parametro = malloc(sizeof(*parametro));

	retorno2* retorno;
	retorno = (retorno2*) calloc(1,sizeof(*retorno));

	//Crea las variables locales para obtener la longitud del fichero
	int total = 0;
	struct stat statFichero;

	//Obtiene la informacion de un fichero
	if(stat(src,&statFichero) < 0){
		perror("El fichero de origen no existe");
        exit(1);
	}
	total = statFichero.st_size;

	//Abre un fichero
	FILE *archivo = fopen(src,"r");
	if(archivo == NULL){
		perror("El fichero de origen no se ha abierto");
		exit(1);
	}

	//Crea la memoria de una cadena y copia a memoria el fichero
	parametro->cadena = calloc(total, sizeof(char));
	fread(parametro->cadena,sizeof(char),total,archivo);

	//Cierra el fichero
	fclose(archivo);

	// Se le pasa la ip, el puerto y la longitud
	parametro->ip = (char*)calloc(strlen(ipLocal),sizeof(char));
	memcpy(parametro->ip, ipLocal, strlen(ipLocal));

	parametro->port = 111;
	parametro->longitud = total;

	//Llamada a metodo remoto. Le envía y recibe los datos
	f_swap_1(parametro, retorno, _client);

	//Recibe la cantidad de letras cambiadas y las imprime por pantalla
	fprintf(stderr, "%u \n", retorno->letrasCambiadas);

	//Graba los datos en un fichero
	FILE *archivo2 = fopen(dst,"w");
	if(archivo2 == NULL){
		perror("El fichero de destino no se ha abierto");
		exit(1);
	}

	//Recibe la nueva cadena y la mete en el archivo
	fputs(retorno->cadena, archivo2);

	//Cierra el fichero
	fclose(archivo2);

	//Libera memoria
	free(parametro->ip);
	free(parametro->cadena);
	free(parametro);
	free(retorno->cadena);
	free(retorno);

	//Se suma la llamada
	_swap++;

}

/* HASH
 *
 * PARAMETRO: Ip, port, longitud, cadena.
 * RETORNO:
 * RETURN: unsigned int hash.
 **/
void f_hash(char *src){
	if (debug)
		printf("HASH <SRC=%s>\n", src);
	
	// Write code here
	parametro4* parametro = malloc(sizeof(*parametro));

	//Crea las variables locales para obtener la longitud del fichero
	int total = 0;
	struct stat statFichero;

	//Obtiene la informacion de un fichero
	if(stat(src,&statFichero) < 0){
		perror("El fichero de origen no existe");
        exit(1);
	}
	total = statFichero.st_size;

	//Abre un fichero
	FILE *archivo = fopen(src,"r");
	if(archivo == NULL){
			fprintf(stderr, "El fichero no existe \n");
			exit(1);
	}

	//Crea la memoria de una cadena y copia a memoria el fichero
	parametro->cadena = calloc(total, sizeof(char));
	fread(parametro->cadena,sizeof(char),total,archivo);

	//Cierra el fichero
	fclose(archivo);


	// Se le pasa la ip, el puerto y la longitud
	parametro->ip = (char*)calloc(strlen(ipLocal),sizeof(char));
	memcpy(parametro->ip, ipLocal, strlen(ipLocal));

	parametro->port = 111;
	parametro->longitud = total;

	//Recibe el hash
	unsigned int hash;

	//Llamada a metodo remoto. Le envía y recibe los datos
	f_hash_1(parametro, &hash, _client);

	//Se imprime por pantalla
	fprintf(stderr, "%u\n", hash);

	//Libera la memoria
	free(parametro->ip);
	free(parametro->cadena);
	free(parametro);

	//Se suma la llamada
	_hash++;
}

/* CHECK
 *
 * PARAMETRO: Ip, port, longitud, cadena, hash.
 * RETORNO:
 * RETURN: Char correcto.
 **/
void f_check(char *src, int hash){
	if (debug)
		printf("CHECK <SRC=%s> <HASH=%d>\n", src, hash);
	
	// Write code here
	parametro5* parametro = malloc(sizeof(*parametro));

	//Crea las variables locales para obtener la longitud del fichero
	int total = 0;
	struct stat statFichero;

	//Obtiene la informacion de un fichero
	if(stat(src,&statFichero) < 0){
		perror("El fichero de origen no existe");
        exit(1);
	}
	total = statFichero.st_size;

	//Abre un fichero
	FILE *archivo = fopen(src,"r");
	if(archivo == NULL){
			fprintf(stderr, "El fichero no existe \n");
			exit(1);
	}

	//Crea la memoria de una cadena y copia a memoria el fichero
	parametro->cadena = calloc(total, sizeof(char));
	fread(parametro->cadena,sizeof(char),total,archivo);

	//Cierra el fichero
	fclose(archivo);

	// Se le pasa la ip, el puerto y la longitud
	parametro->ip = (char*)calloc(strlen(ipLocal),sizeof(char));
	memcpy(parametro->ip, ipLocal, strlen(ipLocal));

	parametro->port = 111;
	parametro->longitud = total;

	//Le envia el valor hash
	parametro->hash = hash;

	char correcto;

	//Llamada a metodo remoto. Le envía y recibe los datos
	f_check_1(parametro, &correcto, _client);

	//Se imprime por pantalla
	if(correcto == 0)
		fprintf(stderr, "FAIL\n");
	else
		fprintf(stderr, "OK\n");

	//Libera la memoria
	free(parametro->ip);
	free(parametro->cadena);
	free(parametro);

	//Se suma la llamada
	_check++;
}

/* STAT
 *
 * PARAMETRO:
 * RETORNO: Ping, swap, hash, check, stat
 * RETURN: ¿?.
 **/
void f_stat(){
	if (debug)
		printf("STAT\n");
	
	// Write code here
	parametro7* parametro = malloc(sizeof(*parametro));
	parametro->ip = ipLocal;
	parametro->port = 111;
	parametro->ping = _ping;
	parametro->swap = _swap;
	parametro->hash = _hash;
	parametro->check = _check;
	parametro->stat = _stat;

	//Llamada a metodo remoto. Le envía y recibe los datos
	f_stat_1(parametro, NULL, _client);

	//Se imprime por pantalla
	fprintf(stderr, "ping %u \n", _ping);

	//Se imprime por pantalla
	fprintf(stderr, "swap %u \n", _swap);

	//Se imprime por pantalla
	fprintf(stderr, "hash %u \n", _hash);

	//Se imprime por pantalla
	fprintf(stderr, "check %u \n", _check);

	//Se imprime por pantalla
	fprintf(stderr, "stat %u \n", _stat);

	//Se suma la llamada
	_stat++;
}

void f_quit(){
	if (debug)
		printf("QUIT\n");
	
	// Write code here
	int quit = f_quit_1(NULL, NULL, _client);
	exit(quit);

}

//Obtiene la ip a traves de su hostname
char* obtenerIpServer(char* server) {
	struct sockaddr_in host;
	fprintf(stderr, "2 \n");
	host.sin_addr = * (struct in_addr*) gethostbyname(server)->h_addr;
	fprintf(stderr, "3 \n");
	return inet_ntoa(host.sin_addr);
}

//Obtiene la ip local
char* obtenerIpLocal() {
	struct sockaddr_in host;
	char hostname[255];

	gethostname(hostname, 255);
	host.sin_addr = * (struct in_addr*) gethostbyname(hostname)->h_addr;
	return inet_ntoa(host.sin_addr);
}


void shell() {
	char line[1024];
	char *pch;
	int exit = 0;
	
	wordexp_t p;
	char **w;
	int ret;
	
	memset(&p, 0, sizeof(wordexp));
	
	do {
		fprintf(stdout, "c> ");
		memset(line, 0, 1024);
		pch = fgets(line, 1024, stdin);
		
		if ( (strlen(line)>1) && ((line[strlen(line)-1]=='\n') || (line[strlen(line)-1]=='\r')) )
			line[strlen(line)-1]='\0';
		
		ret=wordexp((const char *)line, &p, 0);
		if (ret == 0) {
			w = p.we_wordv;
		
			if ( (w != NULL) && (p.we_wordc > 0) ) {
				if (strcmp(w[0],"ping")==0) {
					if (p.we_wordc == 1)
						f_ping();
					else
						printf("Syntax error. Use: ping\n");
				} else if (strcmp(w[0],"swap")==0) {
					if (p.we_wordc == 3)
						f_swap(w[1],w[2]);
					else
						printf("Syntax error. Use: swap <source_file> <destination_file>\n");
				} else if (strcmp(w[0],"hash")==0) {
					if (p.we_wordc == 2)
						f_hash(w[1]);
					else
						printf("Syntax error. Use: hash <source_file>\n");
				} else if (strcmp(w[0],"check")==0) {
					if (p.we_wordc == 3)
						f_check(w[1], atoi(w[2]));
					else
						printf("Syntax error. Use: check <source_file> <hash>\n");
				} else if (strcmp(w[0],"stat")==0) {
					if (p.we_wordc == 1)
						f_stat();
					else
						printf("Syntax error. Use: stat\n");
				} else if (strcmp(w[0],"quit")==0) {
					if (p.we_wordc == 1) {
						f_quit();
						exit = 1;
					} else {
						printf("Syntax error. Use: quit\n");
					}
				} else {
					fprintf(stderr, "Error: command '%s' not valid.\n", w[0]);
				}
			}
			
			wordfree(&p);
		}
	} while ((pch != NULL) && (!exit));
}

int main(int argc, char *argv[]){
	char *program_name = argv[0];
	int opt;
	char *server;

	setbuf(stdout, NULL);
	
	// Parse command-line arguments
	while ((opt = getopt(argc, argv, "ds:")) != -1) {
		switch (opt) {
			case 'd':
				debug = 1;
				break;
			case 's':
				server = optarg;
				break;
			case '?':
				if ((optopt == 's') || (optopt == 'p'))
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			default:
				usage(program_name);
				exit(EX_USAGE);
		}
	}
	
	if (debug)
		printf("SERVER: %s\n", server);
	
	/*
	 *  EMPIEZA EL CODIGO PROPIO
	 *  NO SE SI HAY QUE USAR STDERR O STDOUT. Realmente no piden salida, se puede usar printf.
	 **/

	if(argc < 3 | argc > 4){
		perror("Numero de parametros incorrectos: ./client -d -s <ip>");
		exit(-1);
	}

	//Se crea el cliente
	_client = malloc(sizeof(*_client));
	_client = clnt_create (server, SERVICIOPROG, SERVICIOVERS, "TCP");

	if (_client == NULL) {
		//Si no se crea el cliente, entonces busca la ip del server
		char* ip = obtenerIpServer(server);
		_client = clnt_create (ip, SERVICIOPROG, SERVICIOVERS, "TCP");
		if (_client == NULL) {
			perror("Error creando el cliente");
			exit(-1);
		}
	}

	//Obtiene la ip local
	ipLocal = obtenerIpLocal();

	//Llama a la consola

		shell();

	//Se cierra el cliente
	clnt_destroy (_client);

	exit(EXIT_SUCCESS);
}

