#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>

int main(void)
{
	int fd; /* Descriptor del FIFO*/
	int len; /* Bytes leidos del FIFO*/
	char buf[PIPE_BUF];
	mode_t mode = 0666;

	if((mkfifo("fifo1", mode)) < 0)
	{
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}

	/* Apertura del FIFO de solo lectura*/
	if ((fd = open("fifo1", O_RDONLY)) < 0)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}

	/* Lectura y visualización de la salida del FIFO hasta EOF */
	while((len = read(fd, buf, PIPE_BUF - 1)) > 0)
	{
		printf("rdfifo read: %s", buf);
	}

	close(fd);

	exit(EXIT_SUCCESS);
}