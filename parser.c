//
//  main.c
//  proj1_test
//
//  Created by Doug Ott II on 1/16/18.
//  Copyright © 2018 Ott Productions. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// primary functions
void setup(char*);
void prompt(void);
char* readInput(void);
char** parse(char*);
void execute(void);
void clean(void);

// secondary functions
char* parse_whitespace(char *line);
char** parse_arguments(char *line);

int main(int argc, const char * argv[]) {

    // DATA:
    char* line;
    char** cmd;
    
    // setup
    //setup(line);
    
    // print prompt
    prompt();
    
    // get user input
    line = readInput();
    
    // parse
    cmd = parse(line);

    // execute
    execute();
    
    // clean
    clean();
    
    return 0;
}


// PRIMARY FUNCTIONS
// -----------------

void setup(char* line) {
    
}

void prompt() {
    printf("cmd: ");
}

char* readInput() {
    char* input;
    input = malloc (sizeof (char) * 256);
    fgets(input, 256, stdin);
    return input;
}

char** parse(char* line) {
    char** args;
    
    line = parse_whitespace(line);
    args = parse_arguments(line);
    //args = expand_variables(args);
    //args = resolve_paths(args);
    
    return args;
    //return NULL;
}

void execute() {
    
}

void clean() {

}

// SECONDARY FUNCTIONS
// -------------------

char* parse_whitespace(char* line) {
    // takes line and returns it after removing unnecessary whitespace
    
    // Remove Leading Whitespace:
    int hasLeadingSpace = 1;
    while (hasLeadingSpace==1) {
        if (line[0] == ' ')
            line++;
        else
            hasLeadingSpace=0;
    }
    
    /*
    // Remove Extra Middle Whitespace:
    for (int i=0; i<strlen(line); i++) {            // for each char in line...
        if (isspace(line[i]) && isspace(line[i+1])) {   // if two whitespace adjacent...
            char* tempStr;                                // creat temp string
            tempStr = malloc (sizeof (char) * 256);
            printf("extra: (%c)\n", line[i]);
            for (int i2=0; i2<strlen(line); i2++) { // for each char in line...
                if (i2 != i) {                          // if on index of extra whitespace
                    tempStr[i2] = line[i2];             // copy line to temp
                    line = tempStr;                     // assign temp to line
                    printf("line: %s!\n", line);
                }
            }
            i = 0;  // reset index
        }
    }
    */
    
    // Remove Trailing Whitespace:

    // find initial index of null terminator
    int nullIndex = 0;
    for (int i=0; i<strlen(line)+1; i++) {
        if (line[i]=='\0')
            nullIndex = i;
    }
    
    int hasTrailingSpace = 1;
    while (hasTrailingSpace==1) {
        if (isspace (line[nullIndex-1])) {       // if last char is whitespace...
            line[nullIndex-1] = '\0';             // ... change last char to NULL term.
            nullIndex--;                           // ... decrement index of NULL to reflect change
        }
        else
            hasTrailingSpace=0;
    }
    
    return line;
}

char** parse_arguments(char* line) {
    char* arg;      // single argument
    char** args;    // array of arguments
    
    // initialize
    arg = malloc (sizeof (char) * 256);
    args = (char**) malloc (sizeof (char) * 256);
    int currentArg = 0; // arg currently being filled
    
    // fill arguments array
    for (int currentChar=0; currentChar<strlen(line); currentChar++) {
        arg[currentChar] = line[currentChar];
        if (isspace (line[currentChar])) {
            args[currentArg][currentChar] = *arg;
            currentArg++;
        }
    }

    return args;
    //return NULL;
}
