#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int i;
	if(argc != 3)
	{
		printf("Unexpected number of arguments");
		return 0;
	}
	int h,c,pid,p,status;
	h = atoi(argv[1]);
	c = atoi(argv[2]);
	pid = getpid();
	printf("%d: Process starting\n", pid);
	printf("%d: Parent's id = %d\n", pid, getppid());
	printf("%d: Height in the tree = %d\n", pid, h);
	printf("%d: Creating %d children at height %d\n", pid, c, h-1);
	if(h > 1)
	{
		for(i = 0;i < c;++i)
		{
			if((pid = fork()) == 0)
			{
				char buffer1[5], buffer2[5];
				sprintf(buffer1, "%d", h-1);
				sprintf(buffer2, "%d", c);
				execlp("./test", "test", buffer1, buffer2, NULL);
			}
		}
		while(wait(&status) > 0);
		printf("%d: Terminating at height %d\n", pid, h);
	}
	return 0;
}
