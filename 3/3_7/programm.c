#include<stdlib.h>
 #include<stdio.h>
 #include<string.h>
 #include<unistd.h>
 #include<sys/types.h>
 #include<sys/ipc.h>
 #include<sys/msg.h>

 #define _MAX_ 512
 
struct my_msg{
	long int type;
	char sometext[_MAX_];
};
 
 int main() {
     int isRunning = 1;
     
     struct my_msg somedata;
     
     char text[256]; 
     FILE* fp;
     
     int msgid = msgget((key_t)14534,0666|IPC_CREAT);
     
     if (msgid == -1) {
            perror("msgget() error");
            exit(EXIT_FAILURE);
     }
     
	if((fp=fopen("data.txt","rb")) == NULL) {
			perror("fopen() error");
			exit(EXIT_FAILURE);
	 }
	 
     while(isRunning){
     
     	while (fgets(text, 256, fp) != NULL){
     	
     		fread(text, sizeof(text), 1, fp);
			
            somedata.type = 1;           
            strcpy(somedata.sometext,text);
            
            if(msgsnd(msgid,(void *)&somedata, _MAX_,0)==-1) {
            	perror("msgsnd() error");
       		}
            
            sleep(2);
     		                       
     	}     	
		fclose(fp);
				
 		strcpy(text, "end of file.");
 		
 		somedata.type = 255;
        strcpy(somedata.sometext,text);
        
        if(msgsnd(msgid,(void *)&somedata, _MAX_,0)==-1) {
        	perror("msgsnd() error");
        } 
        
        isRunning = 0;     

     } 
   exit(EXIT_SUCCESS);
 }
 
