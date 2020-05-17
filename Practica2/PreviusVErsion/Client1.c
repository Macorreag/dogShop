#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 3535

/*Socket  MODO CLIENTE */


char opcion;
int clientfd,r; 

FILE * historia;

struct dogType
{
	char nombre[32];
	char tipo[32];
	int edad;
	char raza[16];
	int  estatura;
	float peso;
	char sexo;
	unsigned long previus;
	unsigned long next;
	
};

int  send_all (int socket ,void *buffer,size_t lenght);
/*Funcion para verificar el envio completo de datos */

int recibir(int clientfd, void * data, size_t size);

void scanchar(int length, char * character, char * dictionary);
void scanletter(int length, char * array);
void scandigit(int length, int * number);
void scandecimal(int length, float * decimal);


char* concat(const char *s1, const char *s2);
/*concatena dos Strings*/

int  ver(struct dogType *  pet);
/*mostrar los datos de un perro */

int openFileHistory(FILE * txt, char * name ,char * data,int size, int client);
/*crear escribir un archivo un archivo */

int send_file(char * name,int client);
/*enviar archivo */

int getch(void);
void menu();

struct dogType getData();


int main(int argc,char * argv[]){ 
//argv[0]Nombre executable,[1]primer parametro VA EL PUERTO (BACKLOG),[2]segundo parametro separado por espacio 
    
    
    clientfd=socket(AF_INET,SOCK_STREAM,0);
    /*  *AF_INET especifica coneccioón mediante ip_v4
        *SOCK_STREAM especifica tipo de tcp
    */
    
    struct sockaddr_in client;
    socklen_t size =sizeof(struct sockaddr_in );
    if(clientfd==0){
    
        perror("error socket");
        
    }
    client.sin_family=AF_INET;
    client.sin_port=htons(PORT); /*htons soluciona problema envianismo*/
    client.sin_addr.s_addr=inet_addr(argv[1]); 
    /*Configuracion de la Direccion con la que hacemos login
    *inet_addr() ->nos convierte el numero pasado a una direccion de acceso valido el parametro pasado esta en el argumento al compilar./     
    */
    bzero(client.sin_zero,8);
    
    
    r=connect(clientfd,(struct sockaddr *)&client,size);
    if(r==-1){
        perror ("ERROR in conect");/*error al conectarse al servidor */
    }
    /*ENVIO DE PETICIONES AL SERVIDOR */  
    
    
    
    
    menu();
    
    
    
    
    
    
    /*unsigned long a=9876;
    char listen[100];
    //send_all(clientfd,"b",1);
    //send_all(clientfd,&a,8);
    recibir(clientfd,listen,100) ;
    printf("%s",listen);
    fflush(stdout);
     
    //close(clientfd);
    send_file("archivo.txt",clientfd);
    */



}

int recibir(int clientfd, void * data, size_t size)
	{
		int r;
		r = recv(clientfd, data, size, 0);
		
		if(r == -1)
		{
			//msg_log("Error en recibir", 1);
			perror("\n No puedo recibir");
			return 0;
		}
		
		return 1;
	}


int  send_file(char * name,int client ){
      
      FILE* archivo = fopen(name,"r+");
      int i=0, c;
      if(archivo == NULL){
        FILE* archivo = fopen(name,"a+");
        send_all(client,&i,sizeof(i));
        fclose(archivo);
        return 0;  
      }
      
      char * buffer = NULL;
      
      c = fgetc(archivo);
      while(!feof(archivo)){
      
          buffer = (char*)realloc(NULL, sizeof(char));
            i = 0;
          while( c != '\n')
            {
              buffer[i] = c;
              i++;
              buffer = (char*)realloc(buffer, (i+1)*sizeof(char));
              c = fgetc(archivo);
            }
          /*Agrego el \n al buffer*/
          buffer = (char*)realloc(buffer, (i+2)*sizeof(char));
          buffer[i] = c;
          buffer[i+1] = 0;
          /*Trabajar con el buffer*/
          //fclose(archivo);
          
          fclose(archivo);
          send_all(client,&i,sizeof(i)); 
          send_all(client,buffer,i);
         
         return i;
          
    }
    
      //close(archivo);
}


int  send_all(int socket, void *buffer, size_t length)
{
    char *ptr = (char*) buffer;
    while (length > 0)
    {
        int i ;
        i = send(socket, ptr, length,0);
        if (i < 1) return 0;
        ptr += i;
        length -= i;
    }
    return 1;
}


struct dogType getData(){
    /*Recoleccion datos una  mascota*/
    struct dogType dog;
    /*Rellenar el perro de ceros */
    bzero(dog.nombre,32); 
    bzero(dog.tipo,32); 
	dog.edad=0;
	bzero(dog.raza,16);
	dog.estatura=0;
	dog.peso=0;
	dog.sexo='\0';
	dog.previus=0;
    dog.next=0; 


printf("\n\t--- Ingrese datos de la mascota ---");
			printf("\n\tNombre [32 max.]:\t");		scanletter(32, dog.nombre);     /*captura nombre */
			printf("\n\tTipo [32 max.]:\t\t");		scanletter(32, dog.tipo);    
			printf("\n\tEdad:\t\t\t");		        scandigit(3, &dog.edad);
			printf("\n\tRaza [16 max.]:\t\t");		scanletter(16, dog.raza);
			printf("\n\tEstatura [cm]:\t\t");		scandigit(3, &dog.estatura);
			printf("\n\tPeso [Kg]:\t\t");			scandecimal(6, &dog.peso);
			printf("\n\tGénero [M/F]:\t\t");		scanchar(1, &dog.sexo, "MFmf");

            
return dog;


}

void menu()
	{
	    int validate=0,b,letter=0,accept=0;
	    unsigned long number = 0;
	    struct dogType dogPri;
	    char * buffer;
	
	    
		printf
            	(
		        "\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\n\t",
		        "1. Ingresar registro",
		        "2. Ver registro",
		        "3. Borrar registro",
		        "4. Buscar registro",
		        "5. Salir"
            	);
        printf("Ingrese el Numero de la opcion deseada:");
		scanchar(1, &opcion, "12345");
        
		/*if(opcion == '5')
		{
			regresar(&menu, &salir, "¿Desea salir del programa? [S/N]:");
		}*/
		

		//else
	//	{
			system("clear");
			switch(opcion)
		    	{
				case '1':        
				    send_all(clientfd,"a",1);	   				    
				    dogPri=getData(dogPri);				    
				    send_all(clientfd,&dogPri,sizeof(struct dogType));
				   	recibir(clientfd,&validate,4) ;
				   	if(validate){
				       	printf("El perro fue Insertado");
				       	fflush(stdout);
				       	break;
				   	}else{
				   	    printf("error en el server");
				   	    fflush(stdout);
				   	    exit(1);
				   	}		    
				    
				case '2':        /*viewReg();*/
				    
				    send_all(clientfd,"b",1);
				    recibir(clientfd,&number,8) ;
				    printf("La cantidad de registros es de :%lu",number);
				   	fflush(stdout);
				   	//scanf("%i",&b); 
				    scandigit(15,&b);
				    if(b > -1 && b <= number){
				        send_all(clientfd,&b,4);
				        recibir(clientfd,&dogPri,sizeof(struct dogType)) ;
				        accept=ver(&dogPri);
				        send_all(clientfd,&accept,sizeof(accept));
				       if(accept){
				            fflush(stdout);
				            //
				            
				            recibir(clientfd,&letter,sizeof(letter)) ;
				            if(letter){
                                /*Existe un archivo en el servidor */
				                buffer = (char*)realloc(NULL, letter);
				                recibir(clientfd,buffer,letter) ;
				                printf("|%i |",letter);
				                printf("|%s|",buffer);
				                fflush(stdout);
				                historia = fopen(concat(concat (dogPri.nombre,dogPri.raza),".txt"),"w+");
				                fwrite(buffer, letter,1, historia);
				                printf("Hola");
				                fclose(historia);
				                system(concat(concat("nano ",concat (dogPri.nombre,dogPri.raza)),".txt"));
				                send_file(concat(concat (dogPri.nombre,dogPri.raza),".txt"),clientfd);
				                break;
    				            

				            }else{
				                /*No hay un archivo en el servidor */
				                system(concat(concat("nano ",concat (dogPri.nombre,dogPri.raza)),".txt"));
				                send_file(concat(concat (dogPri.nombre,dogPri.raza),".txt"),clientfd);
    				            printf("|%i|",letter);
    				            fflush(stdout);
				            }
				            
				            
				            //printf("%s",buffer);
				            //openFileHistory(h, concat(concat(dogPri.nombre,dogPri.raza),".txt") ,text,100,clientfd);
				            //system(concat(concat("nano ",concat(dogPri.nombre,dogPri.raza)),".txt"));				    
				    }
				    
				    }else{
				        printf("Numero erroneo intente nuevamente ");
				    
				    }
				
				    
				         	break;
				case '3':        /*deleteDog();*/	        break;
				case '4':        /*viewDog(takeName());*/	break;
				//default:         
				/*regresar(&menu, &salir, "¿Desea salir del programa? [S/N]:");*/
		    	}
		//}
	}


void scanchar(int length, char * character, char * dictionary){
		struct termios oldattr, newattr;
		int ch, counter = 0,i;

		tcgetattr(STDIN_FILENO, &oldattr);
		newattr = oldattr;
		newattr.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

		do{
			ch = getchar();

			if(ch == 127 && counter > 0){
			
				printf("\b \b");
				counter--;
				*character = '\0';
				
			}

			else if(counter < length){
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
		}while(ch != 10 || character[0] == '\0');

		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}

void scanletter(int length, char * array){
    /* 
	*  Captura valor ingresado por usuario donde sólo
	*  permite ingresar letras a-z, A-Z y espacio.
	*/
	struct termios oldattr, newattr;
	int ch, counter = 0;

	tcgetattr(STDIN_FILENO, &oldattr);
	newattr = oldattr;
	newattr.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

	do{
        ch = getchar();
    	if(ch == 127 && counter > 0){
				printf("\b \b");
				counter--;
				array[counter] = '\0';
		}else if( (counter < length) && ((ch == 32) ||
				 (ch >= 65 && ch <= 90) ||
				 (ch >= 97 && ch <= 122)) 
                ){
				    printf("%c", ch);
				    array[counter] = ch;
				    counter++;
			    }
        }while(ch != 10 || array[0] == '\0');

	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}

void scandigit(int length, int * number){
	/* 
	*  Captura valor ingresado por usuario donde sólo
	*  permite ingresar dígitos 0-9.
	*/
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
		}while(ch != 10 || array[0] == '\0');

		*number = atoi(array);
		free(array);
		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}
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

		do{
		    ch = getchar();
			if(ch == 127 && counter > 0){
				
				printf("\b \b");
				counter--;
				array[counter] = '\0';

				if((counter == separator) && (flag == 1)){
				    flag = 0; 
				}
			}else if( (counter < length) && (ch >= 48 && ch <= 57) ||
				 ((ch == 46) && (flag == 0)) 
				 ){
                        printf("%c", ch);
				        array[counter] = ch;

				        if(ch == 46)
				        {
					        flag = 1;
					        separator = counter;
				        }

				        counter++;
			        }
		}while(ch != 10 || array[0] == '\0');

		*decimal = atof(array);
		free(array);
		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}

int getch(void){
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
int ver(struct dogType *  pet){	
	 
    char * history;
    history = malloc(sizeof(char));      
	       
				printf(
					"\n\n\t%s \n\t%s\t\t%s \n\t%s\t\t%s \n\t%s\t\t%i \n\t%s\t\t%s \n\t%s\t%i%s \n\t%s\t\t%.2f%s \n\t%s\t\t%c \n\t%s",
					"------------ Mascota ------------",
					"Nombre:", pet->nombre,
					"Tipo:",pet->tipo,
					"Edad:", pet->edad,
					"Raza:", pet->raza, 
					"Estatura:", pet->estatura, " cm(s)",
					"Peso:", pet->peso, " Kg(s)",
					"Genero:", pet->sexo    ,
					"---------------------------------"
				);
          
        
        printf("\n\t Si desea ver la Historia Clinica presione 'S', si no presione 'N':");
        scanchar(1, history , "SNsn");  
        
        if( strcmp(history,"S") == 0  || strcmp(history,"s") == 0 ){
            //system("nano history.txt");
            free(history);            
            //continuar(menu);
            return 1;
        }else{
            free(history);            
            //continuar(menu);
            return 0;
		}
}
char * concat(const char *s1, const char *s2){

    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int openFileHistory(FILE * txt, char * name ,char * data,int size,int client){
       
       txt= fopen(name, "w+");
       fwrite(data,size,1, txt);
       fclose(txt);
       //close(fileno(txt));
       
       
        system(concat("nano ",name)); 
        //send_file(name, client);
        return 1;

}


