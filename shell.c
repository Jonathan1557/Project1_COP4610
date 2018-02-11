#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "functionality.c"

char * nxtoken(char *);
char * advance(char *);
char * firstadvance(char *);
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
printf("%s@%s :: %s =>",getenv("USER"), getenv("MACHINE"),getenv("PWD"));

getline(&line, &size, stdin);

arg = parse(line);     

int i = 0;
int a = 0;

while(i < argcount(arg)){
while(arg[i][a] != 0) {
//	printf("%d\n", arg[i][a]);
	printf("Arg[%d][%d]: %s\n",i,a,arg[i][a]);
	a++;
	}
i++;
a=0;
}


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

arg = malloc(200); // we know how many commands there are, so allocate space for them
arg[cmdcount] = 0;	// terminate the args with a null pointer;
itr = line;
int i = 0;
int a;
char * start;
char * nxtstart = line;	// this is where the next command starts

while(i < cmdcount){
argcount = 1;
start  = firstadvance(nxtstart);		// this is where this command starts
itr = start;
itr++;
while(*itr != '\0' && *itr != '|'){
if(*itr == '<' | *itr == '>' | *itr == '&'){argcount++;}
else if(*itr != '\n' && *itr != ' ' && *(itr - 1) == ' ' && *itr != '|'){argcount++;}
else if(*itr != '\n' && *itr != ' ' && (*(itr -1) == '>' | *(itr - 1) == '<') && *itr != '|'){argcount++;}
itr++;
}

//arg[i] = malloc(argcount + 1);					//set arg[i] to hold all its arguments
arg[i] = malloc(200);					//set arg[i] to hold all its arguments
arg[i][argcount] = 0; 						// terminate it with a null pointer
nxtstart = itr;							// this is where the next command starts

//arg[i][0] = start;
itr = start;

// set each argument in arg[i]
a = 0;
while(a < argcount){
arg[i][a] = nxtoken(itr);
itr = advance(itr);
a++;
}

arg[i][argcount] = 0;

i++;
} 

arg[cmdcount] = 0; 			// make sure arg is null terminated

return arg;
}

char * nxtoken(char * line){
char * token = malloc(strlen(line));
strcpy(token, line);
// change char after token to null 
char * itr = token;
while(*itr != '\0'){
if(*itr == ' ' | *itr == '|' | *itr == '\n'){
	*itr = '\0'; 
} 
itr++;
}
return token;
}

char * firstadvance(char * line){
char * itr = line;
while(itr != 0){
  if(*itr != ' ' && *itr != '&' && *itr != '|'){return itr;}
  itr++;
 }
return 0;
}


char * advance(char * line){	// returns the first char of the next token, or null if there are no more tokens.
char * itr = line;
char * start = itr; 		// this is the starting position
while(*itr != '\0' && *itr != '|'){
	if(itr != start){
		if(*itr == '<' | *itr == '>' | *itr == '&'){return itr;}
		else if(*itr != ' ' && *(itr - 1) == ' ' && *itr != '|'){return itr;}
		else if(*itr != ' ' && (*(itr -1) == '>' | *(itr - 1) == '<') && *itr != '|'){return itr;}
	}
itr++;
}
return 0;
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

