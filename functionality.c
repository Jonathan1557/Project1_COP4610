#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define S_COMMAND 33261
#define S_DIR 16749
#define S_FAILURE 194
#define INPUT 1 
#define OUTPUT 2


#define EXIT 0
#define CD 1
#define ECHO 2
#define ETIME 3
#define IO 4

void control(char *** arg);				// the main controller function. decides what other functions needs to be called
void exeManager(char ** arg);				// decides how the current command needs to execute
void pipeSolo(char *** arg);				// a single pipe
void pipeManager(char *** arg, int i);			// multiple pipes. i = number of pipes
char * translate(char * s);				// if s is an envirnomental variable, it returns that value. Otherwise it returns s.
int myexe(char ** arg);					// basic execution with no redirection
int exeRedirOutput(char ** arg, char * output);		// execution with output redirection
int exeRedirInput(char ** arg, char * input);		// execution with input redirection
int argcount(char *** arg);				// returns the number of commands in arg.

void control(char *** arg){
int count = argcount(arg);
if(count == 0){return;}					// if there are no commands, do nothing
if(count == 1){exeManager(arg[0]);}			// if there is one command, execute it
else if(count == 2){pipeSolo(arg);}			// if there are two commands, put them in a single pipe
else{pipeManager(arg, count - 1);}			// if there are more than two commands, use multiple pipes
}

void cd(char ** arg){					// change the current directory
struct stat buf;
if(arg[1] == 0)						// if cd has no destination argument, make the destination $HOME
 {
  arg[1] = malloc(30);
  
  arg[1] = translate("$HOME");
  setenv("PWD",arg[1],1);
  chdir(arg[1]);
  return;
  // change to $home directory
 }
if(arg[2] != 0)						// if cd has more than one argument, return an error and stop
 {
  printf("Error: too many arguments for cd\n");
  return;
 }
// else, change to directory specified by arg[1]

stat(arg[1], &buf);
if((buf.st_mode & S_IFMT) == S_IFDIR){
setenv("PWD",arg[1],1);					// change $PWD and the working directory to the destination directory
chdir(arg[1]);}						//
else{printf("Invalid directory for cd\n");}
}

void io(char ** arg)
{
int i = 0;
while(arg[i + 1] != 0){strcpy(arg[i],arg[i + 1]);i++;}
arg[i] = 0;

int status;
pid_t pid = fork();
if(pid == 0)
{
exeManager(arg);
}
else
 {// parent
  
  waitpid(pid,&status,0);
	//print io data

 char src[50], dest[50];

   strcpy(dest,  "/proc/");

   strcat(dest, src);
   strcpy(src, "/io");

   printf("Final destination string : |%s|\n", dest);

/*
 char * narg[50];
 narg[0] = "/bin/head/";
 //narg[1] = "/proc/";
 strcpy(narg[1], "/proc/");
 printf("%s\n",narg[1]);//**************************
 printf("TEST\n");//*******************************
 strcat(narg[1],pid);
 printf("%s\n",narg[1]);//**************************
 printf("TEST\n");//*******************************
 strcat(narg[1],"/io");
 printf("%s\n",narg[1]);//**************************
 printf("TEST\n");//*******************************
 narg[2] = 0;
 printf("%s\n",narg[1]);//**************************
 printf("TEST\n");//*******************************
 exeManager(narg);*/
 }
}

void etime(char ** arg)						// etime built in
{
int i = 0;							//
while(arg[i + 1] != 0){strcpy(arg[i],arg[i + 1]);i++;}		// step one: remove "etime" from the arg array
arg[i] = 0;							//

struct timeval tv1;						// tv1 is the start time
struct timeval tv2;						// tv2 is the end time

gettimeofday(&tv1, NULL);					// set tv1

exeManager(arg);						// execute the function
gettimeofday(&tv2, NULL);					// set tv2

printf("Elapsed time:%d.%d\n",tv2.tv_sec - tv1.tv_sec,tv2.tv_usec - tv1.tv_usec ); // return the difference between tv2 and tv1
}

void echo(char ** arg){						// echo built in

int i = 0;							//
while(arg[i + 1] != 0){strcpy(arg[i],arg[i + 1]);i++;}		// step one: remove "echo" from the arg array
arg[i] = 0;							//


i = 0;								// 
while(arg[i] != 0){arg[i] = translate(arg[i++]);}		//translate any envirnomental variables

i = 0;
while(arg[i] != 0){						//
printf("%s ",arg[i++]);						// print each argument
}
printf("\n");							// end with a new line
}

void exeManager(char ** arg){ 					// this functions decides if a command needs io redirection
int i = 0;
char * itr = arg[0];
char * file;
int redir = 0;
while(itr != 0 && (strcmp(itr,">") != 0 && strcmp(itr, "<") != 0)){
itr = arg[++i];} 						// if there is a > or < symbol, set itr to that
 if(itr != 0){
  if(strcmp(itr, "<") == 0){redir = INPUT;}			// if there is a < symbol, we will need input redirection,
  if(strcmp(itr,">")==0){redir = OUTPUT;}			// if there is a > symbol, we will need output redirection
 }
if(isBuiltIn(arg[0]) != -1){					// if this is a built in function, use the associated function
switch(isBuiltIn(arg[0])){

case EXIT:							// for exit, just print the exit message. The shell will exit
printf("Exiting Shell....\n"); 					// after this
break;

case CD:							// for cd, do nothing. Cd is handled in shell.c
break;

case IO:
io(arg);
break;

case ETIME:							// for etime, call the etime function
etime(arg);
break;

case ECHO:							// for echo, call the echo function
echo(arg);
break;

default:
break;
}
return;								// since it was a built in, return and don't execute anything else
}
if(redir == 0){myexe(arg);}					// if there's no redirection, call myexe
else{								// if there is redirection,
  file = arg[i + 1];						// set the file variable to the redirection file
  arg[i] = 0;							// and replace the redirection symbol with a null pointer
 }
if(redir == INPUT){exeRedirInput(arg, file);}			// if it's input redirection, call exeRedirInput
if(redir == OUTPUT){exeRedirOutput(arg, file);}			// if it's output redirection, call exeRedirOutput
}


char * translate(char * s){					// translate returns the value of an envirnomental variable
 if(s[0] == '$'){						// if the first char in s is $, it's an envirnomental variable
 s = getenv(++s);}						// so return the value of that environmental variable
 else if(s[0] == '~'){						// if s is ~,
 s = getenv("HOME");}						// return the value of $HOME
 return s;							// if s isn't an environmental variable, return s unchanged
}

int myexe(char ** arg){						// this is the basic execution function
int status;		
pid_t pid = fork();
if(pid == -1){return 1;}
else if(pid == 0){		// for the child
execv(arg[0], arg);		// execute arg
}
else{
 waitpid(pid, &status,0);	// parent needs to wait on the child
 }

return 0;

}

int exeRedirInput(char ** arg, char * input){			// this is input redirection
int status;
pid_t pid = fork();
if(pid == -1){return 1;}
else if(pid == 0){
fclose(stdin);

FILE * fd = fopen(input, "r");					// open the input file

dup2(fileno(fd), 0);						// replace stdin with the input file
execv(arg[0], arg);						// execute arg
}
else{
 waitpid(pid, &status,0);	// parent needs to wait
 }

return 0;
}


int exeRedirOutput(char ** arg, char * output){			// this is output redirection
int status;
pid_t pid = fork();
if(pid == -1){return 1;}
else if(pid == 0){
fclose(stdout);

FILE * fd = fopen(output, "w");					// open the output file
dup2(fileno(fd), 1);						// replace stdout with the output file

execv(arg[0], arg);						// execute arg
}
else{
 waitpid(pid, &status,0);	// parent needs to wait
 }

return 0;
}

void pipeManager(char *** arg, int i){
int status;
pid_t pid = fork();
if(pid == 0){

int save_in, save_out; 				// 
save_in = dup(STDIN_FILENO); 			//  this bit comes from stackoverflow
save_out = dup(STDOUT_FILENO); 			//  it saves stdin and stdout for later

int fd[2];
pipe(fd);
pid_t pid2 = fork();
	if(pid2 == 0){
	fclose(stdout);				// this section does the output of the first pipe
	dup2(fd[1],1);
		if(i == 1){
		pipeSolo(arg);		
		}
		else
		{
		pipeManager(arg, i -1);
		}
	}
	else{
	waitpid(pid2, &status, 0);
	close(STDIN_FILENO);			// close stdin and use the pipe for input from now on
	dup2(fd[0],0);

	open(fileno(stdout));			// re-open stdout
	dup2(save_out, STDOUT_FILENO);		// final command
	close(fd[0]);
	close(fd[1]);
	exeManager(arg[i]);
	}
}
else{
 waitpid(pid, &status, 0);
 }

}

void pipeSolo(char *** arg){


int status;
pid_t pid = fork();
if(pid == 0){
int save_in, save_out; 				//  
save_in = dup(STDIN_FILENO); 			// this bit comes from stackoverflow
save_out = dup(STDOUT_FILENO); 			// it saves stdin and stdout for later

int fd[2];				//
pipe(fd);				// create the pipe

fclose(stdout);				// this section does the first command
dup2(fd[1],1);				// replace stdout with the pipe
exeManager(arg[0]);			// and execute the first command

close(STDIN_FILENO);			// close stdin and use the pipe for input from now on
dup2(fd[0],0);				//


open(fileno(stdout));			// re-open stdout
dup2(save_out, STDOUT_FILENO);		// for the final command

close(fd[0]);
close(fd[1]);
exeManager(arg[1]);			// execute the final command
}

else{
 waitpid(pid, &status, 0);		// the parent waits for the child to finish
 }

}

int argcount(char *** arg){		// return the number of commands in arg
int count = 0;
int i = 0;
while(arg[i++] != 0){count++;}
return count;
}
