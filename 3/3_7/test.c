#include<stdlib.h>
 #include<stdio.h>
 #include<string.h>
 #include<unistd.h>
 #include<sys/types.h>
 #include<sys/ipc.h>
 #include<sys/msg.h>
 
 struct my_msg{
	long int msgtype;
	char sometext[BUFSIZ];
};
 
int main(){
	int isRunning=1;
	
	struct my_msg somedata;
	long int message = 0;
	
	int msgid = msgget((key_t)14534,0666|IPC_CREAT);
	
	while(isRunning){
		msgrcv(msgid, (void *)&somedata, BUFSIZ, message, 0);  
		               
		printf("Say: %s\n", somedata.sometext);
		
		if(somedata.msgtype == 255){
			isRunning = 0;
		}
	}
	
	msgctl(msgid,IPC_RMID,0);
  exit(EXIT_SUCCESS);
}
