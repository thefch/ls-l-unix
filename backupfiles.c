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
	strftime(str, 100, "%b  %2d %H:%M", localtime(&val));  //localtime(&val)
	return str;
}

//converting from a time_t value to string in the format YYYY-MM-DD hh:mm:ss
//eg 2018-12-10 20:08:40
char* datefind(char* str, time_t val)
{
	strftime(str, 100, "%Y-%m-%d %H:%M:%S", localtime(&val));
	return str;
}

//list the files ,sub-directories and content of sub-directories after
//a given date which is argument backup
void listdir(const char *name, const char *backup)
{
	DIR *dir;
	struct dirent *entry;
	struct stat *info = malloc(sizeof(struct stat));
	struct passwd *pwd = malloc(sizeof(struct passwd));
	struct group *groupInfo = malloc(sizeof(struct group));

	char path[1024];
	char timeBUFF[100];
	char timebackup[100];
	char tmpBUFF[100];
	char temp[1024];

	
	//this an error check if the specified current directory is correct
	if (!(dir = opendir(name))) {
		printf("Error opening the file :%s", name);
		return;
	}
	
	//loops through the current directory and print the file permission of  
	//the files and directories with the last date and time modified
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
		
		strcpy(timebackup, datefind(tmpBUFF, info->st_mtime));
		//printf("%s \n", timebackup);		//use for testing
		//printf("%d \n",strcmp(backup, timebackup));  //use for testing


		//if date is the same or greater than the given one then print it
		if (strcmp(backup, timebackup) <= 0) {
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
		}
		
		//if a directory is found it turns the found directory
		//into the current directory and feed it into this function
		if (entry->d_type == DT_DIR)
		{
			snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
			listdir(path, backup);
		}
	}
	closedir(dir);
}

void findfile(const char *name, const char *backup, const char *directory)
{
	//printf("%s \n", name);	//this is for testing
	//printf("%s \n", backup);	//this is for testing
	
	DIR *dir;
	struct dirent *entry;
	struct stat *info = malloc(sizeof(struct stat));

	char path[1024];
	char timebackup[100];
	char tmpBUFF[100];
			
	//this an error check if the specified current directory is correct
	if (!(dir = opendir(name)))
		return;
	
	while ((entry = readdir(dir)) != NULL) {

		snprintf(path, sizeof(path), "%s/%s",name,entry->d_name);
		//printf("%s \n", path);
		int a = stat(path, info);
	
		//it ignores the notations "." and ".."
		if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0){
			continue;
		}
		/*if entry name and backup(filename) are the same
		 *turn the time_t into the correct format using datefind 
		 *then pass it into the listdir*/
		 //printf("%s \n", entry->d_name);
		 //printf("%s \n", backup);
		if(strcmp(entry->d_name, backup) == 0)
		{
			//printf("works so far \n");	//this is for testing
			//printf("%s \n", entry->d_name);	//this is for testing
			strcpy(timebackup, datefind(tmpBUFF, info->st_mtime));
			listdir(directory, timebackup);
		}
		
		//if a directory is found it turns the found directory
		//into the current directory and feed it into this function
		if (entry->d_type == DT_DIR)
		{
			snprintf(path, sizeof(path), "%s/%s", name,entry->d_name);
			findfile(path, backup, directory);
		}
	}
}

int main(int argc, char *argv[]) {
	//these are the options to run the program
	char *a = "-t";	   //-t is to run the program with a date or a file's name
	char *b = "-h";	   //-h is to run the help messages
	int d = 1;
	int e = 0, f = 0;
	printf("Running - %s \n", argv[0]);
	printf("%d \n", argc);
	char combine[2000];

	while (d <= argc - 1)
	{
		if(strcmp(argv[d],a) == 0)
		{
			
			e = d;
			while (e <= (argc - 2))
			{
				if (strcmp(argv[e], b) == 0)
				{
					e++;
					continue;
				}
				e++;
				f++;
			}

			f = f - 1;

			if (f == 2)
			{
    			//this combines the two args which are
				//argv[2]=YYYY-MM-DD 
				//argv[3]=hh:mm:ss
				//this is so the listdir function works as intended
				strcpy(combine, argv[d+(f-2)]);
				strcat(combine, " ");
				strcat(combine, argv[d+(f-1)]);

				listdir(argv[(argc - 1)], combine);
			}
			else if (f == 1)
			{
				findfile(argv[(argc-1)], argv[d+f], argv[(argc-1)]);
			}
			else if (f < 1)
			{
				printf("not date specified");
				listdir(argv[(argc-1)], "1970-01-01 00:00:00");
			}
		}
		else if(strcmp(argv[d], b) == 0)
		{
			//prints help messages
			printf("-t {<filename>,<date>} \n");
			printf("date must be in the form YYYY-MM-DD hh:mm:ss \n");
			printf("./backupfiles -t {<filename>,<date>} path \n");
		}
		d++;
	}
	return 0;
}