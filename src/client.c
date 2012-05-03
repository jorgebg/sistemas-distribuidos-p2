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

/**
 * Funciones
 * */

void recibir(int serverConnected, char* copia, unsigned int longitud);
char* obtenerIpServer(char* server);

/**
 * Atributos Globales:
 * */

int debug = 0;
int serverConnected;
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

	//ack
	char ack;

	//Tiempo
	clock_gettime(CLOCK_REALTIME, &ini);

	//Llamada a metodo remoto. Recibe el ack
	f_ping_1(NULL, &ack, _client);

	//Tiempo
	clock_gettime( CLOCK_REALTIME, &end);

	fprintf(stderr, "%f s\n", (float)(1.0*(1.0*end.tv_nsec - ini.tv_nsec*1.0)*1e-9 + 1.0*end.tv_sec - 1.0*ini.tv_sec));
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
	retorno2* retorno = malloc(sizeof(*retorno));

	//Obtiene los datos del fichero
	FILE *archivo = fopen(src,"r");
	if(archivo == NULL)
		exit(1);

	char caracteres[10];
	int total = 0;
	char* copia = calloc(total, sizeof(char));
	char* resultado;

	//Se obtiene el fichero
	while (fgets(caracteres,10,archivo) != NULL)
	{
		total = total + 10;
		resultado = calloc(total, sizeof(char));

		strcpy(resultado, copia);
		strcat(resultado, caracteres);
		copia = calloc(total, sizeof(char));
		strcpy(copia, resultado);
	}

	free(resultado);
	fclose(archivo);

	//Le envia la longitud del texto
	unsigned int longitud = strlen(copia);
	parametro->longitud = longitud;

	//Le envia un cadena
	parametro->cadena = copia;

	//Llamada a metodo remoto. Le envía y recibe los datos
	f_swap_1(parametro, retorno, _client);

	//Recibe la cantidad de letras cambiadas
	unsigned int letrasCambiadas = retorno->letrasCambiadas;


	//Recibe la nueva cadena
	free(copia);
	copia = calloc(longitud, sizeof(char));
	recibir(serverConnected,copia, longitud);

	//Se imprime por pantalla
	fprintf(stderr, "%i\n", letrasCambiadas);

	//Graba los datos en un fichero
	FILE *archivo2 = fopen(dst,"w");
	if(archivo2 == NULL)
		exit(1);

	fputs(copia, archivo2);

	fclose(archivo2);
	free(copia);
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

	//Obtiene los datos del fichero
	FILE *archivo;
	char caracteres[10];

	archivo = fopen(src,"r");
	if(archivo == NULL)
		exit(1);

	int total = 0;
	char* copia = calloc(total, sizeof(char));
	char* resultado;

	while (fgets(caracteres,10,archivo) != NULL)
	{
		total = total + 10;
		resultado = calloc(total, sizeof(char));

		strcpy(resultado, copia);
		strcat(resultado, caracteres);
		copia = calloc(total, sizeof(char));
		strcpy(copia, resultado);
	}

	free(resultado);
	fclose(archivo);

	//Le envia la longitud del fichero
	unsigned int longitud = strlen(copia);
	parametro->longitud = longitud;

	//Le envia un cadena
	parametro->cadena = copia;

	//Recibe el hash
	unsigned int hash;

	//Llamada a metodo remoto. Le envía y recibe los datos
	f_hash_1(parametro, &hash, _client);

	//Se imprime por pantalla
	fprintf(stderr, "%u\n", hash);

	free(copia);
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

	//Obtiene los datos del fichero
	FILE *archivo;
	char caracteres[10];

	archivo = fopen(src,"r");
	if(archivo == NULL)
		exit(1);

	int total = 0;
	char* copia = calloc(total, sizeof(char));
	char* resultado;

	while (fgets(caracteres,10,archivo) != NULL)
	{
		total = total + 10;
		resultado = calloc(total, sizeof(char));

		strcpy(resultado, copia);
		strcat(resultado, caracteres);
		copia = calloc(total, sizeof(char));
		strcpy(copia, resultado);
	}

	free(resultado);
	fclose(archivo);

	//Le envia la longitud del texto
	int longitud = strlen(copia);
	parametro->longitud = longitud;

	//Le envia un cadena
	parametro->cadena = copia;

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

	free(copia);
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
	retorno5* retorno = malloc(sizeof(*retorno));

	//Llamada a metodo remoto. Le envía y recibe los datos
	f_stat_1(NULL, retorno, _client);

	//Recibe el valor de ping
	unsigned int ping = retorno->ping;

	//Se imprime por pantalla
	fprintf(stderr, "ping %u \n", ping);

	//Recibe el valor de swap
	unsigned int swap = retorno->swap;

	//Se imprime por pantalla
	fprintf(stderr, "swap %u \n", swap);

	//Recibe el valor de hash
	unsigned int hash = retorno->hash;

	//Se imprime por pantalla
	fprintf(stderr, "hash %u \n", hash);

	//Recibe el valor de check
	unsigned int check = retorno->hash;

	//Se imprime por pantalla
	fprintf(stderr, "check %u \n", check);

	//Recibe el valor de stat
	unsigned int stat = retorno->stat;

	//Se imprime por pantalla
	fprintf(stderr, "stat %u \n", stat);
}

void f_quit(){
	if (debug)
		printf("QUIT\n");
	
	// Write code here
	int quit = f_quit_1(NULL, NULL, _client);

}

void recibir(int serverConnected, char* copia, unsigned int longitud){
	//Envia la nueva copia de la cadena
	int datosRestantes = longitud;
	int enviado = 0;

	while (datosRestantes != 0){
		char* cadena = calloc(datosRestantes, sizeof(char));
		enviado = read(serverConnected, cadena, datosRestantes);
		datosRestantes = datosRestantes - enviado;
		strcat(copia, cadena);
		free(cadena);
	}
}

//Obtiene la ip a traves de su hostname
char* obtenerIpServer(char* server) {
	struct sockaddr_in host;

	host.sin_addr = * (struct in_addr*) gethostbyname(server)->h_addr;
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

	struct sockaddr_in serverIn;

	//Se crea el socket
	serverConnected = socket(AF_INET, SOCK_STREAM, 0);

	if(serverConnected < 0){
		perror("Error creando socket");
	}
	int port;
	//Obtiene la direccion del servidor
	serverIn.sin_family = AF_INET;
	serverIn.sin_port = htons(port);
	serverIn.sin_addr.s_addr = inet_addr(server);
	//serverIn.sin_addr.s_addr = INADDR_ANY;

	//Comprueba si se puede conectar
	if(connect(serverConnected, (struct sockaddr *) &serverIn, sizeof(serverIn)) <0){
		char* ip = obtenerIpServer(server);
		serverIn.sin_addr.s_addr = inet_addr(ip);
		if(connect(serverConnected, (struct sockaddr *) &serverIn, sizeof(serverIn)) <0){
			fprintf(stderr, "Error en la conexion con el servidor %s:%i", server, port);
			exit(-1);
		}else
			shell();
	}else
		shell();

	close(serverConnected);

	exit(EXIT_SUCCESS);
}

