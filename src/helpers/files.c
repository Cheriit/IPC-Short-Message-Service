//
// Created by cherit on 12/27/19.
//

#include "files.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int read_to_char(int file, char separator, char** string){
    char detector[1];
    int line_beginning = lseek(file, 0, SEEK_CUR);
    int line_size = 0;
    while(read(file, detector, 1))
    {
        if( detector[0] == separator) break;
        line_size++;
    }
    lseek(file, line_beginning, SEEK_SET);
    free(*string);
    *string = (char*)malloc((line_size)* sizeof(char));
    read(file, *string, line_size);
    lseek(file, 1, SEEK_CUR);
    return line_size;
}

int get_line(int file, char** string) {
    return read_to_char(file, '\n', &(*string));
}
