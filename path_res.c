//
//  path_res.c
//  
//
//  Created by Doug Ott II on 2/11/18.
//

#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

//char ** resolve_paths(char**);
char * resolve_path(char * arg, int);
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
char * removeCWDSymbol(char* path);
//int myexe_background(char ** arg);

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
char * resolve_path(char * arg, int i)
{
	// expand each arg to its absolute path
	if (arg!=NULL) {
		return expand_path(arg, i);
	}
	//else return "resolve_path() FAILURE";
	else return NULL;
}

char * expand_path(char *path, int cmd_p){
	//returns expanded argument, does nothing in many cases (determined by is_command)
	
	
	
	// Step 1: if environment variable, return value (path)
	if (path[0] == '$') {
		char envVarOnly[20];
		int envVarComplete=0;
		char pathAfterEnvVar[255];
		//char envVarAbs[255];
		
		int pathAfterEnvVarIndex = 0;
		
		// split path into path before
		int i=0;
		while (i<strlen(path)) {				// for each char in path

			if (path[i]=='/') {		// if '/' seen, envVar is complete
				envVarComplete=1;
				//printf("/ found. envVarComplete=%d\n", envVarComplete);
			}
			if (envVarComplete) {	// if envVar is already done, fill path after
				pathAfterEnvVar[pathAfterEnvVarIndex] = path[i];
				//printf("pathAfterEnvVar[i] = path[i] = %c\n", path[i]);
				//printf("pathAfterEnvVar = %s\n"), pathAfterEnvVar;
				pathAfterEnvVarIndex++;
			}
			else {
				envVarOnly[i] = path[i];		// add char to envVarOnly
				//printf("envVarOnly[i] = path[i] = %c\n", path[i]);
			}
			i++;
		}
		i=0;
		if (envVarComplete) {
			char envVarAbs[255];
			strcpy(envVarAbs, translate(envVarOnly));
			//printf("char* envVarAbs = translate(envVarOnly) = %s\n", envVarAbs);
			//printf("pathAfterEnvVar =  %s\n", pathAfterEnvVar);
			strcat(envVarAbs, pathAfterEnvVar);
			envVarComplete=0;
	
			return envVarAbs;
		}
		
		return translate(path);
		//return "end of env testing\n";
	}
	
	// Step 2: if command, find and return path if it exists
	//else if (cmd_p!=0) {	// if a command
	/*
		if (cmd_p==3) {	// if built-in
			// do not expand command, do not expand arguments
			return path;
		}
		if (cmd_p==2) {	// if cd
			//don't expand
			return path;
		}
	*/
	if (is_builtin_command(path)>0) {
		return path;
	}
	
		//else if (cmd_p == 1) {	// if external command:
			// expand command, not arguments
			char * path_str = getPATH();	// get list of paths
			
			char single_path[255];	// an individual path from $PATH
			char test_path[255];	// a single_path/command
			int single_path_index = 0;		// index currently being filled
			
			// parse full $PATH into it's separate paths
			int i=0;
			while (i<strlen(path_str)) {
				single_path[single_path_index] = path_str[i];	// add current char to single_path
				single_path_index++;
				
				if (path_str[i]==':') {				// if single path found
					single_path[single_path_index-1] = 0;	// set last char to NULL
					
					// create test_path
					strcpy(test_path, single_path);	// test_path = single_path
					strcat(test_path, "/");			// test_path + '/'
					strcat(test_path, path);		// test_path + cmd
					
					printf("test_path: %s\n", test_path);
					
					// check if path contains the command
					if (is_external_command(test_path) && fileExists(test_path)) {
						return path = test_path;	// if so, return this path
					}
					
					single_path_index=0;	// reset index of single_path_char
				}
				i++;
			}
			
			//return path_str;
		//}
	//}
	
	// Step 3: if argument, get path if it exists
	//else {
		char abs_path[255];
		int homeAppended = 0;
		
		// if HOME symbol provided
		if ((path[0]=='~' && path[1]==0) ||
			(path[0]=='~' && path[1]=='/' && path[2]==0)) {
			return getHOME();					// return HOME path
			//strcat(abs_path, getHOME());
			//homeAppended = 1;
		}
		// check for arg in current directory
		if (fileExists(path)) {				// if file exists in directory (can have ".." or ".")

			//strcpy(abs_path, getPWD());	// test_path = single_path
			if (homeAppended=0) {
				//strcat(abs_path, getPWD());
			}
			strcpy(abs_path, getPWD());
			strcat(abs_path, "/");
			strcat(abs_path, path);
			//strcpy(abs_path, removeCWDSymbol(abs_path));	// remove any cwd symbol
			return abs_path;
			//return "This file exists.\n";
		}
		else {
			//return "No such file or directory.\n";
			return 0;
		}
		
		
		/*
		if (isFile(path)) {
			return "This is a file.\n";
		}
		else {
			return "This is NOT a file.\n";
		}
		if (isDir(path)) {
			return "This is a directory.\n";
		}
		else {
			return "This is NOT a directory.\n";
		}
		 */

	//}
	
	//return "expand_path() FAILURE\n";
	return NULL;
	
} // end of expand_path()

int is_command(char** args, int i) {
	// true if greater than ZERO.
	// 0=false, 1=true(built-in), 2=true(external)
	
	char * arg = args[i];	// get the arg
	if (is_builtin_command(arg)>1) {	// if built-in
		return 3;
	}
	else if (is_builtin_command(arg)==1) { // if cd
		return 2;
	}
	else if (is_external_command(arg)){	// if external (CAN IT KNOW?)
		return 1;
	}
	return 0;	// if not command
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
	else if(strcmp(arg, "exit")==0)
		return 5;
	
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

char * removeCWDSymbol(char* path) {
	// removes the CWD symbol (./) from a path
	char pathLeadingCWDSymbol[255];
	char pathTrailingCWDSymbol[255];
	char pathMinusCWDSymbol[255];
	int CWDSymbolFound = 0;
	
	int i=0;
	while (i<strlen(path)) {
		if (CWDSymbolFound==0) {
			pathLeadingCWDSymbol[i] = path[i];	// add to cwd-leading path
		}
		if (path[i-1]!='.' && path[i]=='.' && path[i+1]=='/') {	// if cwd symbol found
			CWDSymbolFound = 1;					// flag as found
			i = i + 2;							// skip past the cwd symbol
		}
		if (CWDSymbolFound==1) {
			pathTrailingCWDSymbol[i] = path[i];	// add to cwd-trailing path
		}
		i++;
	}
	
	strcat(pathMinusCWDSymbol, pathLeadingCWDSymbol);
	strcat(pathMinusCWDSymbol, pathTrailingCWDSymbol);
	return pathMinusCWDSymbol;
}
/*
int myexe_background(char ** arg){
	
	int status;
	pid_t pid = fork();
	if(pid == -1){return 1;}
	else if(pid == 0){
		execv(arg[0], arg);
	}
	else{
		//waitpid(pid, &status,0);	// parent needs to wait
		waitpid(-1, &status, WNOHANG);

	}
	
	return 0;
}
*/
