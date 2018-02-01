#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define S_COMMAND 33261
#define S_DIR 16749
#define S_FAILURE 194


char * translate(char * s);
int myexe(char ** arg);


int main()
{

struct stat buf;

char * word = "$HOME";

word = translate(word);

char * command = "/bin/echo";
char * arg[3];
arg[0] = command;
arg[1] = word;
arg[2] = 0;


stat(command, &buf);

if(buf.st_mode == S_COMMAND)
{
myexe(arg);
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
