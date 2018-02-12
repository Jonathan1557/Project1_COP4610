#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "functionality.c"
#include "path_res.c"

#define EXIT 0
#define CD 1
#define ECHO 2
#define ETIME 3
#define IO 4

char * nxtoken(char *);
char * advance(char *);
char * firstadvance(char *);
char *** parse(char *);
int isBuiltIn(char *);
void printCommand(int);

int main(){
char * line = "";
line = malloc(255);
size_t size = 255;
char *** arg;

int status;
pid_t pid;


LOOP:

printf("%s@%s :: %s =>",getenv("USER"), getenv("MACHINE"),getenv("PWD"));

getline(&line, &size, stdin);
arg = parse(line);

/*	
// test code --------------->
int i = 0;
int a = 0;

while(i < argcount(arg)){
	while(arg[i][a] != 0) {
		//	printf("%d\n", arg[i][a]);
		printf("Arg[%d][%d]: %s\n",i,a,arg[i][a]);
		char * absPath = resolve_path(arg[i][a]);
		if(isCommand(arg[i][a]) != -1 && strcmp(absPath,"expand_path() FAILURE") != 0){strcpy(arg[i][a],absPath);}
		printf("AbsPath: %s\n",arg[i][a]);
		a++;
	}
	i++;
	a=0;
}
// <-------------- test code

//while(strcmp(line, "exit") != 0){
*/

if(strcmp(arg[0][0],"cd") == 0)
{
cd(arg[0]);
}

pid = fork();

if(pid == 0){
control(arg);
}
else{
waitpid(pid, &status,0);
if(strcmp(arg[0][0], "exit") != 0){goto LOOP;}
}
//} // end of while loop    
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

int isBuiltIn(char * cmd){
int code = -1;
if(strcmp(cmd,"exit") == 0){code = EXIT;}
else if(strcmp(cmd,"cd") == 0){code = CD;}
else if(strcmp(cmd,"echo") == 0){code = ECHO;}
else if(strcmp(cmd,"etime") == 0){code = ETIME;}
else if(strcmp(cmd,"io") == 0){code = IO;}
return code;
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
		default:
			break;
	}
}


