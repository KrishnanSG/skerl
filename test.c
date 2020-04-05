#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main()
{
    int l = creat('pp.txt', 'rw');
    int file_desc = open("lllll.txt", O_CREAT | O_WRONLY | O_RDWR | O_APPEND);

    // here the newfd is the file descriptor of stdout (i.e. 1)
    dup2(file_desc, 1);

    // All the printf statements will be written in the file
    // "tricky.txt"
    printf("I will be printed in the file tricky.txt\n");

    return 0;
}