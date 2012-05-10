/* Write code here */
struct parametro2{
	string ip<>;
	int port;
};

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

struct parametro7{
	string ip<>;
	int port;
	unsigned int ping;
	unsigned int swap;
	unsigned int hash;
	unsigned int check;
	unsigned int stat;
};

struct retorno2{
	unsigned int letrasCambiadas;
	string cadena<>;
};

program SERVICIOPROG{
   version SERVICIOVERS{
	char F_PING(parametro2) = 1;
	retorno2 F_SWAP(parametro4) = 2;
	unsigned int F_HASH(parametro4) = 3;
	char F_CHECK(parametro5) = 4;
	void F_STAT(parametro7) = 5;
	int F_QUIT() = 6;
   } = 1;
} = 99;

