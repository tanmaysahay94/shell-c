#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>

#define error_handling(msg) do {perror(msg); exit(EXIT_FAILURE);} while(0)
#define BUF_SIZE 1024*1024*5

int main(int argc, char** argv)
{
	int i;
	char path[100000];
	strcat(path, "/proc/");
	if (argc >= 2)
		strcat(path, argv[1]);
	strcat(path, "/status");
	FILE* fp;
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	fp = fopen(path, "r");
	if (fp == NULL)
	{
		printf("Pid %s does not exist.\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	char attribute[100];
	while ((read = getline(&line, &len, fp)) != -1)
	{
		for (i = 0; i < read; i++)
		{
			if (line[i] == ':')
				break;
			attribute[i] = line[i];
		}
		attribute[i] = '\0';
		if (strcmp(attribute, "State") == 0 || strcmp(attribute, "PPid") == 0 || strcmp(attribute, "Threads") == 0)
			printf("%s", line);
	}
	return 0;
}
