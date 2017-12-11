#include <stdio.h>

int main()
{
   FILE *fp; 					//Puntero al archivo 	
   char nombre[11] = "data.dat";		//Nombre del archivo 
   char *dinero= "a";
   char leer[2]={'x','x'};
   
   unsigned int i=0;


   	
   fp = fopen( nombre, "wb+" ); 		//Abrimos el archivo con (Abre un fichero en modo binario para actualización (lectura y escritura))
   printf( "Fichero: %s -> ", nombre );	
   if( fp)			// Return true confirmacion efectiva
      printf( "creado (ABIERTO)\n" );		
   else
   {
      printf( "Error (NO ABIERTO)\n" );	// Return false confirmacion efectiva
      return 1;				//mata el programa 	
   }

   printf( "Escribiendo cantidades:\n" );  //Confirmacion Escritura 



   for( i=0; i<20001; i++ ){
     //printf( "%c", dinero );		//Aviso inicio escritura
   fwrite( dinero, sizeof(char),1, fp );	//Inicio escritura
	//rewind( fp );   
fflush(fp);}

   printf( "\nLeyendo los datos del fichero \"%s\":\n", nombre );
   rewind( fp ); 				//coloca el indicador de posición al comienzo del fichero
   fread( leer, sizeof(unsigned  long), 2, fp );

   for( i=0; i<20001; i++ ){
	 fread( leer, sizeof(unsigned  long), 2, fp );
     printf( "%c", leer[i] ); //confirmacion lectura
}
   if( !fclose(fp) )  // confirmacion cierre fichero
      printf( "\nFichero cerrado\n" );		
   else				// ERROR  cierre fichero
   {
      printf( "\nError: fichero NO CERRADO\n" );
      return 1;
   }

   return 0;
}
