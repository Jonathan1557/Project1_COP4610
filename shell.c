#include <stdio.h>
#include <stdbool.h>

bool parse(char *, char *, char *);
int isCommand(char *);

int main(){
char line[255];
char command[255];
char arg[255];

while(strcmp(line, "exit") != 0)
{
printf("myshell> ");
scanf("%s",line);
 while(parse(line, command, arg))
  {
   // execute the current operation
  }
}
printf("exit\n");
return 0;
}

bool parse(char * line, char * command, char * arg){
// find the first command in line, cut and paste it into command and return true
// return false if there are no more commands in command. If there is an invalid command in command, return false and give an error message

return false;
}

int isCommand(char * token){
    // compare input string token to a list of known commands
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
