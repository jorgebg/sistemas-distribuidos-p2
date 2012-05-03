/* Write code here */
struct parametro4{
	string ip<>;
	int port;
	unsigned int longitud;
	string cadena<>;
};

struct parametro5{
	string ip<>;
	int port;
	unsigned int longitud;
	string cadena<>;
	unsigned int hash;
};

struct retorno2{
	unsigned int letrasCambiadas;
	string cadena<>;
};

struct retorno5{
	unsigned int ping;
	unsigned int swap;
	unsigned int hash;
	unsigned int check;
	unsigned int stat;
};

program SERVICIOPROG{
   version SERVICIOVERS{
	char F_PING() = 1;
	retorno2 F_SWAP(parametro4) = 2;
	unsigned int F_HASH(parametro4) = 3;
	char F_CHECK(parametro5) = 4;
	retorno5 F_STAT() = 5;
	int F_QUIT() = 6;
   } = 1;
} = 99;

