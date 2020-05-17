	
	#include <termios.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sys/types.h>

	void menu(void);
	int searchnode(long hashnumber, long dircluster);

	FILE *archivo;
	
	char opcion;
	char file[] = "data.dat";
	int i, j, nregisters = 0;
	long posreg, posnode = 0, block = 0, test;
    
	struct dogType
	{
		char nombre[32];
		int edad;
		char tipoAnimal[32];
		char raza[16];
		int estatura;
		float peso;
		char genero;
	};

	struct dogType *pet;

	

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
				 (ch >= 97 && ch <= 122)) )
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
	void scanuldigit(int length, long * number)
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

		*number = strtoul(array, NULL, 10);
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
	void toLowerCase(char * str)
	{
		char *temp = strdup(string);
		unsigned char *tptr = (unsigned char *)temp;

		while(*tptr)
		{
		    *tptr = tolower(*tptr);
		    tptr++;
		}

		free(temp);
	}
*/	
	/*Funcion Hash (muchas Coliciones )*/
	long hash(const char *str)
	{
		long hashnumber;
		const char *ustr;

		ustr = (const char *) str;
	 
		hashnumber = 0;

		while(*ustr != '\0')
		{
			hashnumber = (hashnumber * 256 + *ustr) % 1000;
			ustr++;
		}

		return hashnumber;
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

	int abrir(char * fuente, char * modo)
	{
		archivo = fopen(fuente, modo);
	
		if(archivo == NULL)
		{
			printf
			(
				"\n\t%s%s%s\n\t%s\n\t",
				"El archivo \"", fuente, "\" no fue encontrado...",
				"¿Desea crearlo? [S/N]:"
			);

			scanchar(1, &opcion, "SYNsyn");

			if(opcion == 'N' || opcion == 'n')
			{ return 0; }

			else
			{
				if(abrir(fuente, "w") == 1)
				{
					if(abrir(fuente, modo) == 1)
					{
						printf("\n\t%s%s%s", "Generando \"", file, "\"...");
						return 2;
					}

					else
					{
						printf("\n\t%s%s%s", "Error generando archivo\"", file, "\".");
						return 0;
					}
				}

				else
				{ return 0; }
			}
		}

		return 1;
	}

	int leer(void * registro, size_t tam, size_t nregistros)
	{
		if(fread(registro, tam, nregistros, archivo) < 0)
		{
			printf("\n\tError de lectura...");
			return 0;
		}

		return 1;
	}
	/*Ingreso datos en el archivo with fwrite  */
	int escribir(void * registro, size_t tam, size_t nregistros)
	{
		if(fwrite(registro, tam, nregistros, archivo) < 0)
		{
			printf("\n\tError de escritura...");
			return 0;
		}

		fflush(archivo); /*Redimencion Archivo*/
		return 1;
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

	int numreg()
	{
		rewind(archivo);
		if(leer(&nregisters, 4, 1) == 1)
		{
			printf("\n\tRegistros actuales: %i", nregisters);

			if(nregisters != 0)		
			{
				printf("\n\tIngrese número de registro:\n\t");
				scanuldigit(10, &posreg);

				// Cambiar por busqueda
				if(posreg > nregisters)
				{
					printf("\n\tFuera del rango.");
					return 2;
				}
				//*********************

				else
				{ return 1; }
			}

			else
			{ return 2; }
		}

		else
		{
			printf("\n\tFallo en obtener número de registros.");
			return 0;
		}

		return 1;
	}

	void ingresar(void)
	{
		system("clear");
		rewind(archivo);

		if(leer(&nregisters, 4, 1) == 1)
		{
			printf("\n\t--- Ingrese datos de la mascota ---");
			printf("\n\tNombre [32 max.]:\t");		scanletter(32, pet->nombre);
			printf("\n\tEdad:\t\t\t");			scandigit(3, &pet->edad);
			printf("\n\tRaza [16 max.]:\t\t");		scanletter(16, pet->raza);
			printf("\n\tEstatura [cm]:\t\t");		scandigit(3, &pet->estatura);
			printf("\n\tPeso [Kg]:\t\t");			scandecimal(6, &pet->peso);
			printf("\n\tGénero [H/M]:\t\t");		scanchar(1, &pet->genero, "HMhm");

			if(abrir(file, "ab") == 1)
			{
				nregisters++;

				escribir(&pet->nombre, sizeof(pet->nombre), 1);
				escribir(&pet->edad, 4, 1);
				escribir(&pet->raza, sizeof(pet->raza), 1);
				escribir(&pet->estatura, 4, 1);
				escribir(&pet->peso, 4, 1);
				escribir(&pet->genero, 1, 1);

				if(abrir(file, "rb+") == 1)
				{
					rewind(archivo);

					if(escribir(&nregisters, 4, 1) == 1)
					{
						printf
						(
							"\n\n\t%s\n\t%s%li.",
							"Datos registrados satisfactoriamente",
							"No. actual de registro asignado: ",
							posnode
						);
					}
				}
			}

			/*printf(
				"\n\t%i\n\t%s\n\t%i\n\t%s\n\t%i\n\t%f\n\t%c",
				pet->id, pet->nombre, pet->edad, pet->raza, 
				pet->estatura, pet->peso, pet->genero
			);*/
		}

		else
		{
			printf("\n\tFallo en obtener número de registros.");
		}


		continuar(&menu);
	}

	void ver(void)
	{	
		
		if(abrir(file, "rb") == 1)
		{
			if(numreg() == 1)
			{
				

				leer(&pet->nombre, sizeof(pet->nombre), 1);

				strcpy(pet->nombre, "THE BOSS");
				leer(&pet->edad, 0, 1);
				leer(&pet->raza, sizeof(pet->raza), 1);
				leer(&pet->estatura, 4, 1);
				leer(&pet->peso, 4, 1);
				leer(&pet->genero, 1, 1);

				printf(
					"\n\n\t%s \n\t%s\t\t%s \n\t%s\t\t%i \n\t%s\t\t%s \n\t%s\t%i%s \n\t%s\t\t%.2f%s \n\t%s\t\t%c \n\t%s",
					"------------ Mascota ------------",
					"Nombre:", pet->nombre,
					"Edad:", pet->edad,
					"Raza:", pet->raza, 
					"Estatura:", pet->estatura, " cm(s)",
					"Peso:", pet->peso, " Kg(s)",
					"Genero:", pet->genero,
					"---------------------------------"
				);
				
			}
		}

		continuar(&menu);
	}

	void borrar(void)
	{
		if(numreg() == 1)
		{
			
		}

		continuar(&menu);
	}
    
	void buscar(void)
	{
		system("clear");
		continuar(&menu);
	}

f	void salir()
	{
		free(pet);
		fclose(archivo);
		exit(0);
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
				case '1':        ingresar();	break;
				case '2':        ver();		break;
				case '3':        borrar();	break;
				case '4':        buscar();	break;
				default:         
				regresar(&menu, &salir, "¿Desea salir del programa? [S/N]:");
		    	}
		}
	}

	void formatear()
	{
		/* Bloque de números de registros activos */
		block = 0;
		escribir(&block, 8, 1);

		/* Bloque de direcciones de cada nodo */
		for(i = 0; i < 1000; i++)
		{
			block = (8816 * i) + 8016;
			escribir(&block, 8, 1);
		}

		block = -1;
		escribir(&block, 8, 1);

		/* Bloques de almacenamiento para cada nodo del arreglo */
		for(i = 2; i < 1002; i++)
		{	
			block = -i;
			escribir(&block, 8, 1);

			block = 0;

			for(j = 0; j < 1100; j++)
			{
				escribir(&block, 8, 1);
			}

			block = -1;
			escribir(&block, 8, 1);
		}

		printf("\n\tProceso de formato finalizado.");
		block = 0;
	}

	void push(struct dogType *pet)
	{
		abrir(file, "ab");
		escribir(pet, sizeof(struct dogType), 1);
		posreg = ftell(archivo) - sizeof(struct dogType);

		printf("\n%li\n", hash(pet -> nombre));
		seeknode(hash(pet -> nombre));

//		if(seeknode(hash(pet -> nombre)) == 1)
//		{
/*			do
			{
				leer(&block, 8, 1);

				if(block == -1)
				{
					// control de colisión
					break;
				}
			}
			while(block != 0);

			fseek(archivo, -8, SEEK_CUR);
			escribir(&posreg, 8, 1);*/
//		}
	}
/*
	void pop(char * parameter)
	{
		if(seeknode(hash(parameter)) == 1)
		{
			char * name = malloc(32);
			leer(name, 32, 1);

			if(strcmp(pet->nombre, name) == 0)
			{
				fseek(archivo, -32, SEEK_CUR);
				leer(pet, sizeof(struct dogType), 1);
			}
		}
	}
*/
	int seeknode(long hashnumber)
	{
		if(hashnumber < 0 || hashnumber > 1000)
		{
			printf("\n\tError en número hash: %li", hashnumber);
			return 0;
		}

		else
		{
			printf("\nhash: %li\n", hashnumber);
			// Ubica la dirección del nodo
			fseek(archivo, (hashnumber * 8) + 8, SEEK_SET);
			printf("\nposnode: %li\n", ftell(archivo));
			
			if(leer(&posnode, 8, 1) == 1)
			{
				printf("\ndirnode: %li\n", posnode);

/*				if(posnode == -1)
				{
					printf("\n\tError en búsqueda de nodo.");
					return 0;
				}

				else
				{
					if(block == 0)
					{ return searchnode(hashnumber, posnode); }
			
					else
					{ return searchnode(block, posnode); }
				}*/
			}
		}
	}

	int searchnode(long hashnumber, long dircluster)
	{
		long cluster;

		// Lectura del inicio del nodo
		fseek(archivo, dircluster, SEEK_SET);
		leer(&cluster, 8, 1);

		// Verificación de nodo
		if(cluster < -1 && -hashnumber-2 == cluster)
		{
			//fseek(archivo, -8, SEEK_CUR);
			//leer(&test, 8, 1);
			//printf("\n%li\n", test);
			return 1;
		}

		// Fin del bloque de posiciones de memoria
		else if(cluster == -1)
		{
			j = 0;
			searchnode(hashnumber, dircluster + 8);
		}

		else if(cluster > -1)
		{
			if(j == 0)
			{ block = hashnumber; }

			j++;
			seeknode(j - 1);
		}
		
		else
		{
			printf("\n\tError en lectura de nodo.");
			return 0;
		}
	}

	void main(void)
	{
		system("clear");

		pet = malloc(sizeof(struct dogType));
		int state_file = abrir(file, "rb+");

		if(state_file == 0)
		{ exit(0); }

		else if(state_file == 2)
		{
			printf("\n\tArchivo creado satisfactoriamente.");
			formatear();
		}

		else
		{ system("clear"); }

		//seeknode(251);//seeknode(1);seeknode(2);seeknode(3);
		
		

		strcpy(pet->nombre, "lucas");
		pet->edad = 15;
		strcpy(pet->raza, "boxer");
		pet->estatura = 20;
		pet->peso = 30;
		pet->genero = 'M';
/*
		printf(
				"\n\t%s\n\t%i\n\t%s\n\t%i\n\t%f\n\t%c",
				pet->nombre, pet->edad, pet->raza, 
				pet->estatura, pet->peso, pet->genero
		);
*/
//		printf("%li\n", hash(pet -> nombre));
		struct dogType pet2;

		push(pet);
		fread(pet,sizeof(struct dogType),5,archivo ) ;
		menu();	
		printf("%s",pet->nombre);
		
		

		//menu();*/
	}
