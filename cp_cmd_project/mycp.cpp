#include <stdio.h>
#include <errno.h>
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




bool isdir(char *filename){
	struct stat file_info;
	
	//write status information of file into stat object
	if(!stat(filename, &file_info)){
		
		if(IS_DIR(file_info.st_mod)){
			return true;
		}
	
	}else{
		printf("Directory error\n");
		exit(1);
	}

	return false;
}



int copyFile(char *src, char *dest){

	int in_fd;
	int out_fd;
	int num_char;

	char buffer[BUFFER_SIZE];
	
	if(isdir(dest)){
		//create a new file of same name in the
		//destination directory

		int count = 0;//count file name including '\0'

		int len = strlen(src);

		while(src[len-count] != '/'){//find last '/'
			count++;
		}

		char new_file_name[count];
	
		strcpy(new_file_name, &src[len-count+1]);

		strcat(dest, new_file_name);
	}
	

	if((in_fd = open(src, O_RDONLY) == -1)){
		return 1;
	}
	
	if((out_fd = open(dest, O_WRONLY | O_CREATE, PERMISSION_BIT))
		== -1)
	{return 1;}
	


	while(num_char = read(in_fd, buffer, BUFFER_SIZE)){
		
		if(num_char < 0){
			printf("fails to read file %s\n", src);
			return 1;
		}

		if(write(out_fd, buffer, num_char) < 0){
			printf("fails to write file %s\n", dest);
			return 1;
		}
	}
	

	if(close(in_fd) == -1 || close(out_fd) == -1){
		printf("fails to close files\n");
		return 1;
	}	
	return 0;
}



int copyDirectory(char *src, char *dest){
	
	/*a pointer to directory stream which 
	 contains the ordered sequenc of 
	all directory entries in a directory
	*/
	DIR* ds = NULL;
	struct dirent *de;
	
	if((ds = opendir(src)) == NULL){
		return 1;
	}
	
	else{
		/*readdir() return the directory
		  entry at the position specified by
		ds, and position the stream pointer to
		the next entry in the stream 
		*/
		errno = 0;
		while(de == readdir(ds)){
			
			if(errno != 0){
				return 1;
			}

		
			if(!isdir(de->d_name)){
				/*copty the non-directory
				  entry to destination using
				 copyFile()
				*/
				
			
			char temp_src[256];
			char temp_dest[256];
			
			strcpy(temp_src, src);
			strcpy(temp_dest, dest);
			strcat(temp_src, de->d_name);
			strcat(temp_dest, de->d_name);
			copyFile(temp_src, temp_dest);

			 
			}
		}

		closedir(ds);
		return 0;	
	}
}
