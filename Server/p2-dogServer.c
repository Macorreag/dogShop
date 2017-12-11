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

int const multiplier=263;
long const prime =1000000007;

unsigned long head[SIZE_HASH]={0,0,0,0,0,0,0,0,0,0};  //Dimencion de nuestra tabla Hash cabezas
unsigned long tail[SIZE_HASH]={0,0,0,0,0,0,0,0,0,0};  //Dimencion de nuestra tabla Hash colas 




struct dogType
{
	char nombre[32];
	char tipo[32];
	//char raza[16];
	
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

unsigned long  copyDog(struct dogType * A,struct dogType B){
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

unsigned long hashC(char * name) {
           
        unsigned  long nHash = 0;
        int i;
        for ( i= (sizeof(name)/sizeof(char)) - 1; i >= 0; --i){
            if(name[i]!='\0'&&name[i]!=0){
            nHash = (nHash * multiplier + name[i]) % prime;
            }
        }
        //printf("%luhash:",nHash % SIZE_HASH);
        
        return (unsigned long )nHash % SIZE_HASH;
              
    }
       /*MIGUEL HASH */
/*unsigned long  hashC(char * str) {
         unsigned  long * hash_number ;
         hash_number=malloc(8);
         *hash_number=0;
         
        const char * ustr;
        ustr=(const char *) str;
        
        while(*ustr !='\0'){
            *hash_number=(*hash_number*multiplier+*ustr)%prime;
            ustr++;
            
        
        }
        *hash_number %=SIZE_HASH;
        
        return  * hash_number;
}*/




unsigned long  inDog(char * name ){/*agregar NUEVO perro */
	/*Recoleccion e ingreso de una mascota*/
	/*Recolección y validación de Datos */
	/*FALTAN TODOS LOS DEMAS DATOS DE LA MASCOTA*/
	unsigned long adressPri,adressTail,AdressHead,hash;
	struct dogType dogPri;
	adressPri=send(-2);
	//printf("|adr:%lu|",adressPri);
	 bzero(dogPri.nombre,32);
    strcpy(dogPri.nombre,name);	
    
	/*obtener el hash */
	hash=hashC(name); /*obtuvimos el hash cero llamando a function hash */	
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
    adress = ( adress - (sizeof(head) + sizeof(tail)) );
    
    return adress/sizeof(struct dogType);
}
unsigned long adressDog(unsigned long number){
    /*Registro con el que esta el perro */
    number=number*((unsigned long)(sizeof(struct dogType)));
    return (number+(unsigned long)sizeof(head)+(unsigned long)sizeof(tail));

}

unsigned long viewReg(unsigned long  number){ /*Buscar un perro con el numero de registro */
    /*Validacion que el registro existe*/
    unsigned long endDog,size,unico;
    size =(sizeof(head)+sizeof(tail));
    endDog=send(send(-2)-sizeof(struct dogType));
    unico=adressDog(number);
    if(send(-2)!=size&&(endDog>=unico)){
    /*El perro existe */
    
        printf("?%s°",searchDog( unico )->nombre);  
    
    return unico ;
    
    }else{
    /*El perro no existe */
    
    return 0;
    }


}




unsigned long  viewDog(char * name){ /*ver un perro con una cadena de caracteres coincidan totalmente*/
    unsigned long adress,hash;
    struct dogType * temp;
   
    hash=hashC(name);
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
	
	
	unsigned long adress,aux,finalAdress,hashD;
    struct dogType  dogAux;	/*Guardaremos el perro a eliminar  */
    struct dogType  dogMov; /*Guardaremos el perro a mover  */

	
	adress=adressDog(number);   /*Guardamos la direccion del perro a eliminar*/
	  
	finalAdress=send(-2)-(unsigned long )sizeof(struct dogType);
	
  	
	
     copyDog(searchDog(send(finalAdress)),dogMov); /*Copiamos el perro en uno temporal */
	if(number < 0 || adress > finalAdress ){   /*El perro no existe */
	    return 0;
	}
	
	
    if(dogMov.previus!=0){	                    /*ultimo perro a  mover no es una cabeza*/
	    searchDog(dogMov.previus)->next=adress;       /*Actualizamos la direccion del perro que contiene el perro que vamos a mover*/
	    
    }else{
	    updateHeadHash(hashC(dogMov.nombre),adress); /*Actualizamos la cabeza*/
	    printf("%s","Actualizo cabeza");                   
    }
	
	
	    
	    updateTailHash(hashC(dogMov.nombre),adress);     /*Actualizamos la cola pues sabemos que el ultimo perro SIEMPRE ES UNA COLA*/
	    printf("HashMove:%lu|",hashC(dogMov.nombre));
	
	    
	aux = (unsigned long ) mmap_ptr + adress;	
	struct dogType * delete = ( struct dogType *) aux;   /*Recostruimos el perro a eliminar */
   
    copyDog(delete,dogAux); /*Copiamos el perro en uno temporal */

    *delete=dogMov;  /*TRUCO cambiamos los datos que contiene el perro eliminado por el ultimo*/
       
    hashD=hashC(dogAux.nombre);
    
    printf("HashMove:%lu|",hashC(delete->nombre));
	
	

    if(dogAux.previus!=0){      /*si se va a eliminar un perro que no sea la cabeza*/
        sleep(3);
        searchDog(dogAux.next)->previus=dogAux.previus;   /*El perro siguiente se conecta con el perro anterior*/
        
    }else{
    
          updateHeadHash(hashC(dogAux.nombre),dogAux.next); /*Actualizamos la cabez de hash */
          searchDog(dogAux.next)->previus=0;      /*guardo en el perro que el es la cabeza*/
           printf("%s","cabeza");
    
        
    }
    
    
    if(dogAux.next!=0){      /*si se va a aliminar un perro que no es la cola*/
            
            searchDog(dogAux.previus)->next=dogAux.next;      /*el perro anterior se conecta con el siguiente pues el de la mitad desaparace*/
      
    }else{
         updateTailHash(hashC(dogAux.nombre),dogAux.previus); /*Actualizamos la cola de hash */
        searchDog(dogAux.previus)->next=0;      /*guardo en el perro que sera la cola*/
    
        printf("%s","cola");
        
    }




    
    

    
   
    ftruncate(fileno(fp),send(send(-2)-sizeof(struct dogType)));      /*Truncamos el archivo donde este el cursor*/

    return 1;

}

int moveDog(unsigned long posicionMove,struct dogType dog,caddr_t mmap_ptr){
	//if(posicionMove == &dog){
		/*actualizacion*/	
			
	//}else{
		/*Movimiento perro*/
		//unsigned long x=(unsigned long) mmap_ptr + 160;	
		send(posicionMove-(unsigned long)mmap_ptr);
		//struct dogType perro = (struct dogType*) posicionMove;
		fwrite(&dog, sizeof(struct dogType),1, fp);
	//}	



}


static void error(char *mesg){
  perror(mesg);
  
}




int main(int argc, char *argv[]){

    char *file = "dataDogs.dat";	/*Nombre del archivo para la  comunicacion*/	

      	
  	  	
    fp= fopen(file, "wb+");	/*Apertura de Archivo !debe existir! lectura escritura en binario*/
    fid = fileno(fp);		/*Descriptor del archivo*/
  
  

  if (fid < 0){			/*i fileno no pudo crear descriptor retorna -1*/	
    fprintf(stderr,"Bad Open of file <%s>\n", file);
    error("Failed to open mmap file, QUIT!");
   }

    fwrite(head, sizeof(head), 1, fp );
    fwrite(tail, sizeof(tail), 1, fp );/*Escribimos nuestra tabla hash en el archivo */

fclose(fp);



      
 

   fp= fopen(file, "rb+");	/*Apertura de Archivo !debe existir! lectura 	
	escritura en binario*/

    fid = fileno(fp);		/*Descriptor del archivo*/
    remmap();               /*Mapear el archivo*/

    
    if (mmap_ptr == MAP_FAILED){
     error("Parent Memory Map Failed, QUIT!");
    }

   





      	
   



 
 /*  int i=0;
   while(i<6){
    inDog("miller");
   i++;
   }
 /* int i=0;
   
    while(i<65000){
    inDog("Michael");
   i++;
   }
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
  int i=0;
  char  name[32] ;
  while(i<7){
    
    bzero(name,32);
    scanf("%s",name);
    inDog(name);
    i++;
    }
  
  scanf("%s",name);   
        //deleteDog(64980);
    //scanf("%s",name);
    // inDog(name); 
  viewDog(name);
  
   //*/
  
 deleteDog(2);
 viewDog(name);
    //viewDog("miller\0");
    //viewDog("miller");
//*/
    if (status == -1){  
      error("Bad munmap, QUIT!");
    }

    fclose(fp);
    return 0;
 
  }



