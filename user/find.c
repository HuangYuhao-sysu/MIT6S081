
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  //static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  return p;

  // Return blank-padded name.
  //if(strlen(p) >= DIRSIZ)
  //  return p;
  //memmove(buf, p, strlen(p));
  //memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  //return buf;
}

int find(char* path, char* name) {
    //printf("DEBUG: start find %s %s...\n", path, name);
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
      return 0;
    }

    if(fstat(fd, &st) < 0){
      close(fd);
      return 0;
    }

    switch(st.type) {
        case T_FILE:
            printf("Cannot find a file...\n");
            close(fd);
            return 0;
            break;
        case T_DIR:
            strcpy(buf, path);
            //printf("DEBUG: %s %d\n", buf, strlen(buf));
            p = buf+strlen(buf);
            *p++ = '/';
            //printf("DEBUG: %s %s\n", p-1, p);
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0) {
                    //printf("DEBUG4: %s %s\n", path, name);
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0){
                    printf("ls: cannot stat %s\n", buf);
                    continue;
                }
                int cmpDir0 = strcmp(fmtname(buf), ".");
                int cmpDir1 = strcmp(fmtname(buf), "..");
                if (st.type == T_FILE || st.type == T_DEVICE) {
                    int result = strcmp(p, name);
                    if(result == 0) {
                        printf("%s/%s\n", path, p);
                    }
                } else if (st.type == T_DIR && cmpDir0 !=0 && cmpDir1 != 0){
                    //printf("DEBUG: In dir %s found dir %s | %s\n", path, p);

                    // path: .
                    // p: a
                    char* newpath = malloc(sizeof(char)*(strlen(path) + 1));
                    strcpy(newpath, path);
                    strcat(newpath, "/");
                    // newpath: ./
                    strcat(newpath, p);
                    // newpath: ./a
                    find(newpath, name);
                    //printf("DEBUG: return from %s %s\n", newpath, name);
                    free(newpath);
                }
            }
            break;
    }
    close(fd);
    return 0;
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "Usage: find [directory] [name]...\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}