#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define S_COMMAND 33261
#define S_DIR 16749
#define S_FAILURE 194

void pipeSolo(char *** arg);
void pipeManager(char *** arg, int i);
char * translate(char * s);
int myexe(char ** arg);
int exeRedirOutput(char ** arg, char * output);
int exeRedirInput(char ** arg, char * input);
int argcount(char *** arg);

/*
int main()
{

struct stat buf;



char * word = "$HOME";

word = translate(word);

char * command = "/bin/ls";
char *** arg;
arg = malloc(4);
arg[0] = malloc(4);
arg[1] = malloc(2);
arg[2] = malloc(3);
arg[3] = malloc(3);

arg[0][0] = command;
//arg[0][1] = "-al";
arg[0][1] = word; 
arg[0][2] = 0;
arg[0][3] = 0;

command = "/bin/more";
arg[1][0] = command;
arg[1][1] = 0;

command = "/bin/grep";
arg[2][0] = command;
arg[2][1] = "M";
arg[2][2] = 0;

arg[3][0] = command;
arg[3][1] = "E";
arg[3][2] = 0;



stat(command, &buf);

if(buf.st_mode == S_COMMAND)
{
//myexe(arg[0]);
//char * input = "input.txt";
//exeRedirOutput(arg[0], input);
//exeRedirInput(arg[1], input);
pipeManager(arg, 3);
}

return 0;
}*/

char * translate(char * s){
 if(s[0] == '$'){
 s = getenv(++s);}
 return s;
}

int myexe(char ** arg){
int status;
pid_t pid = fork();
if(pid == -1){return 1;}
else if(pid == 0){
execv(arg[0], arg);
}
else{
 waitpid(pid, &status,0);	// parent needs to wait
 }

return 0;

}

int exeRedirInput(char ** arg, char * input){
int status;
pid_t pid = fork();
if(pid == -1){return 1;}
else if(pid == 0){
fclose(stdin);

FILE * fd = fopen(input, "r");

dup2(fileno(fd), 0);
execv(arg[0], arg);
}
else{
 waitpid(pid, &status,0);	// parent needs to wait
 }

return 0;
}


int exeRedirOutput(char ** arg, char * output){
int status;
pid_t pid = fork();
if(pid == -1){return 1;}
else if(pid == 0){
fclose(stdout);

FILE * fd = fopen(output, "w");
dup2(fileno(fd), 1);

execv(arg[0], arg);
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

int save_in, save_out; // ******************* 
save_in = dup(STDIN_FILENO); // ******************** this bit comes from stackoverflow
save_out = dup(STDOUT_FILENO); // ****************

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
	myexe(arg[i]);
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
int save_in, save_out; // ******************* 
save_in = dup(STDIN_FILENO); // ******************** this bit comes from stackoverflow
save_out = dup(STDOUT_FILENO); // ****************

int fd[2];
pipe(fd);
fclose(stdout);				// this section does the first command
dup2(fd[1],1);
myexe(arg[0]);

close(STDIN_FILENO);			// close stdin and use the pipe for input from now on
dup2(fd[0],0);

open(fileno(stdout));			// re-open stdout
dup2(save_out, STDOUT_FILENO);		// final command

close(fd[0]);
close(fd[1]);
myexe(arg[2]);
}
else{
 waitpid(pid, &status, 0);
 }

}

int argcount(char *** arg){
int count = 0;
int i = 0;
while(arg[i++] != 0){count++;}
return count;
}
