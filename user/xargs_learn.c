// // xargs.c
// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"
// #include "kernel/fs.h"

// // 带参数列表，执行某个程序
// void run(char *program, char **args) {
// 	if(fork() == 0) { // child exec
// 		exec(program, args);
// 		exit(0);
// 	}
// 	return; // parent return
// }

// int main(int argc, char *argv[]){
// 	char buf[2048]; // 读入时使用的内存池
// 	char *p = buf, *last_p = buf; // 当前参数的结束、开始指针
// 	char *argsbuf[128]; // 全部参数列表，字符串指针数组，包含 argv 传进来的参数和 stdin 读入的参数
// 	char **args = argsbuf; // 指向 argsbuf 中第一个从 stdin 读入的参数
// 	for(int i=1;i<argc;i++) {
// 		// 将 argv 提供的参数加入到最终的参数列表中
// 		*args = argv[i];
// 		args++;
// 	}
// 	char **pa = args; // 开始读入参数
// 	while(read(0, p, 1) != 0) {
// 		if(*p == ' ' || *p == '\n') {
// 			// 读入一个参数完成（以空格分隔，如 `echo hello world`，则 hello 和 world 各为一个参数）
// 			*p = '\0';	// 将空格替换为 \0 分割开各个参数，这样可以直接使用内存池中的字符串作为参数字符串
// 						// 而不用额外开辟空间
// 			*(pa++) = last_p;
// 			last_p = p+1;

// 			if(*p == '\n') {
// 				// 读入一行完成
// 				*pa = 0; // 参数列表末尾用 null 标识列表结束
// 				run(argv[1], argsbuf); // 执行最后一行指令
// 				pa = args; // 重置读入参数指针，准备读入下一行
// 			}
// 		}
// 		p++;
// 	}
// 	if(pa != args) { // 如果最后一行不是空行
// 		// 收尾最后一个参数
// 		*p = '\0';
// 		*(pa++) = last_p;
// 		// 收尾最后一行
// 		*pa = 0; // 参数列表末尾用 null 标识列表结束
// 		// 执行最后一行指令
// 		run(argv[1], argsbuf);
// 	}
// 	while(wait(0) != -1) {}; // 循环等待所有子进程完成，每一次 wait(0) 等待一个
// 	exit(0);
// }


#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define STDIN_FILENO 0
#define MAXLINE 1024

int main(int argc, char *argv[])
{
    char line[MAXLINE];
    char* params[MAXARG];
    int n, args_index = 0;
    int i;

    char* cmd = argv[1];//实际上是从xargs开始计算的，argv[0]->xargs，所以需要用argv[1]
    for (i = 1; i < argc; i++) params[args_index++] = argv[i];

    while ((n = read(STDIN_FILENO, line, MAXLINE)) > 0) //管道传递信息，所以直接从stdin读入就好
    {
        if (fork() == 0) // child process
        {
            char *arg = (char*) malloc(sizeof(line));
            printf("%d\n",sizeof(line));
            int index = 0;
            for (i = 0; i < n; i++) //n：就是读入了多少字节，一个字节一个字节判断
            {
                if (line[i] == ' ' || line[i] == '\n')//一个参数读取结束
                {
                    arg[index] = 0;//把空格和换行符替换成\0，一个参数就是一个字符串。
                    params[args_index++] = arg;
                    index = 0;
                    arg = (char*) malloc(sizeof(line));
                }
                else arg[index++] = line[i];
            }
            arg[index] = 0;
            params[args_index] = 0;
            exec(cmd, params);
        }
        else wait((int*)0);
    }
    exit(0);
}


#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
	// 第一次传进来的xagrs echo bye
	// 然后要从标准输入中读取数据，直到遇到\n
	char buff[1024];
	int offset = 0;
	int len = 0;
	int i =0;
	char ch;
	  // 传给执行程序依旧使用argv，因为考虑到argc不一定是三个值，可能5，6个都可能，单独创建新的argv需要动态分配内存，比较麻烦
  // argv[0] = “xargs”；我们将argv[0]剔除掉，后面的参数往前挪一位，空出最后一个元素，接受新输入的一行数据即可组成新的argv
	while(i +1 < argc)
	{
		argv[i] = argv[i+1];
		//printf("argv[%d] = [%s]\n", i, argv[i]);            
		i++;
	}
	while((len = read(0, &ch, sizeof(ch))) > 0)
	{
		// 判断遇到\n
		if('\n' == ch)
		{
			// 把buff添加进argv[], 然后exec
			// fork，然后子进程执行argv[1]，然后argv[2]及以后的数据+buff是新的argv
			argv[i] = buff;
			int pid = fork();
			if(pid < 0)
			{
				exit(1);
			}else if(0 == pid)
			{
				exec(argv[0], &argv[0]);
			}else
			{
				wait(0);
			}
			 // 执行完这一行的数据，重置接收区
			memset(buff,0x00,sizeof(buff));
			offset = 0;
		}else
		{

			buff[offset++]=ch;
		}
	}
	exit(0);
}
