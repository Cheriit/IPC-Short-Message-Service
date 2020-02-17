//
// Created by cherit on 12/27/19.
//

#include "files.h"
#include "../definitions.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int read_to_char(int file, char separator, char string[]){
    char detector[1];
    char buffer[MAX_TEXTFIELD_SIZE];
    int line_beginning = lseek(file, 0, SEEK_CUR);
    int line_size = 0;
    while(read(file, detector, 1))
    {
        if( detector[0] == separator) break;
        line_size++;
    }
    lseek(file, line_beginning, SEEK_SET);
    line_size= read(file, buffer, line_size);
    buffer[line_size] = '\0';
    strcpy(string, buffer);
    lseek(file, 1, SEEK_CUR);
    return line_size;
}

int get_line(int file, char string[]) {
    return read_to_char(file, '\n', string);
}
