#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024
#define PERMISSION_BIT 0644  //-rw-r--r--

/*Handle the copy operation from 
directory to directory
*/
int copyDirectory(char *src, char *dest);

//Copy file to file or into directory
int copyFile(char *src, char *dest);

//check whether given name is directory
bool isdir(char* filename);

//reverse the string
char* strrev(char* str);


/*workflow of main():
	
	(1) validate the input arguments and invoke corresponding subroutines

	(2) Accepted options:
(3) 3 flags for each option: opt_r, opt_l, opt_s


*/


int main(int argc, char **argv){


	//represents -r/-R option for recursively copying files

	bool opt_r = false;

	/*represents -l option: establish hard link, which creates pointer 
	pointing to the content of file (stay valid 
	even the original file change name)*/	
	bool opt_l = false;

	

	/*3. s: establish soft link, which creates pointer pointing 
	to the file name (become NULL if the original file change name)*/	
	bool opt_s = false;

	//source file
	char *src = NULL;

	//destination file
	char *dest = NULL;

	//option
	char opt;

	//obtian the options from command line using getopt
	while((opt = getopt(argc, argv, "rRls")) != -1){
		switch(opt){
			
			case 'R':
			case 'r':
				opt_r = true;
				break;
			case 'l':
				opt_l = true;
				break;
			case 's':
				opt_s = true;
				break;
		}
	}


	/*optind counts number of arguments
	including '-' options and first argument
	so a valid input should have optind + 2 
	number of arguments due to two file name
	*/
	if(optind+2 != argc){
		printf("Invalid number of inputs\n");
		exit(1);;
	}


	//extract source and destination file name from arguments
	src = argv[optind];

	dest = argv[optind+1];

	if(opt_l){
		
		if(is_dir(src)){
			printf("cannot create hard link on directory\n");
			exit(1);
		}

		if(link(src, dest) == 0){
			return 0;
		}else{
			printf("faild to create hard link\n");
			exit(1);
		}
	
	}
	

	if(opt_s){
		if(is_dir(src)){
			
			printf("cannot create soft link on directory\n");
			exit(1);
		
		}

		if(symlink(src, det) == 0){
			return 0;
		}else{
			printf("faild to create symbolic link\n");
			exit(1);
		}
	}
	

	if(!isdir(src)){
		
		if(copyFile(src, det) == 0){
			return 0;
		}else{
			printf("Fails to copy\n");
			exit(1);
		}
	}else{
		if(!isdir(dest)){
			printf("Cannot copy directory to a file\n");
			exit(1);
		}

		if(opt_r){
			if(copyDirectory(src, dest) != 0){
				printf("Fails to copy directory\n");
				exit(1);
			}else{
				return 0;
			}
		}else{
			printf("-r option needed\n");
			exit(1);
		}
	}
	return 0;	
}
