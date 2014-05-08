//rm.cc
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
using namespace std;
const int MAX_STRING_SIZE=2048;

//Helper method that recursively deletes all files and folders in a given folder.
void recDelete(char * folderName){
	if(chdir(folderName)==0){
		char * dir_name = ".";
		DIR * dirp;
		if(!(dirp=opendir(dir_name))){
			cout << "Error(" << errno << ")opening " << dir_name << endl;
			return;
		}
		dirent * direntp;
		while((direntp=readdir(dirp))){
			if(unlink(direntp->d_name)!=0) if(errno==EPERM){
				recDelete(direntp->d_name);
				rmdir(direntp->d_name);
			}
		}
		closedir(dirp);
		chdir("..");
	}
}

int main(int argc, char ** argv){
	//Check for error case / no arguments
	if(argc <=1) cerr << "Too few arguments for 'rm' .   Try  './rm FILENAME' ." << endl;
	//check for the -r command
	bool recursiveDelete = false;
	int recPlace=0;
	while(++recPlace<argc){
		if(strncmp(argv[recPlace],"-r", MAX_STRING_SIZE)==0){
			recursiveDelete=true;
			break;
		}
	}
	//Regular delete - using rmdir() for folders and unlink() for files
	//If a file is a folder it will only be deleted if the -r command is provided
	for(int i=1; i<argc; i++){
		//Add a check so it does not try to unlink("-r")
		if(i!=recPlace && unlink(argv[i])!=0){
			if(errno==ENOENT) cerr << "Unable to rm the file: " << argv[i] << " - file does not exist." << endl;
			else if(errno==EPERM){
				if(!recursiveDelete) cerr << "Unable to rm the file: " << argv[i] << " - this is a directory." << endl;
				else{
					if(rmdir(argv[i])!=0){
						//Check if a folder isn't empty then delete it and all of its contents recursively
						if(errno==ENOTEMPTY){
							//cerr << "Unable to rm the this directory because it's filled: " << argv[i] << " - must delete its files first." << endl;
							recDelete(argv[i]);
							rmdir(argv[i]);
						}
					}
				}
			}
		}
	}
	return 0;
}