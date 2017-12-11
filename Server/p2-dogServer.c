#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define CANT 2
#define SIZE_HASH 10
/*IIN THIS VERSION IT WORKS IN DOGS VERY GOOD (Y)*/

int const multiplier=263;
long const prime =1000000007;

unsigned long head[SIZE_HASH]={0,0,0,0,0,0,0,0,0,0};  //Dimencion de nuestra tabla Hash cabezas
//bzero(tail,10);
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

unsigned long  copyDog(struct dogType * A,struct dogType B){    /*Verificar si esta funcion es posible borrarla*/
    /*Copia el perro A en el perro B*/
    strcpy(B.nombre,A->nombre);
    strcpy(B.tipo,A->tipo );
    //strcpy(B.raza,A->raza );	    
	B.previus = A->previus;
	B.next = A->next;
/**/
    
        
    }

unsigned long  viewLocationHead(unsigned long Hash ){

    unsigned long aux = (unsigned long ) mmap_ptr + ( 8 * Hash );
	unsigned long * point = (unsigned long *) aux;   
    return *point;
    

} 
unsigned long viewLocationTail(unsigned long Hash ){

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

void Mayus ( char *c ){ /*Function UpperCase*/

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
 

char * takeName() {
    char *  nombre;
    nombre=malloc(32*sizeof(char));
    bzero(nombre,32);
    printf( "Ingrese el nombre del animal: " );
    scanf("%s",nombre);
  
    return nombre;
}




struct dogType getData(){
/*FALTA ALGUNAS VALIDACIONES !!!*/
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

strcpy(dog.nombre,takeName());


printf( "Ingrese el tipo de animal: " );
scanf("%s",dog.tipo);

printf( "Ingrese la edad del animal (En Numeros): " );
scanf("%i",&dog.edad);

printf( "Ingrese la raza del perro: " );
scanf( "%s", dog.raza); 

printf( "Ingrese la estatura del perro: " );

scanf( "%d", &dog.estatura ); 

printf( "Ingrese el peso del perro: " );

scanf( "%g", &dog.peso );

printf( "Ingrese el genero del perro: " );
getchar();

scanf( "%c", &dog.sexo );

return dog;


}





unsigned long  inDog(){/*agregar NUEVO perro */

    unsigned long adressPri,adressTail,AdressHead,hash;
	struct dogType dogPri;
	/*Recoleccion e ingreso de una mascota*/
	
	/*Recolección y validación de Datos */
	dogPri=getData(dogPri);
	
	/*FALTAN TODOS LOS DEMAS DATOS DE LA MASCOTA*/
	printf("%s",dogPri.nombre);
	char  aux [32];
	bzero(aux,32);
	//aux=malloc(32*(sizeof(char)));
	//aux=*dogPri.nombre;
	strcpy(aux,dogPri.nombre);
	//aux=dogPri.nombre;
	hash=hashC(aux); /* llamamos al  function hash */	
	
	adressPri=send(-2);
	//printf("|adr:%lu|",adressPri);
	 //bzero(dogPri.nombre,32);
    //strcpy(dogPri.nombre,name);	
    
	/*obtener el hash */
	
	adressTail =  viewLocationTail(hash);
	
	       printf("HashIngreso:%lu\n",hash);
	       //printf(":%lutail",adressPri);
			if( adressTail == 0 ){
			    
				
			    /*PRIMER Perro que ingresa*/			
				updateHeadHash(hash,adressPri);/*ActualizoCabeza*/				
				updateTailHash(hash,adressPri);	/*ActualizoCola*/
				dogPri.next=0;
				dogPri.previus=0;
				
				
					
			}else{	
			
			    /*SEGUNDO o MAYOR Perro que ingresa*/
			  	//printf(":%luhas",hash);		    
			    searchDog( adressTail ) -> next = adressPri;/*Actualizacion apuntador que lo contiene*/
			    
			    dogPri.previus = adressTail;
			    
			    updateTailHash(hash,adressPri);	/*ActualizoCola*/
				dogPri.next=0;
				
			}
		
		/*Guardo el perro en el archivo*/
		fwrite(&dogPri, sizeof(struct dogType),1, fp);
		remmap();
	    return 1;	/*validar correcto 1*/
		
};

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

unsigned long viewReg(){ /*Buscar un perro con el numero de registro */
    /*Validacion que el registro existe*/
    unsigned long endDog,size,search,end,* reg;
    endDog=end-(unsigned long )sizeof(struct dogType);   
    size =(sizeof(head)+sizeof(tail));
    end=send(-2);
    
    if(end<=size){
        printf("La cantidad de registros actuales es de : 0");
        return 2;       
    }else{
        printf("La cantidad de registros actuales es de : %lu |",numberDog(end));
    }
   
    reg=malloc(sizeof(unsigned long));
    printf("\nIngrese el Numero de Registro a Buscar:");
    scanf("%lu",reg);
    search=adressDog(*reg);
   
    if((endDog>=search)){
    /*El perro existe */      
        printf("Nombre:%s|°",searchDog( search )->nombre);  
        /*IMPRESION DE LOS DEMAS DATOS DEL REGISTRO CAPTURAR EL PERRO EN VARIABLE LOCAL*/
        
        
        
        
        free(reg);
        return 1 ;
    
    }else{
    /*El perro no existe */       
        printf("El perro no existe");
        free(reg);
        return 0;
    }
}




unsigned long  viewDog(char * name ){ /*ver un perro con una cadena de caracteres coincidan totalmente*/
    unsigned long adress,hash;
    struct dogType * temp;
    
    hash=hashC(name);
    printf("name%s",name);
    printf("HashBusqueda:%lu\n",hash);
    adress =  viewLocationHead( hash );
 
    
    printf("\n%s\n","|Nombre|---|NumeroRegistro|");

     

    while(adress!=0){
        temp=(searchDog( adress ));
        
       /*FALTA  Validar la impresion solo de los que son exactamente iguales */
           printf("|%s|",temp->nombre);    
           printf("|%lu|\n",numberDog(adress)); 
     
       adress=temp->next;
       
    }
     
 
   

}

unsigned long   deleteDog(unsigned long number ){ /*Eliminacion de un perro */
	
	
	unsigned long adress,aux,finalAdress,hashD,hashM;
    struct dogType  dogAux;	/*Guardaremos el perro a eliminar  */
    struct dogType  dogMov; /*Guardaremos el perro a mover  */
    
    
	
	adress=adressDog(number);   /*Guardamos la direccion del perro a eliminar*/
	  
	finalAdress=(unsigned long)send(-2)-(unsigned long )sizeof(struct dogType); 	
	send(finalAdress);
	
	aux = (unsigned long ) mmap_ptr + finalAdress;	
	struct dogType * move = ( struct dogType * ) aux;   /*Recostruimos el perro a mover  */
	
	if(number < 0 || adress > finalAdress ){   /*El perro no existe */
	    printf("El perro No existe");
	    return 0;
	    
	    
	}
	
    copyDog(move,dogMov); /*Copiamos el perro en uno temporal */
    dogMov=*move;
    hashM=hashC(searchDog(finalAdress)->nombre);
    printf("HashMove:%lu|\n",hashM);
    
    
    
    aux = (unsigned long ) mmap_ptr + adress;	
	struct dogType * delete = ( struct dogType * ) aux;   /*Recostruimos el perro a eliminar */
   
    copyDog(delete,dogAux); /*Copiamos el perro en uno temporal */
    dogAux=*delete;
    
    hashD=hashC(delete->nombre);
    *delete=dogMov;  /*TRUCO cambiamos los datos que contiene el perro eliminado por el ultimo*/
       
    
    printf("HashDelete:%lu|\n",hashD);
   
    
    
    
	
	
	
    if(dogMov.previus!=0){	                    /*ultimo perro a  mover no es una cabeza*/
        printf("ultimoperroS" );
	    searchDog(dogMov.previus)->next=adress;       /*Actualizamos la direccion del perro que contiene el perro que vamos a mover*/	    
	    
    }else{
	    updateHeadHash(hashM,adress); /*Actualizamos la cabeza*/
	    printf("%s","Actualizo cabeza");                   
    }
     
	    updateTailHash(hashM,adress);     /*Actualizamos la cola pues sabemos que el ultimo perro SIEMPRE ES UNA COLA*/
	  
	
	    

    
	
	if(dogAux.previus == 0 && dogAux.next == 0){        /*Si un perro es cabeza y cola*/
	        updateHeadHash(hashD,dogAux.next); /*Actualizamos la cabez de hash */
	        updateTailHash(hashD,dogAux.previus); /*Actualizamos la cola de hash */
	
	}else{
/*rev*/
        if(dogAux.previus!=0 ){      /*si se va a eliminar un perro que no sea la cabeza*/
           
            searchDog(dogAux.next)->previus=dogAux.previus;   /*El perro siguiente se conecta con el perro anterior*/
            printf("%s\n","este perro no es cabeza");
        }else{
        
              updateHeadHash(hashD,dogAux.next); /*Actualizamos la cabez de hash */
              searchDog(dogAux.next)->previus=0;      /*guardo en el perro que el es la cabeza*/
               printf("%s\n","este perro es cabeza");
        
            
        }
        
        
        if(dogAux.next!=0){      /*si se va a aliminar un perro que no es la cola*/
                
                searchDog(dogAux.previus)->next=dogAux.next;      /*el perro anterior se conecta con el siguiente pues el de la mitad desaparace*/
          printf("%s\n","este perro NO era una  cola");
        }else{
             updateTailHash(hashD,dogAux.previus); /*Actualizamos la cola de hash */
            searchDog(dogAux.previus)->next=0;      /*guardo en el perro que sera la cola*/
        
            printf("%s\n","este perro era cola");
            
         }

}


    
    

    
   
    ftruncate(fileno(fp),send(send(-2)-sizeof(struct dogType)));      /*Truncamos el archivo donde este el cursor*/

    return 1;

}




static void error(char *mesg){
  perror(mesg);
  
}




int main(int argc, char *argv[]){

    char *file = "dataDogs.dat";	/*Nombre del archivo para la  comunicacion*/	


 /*     	
    fp= fopen(file, "wb+");	
    fid = fileno(fp);		
  
  

  if (fid < 0){			
    fprintf(stderr,"Bad Open of file <%s>\n", file);
    error("Failed to open mmap file, QUIT!");
   }

    fwrite(head, sizeof(head), 1, fp );
    fwrite(tail, sizeof(tail), 1, fp );

fclose(fp);

      
 //*/

   fp= fopen(file, "rb+");	/*Apertura de Archivo !debe existir! lectura 	
	escritura en binario*/

    fid = fileno(fp);		/*Descriptor del archivo*/
    remmap();               /*Mapear el archivo*/

    
    if (mmap_ptr == MAP_FAILED){
     error("Parent Memory Map Failed, QUIT!");
    }

   

    viewReg();

   // getDate();

    /*	
    int i=0;
   
    while(i<6510){
    inDog("Michael");
    printf("%i",i);
   i++;
   }


*/
 //   inDog();
  //  inDog();
   // viewDog(takeName());
//viewReg();
  // int i=0;
  // while(i<6){
  //  inDog();
  // i++;
   //}
 
 // */
 // inDog("Michael");
//	inDog("Michael");//viewDog("Michael");
    //printf("%lu",viewReg(1));
    //printf("%i",deleteDog(1));
    //
        //viewDog("miller");
    //viewDog("Michael");
    //
    //viewReg(2);    
 /* int i=0;
  char  name[32] ;
  while(i<7){
    
    bzero(name,32);
    scanf("%s",name);
    inDog(name);
    i++;
    }
  
  printf("SELECCIONE UN NOMBRE DE LA LISTA");
  scanf("%s",name);   
        //deleteDog(64980);
    //scanf("%s",name);
  //*/
  //char  name[32] ;
  //bzero(name,32);
  //struct dogType dog;
  //bzero(dog.nombre,32);
   //scanf("%s",name);
    //strcpy(dog.nombre,name);
  
   //printf("%s",name);
     //inDog(name); 
  
  //viewDog(takeName());
  
   
  
 //deleteDog(8);
 //*/
 //viewDog(takeName());
    //viewDog("miller\0");
   // viewDog(dog);
//*/
    if (status == -1){  
      error("Bad munmap, QUIT!");
    }

    fclose(fp);
    return 0;
 
  }



