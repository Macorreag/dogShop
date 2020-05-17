#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>

#define CERO 0
struct dogType {	//Informacion de un registro  
	
	char nombre[32]; //32 bites
	char tipoAnimal[32];
  	int edad;
	char raza[32];
	int altura;
	double peso;
	char genero;
	
};
	
FILE *fp;
unsigned static  long Zero=0;
int main(){
	
	int i=0;

	fp=fopen("data.dat","wb+"); // crear un archivo para lectura y escritura, se crea si no existe o se sobreescribe si existe.
	for(;i<20001;i++){
		fwrite(&Zero,8,1,fp);  //160.008bytes
			}
	fclose(fp);

	fp=fopen("data.dat","rb+");// Abre un archivo en modo binario para actualización (lectura y escritura).

	char *name="j"; //suma 3 bytes 
	fseek(fp,SEEK_END,SEEK_END);//agregar al final
	fputs(name,fp);
	
	char *nombre=((char *)malloc(sizeof(char)*1000)); 
	
	fseek(fp,160008,SEEK_SET);		//direccion cuatro bytes antes
	fread(nombre,3,1,fp);	
	printf("%s",nombre);

			

	fclose(fp);
}


