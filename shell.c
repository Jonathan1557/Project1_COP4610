#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "functionality.c"
//#include "path_res.c"
#include <sys/mman.h>

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


int main(){


int background = 0;
char * line = "";
line = malloc(255);
size_t size = 255;
char *** arg;

int status;
pid_t pid;

int * semaphore = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);   // psuedo-semaphore for background processing
int * QN = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);     	    // QN is the current queue number
*QN = 0;											    // QN starts at 0
int * pqueue = mmap(NULL, sizeof(int) * 40, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); // pqueue is the list of running processes

int i;
int a;
i = 0;
while(i < 40){pqueue[i++] = 0;}			// start each member of pqueue as 0 (nothing running)

char * absPath;				

LOOP:											// LOOP is the main loop for the shell.
background = 0;										// we start off assuming foreground processing
printf("%s@%s :: %s =>",getenv("USER"), getenv("MACHINE"),getenv("PWD"));		// print the prompt

getline(&line, &size, stdin);								// get the user input
arg = parse(line);									// and parse it



// if last token is &
i = 0;											//
while(arg[i + 1] != 0){i++;}								//
a = 0;											// find the last token.
while(arg[i][a + 1] != 0){a++;}								//

if(strcmp(arg[i][a],"&")==0)								// if the last token is &,
{											//
arg[i][a] = 0;										// remove & from command list
background = 1;										// and say we will do background processing
}

*semaphore = 0;										// set our lock to 0

if(strcmp(arg[0][0],"cd") == 0)								//
{											// if the command is cd,
cd(arg[0]);										// call the cd function
}											//

if(strcmp(arg[0][0],"exit") == 0)							// if the commmand is exit, 
{											
int done = 1;										//
while(done != 0)									//
{											// wait for each child process 
 i = 0;											// to finish
 done = 0;										//
 while(i < 40)										//
  {											//
   if(pqueue[i] != 0){done++;}								//
   i++;											//
  }
}
printf("Exiting Shell....\n");								// then print the exit message
return;
}

	// test code --------------->
	i = 0;
	a = 0;
	
	while(i < argcount(arg)){
		while(arg[i][a] != 0) {
			//      printf("%d\n", arg[i][a]);
			printf("Arg[%d][%d]: %s\n",i,a,arg[i][a]);
			printf("AbsPath[0][0]: %s\n",arg[0][0]); // ************************************************
			printf("%d,%d\n",i,a);//***********************************************************
			char middleMan[255];
			strcpy(middleMan, resolve_path(arg[i][a], a));
			//absPath = resolve_path(arg[i][a], a);
			//absPath = middleMan;
			printf("AbsPath[0][0]: %s\n",arg[0][0]); // ************************************************
			if (absPath==NULL) {
				printf("ERROR: arg invalid");
				//    break;
			}
			//arg[i][a] = absPath;    // store back to arg <----- not tested
			strcpy(arg[i][a], middleMan);
			printf("AbsPath: %s\n",arg[i][a]);
			a++;
		}
		i++;
		a=0;
	}
	// <-------------- test code

i= 0;
a= 0;
while(i < argcount(arg))
{
 while(arg[i][a] != 0)
 {
  printf("arg[%d][%d]: %s\n",i,a,arg[i][a]);
  a++;
 }
i++;
a=0;
}
*/


if(background == 0){							// foreground processing
pid = fork();

if(pid == 0){
control(arg);								// control will execute arg properly
return;
}
else{
waitpid(pid, &status,0);
}
if(strcmp(arg[0][0], "exit") != 0){goto LOOP;}
}

else 									// background processing
{
 pid = fork();
 if(pid == 0)
  {
   int status2;
   int currentQN = *QN;
   *QN = *QN + 1;
   pid_t pid2 =fork();

   if(pid2 == 0)
    {
     printf("[%d]  %d       %s",currentQN,getpid(),line);
     pqueue[currentQN] = getpid();
     *semaphore = 1;
     control(arg);
     return;
    }
   else
    {
     waitpid(pid2,&status2,0);
     pqueue[currentQN] = 0;
     printf("[%d] is done %d    %s",currentQN,pid2,line);
     printf("%s@%s :: %s =>",getenv("USER"), getenv("MACHINE"),getenv("PWD"));
     return;
    }
  }
 else{ //parent
  while(*semaphore ==0){}
  goto LOOP;
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

arg = malloc(200); 					// allocate space
arg[cmdcount] = 0;					// terminate the args with a null pointer;
itr = line;
int i = 0;
int a;
char * start;
char * nxtstart = line;					// this is where the next command starts

while(i < cmdcount){
argcount = 1;
start  = firstadvance(nxtstart);			// this is where this command starts
itr = start;
itr++;
while(*itr != '\0' && *itr != '|'){										// 
if(*itr == '<' | *itr == '>' | *itr == '&'){argcount++;}							// count the arguments
else if(*itr != '\n' && *itr != ' ' && *(itr - 1) == ' ' && *itr != '|'){argcount++;}				//
else if(*itr != '\n' && *itr != ' ' && (*(itr -1) == '>' | *(itr - 1) == '<') && *itr != '|'){argcount++;}	//
itr++;
}

arg[i] = malloc(200);					//set arg[i] to hold all its arguments
arg[i][argcount] = 0; 						// terminate it with a null pointer
nxtstart = itr;							// this is where the next command starts

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

char * firstadvance(char * line){				// make sure there isn't any whitespace or & before first command
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




