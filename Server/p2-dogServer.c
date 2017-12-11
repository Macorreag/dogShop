#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define CANT 2

struct dogType
{
	char nombre[32];
	char animal[32];
	struct dogType *next;
};

struct celdas
{
	struct dogType p[10];
};

static void
error(char *mesg){
  perror(mesg);
  //exit(2);
}

static void
show_usage(const char *prog_name, const char *mesg ){
  fprintf(stderr, "Error: %s\n"
	  "Usage is:\n%s fname\n\tWhere fname is a file name to use\n",
	  mesg, prog_name);
}

int
main(int argc, char *argv[]){

  FILE *fp;

  int fid, child, status;
  pid_t wait_status;
  caddr_t mmap_ptr;
  char buffer[80];

  char *file="com.dat";

/*  if (argc != 2){
    show_usage(argv[0], "Wrong number of parameters");
    //exit(1);
  }*/
  /* Create a new file for read/write access with permissions restricted
     to owner rwx access only */

  //fid = open(file, O_RDWR | O_EXCL, (mode_t) 0755 );

  fp= fopen("com.dat", "rb+");
  fid = fileno(fp);

  if (fid < 0){
    fprintf(stderr,"Bad Open of file <%s>\n", file);
    error("Failed to open mmap file, QUIT!");
  }
/*
  status = ftruncate(fid, sizeof(buffer)); // make the file the buffer size
  if (status){
      fprintf(stderr,"Could not ftruncate(%d, %lu) = %d\n", fid,
	      sizeof(buffer), status );
      error("Bad Ftruncate");
  }
*/
  child = fork();
  if (child == -1){
    error("Could not fork, QUIT!");
  } else if (child == 0){ /* This is the child process */
    /* Notice that the child uses the write system call in this version */

    //sprintf(buffer, "Welcome to the Jungle!");

    struct celdas *d;
    struct dogType perrito2;
    strcpy(d->p[1].nombre, " theBIg");
    strcpy(d->p[1].nombre, " gatita ");


   strcpy(d->p[0].nombre, " sasha ");
    strcpy(d->p[0].animal, " perrito ");
   /*
    d->p[0].next = malloc( sizeof( struct dogType ) );
    *d->p[0].next = perrito2;
   */
   fwrite(d, sizeof(struct celdas), 1, fp);

   /*
    sprintf(buffer, "Hola Mundo...");
    status = write(fid, buffer, sizeof(buffer));
    if (status < 0){
      error("Bad Child Write, Quit!");
    }
    status = close(fid); // close the mmap file

    if (status == -1){
      error("Bad Child Close, Quit!");
    }*/
    sleep(2);
  } else { /* This is the parent Process */
    sleep(1);
    /* allocate a shared memory region using mmap which can be inherited
       by child processes */
    mmap_ptr = mmap((caddr_t) 0,   /* Memory Location, 0 means O/S chooses */
		    sizeof(struct celdas),/* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    fid,           /* which file is associated with mmap */
		    (off_t) 0);    /* Offset in page frame */
    if (mmap_ptr == MAP_FAILED){
      error("Parent Memory Map Failed, QUIT!");
    }
    printf("Parent's mmap_ptr = %lx\n", (unsigned long) mmap_ptr);

	struct celdas *c;
	c = (struct celdas*) mmap_ptr;
	printf("Parent got the message: %s\n", c->p[0].nombre);
	//printf("Parent got the message: %s\n", c->p[0].animal);
	//printf("Parent got the message: %s\n", c->p[0].nextnombre);

    sleep(1);

    status = munmap(mmap_ptr, sizeof(struct celdas));
    if (status == -1){
      error("Bad munmap, QUIT!");
    }

    fclose(fp);
    /*
    status = close(fid); // close the mmap file
    if (status == -1){
      error("Bad Parent Close, Quit!");
    }
    */
    /*
    //unlink (i.e. remove) the mmap file
    status = unlink(file);
    if (status == -1){
      error("Bad unlink, QUIT!");
    }
    wait_status = wait( &status );
    if (wait_status == -1){
      error("Bad Wait, QUIT!");
    }*/

  }

  return 0;
}
