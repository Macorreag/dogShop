#This is Makefile for get executable the Server
#AQUI VAN LAS VARIABLES	

all: server client
	
	
#sintaxis
#    objetivo: dependencias
#          instrucciones
#Explicacion
#fdconejop correo tutor

#

#Objetivo = que es lo que se busca el "ejecutable"
#dependencias = archivos que son necesarios para hacerlo
#instrucciones = que va a hacer compilar o lo que deba hacer
#compilar el programa principal 

#Crear Archivo Perros
dataDogs:	GeneradorPerros/CreadorPerros.c
	touch dataDogs.dat
	gcc -o GeneradorPerros/creador.runner GeneradorPerros/CreadorPerros.c 
    #//cree un archivo asi
    #//Ejecute el creador varias veces
    #for i in {1..154};do ./creador;done



#para correr use "make "(nombre funcion ejm:server)
server: serverMain.c variables.h manejoArchivos.h funciones.h   
	gcc ManejoHashFile.c serverMain.c -pthread -o server 
	#con seccion critica
	#gcc manejoSeccionCritica.c ManejoHashFile.c serverMain.c -pthread -o server 

client: clientMain.c validarDatos.o interfazCliente.h manejoArchivos.h 
	$(CC) clientMain.c validarDatos.o -o client



#Compilar archivos de codigo objeto .o (no es necesario)
validarDatos.o: validarDatos.c interfazCliente.h
	$(CC) -c validarDatos.c
	
runS:	server 
	./server
runC:	client
	./client 127.0.0.1

#limpiar  archivos 
clean:
	rm -f server*.o
	rm -f client*.o
    
