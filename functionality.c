
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define S_COMMAND 33261
#define S_DIR 16749
#define S_FAILURE 194

int pipeBack(char ** arg, int * fd);
int pipeMiddle(char ** arg, int * fd);
int pipeFront(char ** arg, int * fd);
void pipeSolo(char ** arg1, char ** arg2, int * fd);
char * translate(char * s);
int myexe(char ** arg);
int exeRedirOutput(char ** arg, char * output);
int exeRedirInput(char ** arg, char * input);


int main()
{

struct stat buf;


int fd[2];
pipe(fd);

char * word = "$HOME";

word = translate(word);

char * command = "/bin/ls";
char * arg[2][3];
arg[0][0] = command;
arg[0][1] = word;
//arg[0][1] = 0; 
arg[0][2] = 0;

command = "/bin/head";
arg[1][0] = command;
arg[1][1] = 0;
arg[1][2] = 0;



stat(command, &buf);

if(buf.st_mode == S_COMMAND)
{
//myexe(arg[0]);
//char * input = "input.txt";
//exeRedirInput(arg[0], input);
//exeRedirOutput(arg[0], input);
pipeSolo(arg[0], arg[1], fd);
}


return 0;
}

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

//int fd = (int)open(input, "-r");
dup2(fileno(fd), 0);
//dup(fileno(fd));
//close(fileno(fd));
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


//int fd = (int)open(output, "-w");
//dup2(fd, 1);

FILE * fd = fopen(output, "w");
dup2(fileno(fd), 1);

execv(arg[0], arg);
}
else{
 waitpid(pid, &status,0);	// parent needs to wait
 }

return 0;
}

void pipeSolo(char ** arg1, char ** arg2, int * fd){
pipeFront(arg1, fd);
pipeBack(arg2, fd);
}

int pipeFront(char ** arg, int * fd){
int status;
pid_t pid = fork();
if(pid == -1){return 1;}
else if(pid == 0)
{
fclose(stdout);
close(fd[0]);
dup2(fd[1],1);
execv(arg[0], arg);
}
else{
 waitpid(pid, &status, 0);
 }
return 0;
}

int pipeBack(char ** arg, int * fd){
int status;
pid_t pid = fork();
if(pid == -1){return 1;}
else if(pid == 0)
{
fclose(stdin);
close(fd[1]);
dup2(fd[0],0);
execv(arg[0], arg);
}
else{
 waitpid(pid, &status, 0);
 }
return 0;
}

int pipeMiddle(char ** arg, int * fd){
int status;
pid_t pid = fork();
if(pid == -1){return 1;}
else if(pid == 0)
{
fclose(stdout);
fclose(stdin);
dup2(fd[1], 1);
dup2(fd[0],0);
execv(arg[0], arg);
}
else{
 waitpid(pid, &status, 0);
 }
return 0;
}

