#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("pid: %d\nPress any key to cause a page fault\n", getpid());
    char inp;
    scanf("%c", &inp);
    long pagesize = sysconf(_SC_PAGESIZE);
    unsigned char *p = malloc(pagesize + 1); /* Cross page boundaries. Page fault may occur depending on your allocator / libc implementation. */
    p[0] = 0;        /* Page fault. */
    p[pagesize] = 1; /* Page fault. */
    return 0;
}	
