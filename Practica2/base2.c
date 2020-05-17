
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
	#include <signal.h>
	#include <time.h>
    
	#define SIZE_HASH 2000
	#define NAME_FILE "borrame.dat"
	#define NAME_LOG "serverDogs.log"
	#define PORT 3535
	#define BACKLOG 32

	FILE *fp;
	FILE *_log;
	
	//sem_t *sem_ingresar, *sem_numreg, *sem_log;
	//pthread_mutex_t mutex_ingresar, mutex_numreg, mutex_log;
	int pipe_ingresar[2], pipe_numreg[2], pipe_log[2];

	socklen_t sin_size;
	pthread_t centinela[2], hilo[BACKLOG];
	int flagThread[BACKLOG], flagInterrupt = 0;
	
	time_t t;
	
	char opcion;
	char tiempo[30];
	int j = 0, r, serverfd, clientfd[BACKLOG], opc[BACKLOG];
	long posreg = 0, block = 0, nregisters;

	struct dogType
	{
		char nombre[32];
		int edad;
		char raza[16];
		int estatura;
		float peso;
		char genero;
	};
	
	struct param_t
	{
		int id;
		char nombre[32];
		struct dogType *dog;
	};

	char razas[][32] = {

		"Boxer", "Labrador", "Huskee", "Poodle", "Beagle",
		"Akita", "Dalmata", "Bulldog", "Pitbull", "Bull terrier",
		"Doberman", "Pastor Alemán", "Pastor collie", "Golden retriever", "Mestizo",
		"Pug", "Rottweiler", "Chow Chow", "Bulldog francés", "Chihuahua",
		"Cocker spaniel", "Cotón de tulear", "Dogo", "Fila brasileiro", "Galgo",
		"Gran danés", "Mastín", "Pastor Belga", "Pequinés", "Pinscher",
		"Sabueso", "Samoyedo", "San bernardo", "Schnauzer", "Shar pei",
		"Shiba", "ShihTzu", "Spaniel", "Teckel", "Terranova",
		"Terrier", "Yorshire Terrier", "Pastor Ovejero", "Springer", "Spitz"
	};

	char genero[] = {'h', 'm', 'H', 'M'};
	struct sockaddr_in server, client;
	struct sigaction sigIntHandler;
	struct tm *rawtime;
	const int SIZE_DATA_DOG = sizeof(struct dogType);

	//----
	int seeknode(long hashnumber);
	long push(struct dogType *dog);

	void * ingresar(void * pet);
	void * numreg(void);
	void * borrar(void * posreg);
	void * buscar(void * parametro);
	void * consulta(void * p);
	void cerrar(int s);

	int crear_hilo(pthread_t * hilo, void * rutina, void *arg);
	void * cerrar_hilo(void);
	void * listenInterrupt(void);
	
	void regresar(void (*src)(void), void (*dst)(void), char * message);
	void continuar(void (*dst)(void));
	void msg_log(char * message, int modo);
	void menu(void);
	//---

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
		int i, ch, counter = 0;

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

		int i;
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


	int leer(void * data, size_t size)
	{
		if(fread(data, size, 1, fp) <= 0)
		{
			msg_log("Error de lectura", 0);
			printf("\n\tError de lectura...");
			return 0;
		}

		fflush(fp);
		return 1;
	}

	int escribir(void * data, size_t size)
	{
		if(fwrite(data, size, 1, fp) <= 0)
		{
			msg_log("Error de escritura", 0);
			printf("\n\tError de escritura...");
			return 0;
		}

		fflush(fp);
		return 1;
	}

	long hash(const char *str)
	{
		long hashnumber;
		const char *ustr;

		ustr = (const char *) str;
	 
		hashnumber = 0;

		while(*ustr != '\0')
		{
			hashnumber = (hashnumber * 256 + *ustr) % SIZE_HASH;
			ustr++;
		}

		if(hashnumber < 0)
		{
			hashnumber *= -1;
		}

		return hashnumber;
	}

	long seekreg(long n, long hashnumber)
	{
		long poscur;
		long posnext;

		fseek(fp, 8 * (hashnumber + SIZE_HASH), SEEK_SET);
		fread(&posnext, 8, 1, fp);

		do
		{
			poscur = posnext;
			fseek(fp, poscur + SIZE_DATA_DOG, SEEK_SET);
			fread(&posnext, 8, 1, fp);

			if(posnext == n)
			{
				n = poscur;
				break;
			}
		}
		while(posnext != 0);

		if(n != poscur)
		{ return -1; }

		return n;
	}

	long push(struct dogType *dog)
	{
		long posreg, block = 0;
		
		fseek(fp, 0, SEEK_END);
		posreg = ftell(fp);

		fwrite(dog, SIZE_DATA_DOG, 1, fp);
		fwrite(&block, 8, 1, fp);

		fseek(fp, hash(dog->nombre) * 8, SEEK_SET);
		fread(&block, 8, 1, fp);

		fseek(fp, -8, SEEK_CUR);
		fwrite(&posreg, 8, 1, fp);

		if(block == 0)
		{
			fseek(fp, (8 * SIZE_HASH) - 8, SEEK_CUR);
			fread(&block, 8, 1, fp);

			if(block == 0)
			{
				fseek(fp, -8, SEEK_CUR);
				fwrite(&posreg, 8, 1, fp);
			}

			else
			{
				msg_log("Error en nodo de inicio", 0);
				printf("\n\n\tError en nodo de inicio No. %li", hash(dog->nombre));
			}
		}

		else
		{
			fseek(fp, block + SIZE_DATA_DOG, SEEK_SET);
			fwrite(&posreg, 8, 1, fp);
		}
		
		return posreg;
	}

	void * pop(void * parametro)
	{
		struct param_t *param;
		char msg[30];
		param = parametro;

		long hashnumber, block;
		hashnumber = hash(param->nombre);
		param->dog = malloc(SIZE_DATA_DOG);
		//printf("\nname: %s", param->nombre); fflush(stdout);

		fseek(fp, 8 * (hashnumber + SIZE_HASH), SEEK_SET);
		fread(&block, 8, 1, fp);

		if(block == 0)
		{
			enviar(clientfd[param->id], &block, 8);
		}

		else
		{
			do
			{
				fseek(fp, block, SEEK_SET);
				leer(param->dog, SIZE_DATA_DOG);
				
				//printf("\t::%s", param->dog->nombre); fflush(stdout);

				if(strcmp(param->nombre, param->dog->nombre) == 0)
				{
					block = ((block - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8)) + 1;
					
					if(enviar(clientfd[param->id], &block, 8) == 1)
					{
						enviar(clientfd[param->id], param->dog, SIZE_DATA_DOG);
						sprintf(msg, "Busqueda [ %li | %s ]", block, param->nombre);
						//printf("\n msg: %s", msg); fflush(stdout);
						msg_log(msg, 1);
					}
				}

				fread(&block, 8, 1, fp);
			}
			while(block != 0);

			enviar(clientfd[param->id], &block, 8);
		}
		
		free(param->dog);
	}

	void truncar(long n)
	{
		int i;
		void * buffer;
		char * dogName;
		long * posnext;
		long aux, tambuffer, nregisters;

		dogName = malloc(32);
		posnext = malloc(8);
		
		//printf("\nn = %li\n", n); fflush(stdout);

		n = (n - 1) * (SIZE_DATA_DOG + 8) + (SIZE_HASH * 16);

		fseek(fp, 0, SEEK_END);
		aux = ftell(fp);
		nregisters = ((aux - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8)) - 1;

		fseek(fp, n, SEEK_SET);
		leer(dogName, 32);

		fseek(fp, SIZE_DATA_DOG - 32, SEEK_CUR);
		leer(posnext, 8);

		tambuffer = aux - ftell(fp);

		if(tambuffer != 0)
		{
			buffer = malloc(tambuffer);
			leer(buffer, tambuffer);

			fseek(fp, n, SEEK_SET);
			escribir(buffer, tambuffer);

			free(buffer);
		}

		ftruncate(fileno(fp), aux - (SIZE_DATA_DOG + 8));

		//--
		long hashnumber = hash(dogName);
		free(dogName);

		fseek(fp, 8 * hashnumber, SEEK_SET);
		leer(&aux, 8);

		// Ultimo nodo
		if(aux == n)
		{
			fseek(fp, (8 * SIZE_HASH) - 8, SEEK_CUR);
			fread(&aux, 8, 1, fp);

			if(aux == n)
			{
				aux = 0;
				fseek(fp, 8 * hashnumber, SEEK_SET);
				fwrite(&aux, 8, 1, fp);

				fseek(fp, (8 * SIZE_HASH) - 8, SEEK_CUR);
				fwrite(&aux, 8, 1, fp);
			}

			else
			{
				aux = 0;
				long posback = seekreg(n, hashnumber);

				if(posback == -1)
				{
					msg_log("(NU) - Error en busqueda de registro", 0);
					printf("\n\n\t(NU) - Error en busqueda de registro No. %li", n);
				}

				else
				{
					fseek(fp, -8, SEEK_CUR);
					fwrite(&aux, 8, 1, fp);

					fseek(fp, 8 * hashnumber, SEEK_SET);
					fwrite(&posback, 8, 1, fp);
				}
			}
		}

		else
		{
			fseek(fp, (8 * SIZE_HASH) - 8, SEEK_CUR);
			leer(&aux, 8);

			// Primer nodo 
			if(aux == n)
			{
				fseek(fp, -8, SEEK_CUR);
				fwrite(posnext, 8, 1, fp);
			}

			// Nodos intermedios
			else
			{
				long posback = seekreg(n, hashnumber);

				if(posback == -1)
				{
					msg_log("(NI) - Error en busqueda de registro", 0);
					printf("\n\n\t(NI) - Error en busqueda de registro No. %li", n);
				}

				else
				{
					fseek(fp, -8, SEEK_CUR);
					fwrite(posnext, 8, 1, fp);
				}
			}
		}

		//---
		rewind(fp);

		for(i = 0; i < 2 * SIZE_HASH; i++)
		{
			fread(&aux, 8, 1, fp);

			if(aux > n)
			{
				aux -= (SIZE_DATA_DOG + 8);
				fseek(fp, -8, SEEK_CUR);
				fwrite(&aux, 8, 1, fp);
			}
		}

		fseek(fp, 16 * SIZE_HASH, SEEK_SET);

		for(i = 0; i < nregisters; i++)
		{
			fseek(fp, SIZE_DATA_DOG, SEEK_CUR);
			fread(&aux, 8, 1, fp);

			if(aux > n)
			{
				aux -= (SIZE_DATA_DOG + 8);
				fseek(fp, -8, SEEK_CUR);
				fwrite(&aux, 8, 1, fp);
			}
		}

		free(posnext);
	}

	void formatear()
	{
		int i;
		block = 0;
		j = 2 * SIZE_HASH;

		for(i = 0; i < j; i++)
		{
			fwrite(&block, 8, 1, fp);
		}

		printf("\n\n\tProceso de formato finalizado.");
	}

	int enviar(int clientfd, void * data, size_t size)
	{
		int r;
		r = send(clientfd, data, size, 0);
		
		if(r == -1)
		{
			msg_log("Error en enviar", 1);
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
			msg_log("Error en recibir", 1);
			perror("\n\t Error en recibir");
			return 0;
		}
		
		return 1;
	}
	
	int crear_hilo(pthread_t * hilo, void * rutina, void *arg)
	{
		int r = pthread_create(hilo, NULL, rutina, arg);

		if (r != 0)
		{
			msg_log("Fallo en crear hilo", 0);
			perror("Fallo en crear hilo");
			return 0;
		}
		
		return 1;
	}
	
	int crear_tuberia(int * tuberia)
	{
		int r = pipe(tuberia);

		if (r != 0)
		{
			msg_log("Fallo en crear hilo", 0);
			perror("Fallo en crear hilo");
			return 0;
		}
		
		write(tuberia[1], "T", 1);
		return 1;
	}
	
	void * cerrar_hilo(void)
	{
		int i;
		while(1)
		{
			for(i = 0; i < BACKLOG; i++)
			{
				if(flagThread[i] == -3)
				{
					pthread_join(hilo[i], NULL);
					clientfd[i] = -2;
					flagThread[i] = -2;
				}
			}
			
			if(flagInterrupt == 1)
			{ break; }
		}
	}
	
	void * listenInterrupt(void)
	{
		while(1)
		{
			sigaction(SIGINT, &sigIntHandler, NULL);
			
			if(flagInterrupt == 1)
			{ break; }
		}
	}
	
	void msg_log(char * message, int modo)
	{
		time(&t);
		rawtime = localtime(&t);
		strftime(tiempo, 30, "%Y/%m/%d - %I:%M:%S %p", rawtime);
		
		//sem_wait(sem_log);
		//pthread_mutex_lock(&mutex_log);
		int c;
		read(pipe_log[0], &c, 1);
		
		if(modo == 0)
		{
			fprintf
			(
				_log, "%s %s\n",
				tiempo, message
			);
		}
		
		else
		{
			fprintf
			(
				_log, "%s %d.%d.%d.%d %s\n",
				tiempo,	
				(int)(client.sin_addr.s_addr&0xFF),
				(int)((client.sin_addr.s_addr&0xFF00)>>8),
				(int)((client.sin_addr.s_addr&0xFF0000)>>16),
				(int)((client.sin_addr.s_addr&0xFF000000)>>24),
				message
			);
		}
		
		fflush(_log);
		//sem_post(sem_log);
		//pthread_mutex_unlock(&mutex_log);
		write(pipe_log[1], "T", 1);
	}

	void main(void)
	{
		int i;
		fp = fopen(NAME_FILE, "rb+");
		_log = fopen(NAME_LOG, "ab+");
		fflush(fp);
		
		system("clear");

		if(fp == NULL)
		{
			printf
			(
				"\n\t%s%s%s\n\t%s\n\t",
				"El archivo \"", NAME_FILE, "\" no fue encontrado...",
				"¿Desea crearlo? [S/N]:"
			);

			scanchar(1, &opcion, "SYNsyn");

			if(opcion == 'N' || opcion == 'n')
			{ exit(-1); }

			else
			{
				fp = fopen(NAME_FILE, "w");
				freopen(NAME_FILE, "rb+", fp);
				formatear();

				printf
				(
					"\n\t%s\n\t",
					"¿Desea generar información aleatoria? [S/N]:"
				);

				scanchar(1, &opcion, "SYNsyn");

				if(opcion == 'Y' || opcion == 'y' || opcion == 'S' || opcion == 's')
				{
					FILE *petNames;
					struct dogType *dog;
					dog = malloc(SIZE_DATA_DOG);
					petNames = fopen("nombresMascotas.dat" , "rb+");

					if(petNames == NULL)
					{
						printf("\n\n\tEl archivo \"%s\" no existe.", "nombresMascotas.dat");
					}

					else
					{
						int k;
						srand((unsigned) time(&t));

						char * dogName;
						dogName = malloc(32);

						for(k = 0; k < 1000000; k++)
						{
							fseek(petNames, ((rand() % 1716) * 32), SEEK_SET);
							fgets(dogName, 32, petNames);
							strcpy(dog->nombre, dogName);

							dog->edad = (rand() % 19) + 1;
							strcpy(dog->raza, razas[rand() % 44]);
							dog->estatura = (rand() % 19) + 1;
							dog->peso = (rand() % 123) + 1;
							dog->genero = genero[rand() % 3];

							push(dog);
						}

						free(dogName);
					}

					free(dog);
					fclose(petNames);
				}
			}
		}

		else
		{
			system("clear");
		}

		// Crear socket servidor
		serverfd = socket(AF_INET, SOCK_STREAM, 0);
		
		if(serverfd == -1)
		{
       		perror("\n\t Error en la creación del socket...");
   		}
   		
   		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);
		server.sin_addr.s_addr = INADDR_ANY;
		bzero(server.sin_zero, 8);
		
		sigIntHandler.sa_handler = cerrar;
		sigemptyset(&sigIntHandler.sa_mask);
		sigIntHandler.sa_flags = 0;
		
		r = bind (serverfd, (struct sockaddr *) &server, sizeof(struct sockaddr));
		
		if(r == -1)
		{
			msg_log("Error en la configuración del socket", 1);
    		perror("\n\t Error en la configuración del socket...");
		}
		
		else
		{
			r = listen(serverfd, BACKLOG);
		
			if(r == -1)
			{
				msg_log("Error en función listen", 1);
	    		perror("\n\t Error en función listen()...");
			}
			
			else
			{
		
				for(i = 0; i < BACKLOG; i++)
				{
					clientfd[i] = -2;
					flagThread[i] = -2;
				}
		
				/*
				sem_ingresar = sem_open("sem_ingresar", O_CREAT, 0666, 2);
				sem_numreg = sem_open("sem_numreg", O_CREAT, 0666, 2);
				sem_log = sem_open("sem_log", O_CREAT, 0666, 2);
				*/
				/*
				pthread_mutex_init(&mutex_ingresar, NULL);
				pthread_mutex_init(&mutex_numreg, NULL);
				pthread_mutex_init(&mutex_log, NULL);
				*/
				
				crear_tuberia(pipe_ingresar);
				crear_tuberia(pipe_numreg);
				crear_tuberia(pipe_log);
				
				crear_hilo(&centinela[0], cerrar_hilo, NULL);
				crear_hilo(&centinela[1], listenInterrupt, NULL);

				while(1)
				{
					for(i = 0; i < BACKLOG; i++)
					{
						if(clientfd[i] == -2)
						{
							printf("aceptando... %i", i); 
							fflush(stdout);
							clientfd[i] = accept(serverfd, (struct sockaddr *) &client, &sin_size);
					
							if(clientfd[i] == -1)
							{
								msg_log("Error en recibir la conexión", 1);
								perror("\n\t Error en recibir la conexión...");
							}

							else
							{
								printf("creando hilo...\n");
								fflush(stdout);
								crear_hilo(&hilo[i], consulta, &flagThread[i]);
							}
						}
						
						if(flagInterrupt == 1)
						{ break; }
					}
				}
			}
		}
		
		while(1)
		{
			if(flagInterrupt == 3)
			{ break; }
		}
	}

	void * ingresar(void * pet)
	{
		struct dogType *dog = pet;

		//sem_wait(sem_ingresar);
		//pthread_mutex_lock(&mutex_ingresar);
		int c;
		read(pipe_ingresar[0], &c, 1);
		
		long posreg = push(dog);

		//sem_post(sem_ingresar);
		//pthread_mutex_unlock(&mutex_ingresar);
		write(pipe_ingresar[1], "T", 1);

		msg_log("Insercion", 1);
		return (void*) ((posreg - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8)) + 1;
	}

	void * numreg(void)
	{
		//sem_wait(sem_numreg);
		//pthread_mutex_lock(&mutex_numreg);
		int c;
		read(pipe_numreg[0], &c, 1);

		fseek(fp, 0, SEEK_END);
		long nregisters = (ftell(fp) - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8);
	
		//sem_post(sem_numreg);
		//pthread_mutex_unlock(&mutex_numreg);
		write(pipe_numreg[1], "T", 1);
		return (void*) nregisters;
	}
	
	void * ver(void * posreg)
	{	
		//printf("\nrecibir: %li\n", (long)posreg);
		
		struct dogType *dog;
		dog = malloc(SIZE_DATA_DOG);

		fseek(fp, ((long)posreg - 1) * (SIZE_DATA_DOG + 8) + (SIZE_HASH * 16), SEEK_SET);
		
		if(leer(dog, SIZE_DATA_DOG) == 1)
		{ 
			msg_log("Lectura", 1);
			return (void*) dog;
		}
		
		else
		{ return 0; }
	}
	
	void * borrar(void * posreg)
	{
		//sem_wait(sem_ingresar);
		//pthread_mutex_lock(&mutex_ingresar);
		int c;
		read(pipe_ingresar[0], &c, 1);
		
		truncar((long) posreg);
		msg_log("Borrado", 1);	
			
		//sem_post(sem_ingresar);	
		//pthread_mutex_unlock(&mutex_ingresar);
		write(pipe_ingresar[1], "T", 1);
	}

	void cerrar(int s)
	{
		int i;
		flagInterrupt = 1;
		printf("\ncerrando...\n"); fflush(stdout);
	
		for(i = 0; i < BACKLOG; i++)
		{
			pthread_join(hilo[i], NULL);
		}
	
		printf("\nok\n"); fflush(stdout);
		pthread_join(centinela[0], NULL);
		
		/*
		sem_close(sem_ingresar);
		sem_close(sem_numreg);
		sem_close(sem_log);
	
		sem_unlink("sem_ingresar");
		sem_unlink("sem_numreg");
		sem_unlink("sem_log");
		*/
		
		/*
		pthread_mutex_destroy(&mutex_ingresar);
		pthread_mutex_destroy(&mutex_numreg);
		pthread_mutex_destroy(&mutex_log);
		*/
		
		close(pipe_ingresar[0]);	close(pipe_ingresar[1]);
		close(pipe_numreg[0]);		close(pipe_numreg[1]);
		close(pipe_log[0]);			close(pipe_log[1]);
		
		pthread_join(centinela[1], NULL);

		for(i = 0; i < BACKLOG; i++)
		{
			close(clientfd[i]);
		}
	
		close(serverfd);
		flagInterrupt = 3;
		exit(1);
	}
	
	void * consulta(void * p)
	{
		int opc, flag;
		int * id = p;
		void * valor_devuelto;
		long * posreg;
		pthread_t _hilo;
		struct dogType *dog;
		struct param_t * parametro;

		while(1)
		{
			flag = 0;

			if(recibir(clientfd[*id], &opc, 4) == 1)
			{
				printf("\nopcion: %i\n", opc);
				fflush(stdout);

				switch(opc)
				{
					case 1:
						dog = malloc(SIZE_DATA_DOG);

						if(recibir(clientfd[*id], dog, SIZE_DATA_DOG))
						{
							if(crear_hilo(&_hilo, ingresar, dog) == 1)
							{
								if(pthread_join(_hilo, &valor_devuelto) == 0)
								{
									enviar(clientfd[*id], &valor_devuelto, 8);
								}
							}
						}

						free(dog);
						break;
			
					case 2: case 3:
						crear_hilo(&_hilo, numreg, NULL);

						if(pthread_join(_hilo, &valor_devuelto) == 0)
						{					
							if(enviar(clientfd[*id], &valor_devuelto, 8) == 1)
							{
								if(recibir(clientfd[*id], &valor_devuelto, 8) == 1)
								{
									posreg = valor_devuelto;
									printf("\n%li\n", (long) posreg);
									fflush(stdout);
							
									crear_hilo(&_hilo, ver, valor_devuelto);
							
									if(pthread_join(_hilo, &valor_devuelto) == 0)
									{
										dog = valor_devuelto;
										//printf("\n%s\n", dog->nombre);
										enviar(clientfd[*id], dog, SIZE_DATA_DOG);
										free(dog);
									}
							
									if(opc == 3)
									{
										if(recibir(clientfd[*id], &valor_devuelto, 8) == 1)
										{									
											if((long) valor_devuelto == 1)
											{											
												crear_hilo(&_hilo, borrar, posreg);
														
												if(pthread_join(_hilo, NULL) == 0)
												{
													enviar(clientfd[*id], &opc, 4);
												}
											}								
										}
									}
								}
							}
						}
	
						break;	
			
					case 4:
						parametro = malloc(sizeof(struct param_t));
						
						if(recibir(clientfd[*id], parametro->nombre, 32))
						{
							printf("\n%s", parametro->nombre); fflush(stdout);
							parametro->id = *id;
							parametro->dog = dog;

							if(crear_hilo(&_hilo, pop, parametro) == 1)
							{
								if(pthread_join(_hilo, &valor_devuelto) == 0)
								{
									free(parametro);
								}
							}
						}
	
						break;

					default:
						flagThread[*id] = -3;
						flag = -1;
						break;
				}
			}

			if(flag == -1)
			{ break; }	
		}
	}
