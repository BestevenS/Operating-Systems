/* second pipe example: fork/read/write within same process */
/*      (see pages 40-46 Unix Net Progr by RW Stevens)      */


#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

/* Read characters from the pipe and echo them to stdout. */
void read_from_pipe (int file)
{
  FILE *stream;
  int c;
  char ptr[100];
  stream =  fdopen(file, "r");
  while ( fgets (ptr, 100 , stream) != NULL )
       printf (" read message:%s\n", ptr );
  fclose (stream);
}

void write_to_pipe (int file)
{
  FILE *stream;
  stream = fdopen (file, "w");
  fprintf (stream, "hello, world!\n");
  fprintf (stream, "goodbye, world!\n");
  fclose (stream);
}

int main (void)
{

  pid_t pid1,pid2,pid3;
  int mypipe1[2],mypipe2[2];

  if (pipe (mypipe1) || pipe (mypipe2)) {
      fprintf (stderr, "Pipe failed.\n");
      return EXIT_FAILURE;
  }

  pid1 = fork ();
  pid2 = fork ();
  if (pid1 == (pid_t) 0) {

      close (mypipe1[1]);

      read_from_pipe (mypipe1[0]);
      exit(0); 
  }
  else if (pid1 < (pid_t) 0) {
      fprintf (stderr, "Fork failed.\n");
      return EXIT_FAILURE;
  }
  else {

      close (mypipe1[0]);

      write_to_pipe (mypipe1[1]);
	  wait(0); 
  }

  pid3 = fork ();

  if (pid2 == (pid_t) 0) {

      close (mypipe2[1]);

      read_from_pipe (mypipe2[0]);
      exit(0);
  }
  else if (pid1 < (pid_t) 0) {

      fprintf (stderr, "Fork failed.\n");
      return EXIT_FAILURE;
  }
  else {

      close (mypipe2[0]);

      write_to_pipe (mypipe2[1]);
	  wait(0);
  }
}