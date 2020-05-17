# dogShop
Proyecto Sistemas Operativos,basado en C  y llamadas de tipo POSIX.

El proyecto intenta desarrollar un archivo de busquedas rapidas mediante la implementación de una tabla Hash  y una LinkedList.

## Getting Started

# Menu de petShop

Las funcionalidades que este programa puede ejecutar sobre un documento de texto plano que se encuentra en disco y no en RAM son:

- **Ver Registro :** A el número proporcionado le aplica la función Hash y luego muestra la LinkedList asociada a este.
- **Borrar Registro :** Corta el último registro y reemplaza el numero de registro que se proporciono.
- **Buscar Registro :** Ir con el número mostrado en el ver.





## Requerimientos del proyecto
https://github.com/capedrazab/os-20192

Modelo del Trabajo,acceso solo con cuenta de la Universidad Nacional de Colombia.
https://drive.google.com/a/unal.edu.co/file/d/0B3320U5m5SLbajVtMFBMcktlc2c/view?usp=sharing


**Makefile**
 - Se debe generar el archivo en el cual realizar la busqueda;este puede superar el tamaño de 1Gb y se puede hacer mediante el Makefile
 - Tambien se generan mediante este archivo los ejecutables

Explicacion de como usar la Hash+ Linkked List
https://youtu.be/tx1GqgbsDmI
https://youtu.be/eUaHUsWpn8U

## Optimizaciones 
Se usa Mmap para optimizar la busqueda en disco duro

## Caracteristicas
## Propiedades que contiene un registro

Este proyecto permite almacenar en disco estructuras de tipo Struct en un archivo de texto plano con las siguientes caracteristicas, el registro almacenado dentro del documento posee las siguientes celdas:

```cpp
    //Information of a Pet
    char nombre[32];
    char tipo[32];
    int edad;
    char raza [16];
    int estatura;
    double peso;
    char sexo;
    // This fields to manage LinkedList
    unsigned long next;
    unsigned long previus
```

  
Acceso a atributos en C:



**Dos formas para acceder a datos de la estructura**

```
//Con el apuntador y metodo
Data d;
Data\* pd = &d;
(\*pd).edad = 23;
// Mediante Arrow
pd -> edad = 23;
```


jdjek
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites


Genere el archivo dataDogs.dat con el uso de los archivos razas.txt y nombres.txt al ejecutar el creadorPerros.c

Mediante los siguientes comandos :

Compile el programa mediante 

```c
    gcc -o creador CreadorPerros.c 

    //cree un archivo asi

    touch dataDogs.dat

    //Ejecute el creador varias veces

    for i in {1..154};do ./creador;done
```




## Ejecución
Para ejecutar el programa creado simplemente abra un terminal en el directorio y ejecute

    ./hola
strong text1 ->Generador de perros 
2 ->Server
3 ->Client
4 ->makeFile



## Cabezera de inicio del archivo

El archivo posee una cabezera de tamaño reducido con el objetivo de manejar mas efectivamente 


## Función para el control del apuntador dentro del archivo

Esta función altera el lugar donde se esta apuntando a realizar modificaciones en el archivo ```dataDogs.dat```

```c
    //Envia el apuntador al ininio del archivo
    send(1);
    //Envia el apuntador a donde indica position
    send(position);
    //Envia el apuntador al final del archivo
    send(-2);
```


## Caracteristicas de una eliminación
- El elemento es cabeza de una LinkedList
- El elemento esta en la mitad de una LinkedList
- El elemnto es una cola de una LinkedList








What things you need to install the software and how to install them

```
Give examples
```

### Installing

A step by step series of examples that tell you have to get a development env running

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

### .gitignore

## Authors

* **Miller Correa** - *Build Project* - [Macorreag](https://github.com/macorreag)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone who's code was used
* Inspiration
* etc
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTEzNjYyNjQ3NTEsLTE3MDMyNDcwNTEsLT
IwNDgzNjQ0ODRdfQ==
-->
