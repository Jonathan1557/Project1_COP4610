#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool parse(char *, char *, char *);
int isCommand(char *);
void printCommand(int);

int main(){
char line[255];
char command[255];
const char* arg[255];   // an array of "strings"


while(strcmp(line, "exit") != 0)
{
printf("myshell> ");
scanf("%s",line);
    
 while(parse(line, command, arg))
  {
   // execute the current operation
    
  }
}
    
return 0;
}

bool parse(char * line, char * command, char * arg){
// find the first command in line, cut and paste it into command and return true
// return false if there are no more commands in command. If there is an invalid command in command, return false and give an error message
	int commandID;
	//command = strtok(line, " ");
    const char* tempArgs[255]; // temporary arg list, to be copied to main arg list
    int argCount=0;
    
    // tokenize line and store command/arguments
    char* token = strtok(line, " ");
    while (token) {
        printf("token: %s\n", token);
        if (isCommand(token))
            commandID = isCommand(token);
        else {
            tempArgs[argCount] = token;
            argCount++;
        }
        printCommand;
        printf("(%d)\n",commandID);
        token = strtok(NULL, " ");
    }
    
    strcpy(arg, tempArgs);  // copy args to main array
    
    //commandID = isCommand(token);

	//printf("%d\n",commandID);

	if (commandID != -1) { return true; }

return false;
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

