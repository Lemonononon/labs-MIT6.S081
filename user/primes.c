#include "kernel/types.h"
//#include "kernel/stat.h"
#include "user/user.h"

#define STDIN   0
#define STDOUT  1
#define STDERR  2

#define READEND   0
#define WRITEEND  1

// p = get a number from left neighbor
// print p
// loop:
//     n = get a number from left neighbor
//     if (p does not divide n)
//         send n to right neighbor

int 
main( int argc, char *argv[] )
{
    int p[2], number[40];
    int index = 0;
    for ( int i = 2; i < 36; i++)
    {
        number[index++] = i; //共有 2-35 34个number
    }

    
    while ( index > 0 )
    {
        pipe(p);
        uint pid = fork();
        if (pid < 0) 
        {
            fprintf(STDERR,"fork error\n");
            exit(0);
        }
        else if (pid>0)
        {
            close(p[READEND]);
            for (int i = 0; i < index; i++)
            {
                write(p[WRITEEND], &number[i], 4);
            }
            close(p[WRITEEND]);
            wait(0);
            exit(0);
        }

        else{
            int tmp = 0;
            int prime;
            index = 0;
            close(p[WRITEEND]);

            //first
            if (read(p[READEND], &tmp, 4)){
                prime = tmp;
                printf("prime %d\n", prime);
            }
            else exit(0);
            
            while(read(p[READEND], &tmp, 4))
            {
                if( tmp%prime != 0 ) number[index++] = tmp;
            }
            close(p[READEND]);
        }
        
    }

    exit(0);
}