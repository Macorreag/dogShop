#include <sys/types.h>          
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#define PORT 3535
#define NAME_LOG "serverDogs.log"
#define BACKLOG 3


/*
    MAKEFILE  
    DOGFILE     se envia desde el cliente al servidor no es LOCAL
    MANUAL 
    DIAGRAMAS 
    
*/
FILE *fp;
FILE *fp_log;
/*descriptor del archivo del log */

pthread_t  hilo[BACKLOG];


time_t t;
struct tm *rawtime;
char tiempo[30];
/*controlan el envio de la hora en el Log*/

int serverfd;

socklen_t size_sin;

pthread_t centinela[2];
/*hilo de control sobre los demas hilos para verificar cuando se libera alguno*/

struct sockaddr_in server, client;

struct sigaction sigIntHandler;
/*Controlador de la señal de cierre*/
int serverfd,r;
    /* la variables r es el validador de errores
    *i es usado como  contador */

struct sockaddr_in server;
    //socklen_t size =sizeof(struct sockaddr_in);
int size_sock = sizeof(struct sockaddr);


int clientfd[BACKLOG];
/*Clientfd mantiene los descriptores de acceso de un cliente */

int flagThread[BACKLOG],flagInterrupt = 0;

/*flag Interrupt valida que el programa se cierre todo antes de morir */


int crear_hilo(pthread_t * hilo, void * rutina, void *arg);


void * attend(int clientfd);

void msg_log(char * message, int modo);
/*Recibe a un cliente */
void cerrar(int s );
void * cerrar_hilo(void);
/*cerrar todo para reutilizar el Socket */
void * listenInterrupt(void);





/*Configuración Socket MODO SERVIDOR*/


int main(){
  
    int i;
    
    fp_log = fopen( NAME_LOG, "ab+" );
    /*cargamos el arhivo para el log*/
   
    
    
        
    char time[30];
    
    serverfd=socket(AF_INET,SOCK_STREAM,0);
    /*creación server
    *(AF_INET->especifica comunicación IP_v4 es decir el dominio de     Internet
    * SOCK_STREAM tipo de coneccion en este caso se leera como archivo
    *Protocolo Si es 0 el SO escoje el mejor 
    */ 
    
    if(serverfd == -1){
    
        perror("Error Creacion Socket");
    
    }
    
  
   
    server.sin_family =AF_INET;
    server.sin_port=htons(PORT);
    /*htons ->Soluciona Problema del ORDEN al enviar datos "envianismo "*/
    server.sin_addr.s_addr=INADDR_ANY;  /*Inaddr_Any es constante '0' con formato de direccion ip*/
    bzero(server.sin_zero,8);/*rellena de zeros*/
    
    	//Se activa señal de cerrar todo si algo falla
		sigIntHandler.sa_handler = cerrar;
		sigemptyset(&sigIntHandler.sa_mask);
		sigIntHandler.sa_flags = 0;
	    	
    
    
    
    
    
    r=bind(serverfd,(struct sockaddr *)&server,size_sock    );/*bautizo socket*/
    
    
    if(r == -1){
        msg_log("Error en la configuración del socket", 1);		
    	perror("\n\t Error en la configuración del socket...");
    	
	}else{
	
	    r=listen( serverfd , BACKLOG ); /*configuracion de escucha*/
	    
	    if(r == -1){				
				msg_log("Error en función listen", 1);
	    		perror("\n\t Error en función listen()...");
	    		
		}else{
		    printf( " Escuchando... " );
			fflush(stdout);
			
		    for(i = 0; i < BACKLOG; i++){
		    
					clientfd[i] = -2;/*Limpiamos el array de            descriptores */
					flagThread[i] = -2; 
					
			}
			
			crear_hilo(&centinela[0], cerrar_hilo, NULL);
			crear_hilo(&centinela[1], listenInterrupt, NULL);    
			
			while( 1 ){
			    for(i = 0 ;i < BACKLOG; i++){
			       	if(clientfd[i] == -2){
							printf("aceptando... %i", i); 
							fflush(stdout);
							clientfd[i] = accept(serverfd, (struct sockaddr *) &client, &size_sin);
							/*Función bloqueante hasta la espera de la conexión*/                   
							printf("aceptvalido... %i", i); 
							fflush(stdout);
					
							if(clientfd[i] == -1){
								msg_log("Error en recibir la conexión", 1);
								perror("\n\t Error en recibir la conexión...");
							}else{
								printf("creando hilo...\n");
								fflush(stdout);
								flagThread[i] = i;
								crear_hilo(&hilo[i], attend(clientfd[i]), &flagThread[i]);
							}
				    }
						
					if(flagInterrupt == 1){
					    break; }
			        
	           }		    
	        }
	    }
			
			
			
		 
	    
	    
	    
	    }
	
	
	
	}
    
    
   
    
    
    /*ATENCION A LOS CLIENTES POR LOS HILOS PARA GESTIONAR PETICIONES */  
    
    
    
  // for(i = 0;i < CLIENTS ; i++){
   
      //  pthread_t hilo;  /*identificador del hilo */  
      //  r=pthread_create(&hilo,NULL,attend(serverfd),NULL); 
      //r=pthread_create(&hilo2,NULL,attend(serverfd),NULL);    
     
             
    //attend(serverfd); 
    
    
    //}    
        
        
      //  if(r<0){
        //    perror("error_pthread_create");
        //}
    //}
    // printf("[%i]",client.sin_addr.s_addr);/*Capturamos la IP de coneccion*/ 
        
    //for(i = 0;i < CLIENTS ; i++){
        //hilo=pthread_self();/*retorna el descriptor del hilo en ejecucio*/
     ///r=pthread_join(hilo,NULL);  /*Espera terminacion del hilo "hilo"*/
     //r=pthread_join(hilo2,NULL);  /*Espera terminacion del hilo "hilo"*/
      //  if(r != 0){
            
        //    perror("error_pthread_join");
            
        //}
   // }
        
    //close(serverfd);
    //
    



void * attend(int clientfd){

   /*int r=0;
   char buffer[32];
    r=recv(clientfd,buffer,1,0);
   
    
   
    if(r<=0){
        perror("error en el msj");
    }
    
    printf("%s",buffer);
    fflush(stdout);
    //close(clientfd);
    
    
     //timeFormat(time);/*Capturamos la hora de ingreso */
    
    
    /*Limpia el buffer de  la salida */
    
    
    
    
        
}



int crear_hilo(pthread_t * hilo, void * rutina, void *arg){
	int r = pthread_create(hilo, NULL, rutina, arg);

	if (r != 0){
	
			perror("Fallo en crear hilo");
			return 0;
			
	}
		
		return 1;
}

void msg_log(char * message, int modo)
	{
		time(&t);
		rawtime = localtime(&t);
		strftime(tiempo, 30, "%Y/%m/%d - %I:%M:%S %p", rawtime);
		
		int c;
		//read(pipe_log[0], &c, 1);
		
		if(modo == 0)
		{
			fprintf
			(
				fp_log, "%s %s\n",
				tiempo, message
			);
		}
		
		else
		{
			fprintf
			(
				fp_log, "%s %d.%d.%d.%d %s\n",
				tiempo,	
				(int)(client.sin_addr.s_addr&0xFF),
				(int)((client.sin_addr.s_addr&0xFF00)>>8),
				(int)((client.sin_addr.s_addr&0xFF0000)>>16),
				(int)((client.sin_addr.s_addr&0xFF000000)>>24),
				message
			);
		}
		
		fflush(fp_log);
		//write(pipe_log[1], "T", 1);
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
void cerrar(int s){
		int i;
		flagInterrupt = 1;
		printf("\ncerrando...\n"); fflush(stdout);
	
		for(i = 0; i < BACKLOG; i++)
		{
			pthread_join(hilo[i], NULL);
		}
	
		printf("\nok\n"); fflush(stdout);
		pthread_join(centinela[0], NULL);
		
		//close(pipe_ingresar[0]);	close(pipe_ingresar[1]);
		//close(pipe_numreg[0]);		close(pipe_numreg[1]);
		//close(pipe_log[0]);			close(pipe_log[1]);
		
		pthread_join(centinela[1], NULL);

		for(i = 0; i < BACKLOG; i++)
		{
			close(clientfd[i]);
		}
	
		close( serverfd );
		flagInterrupt = 3;
		exit(1);
}
void * listenInterrupt(void){
		while(1)
		{
			sigaction(SIGINT, &sigIntHandler, NULL);
			
			if(flagInterrupt == 1)
			{ break; }
		}
}	
	



