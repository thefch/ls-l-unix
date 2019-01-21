#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <grp.h>
#include <string.h>
#define __USE_XOPEN
#include <time.h>
#include <errno.h>
#include <pwd.h>
#include <utime.h>
#include <fcntl.h>

//formating the date to Month Day hh:mm 
//eg. Dec 10 20:08 
char* formatdate(char* str, time_t val)
{
	strftime(str, 100, "%b  %2d %H:%M", localtime(&val));
	return str;
}

//converting from a time_t value to string in the format YYYY-MM-DD hh:mm:ss
//eg 2018-12-10 20:08:40
char* datefind(char* str, time_t val)
{
	strftime(str, 100, "%Y-%m-%d %H:%M:%S", localtime(&val));
	return str;
}
// turns a char* into a time_t 
// it is given a char* reprisenting the time e.g. the char* is "2018-12-10 20:08:40" 
time_t dateSet(char *str)
{
	struct tm tm;
	strptime(str, "%Y-%m-%d %H:%M:%S", &tm);
	time_t time = mktime(&tm);
	return time;
}

/* adds a file/directory to the archive file given its path, *
 * time of modification, name of file being archived		 *
 * the archive file name and its permissions                 */
void addArch(char* name, int modeOfFile, char time[], char path[],long linecount, const char *backup)
{
	char pathtemp[100];
	FILE *fp;
	FILE *file;
	char c;
	char counterString[50];
	char buffer[4096];
	int count;
	printf("%s \n", backup);
	printf("%s \n", path);

	fp = fopen(backup, "a+");
	//printf(" countTotal  = %d\n", countTotal);
	printf("path: %s \n", path);
	printf("date: %s \n", time);
	printf("name: %s \n", name);

	// adds the files path to the archive
	fputs(path, fp);
	fputs("\n", fp);

	// adds the permissions to the archive
	fputs(((S_ISDIR(modeOfFile)) ? "d" : "-"), fp);
	fputs(((modeOfFile & S_IRUSR) ? "r" : "-"), fp);
	fputs(((modeOfFile & S_IWUSR) ? "w" : "-"), fp);
	fputs(((modeOfFile & S_IXUSR) ? "x" : "-"), fp);
	fputs(((modeOfFile & S_IRGRP) ? "r" : "-"), fp);
	fputs(((modeOfFile & S_IWGRP) ? "w" : "-"), fp);
	fputs(((modeOfFile & S_IXGRP) ? "x" : "-"), fp);
	fputs(((modeOfFile & S_IROTH) ? "r" : "-"), fp);
	fputs(((modeOfFile & S_IWOTH) ? "w" : "-"), fp);
	fputs(((modeOfFile & S_IXOTH) ? "x" : "-"), fp);
	fputs("\n", fp);

	// adds the files name to the archive
	fputs(name, fp);
	fputs("\n", fp);

	// adds the files modification date to the archive
	fputs(time, fp);
	fputs("\n", fp);

	// adds the character count to the file
	snprintf(counterString, sizeof(counterString), "%ld", linecount);
	fputs(counterString, fp);
	fputs("\n", fp);
	

	////if a file gets the text from it and copys it to the file
	if (!(S_ISDIR(modeOfFile))) {
		file = fopen(path, "r");
		while ((count = fread(buffer,1,sizeof(buffer),file)) > 0)
		{
			printf("count    = %d\n", count);
			int dinosaur = fwrite( buffer,1,count,fp);
			//printf("dinosaur = %d\n", dinosaur);
		}
		fclose(file);
	}
	fputs("\n", fp);
	fclose(fp);
	//fputs("\n", fp);
	//fputs("\n", fp);
	//


	
	printf("finish \n");
}

//list the files ,sub-directories and content of sub-directories after a given date which is argument backup
void listdir(const char *name, const char *backup, const char *file)
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
	int linecount = 0;

	
	//this an error check if the specified current directory is correct
	if (!(dir = opendir(name)))
		return;
	
	
	//loops through the current directory and print the file permission of the 
	//files and directories with the last date and time modified
	while ((entry = readdir(dir)) != NULL) {
		
		snprintf(path, sizeof(path), "%s/%s",name,entry->d_name);
		int a = stat(path, info);
		int modeOfFile = info->st_mode;
		pwd = getpwuid(info->st_uid);
		groupInfo = getgrgid(info->st_gid);

		//it ignores the notations "." and ".."
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
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

			//this runs the formt date function
			strcpy(timeBUFF, formatdate(tmpBUFF, info->st_mtime));	

			//prints the number of hard links of the specified file/directory
			printf("  %ld", info->st_nlink);	

			//prints the username and the group name
			printf(" %s %s", pwd->pw_name, groupInfo->gr_name);

			//prints the size of file/directory, 
			//the date of last modification 
			//and the name of the file/directory 
			printf(" %6d %s %s\n", info->st_size, timeBUFF, entry->d_name);	
		
			addArch(entry->d_name, modeOfFile, timebackup, path,info->st_size,file);
		}
		
		//if a directory is found it turns the found directory into the current directory and feed it into this function 
		if (entry->d_type == DT_DIR)
		{
			snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
			listdir(path, backup,file);
		}
	}
	closedir(dir);
}

// if given a filename as an argument this function finds the file and gives the listdir function 
// a char* with the earliest time the files need to be backup 
void findfile(const char *name, const char *backup,const char *directory, const char *file)
{
	printf("%s \n", name);	//this is for testing
	printf("%s \n", backup);	//this is for testing
	
	DIR *dir;
	struct dirent *entry;
	struct stat *info = malloc(sizeof(struct stat));

	char path[1024];
	char timebackup[100];
	char tmpBUFF[100];
			
	//this an error check if the specified current directory is correct
	if (!(dir = opendir(name)))
	{
		printf("directory does not exist %s", name);
		return;
	}
	while ((entry = readdir(dir)) != NULL) {
		printf("penguin \n");
		snprintf(path, sizeof(path), "%s/%s",name,entry->d_name);
		//printf("%s \n", path);
		int a = stat(path, info);
	
		//it ignores the notations "." and ".."
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		//if entry name and backup(filename) are the same
		//turn the time_t into the correct format using datefind 
		//then pass it into the listdir
		printf("%s \n", entry->d_name);
		printf("%s \n", backup);
		if(strcmp(entry->d_name, backup) == 0)
		{
			printf("works so far \n");	//this is for testing
			//printf("%s \n", entry->d_name);	//this is for testing
			strcpy(timebackup, datefind(tmpBUFF, info->st_mtime));
			listdir(directory,timebackup,file);
		}
		
		//if a directory is found it turns the found directory into the current directory 
		//and feed it into this function 
		if (entry->d_type == DT_DIR)
		{
			snprintf(path, sizeof(path), "%s/%s", name,entry->d_name);
			findfile(path, backup, directory,file);
		}
	}
}

void copyContent(char const *linecount, FILE *file,FILE *arc)
{
	int nBytes = 0;
	int count;
	int countTotal;
	char hold[1000];
	nBytes = atoi(linecount);
	int i = 0;
	char c;
	char buffer[4096];

	/*while (i < (line - 1))
	{
		c = fgetc(arc);
		if (c != 'ÿ')
		{
			fputc(c, file);
			putchar(c);
		}
		i++;
	}*/
	count = 0;
	printf("Bytes = %d \n", nBytes);
	int buffer_SIZE = sizeof(buffer);
	for (int remaining = nBytes; remaining > 0; remaining -= buffer_SIZE)
	{
		if (remaining > buffer_SIZE)
		{
			count = fread(buffer, 1, buffer_SIZE, arc);
		}
		else
		{
			count = fread(buffer, 1, remaining, arc);
		}
		printf("count    = %d\n", count);
		int dinosaur = fwrite(buffer, 1, count, file);
	}

	//count = fread(buffer, 1, line , arc);
	//countTotal = countTotal + count;
	//printf("count    = %d\n", count);
	//int dinosaur = fwrite(buffer, 1, count, file);
	////printf("dinosaur = %d\n", dinosaur);

	printf("finished \n");
}

void restoreFiles(char *archieveFile)
{
	FILE *arc;
	char orginalPath[300];
	char path[300];
	char permissions[300];
	char fileName[300];
	char date[300];
	char bytecount[300];
	char line[1000];
	char *nOPath;
	char *nPath;
	char divide[2] = "/";
	int countPath = 0;
	int count = 0;
	char newPath[300];
	char pathtemp[100];
	char finalPath[300];
	char perhold[10];
	int a = 0;
	time_t time;
	FILE *file;

	struct utimbuf t;

	arc = fopen(archieveFile, "r");

	if (arc == NULL)
	{
		printf("archive file does not exist \n");
		return;
	}
	

	fgets(line, 1000, arc);
	sscanf(line, "%[^\r\n]", orginalPath);

	printf("%s \n", orginalPath);

	nOPath = strtok(orginalPath, divide);

	while (nOPath != NULL) {
		//printf(" %s\n", nOPath);
		countPath++;
		nOPath = strtok(NULL, divide);
	}
	printf("%d \n", countPath);

	
	//for(int i = 0; i <= 1;i++)
	while (!feof(arc))
	{
		/*pathtemp[0] = '\0';
		path[0] = '\0';
		permissions[0] = '\0';
		fileName[0] = '\0';
		date[0] = '\0';
		linecount[0] = '\0';*/

		getcwd(pathtemp, sizeof(pathtemp));
		printf("hi \n");
		fgets(line, 1000, arc);
		sscanf(line, "%[^\r\n]", path);
		printf("path = %s \n", path);
		fgets(line, 1000, arc);
		sscanf(line, "%[^\r\n]", permissions);
		printf(" permissions = %s \n", permissions);

		fgets(line, 1000, arc);
		sscanf(line, "%[^\r\n]", fileName);
		printf("filename = %s \n", fileName);

		fgets(line, 1000, arc);
		sscanf(line, "%[^\r\n]", date);
		printf("date = %s \n", date);

		fgets(line, 1000, arc);
		sscanf(line, "%[^\r\n]", bytecount);
		printf("bytecount = %s \n", bytecount);

		nPath = strtok(path, divide);
		//printf("the nPath \n");
		while (nPath != NULL)
		{
			//printf(" %s\n", nPath);
			count++;
			if (count >= (countPath + 1))
			{
				//printf("keep %s\n", nPath);
				//printf("%d, %d \n", count, countPath);

				strcat(pathtemp,"/");
				strcat(pathtemp, nPath);
				//snprintf(newPath, sizeof(newPath), "%s%s", newPath, "/");
				//strcat(newPath, nPath);
				printf("newPath = %s \n", pathtemp);
			}
			nPath = strtok(NULL, divide);
		}
		count = 0;

		//printf("finalPath = %s \n", pathtemp);

		//printf("%s \n", permissions);
		char *per = permissions;
		while (*per != '\0')
		{
			//printf("%c \n", *per);
			perhold[a] = *per;
			//printf("%c \n", perhold[a]);
			per++;
			a++;
		}
		a = 0;

		printf("%c \n", perhold[a]);
		mode_t mode;

		if (perhold[1] == 'r')
		{
			//snprintf(mode, sizeof(mode), "%s|", "S_IRUSR");
			mode = S_IRUSR;
		}
		if (perhold[2] == 'w')
		{
			printf("here \n");
			mode = mode + S_IWUSR;
		}
		if (perhold[3] == 'x')
		{
			mode = mode + S_IXUSR;
		}
		if (perhold[4] == 'r')
		{
			mode = mode + S_IRGRP;
		}
		if (perhold[5] == 'w')
		{
			mode = mode + S_IWGRP;
		}
		if (perhold[6] == 'x')
		{
			mode = mode + S_IXGRP;
		}
		if (perhold[7] == 'r')
		{
			mode = mode + S_IROTH;
		}
		if (perhold[8] == 'w')
		{
			mode = mode + S_IWOTH;
		}
		if (perhold[9] == 'x')
		{
			mode = mode + S_IXOTH;
		}
		if (perhold[0] == 'd')
		{
			printf("is a directory \n");
			printf("%o \n", mode);
			mkdir(pathtemp, mode);
			fgets(line, 1000, arc);
		}
		else if (perhold[0] == '-')
		{
			printf("is a file \n");
			file = fopen(pathtemp, "w");
			chmod(pathtemp, mode);
			copyContent(bytecount,file,arc);
			//printf("finished \n");
			fgets(line, 1000, arc);
		
		}
		// restores last time of modification
		time = dateSet(date);
		printf("time = %ld \n", time);
		t.actime = time;
		t.modtime = time;
		utime(pathtemp, &t);
	}
	fclose(file);
	printf("finish");
	fclose(arc);
}

int main(int argc, char *argv[]) {
	//these are the options to run the program
	char *a = "-t";		//-t is to run the program with a date or a file's name
	char *b = "-h";		//-h is to run the help messages
	char *c = "-f";		//-f is to run 
	int d = 1;
	int e = 0, f = 0;
	printf("Running - %s \n", argv[0]);
	printf("%d \n", argc);
	char combine[2000];
	FILE *fp;
	int found = 0;
	int archieveFileName = 0;

	// if file starts with ./backup run the loop  
	if (strcmp(argv[0], "./backup") == 0)
	{

		while (d <= argc - 1)
		{
			//printf("d = %d \n", d); // used for testing
			//printf( "argv = %s \n", argv[d]); // used for testing
			// if argv[d] equalled -t
			if (strcmp(argv[d], a) == 0)
			{

				e = d;
				while (e <= (argc - 2))
				{
					// if -h is found this does not matter a it will be run later
					if (strcmp(argv[e], b) == 0)
					{
						e++;
						continue;
					}
					// if argv[e] equalled -f
					else if (strcmp(argv[e], c) == 0)
					{
						
						//printf("%s \n", argv[e + 1]); //used for testing
						fp = fopen(argv[e + 1], "a+");
						fputs(argv[(argc - 1)], fp);
						fputs("\n", fp);
						fclose(fp);

						archieveFileName = (e + 1);
						found = 1;
						e++;
						continue;
					}
					e++;
					f++;
					printf("%d \n", e);
				}

				// found it used to work out what f will be as 
				// f will need to have an amount subtracted off it
				// if -f is after -t then it will need to skip the arguments the archive 
				// file name and the path    
				if (found == 1)
				{
					f = f - 2;
				}
				else if (found == 0)
				{
					printf("archive file has not been named \n \
						please use -h to find out more \n");
					return 0;
				}
				// if-f is before -t then it only need to skip the path
				else if(found == 2)
				{
					f = f - 1;
				}
				printf("f = %d \n", f);

				if (f == 2)
				{
					//printf("%s \n", argv[d + (f - 2)]));	//use for testing
					//printf("%s \n", argv[d + (f - 1)]));	//use for testing

					//this combines the two args which are
					//argv[d + (f - 2)]=YYYY-MM-DD 
					//argv[d + (f - 1)]=hh:mm:ss
					//this is so the listdir function works as intended
					strcpy(combine, argv[d + (f - 2)]);
					strcat(combine, " ");
					strcat(combine, argv[d + (f - 1)]);
					//printf("combine = %s \n", combine);	//use for testing
					listdir(argv[(argc - 1)], combine,argv[archieveFileName]);
				}
				else if (f == 1)
				{
					printf("file = %d %s \n", (f - 1), argv[d + (f )]);
					findfile(argv[(argc - 1)], argv[d + (f)],argv[(argc - 1)],argv[archieveFileName]);
				}
				else if (f < 1)
				{
					printf("not date specified \n");
					listdir(argv[(argc - 1)], "1970-01-01 00:00:00", argv[archieveFileName]);
				}
			}
			else if (strcmp(argv[d], b) == 0)
			{
				//prints help messages
				printf("-t {<filename>,<date>} \n");
				printf("date must be in the form YYYY-MM-DD hh:mm:ss \n");
				printf("-f {<filename>} \n");
				printf("filename is the name that you wish to give your archive file \n");
				printf("./backup -t {<filename>,<date>} -f {<filename>} directory \n");
				printf("switch -t, -f and -h can appear in any order \n");
			}
			// used only if -f is before -t
			else if (strcmp(argv[d], c) == 0)
			{
				
					printf("%s \n", argv[d + 1]);
					fp = fopen(argv[d + 1], "a+");
					fputs(argv[(argc - 1)],fp);
					fputs("\n", fp);
					fclose(fp);
					archieveFileName = (d + 1);
					found = 2;

			}
			d++;
		}
	}
	// if the first argument is ./restore
	// -f and -h are the only switchs used in restore
	else if (strcmp(argv[0], "./restore") == 0)
	{
		while (d <= argc - 1)
		{
			// if argv[d] equals -f
			if (strcmp(argv[d], c) == 0)
			{
				printf("%s \n", argv[(d + 1)]);
				restoreFiles(argv[(d + 1)]);
			}
			else if (strcmp(argv[d], b) == 0)
			{
				//prints help messages
				printf("-f {<filename>} \n");
				printf("filename is the name of the archive file you wish to restore \n");
				printf("./restore -f {<filename>} \n");
				printf("if archive file is in another directory  \
					please use its path in the filename \n");
			}
			d++;
		}
	}
	return 0;
}
