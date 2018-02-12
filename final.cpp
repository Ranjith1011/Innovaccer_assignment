/*
			Infrastructure Engineering Assignment--E.Ranjith Kumar(150001011)
-->Program that gives information of top ten-size files existing in the specified directory(mentioned)/whole system.
-->Program that cleans-up the souce directory mentioned and transfer those files to the destination directory,
   storing them in the folders according to their extensions.
*/


#include <unistd.h>							//Including all the header files required
#include <sys/types.h>
#include <dirent.h>
#include<bits/stdc++.h>
#include <fcntl.h>
#include<sys/stat.h>

using namespace std;

int opt,ch;
char sou[1024];
char des[1024];

struct node{
      char p[1024];							//Linked list each node structure to maintain the information of files
      char n[256];							// i.e., path,name and size
      off_t s;
      struct node *next;
};

struct node *head=NULL;
int  node_size=0;


off_t file_size(const char* path){			//Function to get the size of the file
	struct stat filestatus;
 	stat(path,&filestatus);
  	return filestatus.st_size;
}
		                      //The below three functions help in maintaining the top ten files info. in the linked-list 
struct node* createNode(const char *route,char *name,off_t size)
{
     struct node*temp=(struct node*)malloc(sizeof(node));
     snprintf(temp->p, sizeof(temp->p), "%s", route);                                          
     snprintf(temp->n, sizeof(temp->n), "%s", name); 
     temp->s=size;
     return temp;
}


struct node* enter(const char *route,char *name,off_t size){
	struct node *pre=head;
	struct node *temp=head->next;
	if((head->s)>=size){
		temp=createNode(route,name,size);	
		temp->next=head;
		return temp;
	}	
        while(temp!=NULL && temp->s<size){
		pre=temp;
		temp=temp->next;
	}
        struct node* temp1=createNode(route,name,size);
        pre->next=temp1;
        temp1->next=temp;
        return head;
}

struct node* insert(const char *route,char *name,off_t size)
{
	if((node_size)==0)
  	{
		(node_size)++;
		return createNode(route,name,size);
  	}
  	else if((node_size)<10){
 		(node_size)++;
		return enter(route,name,size);
  	}
	if((head->s)<size){
		struct node *temp=head->next;
                free(head);
                head=temp;
		return enter(route,name,size);
	}
	return head;
}

					
void copy_cut_file(char* sou,char* des){              //Function to perform copy files from source to destination 
	int n;											  //And then to remove the file from source.
	char buff[1024];
	int s_fd=open(sou,O_RDONLY,S_IRWXU);
    	int d_fd=open(des,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
	
    	while ((n=read(s_fd,buff,1024))>0)
    	{
        	write(d_fd,buff,n);
    	}
	close(s_fd);
	remove(sou);
	close(d_fd);
}


void move_file(char *path){							//Function to move files according to their extensions from source to destination.
	char d[1024];
	char f[1024];
	string s;
	s=path;
	string ext=s.substr(s.find_last_of('.')+1);
	char c[ext.size()+1];
	strcpy(c, ext.c_str());
	c[ext.size()]='\0';
	snprintf(d, sizeof(d),"%s/%s",des,c);
	string n=s.substr(s.find_last_of('/')+1);
	char b[n.size()+1];
	strcpy(b, n.c_str());
	b[n.size()]='\0';
	snprintf(f, sizeof(f),"%s/%s/%s",des,c,b);
	if(ext!="desktop"){
		mkdir(d, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		copy_cut_file(path,f);
	}
}


void explore_dir(char *name)							//Function to explore directory and sub-directories existing in the entered path
{														//And to get info. of files in the directories.
    	DIR *dir;
    	struct dirent *dir_info;
    	if (!(dir = opendir(name))){
			cout << "ERROR::NO SUCH PATH EXISTS!!!" << endl;
        	return ;
		}
    
    	while ((dir_info = readdir(dir)) != NULL) {
		char path[1024];
        	if (dir_info->d_type == DT_DIR) {
            		if (strcmp(dir_info->d_name, ".") == 0 || strcmp(dir_info->d_name, "..") == 0)
                		continue;
            		snprintf(path, sizeof(path), "%s/%s", name, dir_info->d_name);
            		explore_dir(path);
        	} 
		else {
			snprintf(path, sizeof(path), "%s/%s", name, dir_info->d_name);
			if(opt==1){
	    			head=insert(name,dir_info->d_name,file_size(path));
			}
			else if(opt==2){
	    			move_file(path);
			}
        	}
        }
    	closedir(dir);
}


void print_file_info(struct node* info){
	if(info!=NULL){
		print_file_info(info->next);
		cout << "Path:: " << info->p << endl << "Name:: " << info->n << endl << setprecision(8) <<"Size:: " << (double)((info->s)/(1024*1024))/* temp->s*/ << " MB" << endl << endl;
	}
}



int main(void) {
	cout << "1.Get top 10 size files in the certain directory\n2.Cut files from source directory and create folders according to the file type in destination directory\nEnter your option::";
	cin >> opt;
	if(opt==1){ 
		cout << "1.Explore whole system\n2.Explore specific directory\nEnter your option::";
		cin >> ch;
		if(ch==1){
			snprintf(sou, sizeof(sou), "/");
		}	
		else if(ch==2){
			cout << "Enter the path to explore::";
        		cin >>sou;
		}
    	explore_dir(sou);
    	if(head==NULL){
    		cout << "NO FILES EXIST IN THE DIRECTORY" << endl;
    		return 0;
		}
		cout <<"***************************Top Size Files***************************\n\t\t*************************************\t\t"<< endl;
		print_file_info(head);
		cout << "\t\t***********************************************\t\t"<<endl;
	}
	else if(opt==2){
		cout << "Enter the Source directory(path) to clean-up::";
		cin >> sou;
		cout << "Enter the Destination directory(path) to paste-all::";
		cin >> des;
		explore_dir(sou);
		cout << "Files Successfully transferred from " << sou << "to" << des <<" and stored in folders according to their extensions..." << endl; 
	}
    	return 0;
}
