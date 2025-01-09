
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXARGS 10

int countChars( char* s, char c )
{
    return *s == '\0'
              ? 0
              : countChars( s + 1, c ) + (*s == c);
}

char **str_split(char *string, const char delimiter) {
    int length = 0, count = 0, i = 0, j = 0;
    while(*(string++)) {
        if (*string == delimiter) count++;
        length++;
    }
    string -= (length + 1); // string was incremented one more than length
    char **array = (char **)malloc(sizeof(char *) * (length + 1));
    char ** base = array;
    for(i = 0; i < (count + 1); i++) {
        j = 0;
        while(string[j] != delimiter) j++;
        j++;
        *array = (char *)malloc(sizeof(char) * j);
        memcpy(*array, string, (j-1));
        (*array)[j-1] = '\0';
        string += j;
        array++;
    }
    *array = '\0';
    return base;  
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: xargs cmd...\n");
    exit(1);
  }

  char cmdargv[MAXARGS][32];
  for(int i = 0; i < argc; ++i) {
    //printf("DEBUG: copy %s to cmdargv[%d]\n", argv[i], i);
    strcpy(cmdargv[i], argv[i]);
  }

  char** splitargv;
  splitargv = malloc(sizeof(char*)*MAXARGS);
  for(int i = 0; i < MAXARGS; ++i) {
    splitargv[i] = malloc(sizeof(char)*32);
  }
  //printf("DEBUG: Split argv...\n");
  char* buf = malloc(sizeof(char)*64);
  int num = read(0, buf, 64);
  char* realbuf = malloc(sizeof(char)*num);
  strcpy(realbuf, buf);
  //printf("DEBUG: realbuf: %s\n buf: %s\n num: %d\n", realbuf, buf, num);
  int c = countChars(realbuf, '\n');
  if(c > 1) {
    splitargv = str_split(realbuf, '\n');
  } else {
    strcpy(splitargv[0], realbuf);
  }
  //printf("DEBUG: Split count is %d\n", c);

  char* finalargv[c*4];
  for(int i = 0; i < c*4; ++i) {
    finalargv[i] = malloc(sizeof(char)*32);
  }
  for(int i = 0; i < c; ++i) {
    strcpy(finalargv[i*4+0], cmdargv[1]);
    strcpy(finalargv[i*4+1], cmdargv[2]);
    strcpy(finalargv[i*4+2], splitargv[i]);
    finalargv[i*4+3] = 0;
    //printf("DEBUG: Final argv: %s %s %s\n", finalargv[i*4+0], finalargv[i*4+1], finalargv[i*4+2]);
  }

  for(int i = 0; i < c; ++i) {
    int pid = fork();
    if(pid == 0) {
        //printf("DEBUG: exec %s\n", cmdargv[1]);
        exec(cmdargv[1], finalargv+i*4);
        //for(int i = 0; i < MAXARGS; ++i) {
        //  free(splitargv[i]);
        //}
        //free(splitargv);
        //for(int i = 0; i < c*4; ++i) {
        //    free(finalargv[i]);
        //}
    } else {
        wait(0);
    }
  }
  exit(0);
}
