#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main()
{

	pid_t pid = getpid();		//Получение PID процесса
	printf("pid: %d\n", pid);
	pid_t ppid = getppid();		//Получение PPID процесса
	printf("ppid: %d\n", ppid);
	uid_t uid = getuid();		//Получение фактического индификатора пользоваетля процессом
	printf("uid: %d\n", uid);
	uid_t euid = geteuid();		//Получение эффективного индификатора пользователя процессом
	printf("euid: %d\n", euid);
	gid_t gid = getgid();
	printf("gid: %d\n", gid);
	pid_t sid = getsid(pid);
	printf("sid: %d\n", sid);
	
	int suid = setuid(uid);
	printf("suid: %d\n", suid);
	int seuid = seteuid(uid);
	printf("seuid: %d\n", seuid);
	int sgid = setgid(gid);
	printf("sgid: %d\n", sgid);
	pid_t ssid = setsid();
	printf("ssid: %d\n", ssid);
	return 0;
}
