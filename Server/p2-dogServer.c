#include<stdio.h>
#include<stlib.h>


struct dogType {	//Informacion de un registro  
	
	char *name; //32 bites
	char *type;
  	int age;
	char *breed;
	int height;
	double weight;
	char sex;
	struct dogtype *next; 
};

int  saveAnimal(struct dogType *Animal){
	FILE *ap;				//Creacion Pointer Archivo
	ap=fopen("data.dat","a+");
		
		
    	if (ap == NULL){     			//validacion apertura archivo 
       		perror("No se puede crear o abrir el archivo porque:");
        	exit(-1);   
    	size_t r;		}
	r=fwrite(Animal,12345,1,ap);
	if (r != ){     			//validacion escritura archivo 
       		perror("No se puede escribir el archivo porque:");
        	exit(-1);   
    			}
	
		//validar escritura 
		r=fclose(ap);
		exit(0); 
}

struct Connected {
	int index;
	struct dogtype *next;
	struct Connected *next;
	
};

void printPet(dogType Animal){
	printf("Name: %s\n",dogType.name);
	printf("Type: %s\n",dogType.type);
	printf("Age: %i\n",dogType.age); 
	printf("Breed: %s\n",dogType.breed);
	printf("Height: %i\n",dogType.height);
	printf("Weight: %f\n",dogType.weight);
	printf("Sex: %c\n",dogType.sex);
};

int main(){






	}
