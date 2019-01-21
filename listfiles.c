#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <grp.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pwd.h>

//formating the date to Month Day hh:mm 
//eg. Dec 10 20:08 
char* formatdate(char* str, time_t val)	
{
	strftime(str, 100, "%b  %2d %H:%M", localtime(&val));
	return str;
}

//list all the files ,sub-directories and content of sub-directories
void listdir(const char *name)
{
	DIR *dir;
	struct dirent *entry;
	struct stat *info = malloc(sizeof(struct stat));
	struct passwd *pwd = malloc(sizeof(struct passwd));
	struct group *groupInfo = malloc(sizeof(struct group));

	char path[1024];
	char timeBUFF[100];
	char tmpBUFF[100];
	char temp[1024];

	//this an error check if the specified current directory is correct
	if (!(dir = opendir(name))) {
		printf("Error opening the file :%s", name);
		return;
	}

	//loops through the current directory and print the file permission of the
	//files and directories with the last date and time modified
	while ((entry = readdir(dir)) != NULL) {

		snprintf(path, sizeof(path), "%s/%s",name,entry->d_name);
		int a = stat(path, info);
		int modeOfFile = info->st_mode;

		pwd = getpwuid(info->st_uid);
		groupInfo = getgrgid(info->st_gid);

		//it ignores the notations "." and ".."
		if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0){
			continue;
		}
		//this prints out the file permissions		
		printf((S_ISDIR(info->st_mode)) ? "d" : "-");
		printf((modeOfFile & S_IRUSR) ? "r" : "-");
		printf((modeOfFile & S_IWUSR) ? "w" : "-");
		printf((modeOfFile & S_IXUSR) ? "x" : "-");
		printf((modeOfFile & S_IRGRP) ? "r" : "-");
		printf((modeOfFile & S_IWGRP) ? "w" : "-");
		printf((modeOfFile & S_IXGRP) ? "x" : "-");
		printf((modeOfFile & S_IROTH) ? "r" : "-");
		printf((modeOfFile & S_IWOTH) ? "w" : "-");
		printf((modeOfFile & S_IXOTH) ? "x" : "-");
		
		//this runs the format date function
		strcpy(timeBUFF, formatdate(tmpBUFF, info->st_mtime));	
		
		//prints the number of hard links of the specified file/directory
		printf("  %ld", info->st_nlink);

		//prints the username and the group name
		printf(" %s %s", pwd->pw_name, groupInfo->gr_name);

		//prints the size of file/directory,the date of last modification 
		//and the name of the file/directory 
		printf(" %6d %s %s\n", info->st_size, timeBUFF, entry->d_name);	 

		//if a directory is found it turns the found directory
		//into the current directory and feed it into this function
		if (entry->d_type == DT_DIR) 
		{
			snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);	
			listdir(path);
		}
	}
	closedir(dir);
}

int main(int argc, char *argv[]) {
	
	listdir(".");
	return 0;
}
