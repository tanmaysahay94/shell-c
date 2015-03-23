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

#define error_handling(msg) do {perror(msg); exit(EXIT_FAILURE);} while(0)
#define BUF_SIZE 1024*1024*5

void my_strcat(char *a, char *b)
{
	while (*a) a++;
	while (*a++ = *b++);
}

int my_strlen(char* string)
{
	char *copy = string;
	int len = 0;
	while (*copy)
	{
		copy++;
		len++;
	}
	return len;
}

int my_strcmp (const char *a, const char *b) 
{
    const unsigned char *p = (const unsigned char *)a;
    const unsigned char *q = (const unsigned char *)b;
    while (*p != '\0') 
    {
        if (*q == '\0') return  1;
        if (*q > *p)   return -1;
        if (*p > *q)   return  1;
        p++;
        q++;
    }
    if (*q != '\0') return -1;
    return 0;
}

int write_wrapper(char* string)
{
	int len = my_strlen(string);
	if (syscall(SYS_write, STDOUT_FILENO, string, len) != len)
	{
		syscall(SYS_write, STDERR_FILENO, "ERROR WHILE WRITING\n", 32);
		return -1;
	}
	return 0;
}

int main(int argc, char** argv)
{
	int i, j;
	char path[100000];
	my_strcat(path, "/proc/");
	if (argc >= 2)
		my_strcat(path, argv[1]);
	my_strcat(path, "/status");
	int fd, read_limit;
	char buffer[1025];
	fd = open(path, O_RDONLY);
	if (fd == -1)
		error_handling("open");
	else
	{
		while(read(fd, &buffer, 1024) != 0)
		{
			read_limit = my_strlen(buffer) < 1024 ? my_strlen(buffer) : 1024;
			char line[1024], start[10];
			char State[] = "State", PPid[] = "PPid", Threads[] = "Threads";
			int lineLen = 0;
			for (i = 0; i < read_limit; i++)
			{
				if (buffer[i] == '\n')
				{
					line[lineLen++] = '\0';
					for (j = 0; j < lineLen; j++)
					{
						if (line[j] == ':')
						{
							start[j] = '\0';
							break;
						}
						start[j] = line[j];
					}
					if(!my_strcmp(start + 1, State) || !my_strcmp(start + 1, PPid) || !my_strcmp(start + 1, Threads))
						write_wrapper(line);
					lineLen = 0;
				}
				line[lineLen++] = buffer[i];
			}
			write_wrapper("\n");
		}
	}
	return 0;
}
