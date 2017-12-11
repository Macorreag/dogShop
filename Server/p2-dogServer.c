#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define CANT 2

unsigned long head[10]={0,0,0,0,0,0,0,0,0,0};  //Dimencion de nuestra tabla Hash cabezas
unsigned long tail[10]={0,0,0,0,0,0,0,0,0,0};  //Dimencion de nuestra tabla Hash colas 


struct dogType
{
	char nombre[32];
	char animal[32];
	unsigned long previus;
	unsigned long next;
};

    int fid,status;
    FILE *fp;		/*Puntero para crear el archivo*/
    caddr_t mmap_ptr; /*Puntero cabezera mmap*/
	
	



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

struct dogType *searchDog(unsigned long location){
/*Encuentra y devuelve  el perro en la localizacion "location"*/

    
    
  unsigned long x=(unsigned long) (mmap_ptr + location); /*Movemos a la posicion del perro */
	
	
  struct dogType *dog = (struct dogType*) x;/*Casteamos para acceder al perro de forma facil */

    return dog;


} 

unsigned long * viewLocationHead(long Hash ){

    unsigned long aux = (unsigned long ) mmap_ptr + (8*Hash);
	unsigned long *point = (unsigned long *) aux;   
    return point;

} 
unsigned long * viewLocationTail(long Hash ){

    unsigned long aux = (unsigned long ) mmap_ptr + (8*Hash) + sizeof(head);
	unsigned long * point = (unsigned long *) aux;   
    return point;

} 
void updateHeadHash(long rank,unsigned long adress){
	/*actualiza la tabla de punteros Cabeza del Hash con la ubicacion del hash"rank "*/
	
	*(mmap_ptr+rank*8)=adress;	


}
void updateTailHash(long rank,unsigned long adress){
	/*actualiza la tabla de punteros del Hash con la ubicacion del hash(rank)*/
	
	*(mmap_ptr+rank*8+(sizeof(head)))=adress;	


}



long numberDog(unsigned long adress){

    


}

int  inDog(){   /*agregar NUEVO perro */

	/*Recoleccion e ingreso de una mascota*/
	struct dogType dog;
		
	/*Recolección y validación de Datos */
	strcpy(dog.nombre,"Boss");
	strcpy(dog.animal,"perro");

	
	/*obtener el hash */
	long hash=0; /*obtuvimos el hash cero llamando a function hash */		
	
       
			if(*viewLocationTail(hash) == 0 ){
			    
				
			    /*PRIMER Perro que ingresa*/			
				updateHeadHash(hash,send(-2));/*ActualizoCabeza*/				
				updateTailHash(hash,send(-2));	/*ActualizoCola*/
				dog.next=0;
				dog.previus=0;
				
				
					
			}else{	
			
			    /*SEGUNDO o MAYOR Perro que ingresa*/
			    searchDog(*viewLocationTail(hash))->next=send(-2);/*Actualizacion apuntador que lo contiene*/
			    dog.previus=*viewLocationTail(hash);
			    updateTailHash(hash,send(-2));	/*ActualizoCola*/
				dog.next=0;
				
			}
		
		/*Guardo el perro en el archivo*/
		fwrite(&dog, sizeof(struct dogType),1, fp);
	    return 1;	/*validar correcto 1*/
		
};



int  deleteDog(unsigned long posicionMove,struct dogType dog,caddr_t mmap_ptr){
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

int  moveDog(unsigned long posicionMove,struct dogType dog,caddr_t mmap_ptr){
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
  	
  fp= fopen(file, "rb+");	/*Apertura de Archivo !debe existir! lectura 	
	escritura en binario*/

  fid = fileno(fp);		/*Descriptor del archivo*/

  if (fid < 0){			/*i fileno no pudo crear descriptor retorna 					-1*/	
    fprintf(stderr,"Bad Open of file <%s>\n", file);
    error("Failed to open mmap file, QUIT!");
  }

    fwrite(head, sizeof(head), 1, fp );
	fwrite(tail, sizeof(tail), 1, fp );/*Escribimos nuestra tabla hash	en el archivo */

 fclose(fp);

    fp= fopen(file, "ab+");	/*Apertura de Archivo !debe existir! lectura 	
	escritura en binario*/

  fid = fileno(fp);		/*Descriptor del archivo*/
 mmap_ptr = mmap((caddr_t) 0,   /* Memory Location, 0 means O/S chooses */
		    160 ,/* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    fid,           /* which file is associated with mmap */
		    (off_t) 0);    /* Offset in page frame */
    
    
    
    
    
    
    
    
    if (mmap_ptr == MAP_FAILED){
      error("Parent Memory Map Failed, QUIT!");
    }

    

	
    
    inDog();
    inDog();
    inDog();
    
    
        

    


    if (status == -1){
      error("Bad munmap, QUIT!");
    }






 fclose(fp);

//inDog(fp,,-1);
      return 0;
 
  }



