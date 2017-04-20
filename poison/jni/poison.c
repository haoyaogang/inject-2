#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include "ptrace_utils.h"
#include "elf_utils.h"
#include "log.h"
#include "tools.h"

struct process_hook {
	pid_t 		pid;
	char 		*dso;
} process_hook = {0, ""};

int main(int argc, char* argv[]) {
	LOGI("argv len:%d",argc);
	if(argc < 2){
	   LOGE("exit:argv len:%d",argc);
	   exit(0);
	 }

	struct pt_regs regs;

	process_hook.dso = strdup(argv[1]);
	process_hook.pid = atoi(argv[2]);

	if (access(process_hook.dso, R_OK|X_OK) < 0) {
		LOGE("[-] so file must chmod rx\n");
		return 1;
	}

	const char* process_name = get_process_name(process_hook.pid);
	ptrace_attach(process_hook.pid, strstr(process_name,"zygote"));
	LOGI("[+] ptrace attach to [%d] %s\n", process_hook.pid, get_process_name(process_hook.pid));

	if (ptrace_getregs(process_hook.pid, &regs) < 0) {
		LOGE("[-] Can't get regs %d\n", errno);
		goto DETACH;
	}

	LOGI("[+] pc: %x, r7: %d", regs.ARM_pc, regs.ARM_r7);

	void* remote_dlsym_addr = get_remote_address(process_hook.pid, (void *)dlsym);
	void* remote_dlopen_addr =  get_remote_address(process_hook.pid, (void *)dlopen);

	LOGI("[+] remote_dlopen address %p\n", remote_dlopen_addr);
	LOGI("[+] remote_dlsym  address %p\n", remote_dlsym_addr);

	if(ptrace_dlopen(process_hook.pid, remote_dlopen_addr, process_hook.dso) == NULL){
		LOGE("[-] Ptrace dlopen fail. %s\n", dlerror());
	}

	if (regs.ARM_pc & 1 ) {
		regs.ARM_pc &= (~1u);
		regs.ARM_cpsr |= CPSR_T_MASK;
	} else {
		regs.ARM_cpsr &= ~CPSR_T_MASK;
	}

	if (ptrace_setregs(process_hook.pid, &regs) == -1) {
		LOGE("[-] Set regs fail. %s\n", strerror(errno));
		goto DETACH;
	}

	LOGI("[+] Inject success!\n");

DETACH:
	ptrace_detach(process_hook.pid);
	LOGI("[+] Inject done!\n");
	return 0;
}
