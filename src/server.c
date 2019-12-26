#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <zconf.h>

char** read_line(int file){
    char detector = 0;
    int line_beginning = lseek(file, 0, SEEK_CUR);
    int line_size = 0;
    while(read(file, detector, 1))
    {
        if( detector == '\n') break;
        line_size++;
    }
    lseek(file, line_beginning, SEEK_SET);
    char** line_content = (char**) malloc(line_size* sizeof(char*));
    read(file, line_content, line_size);
    return line_content;
}

int main(int argc, char** argv) {
    int config_file = open('../config.txt', O_RDONLY);
    if(config_file==0)
    {
        return 0;
    }
    char** string;
    string = read_line(config_file);
    printf('%s\n' ,string);

    close(config_file);
    printf("Hello, Server!\n");
    return 0;
}
