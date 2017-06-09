#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct userdata
{
	int pid;
	int ppid;
	int cpu;
	char state[500];
} userdata;

int main()
{
	int fd = open("/dev/process_list_device", O_RDONLY);
	if(fd == -1)
	{
		printf("Could not open device...\n");
		return 1;
	}
	userdata *buf = (userdata *) malloc(sizeof(userdata));
	int read_err = 0;
	while(!read_err)
	{
		read_err = read(fd, buf, sizeof(userdata));
		if(!read_err) printf("PID=%d PPID=%d CPU=%d STATE=%s\n", buf->pid, buf->ppid, buf->cpu, buf->state);
	}
	close(fd);
	return 0;
} 
