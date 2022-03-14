#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char* path)
{   
    static char buf[DIRSIZ+1];
    char *p;
    for( p = path + strlen(path); p>=path && *p != '/'; p--);
    p++; //to the char after '/'
    if ( strlen(p) >= DIRSIZ ) return p;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = 0;
    return buf;
}

void find(char* path, char *str)
{
    char buf[512], *p;
    int fd;
    struct stat st;
    struct dirent de;

    if ( (fd = open(path, 0)) < 0)
    {
        fprintf(2, "find open %s error!", path);
        exit(1);
    }
    
    if ( fstat(fd, &st) < 0)
    {
        fprintf(2, "find stat %s error!", path);
        close(fd);
        exit(1);
    } 

    switch ( st.type )
    {
    case T_FILE:
        if ( strcmp(fmtname(path), str) == 0 ) printf("%s\n", path);
        break;
    
    case T_DIR:
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        while ( read(fd, &de, sizeof(de)) == sizeof(de) )
        {
            if (de.inum == 0) continue;

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if ( !strcmp(de.name, ".") || !strcmp(de.name, "..")) continue;
            find(buf, str);
        }
        break;
    }

    close(fd);
}

int 
main( int argc, char* argv[] )
{
    if (argc!=3) 
    {
        fprintf(2, "[usage] find [directory] [str] ...");
        exit(1);
    }
    find(argv[1], argv[2]);
    
    exit(0);
}