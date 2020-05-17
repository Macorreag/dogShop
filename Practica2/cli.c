
	#include <termios.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <sys/socket.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <pthread.h>
	#include <semaphore.h>
	
	#define PORT 3535

	int clientfd, i, j, r;
	long nregisters = 0, posreg = 0, block = 0;
	char opcion;

	struct dogType
	{
		char nombre[32];
		int edad;
		char raza[16];
		int estatura;
		float peso;
		char genero;
	};

	struct dogType *dog;
	const int SIZE_DATA_DOG = sizeof(struct dogType);
	
	void ingresar(void);
	void ver(int modo);
	void borrar(void);
	void buscar(void);
	
	int enviar(int clientfd, void * data, size_t size);
	int recibir(int clientfd, void * data, size_t size);
		
	void regresar(void (*src)(void), void (*dst)(void), char * message);
	void continuar(void (*dst)(void));
	void menu(void);

	int getch(void)
	{
		struct termios oldattr, newattr;
		int ch;
		tcgetattr(STDIN_FILENO, &oldattr);
		newattr = oldattr;
		newattr.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
		ch = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
		return ch;
	}

	/* 
	*  Captura valor ingresado por usuario donde sólo
	*  permite ingresar letras a-z, A-Z y espacio.
	*/
	void scanletter(int length, char * array)
	{
		struct termios oldattr, newattr;
		int ch, counter = 0;

		tcgetattr(STDIN_FILENO, &oldattr);
		newattr = oldattr;
		newattr.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

		for(i = 0; array[i]; i++)
		{ array[i] = '\0'; }

		do
		{
			ch = getchar();

			if(ch == 127 && counter > 0)
			{
				printf("\b \b");
				counter--;
				array[counter] = '\0';
			}

			else if( (counter < length) && ((ch == 32) ||
				 (ch >= 65 && ch <= 90) ||
				 (ch >= 97 && ch <= 122)))
			{
				printf("%c", ch);
				array[counter] = ch;
				counter++;
			}
		}
		while(ch != 10 || array[0] == '\0');

		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
	}

	/* 
	*  Captura valor ingresado por usuario donde sólo
	*  permite ingresar dígitos 0-9.
	*/
	void scandigit(int length, int * number)
	{
		struct termios oldattr, newattr;
		int ch, counter = 0;
		char * array;
		array = malloc(length);

		tcgetattr(STDIN_FILENO, &oldattr);
		newattr = oldattr;
		newattr.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

		do
		{
			ch = getchar();

			if(ch == 127 && counter > 0)
			{
				printf("\b \b");
				counter--;
				array[counter] = '\0';
			}

			else if((counter < length) && (ch >= 48 && ch <= 57))
			{
				printf("%c", ch);
				array[counter] = ch;
				counter++;
			}
		}
		while(ch != 10 || array[0] == '\0');

		*number = atoi(array);
		free(array);
		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
	}

	/* 
	*  Captura valor ingresado por usuario donde sólo
	*  permite ingresar dígitos 0-9.
	*/
	void scanlidigit(int length, long * number)
	{
		struct termios oldattr, newattr;
		int ch, counter = 0;
		char * array;
		array = malloc(length);

		tcgetattr(STDIN_FILENO, &oldattr);
		newattr = oldattr;
		newattr.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

		do
		{
			ch = getchar();

			if(ch == 127 && counter > 0)
			{
				printf("\b \b");
				counter--;
				array[counter] = '\0';
			}

			else if((counter < length) && (ch >= 48 && ch <= 57))
			{
				printf("%c", ch);
				array[counter] = ch;
				counter++;
			}
		}
		while(ch != 10 || array[0] == '\0');

		*number = strtol(array, NULL, 10);
		free(array);
		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
	}

	/* 
	*  Captura valor ingresado por usuario donde sólo
	*  permite ingresar valores decimales 0-9 y separador decimal.
	*/
	void scandecimal(int length, float * decimal)
	{
		struct termios oldattr, newattr;
		int ch, separator, counter = 0, flag = 0;
		char * array;
		array = malloc(length);

		tcgetattr(STDIN_FILENO, &oldattr);
		newattr = oldattr;
		newattr.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

		do
		{
			ch = getchar();

			if(ch == 127 && counter > 0)
			{
				printf("\b \b");
				counter--;
				array[counter] = '\0';

				if((counter == separator) && (flag == 1))
				{ flag = 0; }
			}

			else if( (counter < length) && (ch >= 48 && ch <= 57) ||
				 ((ch == 46) && (flag == 0)) )
			{
				printf("%c", ch);
				array[counter] = ch;

				if(ch == 46)
				{
					flag = 1;
					separator = counter;
				}

				counter++;
			}
		}
		while(ch != 10 || array[0] == '\0');

		*decimal = atof(array);
		free(array);
		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
	}

	/* 
	*  Captura valor ingresado por usuario donde sólo
	*  permite ingresar los caracteres especificados
	*  en el diccionario.
	*/
	void scanchar(int length, char * character, char * dictionary)
	{
		struct termios oldattr, newattr;
		int ch, counter = 0;

		tcgetattr(STDIN_FILENO, &oldattr);
		newattr = oldattr;
		newattr.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

		for(i = 0; character[i]; i++)
		{ character[i] = '\0'; }

		do
		{
			ch = getchar();

			if(ch == 127 && counter > 0)
			{
				printf("\b \b");
				counter--;
				*character = '\0';
			}

			else if(counter < length)
			{
				for(i = 0; i < strlen(dictionary); i++)
				{
					if(ch == dictionary[i])
					{
						printf("%c", ch);
						*character = ch;
						counter++;
					}
				}
				
			}
		}
		while(ch != 10 || character[0] == '\0');

		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
	}
	
	int numreg()
	{
		if(recibir(clientfd, &nregisters, 8) == 1)
		{
			printf("\n\tRegistros: %li", nregisters);

			if(nregisters > 0)
			{
				printf("\n\tIngrese número de registro:\n\t");
				scanlidigit(10, &posreg);

				if(posreg < 1 || posreg > nregisters)
				{ 
					printf("\n\n\tFuera de rango [%li max].", nregisters);
					return 0;
				}

				else
				{
					enviar(clientfd, &posreg, 8);
					return 1;
				}
			}

			else
			{
				printf("\n\tFallo en obtener número de registros.");
				return 0;
			}
		}
	}

	void ingresar(void)
	{
		system("clear");
		printf("\n\t--- Ingrese datos de la mascota ---");
		printf("\n\tNombre [32 max.]:\t");		scanletter(32, dog->nombre);
		printf("\n\tEdad:\t\t\t");				scandigit(3, &dog->edad);
		printf("\n\tRaza [16 max.]:\t\t");		scanletter(16, dog->raza);
		printf("\n\tEstatura [cm]:\t\t");		scandigit(3, &dog->estatura);
		printf("\n\tPeso [Kg]:\t\t");			scandecimal(6, &dog->peso);
		printf("\n\tGénero [H/M]:\t\t");		scanchar(1, &dog->genero, "HMhm");
		
		r = atoi(&opcion);
		r = send(clientfd, &r, 4, 0);
		
		if(r == -1)
		{
			perror("\n\t Error al enviar opcion...");
		}

		else
		{
			r = send(clientfd, dog, SIZE_DATA_DOG, 0);

			if(r == -1)
			{
				perror("\n\t Error al enviar estructura...");
			}

			else if(recibir(clientfd, &posreg, 8))
			{
				printf
				(
					"\n\n\t%s\n\t%s%li",
					"Datos registrados...",
					"No. actual de registro: ",
					posreg
				);
			}

			else
			{
				perror("\n\t Error al recibir número de registro...");
			}
		}

		continuar(&menu);
	}
	
	void ver(int modo)
	{
		int r;
		r = atoi(&opcion);
		r = send(clientfd, &r, 4, 0);

		if(numreg() == 1)
		{
			recibir(clientfd, dog, SIZE_DATA_DOG);
			
			printf
			(
				"\n\n\t%s \n\t%s\t\t%s \n\t%s\t\t%i \n\t%s\t\t%s \n\t%s\t%i%s \n\t%s\t\t%.2f%s \n\t%s\t\t%c \n\t%s",
				"------------ Mascota ------------",
				"Nombre:", dog->nombre,
				"Edad:", dog->edad,
				"Raza:", dog->raza, 
				"Estatura:", dog->estatura, " cm(s)",
				"Peso:", dog->peso, " Kg(s)",
				"Genero:", dog->genero,
				"---------------------------------"
			);
		}

		if(modo == 0)
		{ continuar(&menu); }
	}

	void borrar(void)
	{
		ver(1);
		printf("\n\t%s%li%s\n\t", "¿Desea borrar el registro No. ", posreg, "? [S/N]");
		scanchar(1, &opcion, "SYNsyn");

		if(opcion == 'n' || opcion == 'N')
		{
			block = 0;
			enviar(clientfd, &block, 8);
			menu();
		}

		else
		{
			//truncar(posreg);
			block = 1;
			
			if(enviar(clientfd, &block, 8) == 1)
			{
				if(recibir(clientfd, &block, 4) == 1)
				{
					if(block == 3)
					{
						printf("\n\tRegistro borrado satisfactoriamente");
					}
				}
			}
			
			continuar(&menu);
		}
	}

	void buscar(void)
	{
		system("clear");
		char * dogName;
		dogName = malloc(32);
		dogName[32] = '\0';

		printf("\n\t--- Búsqueda ---");
		printf("\n\tNombre de la mascota: ");
		scanletter(32, dogName);

		j = 0;
		r = 4;

		if(enviar(clientfd, &r, 4) == 1)
		{
			if(enviar(clientfd, dogName, 32) == 1)
			{
				while(1)
				{
					if(recibir(clientfd, &block, 8) == 1)
					{
						if(block == 0)
						{ break; }
						
						else if(recibir(clientfd, dog, SIZE_DATA_DOG) == 1)
						{
							if(j == 0)
							{
								printf("\n\n\t%s\t%s", "No. registro", "Mascota");
							}
						
							printf("\n\t%li\t\t%s", block, dog->nombre);
							j++;
						}
					}
				}
			}
		}
		
		
		if(j < 1)
		{
			printf("\n\t%s%s%s", "No existe registro de la mascota \"", dogName, "\"...");
		}

		free(dogName);
		continuar(&menu);
	}

	void regresar(void (*src)(void), void (*dst)(void), char * message)
	{
		printf("\n\t%s\n\t", message);
		scanchar(1, &opcion, "SYNsyn");
		system("clear");

		if(opcion == 'n' || opcion == 'N')
		{ (*src)(); }
		
		else
		{ (*dst)(); }
	}

	void continuar(void (*dst)(void))
	{
		printf("\n\n\tPresione cualquier tecla para continuar...");
		getch();
		system("clear");
		(*dst)();
	}

	void salir()
	{
		int r = 5;
		r = send(clientfd, &r, 4, 0);
		free(dog);
		exit(0);
		close(clientfd);
	}

	void menu()
	{
		printf
		(
			"\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\n\t",
			"1. Ingresar registro",
			"2. Ver registro",
			"3. Borrar registro",
			"4. Buscar registro",
			"5. Salir"
		);

		scanchar(1, &opcion, "12345");

		if(opcion == '5')
		{
			regresar(&menu, &salir, "¿Desea salir del programa? [S/N]:");
		}

		else
		{
			system("clear");
			switch(opcion)
		    	{
				case '1':        ingresar();		break;
				case '2':        ver(0);			break;
				case '3':        borrar();			break;
				case '4':        buscar();			break;
				default:        
				regresar(&menu, &salir, "¿Desea salir del programa? [S/N]:");
		    	}
		}
	}

	int enviar(int clientfd, void * data, size_t size)
	{
		int r;
		r = send(clientfd, data, size, 0);
		
		if(r == -1)
		{
			perror("\n\t Error en enviar");
			return 0;
		}
		
		return 1;
	}

	int recibir(int clientfd, void * data, size_t size)
	{
		int r;
		r = recv(clientfd, data, size, 0);
		
		if(r == -1)
		{
			perror("\n\t Error en recibir");
			return 0;
		}
		
		return 1;
	}

	void main(void)
	{
		system("clear");
		struct sockaddr_in server;
		dog = malloc(sizeof(struct dogType));	
					
		clientfd = socket(AF_INET, SOCK_STREAM, 0);
		if(clientfd == -1)
		{
    			perror("\n\t Error en la creación del socket...");
		}
		
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);
		server.sin_addr.s_addr = inet_addr("127.0.0.1");
		bzero(server.sin_zero, 8);
	
		//Conexión
		r = connect(clientfd, (struct sockaddr *) &server, sizeof(struct sockaddr));

		if(r == -1)
		{
			perror("\n\t Error al conectar el socket...");
		}

		else
		{ menu(); }
	}
