#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "functionality.c"

char *** parse(char *);
int isCommand(char *);
void printCommand(int);

int main(){
char * line;
line = malloc(255);
size_t size = 255;
char *** arg;

while(strcmp(line, "exit") != 0)
{
printf("myshell> ");
//scanf("%s",line);

getline(&line, &size, stdin);

arg = parse(line);     

int i = 0;
while(arg[0][i] != 0) {printf("%s\n", arg[0][i]);i++;}

}
    
return 0;
}

char *** parse(char * line){
char *** arg;
int cmdcount = 1;
int argcount = 1;

//first pass, determine cmdcount
char * itr = line;
while(*itr != '\0'){
if(*itr == '|'){cmdcount++;};
itr++;
}


arg = malloc(cmdcount + 1); // we know how many commands there are, so allocate space for them
arg[cmdcount] = 0;	// terminate the args with a null pointer;
itr = line;
int i = 0;
int a = 1;
char * start = line;	// this is where this command starts
char * nxtstart;	// this is where the next command starts


itr++;
while(*itr != '\0' && *itr != '|'){
if(*itr == '<' | *itr == '>' | *itr == '&'){argcount++;}
else if(*itr != ' ' && *(itr - 1) == ' ' && *itr != '|'){argcount++;}
else if(*itr != ' ' && (*(itr -1) == '>' | *(itr - 1) == '<') && *itr != '|'){argcount++;}
itr++;
}

arg[i] = malloc(argcount + 1);
arg[i][argcount] = 0; // terminate it with a null pointer
nxtstart = itr;	// this is where the next command starts


arg[i][0] = start;
itr = start;
a = 1;

itr++;
while(*itr != '\0' && *itr != '|'){
if(*itr == '<' | *itr == '>' | *itr == '&'){arg[i][a++] = itr;}
else if(*itr != ' ' && *(itr - 1) == ' ' && *itr != '|'){arg[i][a++] = itr;}
else if(*itr != ' ' && (*(itr -1) == '>' | *(itr - 1) == '<') && *itr != '|'){arg[i][a++] = itr;}
itr++;
}


if(cmdcount > 1){
while(++i < cmdcount){
itr = nxtstart;
start = nxtstart;
argcount = 1;

itr++;
while(*itr != '\0' && *itr != '|'){
if(*itr == '<' | *itr == '>' | *itr == '&'){argcount++;}
else if(*itr != ' ' && *(itr - 1) == ' ' && *itr != '|'){argcount++;}
itr++;
}

arg[i] = malloc(argcount + 1);
arg[i][argcount] = 0; // terminate it with a null pointer
nxtstart = itr;	// this is where the next command starts

arg[i][0] = start;
itr = start;
a = 1;

itr++;
while(*itr != '\0' && *itr != '|'){
if(*itr == '<' | *itr == '>' | *itr == '&'){arg[i][a++] = itr;}
else if(*itr != ' ' && *(itr - 1) == ' ' && *itr != '|'){arg[i][a++] = itr;}
else if(*itr != ' ' && (*(itr -1) == '>' | *(itr - 1) == '<') && *itr != '|'){arg[i][a++] = itr;}
itr++;
}

}}


// turn all spaces and | into null chars
itr = line;
while(*itr != '\0'){
if(*itr == ' ' | *itr == '|' | *itr == '\n'){
	*itr = '\0'; 
} 
itr++;
}

return arg;
}

int isCommand(char * token){
    // 1) compare input string token to a list of known commands
    // 2) return command code associated with that command
    if(strcmp(token, "exit")==0)
        return 0;
    else if(strcmp(token, "cd")==0)
        return 1;
    else if(strcmp(token, "echo")==0)
        return 2;
    else if(strcmp(token, "etime")==0)
        return 3;
    else if(strcmp(token, "io")==0)
        return 4;
    else if(strcmp(token, "ls")==0)
        return 5;
    else return -1;
}

void printCommand(int cmdCode){
    // print the name of the command based on the command code recived
    printf( "\nCommand Received: ");
    switch (cmdCode) {
        case -1:
            printf( "none");
            break;
        case 0:
            printf( "exit");
            break;
        case 1:
            printf( "cd");
            break;
        case 2:
            printf( "echo");
            break;
        case 3:
            printf( "etime");
            break;
        case 4:
            printf( "io");
            break;
        case 5:
            printf( "ls");
            break;

        default:
            break;
    }
}

