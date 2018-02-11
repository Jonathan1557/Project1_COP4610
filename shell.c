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

//char ** resolve_paths(char**);
char * resolve_paths(char * arg);
char * expand_path(char *path, int cmd_p);
int is_command(char** args, int i);
int is_builtin_command(char* arg);
int is_external_command(char* arg);
char * expand_previous(char* arg);
int fileExists(char* file);
int isFile(const char* path);
int isDir(const char* path);
char * getPWD(void);
char * getHOME(void);
char * getPATH(void);

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

// PATH RESOLTION:
// MODES (inputs for expansion)
// 1. get cmd on its own
// 2. get partial path of cmd
// 3. get an arg with no path
// 4. get arg with partial path (if not a DIR or FILE, then path is incorrect)


// use stat(cmd, &buf) to determine if file?, cmd, dir, etc
// if(buf.st_mode == S_COMMAND)
// if not a DIR or FILE, then path is incorrect
/*
 char ** resolve_paths(char ** args)
 {
 // expand each arg to its absolute path
 int i=0;
 while (args[i]!=NULL) {
 expand_path(args[i], is_command(args, i));
 i++;
 }
 return args;
 }
 */
char * resolve_paths(char * arg)
{
	// expand each arg to its absolute path
	int i=0;
	while (arg!=NULL) {
		expand_path(arg, 2);
		i++;
	}
	return arg;
}

char * expand_path(char *path, int cmd_p){
	//returns expanded argument, does nothing in many cases (determined by is_command)
	
	
	// Step 1: if environment variable, return value (path)
	if (path[0] == '$') {
		return translate(path);
	}
	
	// Step 2: if command, find and return path if it exists
	else if (cmd_p!=0) {
		if (cmd_p == 1) {	// if built-in command:
			// do not expand command, do not expand arguments
			// only expand 'cd'
			
			return path;
		}
		else if (cmd_p == 2) {	// if external command:
			// expand command, not arguments
			char * path_str = getPATH();	// get list of paths
			
			char single_path[255];	// an individual path from $PATH
			char test_path[255];	// a single_path/command
			int single_path_index = 0;		// index currently being filled
			
			// parse full $PATH into it's separate paths
			int i=0;
			while (i<strlen(path_str)) {
				single_path[single_path_index] = path_str[i];
				if (path_str[i]==':') {				// if single path found
					single_path[single_path_index] = 0;	// set last char to NULL
					
					// create test_path
					strcpy(test_path, single_path);	// test_path = single_path
					strcat(test_path, "/");			// test_path + '/'
					strcat(test_path, path);		// test_path + cmd
					
					// check if path contains the command
					if (is_external_command(test_path)) {
						return test_path;	// if so, return this path
					}
					
					single_path_index=0;	// reset index of single_path_char
				}
				i++;
			}
		}
	}
	
	// Step 3: if argument, get path if it exists
	else {
		// check for arg in current directory
		
	}
	
} // end of expand_path()

int is_command(char** args, int i) {
	// true if greater than ZERO.
	// 0=false, 1=true(built-in), 2=true(external)
	
	char * arg = args[i];	// get the arg
	if (is_builtin_command(arg)>0) {
		return 1;
	}
	else if (is_external_command(arg)){
		return 2;
	}
	return 0;
}

int is_external_command(char* arg) {
	// determine if external command
	// true = 1, false = 0
	struct stat buf;
	stat(arg, &buf);
	
	if(buf.st_mode == S_COMMAND)
	{
		return 1;
	}
	else return 0;
}

int is_builtin_command(char * arg){
	// 1) compare input string token to a list of known commands
	// 2) return command code associated with that command
	// COMMAND CODES: 0=argument / 1=external_cmd / 2=cd / >2=built_in_cmds
	
	if(strcmp(arg, "cd")==0)
		return 1;
	else if(strcmp(arg, "echo")==0)
		return 2;
	else if(strcmp(arg, "etime")==0)
		return 3;
	else if(strcmp(arg, "io")==0)
		return 4;
	else if(strcmp(arg, "ls")==0)
		return 5;
	else if(strcmp(arg, "exit")==0)
		return 6;
	
	else return 0;		// is an argument
}

char * expand_previous(char* arg){
	// Remove trailing directory from passed in path
	
	int index = 0;		// index being checked for null
	int slashIndex = 0;	// index of last slash
	int nullFound = 0;	// bool indicating a null found
	int nullIndex = 0;	// index of null pointer in path string
	
	while (nullFound==0) {	// while null not found
		if (arg[index]=='/') {		// if slash found
			slashIndex = index;			// set it's index
		}
		else if (arg[index]==0) {	// if null found
			nullFound=1;				// set found
			nullIndex = index;			// set it's index
		}
		else {				// else try next index
			index++;
		}
	}
	
	// remove last directry
	int i=slashIndex;
	while (i<nullIndex) {	// for chars between slash and end...
		arg[i] = 0;		// set character to NULL
		i++;
	}
	
	return arg;
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

int fileExists(char* file) {
	if( access( file, F_OK ) != -1 ) {
		return 1;
	} else {
		return 0;
	}
}

// from http://forum.codecall.net/topic/68935-how-to-test-if-file-or-directory/
int isFile(const char* path) {
	struct stat buf;
	stat(path, &buf);
	return S_ISREG(buf.st_mode);
}

// from http://forum.codecall.net/topic/68935-how-to-test-if-file-or-directory/
int isDir(const char* path) {
	struct stat buf;
	stat(path, &buf);
	return S_ISDIR(buf.st_mode);
}

char * getHOME() {
	return translate("$HOME");
}

char * getPATH() {
	return translate("$PATH");
}

char * getPWD() {
	return translate("$PWD");
}

