#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*Execute in terminal With Bucle for */
/*for i in {1..154};do ./creador;done*/

#define CANT 65000
#define SIZE_HASH 10000


int const multiplier=263;
long const prime =1000000007;

unsigned long head[SIZE_HASH];  //Dimencion de nuestra tabla Hash cabezas

unsigned long tail[SIZE_HASH];  //Dimencion de nuestra tabla Hash colas 


    int fid,status;
    FILE *fp;		/*Puntero para crear el archivo*/
    FILE *nombres;
    FILE *tipos;
    FILE *razas;
        
    caddr_t mmap_ptr; /*Puntero cabezera mmap*/

  struct dogType{
	    char nombre[32];
	    char tipo[32];
	    int edad;
	    char historyLock ;
	    char raza[16];
	    int  estatura;
	    float peso;
	    char sexo;
	    unsigned long previus;
	    unsigned long next;
	};
	
	
unsigned long send(long action ,FILE *p){

	switch(action) {
	      case -1 :
		fseek(p,0,SEEK_SET);/* enviamos el puntero al Inicio*/	
		return ftell(p);
		break;
	      case -2:
		fseek(p,0,SEEK_END);/* enviamos el puntero al final */	
		
		return ftell(p);
		break;
	      default :
		fseek(p,action,SEEK_SET);/* enviamos el puntero a posicion dada en 
					bytes contada desde el inicio */	
		return ftell(p);
		break;	
	}

}


struct dogType * searchDog(unsigned long location){
    /*Encuentra y devuelve  el perro en la localizacion "location"*/  
     
  unsigned long x=(unsigned long) ((unsigned long )mmap_ptr + location); /*Movemos a la posicion del perro */	
  struct dogType * dog = (struct dogType * ) x;/*Casteamos para acceder al perro de forma facil */   
  return  dog;
  
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

unsigned long  remmap(){

     mmap_ptr = mmap((caddr_t) 0,   /* Memory Location, 0 means O/S chooses */
            send(-2,fp),/* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    fid,           /* which file is associated with mmap */
		    (off_t) 0);    /* Offset in page frame */
     return 1;
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
    
    
void saveData(int select,char * renglon,FILE *a){
    
    int i,longReng=0,contActual = 0;
        while( !feof( a) ){
            fgets(renglon, 80, a);
            for(i = 0; i <= strlen(renglon); i++){
                if(renglon[i] == '\0'){
                    contActual++;
                }
            }
            if(contActual == select){
                break;
            }
        }
        
        longReng = strlen(renglon);
        
        if (longReng > 0 && renglon[longReng-1] == '\n'){
            renglon[longReng-1] = '\0';
        }
        
        
}

struct dogType getData(){           /*Recoleccion datos una  mascota*/

     struct dogType dog;
    /*Rellenar el perro de ceros */
    bzero(dog.nombre,32); 
    bzero(dog.tipo,32); 
	dog.edad=0;
	dog.historyLock='T';
	bzero(dog.raza,16);
	dog.estatura=0;
	dog.peso=0;
	dog.sexo='\0';
	dog.previus=0;
    dog.next=0; 
    
    
       
       
       
   
    char renglon[81];
    int sexo;
    send(-1,nombres);
    saveData(rand()%1716,renglon,nombres);
    strcpy(dog.nombre,renglon); 
    send(-1,tipos);  
    saveData(rand()%33,renglon,tipos);
    strcpy(dog.tipo,renglon);
    dog.edad=rand()%50;
    send(-1,razas);
    saveData(rand()%10,renglon,razas);
    strcpy(dog.raza,renglon);
    dog.estatura=rand()%150;
    dog.peso=rand()%40;
    sexo = rand()%2;
        if(sexo){
            dog.sexo='M';
        }else{
            dog.sexo='F';
            }
                  
    return dog;
}


unsigned long  inDog(){/*agregar NUEVO perro */

    unsigned long adressPri,adressTail,hash;
	char  aux [32];
	struct dogType dogPri;
	
	/*Recolección y validación de Datos */
	dogPri=getData(dogPri);	    
	
	bzero(aux,32);
	strcpy(aux,dogPri.nombre);
	hash=hashC(aux); /* llamamos al  function hash */	
	
	
	adressPri=send(-2,fp);	
	adressTail =  viewLocationTail(hash);
	    	    
			if( adressTail == 0 ){			    
				
			    /*PRIMER Perro que ingresa*/			
			    
				updateHeadHash(hash,adressPri);/*ActualizoCabeza*/				
				updateTailHash(hash,adressPri);	/*ActualizoCola*/
				dogPri.next=0;
				dogPri.previus=0;				
					    
			}else{	
			
			    /*SEGUNDO o MAYOR Perro que ingresa*/
			  			    
			    searchDog( adressTail )->next = adressPri;
			    /*Actualizacion apuntador que lo contiene*/
			    dogPri.previus = adressTail;
			    updateTailHash(hash,adressPri);	/*ActualizoCola*/
				dogPri.next=0;
				
			}
		
		/*Guardo el perro en el archivo*/
		fwrite(&dogPri, sizeof(struct dogType),1, fp);
        
		remmap();
		
	    return 1;	/*validar correcto 1*/
		
};



int main(int argc, char *argv[]){

    char *file = "dataDogs.dat";	/*Nombre del archivo con los perros*/	
    nombres = fopen("nombresMascotas.txt","r");
    tipos = fopen("tipos.txt","r");
    razas = fopen("razas.txt","r");
    int i=0;

    fp= fopen(file, "wb+");	
    fid = fileno(fp);		
     
    if (fid < 0){			
        fprintf(stderr,"Bad Open of file <%s>\n", file);
        error("Failed to open mmap file, QUIT!");
    }
    bzero(head,SIZE_HASH);         
    bzero(tail,SIZE_HASH);
    
    fwrite(head, sizeof(head), 1, fp );     
    fwrite(tail, sizeof(tail), 1, fp );


    fp= fopen(file, "rb+");	

    fid = fileno(fp);		 
    
  remmap();                
    
    
    
    if (mmap_ptr == MAP_FAILED){
     error("Parent Memory Map Failed, QUIT!");
    }     
    
    for(i=0;i<CANT;i++){
        if(!inDog()){
            printf("Error De inserción");
            return 0;
        }
    }
    
    
    printf("\nSe insertaron %i registros Aleatorios\n",i);    
    fclose(fp);
    fclose(nombres);
    fclose(razas);
    fclose(tipos);

}
