#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define SIZE_HASH 10000
/*THIS VERSION  WORK VERY GOOD (Y)*/

int const multiplier=263;
long const prime =1000000007;

int i, j ;
void menu(void);
char opcion;

unsigned long head[SIZE_HASH]={0,0,0,0,0,0,0,0,0,0};  //Dimencion de nuestra tabla Hash cabezas
//bzero(head,10);
unsigned long tail[SIZE_HASH]={0,0,0,0,0,0,0,0,0,0};  //Dimencion de nuestra tabla Hash colas 
//bzero(tail,10);
    


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

    int fid,status;
    FILE *fp;		/*Puntero para crear el archivo*/
    caddr_t mmap_ptr; /*Puntero cabezera mmap*/
    struct dogType dog;
    
    struct dogType * dogP;


	void salir()
	{
		
		fclose(fp);
		exit(0);
	}	
	
    	void continuar(void (*dst)(void))
	{
		printf("\n\n\tPresione cualquier tecla para continuar...");
		getch();
		system("clear");
		(*dst)();
	}


unsigned long send(long action ){

	switch(action) {
	      case -1 :
		fseek(fp,0,SEEK_SET);/* enviamos el puntero al Inicio*/	
		return ftell(fp);
		break;
	      case -2:
		fseek(fp,0,SEEK_END);/* enviamos el puntero al final */	
		
		return ftell(fp);
		break;
	      default :
		fseek(fp,action,SEEK_SET);/* enviamos el puntero a posicion dada en 
					bytes contada desde el inicio */	
		return ftell(fp);
		break;	
	}

}

unsigned long  remmap(){

     mmap_ptr = mmap((caddr_t) 0,   /* Memory Location, 0 means O/S chooses */
            send(-2),/* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    fid,           /* which file is associated with mmap */
		    (off_t) 0);    /* Offset in page frame */
     return 1;
}


struct dogType * searchDog(unsigned long location){
    /*Encuentra y devuelve  el perro en la localizacion "location"*/       
    unsigned long x=(unsigned long) ((unsigned long )mmap_ptr + location); /*Movemos a la posicion del perro */	
    struct dogType * dog = (struct dogType * ) x;/*Casteamos para acceder al perro de forma facil */   
    return  dog;
  
} 


unsigned long  viewLocationHead(unsigned long Hash ){
    /*Retorna la posicion donde esta la cabeza de esa Hash*/
    unsigned long aux = (unsigned long ) mmap_ptr + ( 8 * Hash );
	unsigned long * point = (unsigned long *) aux;   
    return *point;
} 
unsigned long viewLocationTail(unsigned long Hash ){
    /*Retorna la posicion donde esta la cola de esa Hash*/
    unsigned long aux = (unsigned long )(( mmap_ptr )+( ( 8*Hash ) + (unsigned long )sizeof(head)));
	unsigned long * point = (unsigned long * ) aux;   
    return *point;
} 
unsigned long  updateHeadHash(unsigned long rank,unsigned long adress){
	/*actualiza la tabla de punteros Cabeza del Hash con la ubicacion del hash"rank "*/
	unsigned long x=(unsigned long) ((((unsigned long ) mmap_ptr+(rank*8)))); /*Movemos a la posicion del perro */	
    unsigned long * place = (unsigned long  * ) x;/*Casteamos para acceder al perro de forma facil */ 	
	*place=adress;	
}


unsigned long  updateTailHash(unsigned long rank,unsigned long adress){
	/*actualiza la tabla de punteros del Hash con la ubicacion del hash(rank)*/
    unsigned long x=(unsigned long) ((unsigned long )mmap_ptr+(rank*8)+(unsigned long )(sizeof(head))); /*Movemos a la posicion del perro */	
    unsigned long * place = (unsigned long  * ) x;/*Casteamos para acceder al perro de forma facil */ 
	*place=adress;	
}

void Mayus ( char *c ){ 
    /*Function UpperCase*/
    while( *c ){ 
        *c = toupper( *c ); 
        c++; 
    }  

}
unsigned long hashC(char * name) {
        Mayus(name);           
        unsigned  long nHash = 0;
        int i;
        for ( i= (sizeof(name)/sizeof(char)) - 1; i >= 0; --i){
            if(name[i]!='\0'&&name[i]!=0){
                nHash = (nHash * multiplier + name[i]) % prime;
            }
        }
        return (unsigned long )nHash % SIZE_HASH;
}
 

char * takeName() {   /*Analizar la posible eliminacion de esta funcion se usa 1 sola vez*/      
    char *  nombre;
    nombre=malloc(32*sizeof(char));
    bzero(nombre,32);
    printf( "Ingrese el nombre del animal: " );
    scanf("%s",nombre);  
    return nombre;
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

void scanchar(int length, char * character, char * dictionary){
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


unsigned long  inDog(){
    /*agregar NUEVO perro */
    unsigned long adressPri,adressTail,hash;
	char  aux [32];
	struct dogType dogPri;
	
	/*Recolección y validación de Datos */
	dogPri=getData(dogPri);	
	
	bzero(aux,32);
	strcpy(aux,dogPri.nombre);
	hash=hashC(aux); /* llamamos al  function hash */	
	
	adressPri=send(-2);	
	adressTail =  viewLocationTail(hash);
	
	    	if( adressTail == 0 ){			
			    /*PRIMER Perro que ingresa*/
			    
				updateHeadHash(hash,adressPri);/*ActualizoCabeza*/				
				updateTailHash(hash,adressPri);	/*ActualizoCola*/
				dogPri.next=0;
				dogPri.previus=0;				
					
			}else{				
			    /*SEGUNDO o MAYOR Perro que ingresa*/
			    			  			    
			    searchDog( adressTail ) -> next = adressPri;/*Actualizacion apuntador que lo contiene*/
			    dogPri.previus = adressTail;
			    updateTailHash(hash,adressPri);	/*ActualizoCola*/
				dogPri.next=0;
				
			}
		
		/*Guardo el perro en el archivo*/
		fwrite(&dogPri, sizeof(struct dogType),1, fp);
        
		remmap();
		continuar(menu);
	    return 1;	/*validar correcto 1*/
		
}


unsigned long numberDog(unsigned long adress){
    /*Registro con el que esta el perro */
    
    adress = ( adress - (unsigned long )(sizeof(head) + sizeof(tail)) );    
    return adress/(unsigned long )sizeof(struct dogType);
}
unsigned long adressDog(unsigned long number){
    /*Registro con el que esta el perro */
    
    number=number*((unsigned long)(sizeof(struct dogType)));
    return (number+(unsigned long)sizeof(head)+(unsigned long)sizeof(tail));
}

void ver(struct dogType *  pet){	
	 
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
            system("nano history.txt");
            free(history);            
            continuar(menu);
        }else{
            free(history);            
            continuar(menu);
		}
}



unsigned long viewReg(){ 
    /*Buscar un perro con el numero de registro */
    
    unsigned long endDog,size,search,end,* reg;
    //struct dogType send;    
    struct dogType * select; 
    int b;
    
    end=send(-2);
    endDog=end-(unsigned long )sizeof(struct dogType);  
    size =(sizeof(head)+sizeof(tail));
            
    if(numberDog(end) == 1){
            printf("\n\tNo existe ningun perro en el registro \n");
            printf("\n\tContinue en el menu principal\n");
            continuar(menu);
    }
    
    do{
        system("clear");
        printf("\n\tLa cantidad de registros actuales es de :| %lu |\n",numberDog(end));
        
        printf("\n\n\t\tIngrese el Numero de Registro a Buscar:");    
        scandigit(15,&b);
        

        if(b <= -1 || b > numberDog(end)-1){
        /*Validacion que el registro existe*/
            system("clear");
            printf("\n%s","Ese numero de registro no existe recuerde que la cuenta de los registros es desde 0");
            printf("\n Presione una tecla y intentelo de nuevo ");
            getch();
        }
    
    }while(b <= -1 || b > numberDog(end)-1);
   
    search=adressDog((unsigned long )b);
    ver(searchDog(search));

}




unsigned long  viewDog(char * name ){ 
    /*ver un perro con una cadena de caracteres coincidan totalmente*/
    unsigned long adress,hash;
    struct dogType * temp;
    char * save, * open;
    int i=0;
    
    open = malloc(32 * sizeof(char));
    bzero(open,32);
    save = malloc(32 * sizeof(char));
    bzero(save,32);
    
    Mayus(name);
    open=name;   
    hash=hashC(name);
   
    
    adress =  viewLocationHead( hash );
 
    
    printf("\n%s\n","|Nombre|--------------------------|NumeroRegistro|");
  

    while(adress!=0){
        temp=(searchDog( adress ));
        
        strcpy(save,temp->nombre);
        Mayus(save);
       
      if(strcmp(save,open) == 0){
           printf("|%s|",temp->nombre);
               for(i=strlen(temp->nombre);i<32;i++){
                    printf(" ");
               }
           printf("|%lu|\n",numberDog(adress));
       } 
       adress=temp->next;
    }
    getch();
    continuar(menu);

}


unsigned long   deleteDog(){ /*Eliminacion de un perro */
    remmap();
    unsigned long adress,aux,finalAdress,hashD,hashM;
    unsigned long * number=0;
    char *  nombre;
    struct dogType * move , * delete ,* deleteCopy;
    number=malloc(sizeof(unsigned long ));
    
    printf("\n\n\t\tIngrese el Numero de Registro a Eliminar:");
    //scandigit(10,number);
    scanf("%lu",number);
    adress=adressDog( *number );   /*Guardamos la direccion del perro a eliminar*/
	
	  
	finalAdress=(unsigned long)send(-2)-(unsigned long )sizeof(struct dogType); 	

    aux = (unsigned long ) mmap_ptr + finalAdress;	
	move = ( struct dogType * ) aux;   /*Recostruimos el perro a mover  */
	
	aux = (unsigned long ) mmap_ptr + adress;	
	delete = ( struct dogType * ) aux;   /*Recostruimos el perro a eliminar */
	
	if(number < 0 || adress > finalAdress ){ 
	  /*El perro no existe */
	    printf("\nEl perro No existe");
	    getch();
	    continuar(menu);	    
	}
	
    nombre=malloc(32*sizeof(char));
    bzero(nombre,32);
    strcpy(nombre,move->nombre);
    hashM=hashC(nombre);
    free(nombre);
    
    deleteCopy = malloc(sizeof(struct dogType));
    
    *deleteCopy=*delete;
       
    *delete=*move;  /*TRUCO cambiamos los datos que contiene el perro eliminado por el ultimo*/
       
    hashD=hashC(deleteCopy->nombre);
      
	if(move->previus!=0){
        /*ultimo perro a  mover no es una cabeza*/  
        searchDog(move->previus)->next=adress;       /*Actualizamos la direccion del perro que contiene el perro que vamos a mover*/	    
	}else{
	    /*Actualizamos la cabeza*/
	    updateHeadHash(hashM,adress);                      
    }
    
    
    updateTailHash(hashM,adress);     /*Actualizamos la cola pues sabemos que el ultimo perro SIEMPRE ES UNA COLA*/
	    
	
	if(deleteCopy->previus == 0 && deleteCopy->next == 0){
	/*Si un perro a eliminar es cabeza y cola*/
	        updateHeadHash(hashD,0); /*Actualizamos la cabez de hash */
	        updateTailHash(hashD,0); /*Actualizamos la cola de hash */
	}else{

        if(deleteCopy->previus!=0 ){
        /*si se va a eliminar un perro que no sea la cabeza*/
            searchDog(deleteCopy->next)->previus=deleteCopy->previus;   /*El perro siguiente se conecta con el perro anterior*/
            
        }else{
        /*Si este perro es cabeza*/
            updateHeadHash(hashD,deleteCopy->next); /*Actualizamos la cabez de hash */
            searchDog(deleteCopy->next)->previus=0;      /*guardo en el perro que el es la cabeza*/
        }
        
        
        if(deleteCopy->next!=0){
        /*si se va a aliminar un perro que no es la cola*/
            searchDog(deleteCopy->previus)->next=deleteCopy->next;/*el perro anterior se conecta con el siguiente pues el de la mitad desaparace*/
          
        }else{
            updateTailHash(hashD,deleteCopy->previus); /*Actualizamos la cola de hash */
            searchDog(deleteCopy->previus)->next=0;      /*guardo en el perro que sera la cola*/

        }

    }


    if(searchDog(deleteCopy->previus)->next == finalAdress){
          /*Si se va a eliminar un perro y el que se mueve va encima  */
          /***Eliminacion 3 caso especial***/  
          searchDog(deleteCopy->previus)->next=adress;
	      delete->next=0; 
    }    
   
    ftruncate(fileno(fp),send(finalAdress));      /*Truncamos el archivo donde este el cursor*/
    
    printf("\nEl animal con registro N:%lu y de nombre: %s Ha sido eliminado Correctamente ",*number,delete->nombre);
    
    getch();
    free(number);
    continuar(menu);
    return 1;

}


/* 
	*  Captura valor ingresado por usuario donde sólo
	*  permite ingresar los caracteres especificados
	*  en el diccionario.
	*/
	
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
	
void regresar(void (*src)(void), void (*dst)(void), char * message){
		printf("\n\t%s\n\t", message);
		scanchar(1, &opcion, "SYNsyn");
		system("clear");

		if(opcion == 'n' || opcion == 'N')
		{ (*src)(); }
		
		else
		{ (*dst)(); }
	}

static void error(char *mesg){
  perror(mesg);
  
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
        printf("Ingrese el Numero de la opcion deseada:");
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
				case '1':        inDog();	            break;
				case '2':        viewReg();         	break;
				case '3':        deleteDog();	        break;
				case '4':        viewDog(takeName());	break;
				default:         
				regresar(&menu, &salir, "¿Desea salir del programa? [S/N]:");
		    	}
		}
	}




int main(int argc, char *argv[]){

    char *file = "dataDogs.dat";	/*Nombre del archivo para la  comunicacion*/	


   fp= fopen(file, "rb+");	/*Apertura de Archivo !debe existir! lectura 	
	escritura en binario*/

    fid = fileno(fp);		/*Descriptor del archivo*/
    remmap();               /*Mapear el archivo*/

    
    if (mmap_ptr == MAP_FAILED){
     error("Parent Memory Map Failed, QUIT!");
    }

   
    menu();
    

    if (status == -1){  
      error("Bad munmap, QUIT!");
    }

    fclose(fp);
    return 0;
 
  }



