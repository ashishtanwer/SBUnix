#ifndef _SYSCALL_H
#define _SYSCALL_H
#include <defs.h>
/*
 static void *syscall[20] = {
 &write, &sbrk, &exit, &sleep_syscall, &forking, &waiting, &waiting_with_pid,
 &execute_it, &alloc_heap, &scanning, &open_file_call, &close_file_call,
 &read_file_call, &open_directory_call, &close_directory_call, &print_ps,
 &read_directory_call, &set_current_dir, &get_current_dir, &ps_id
 };*/
void sys_setcurrentdir(struct register_frame *stack);
void sys_readfile(struct register_frame *stack);
void sys_closedirectory(struct register_frame *stack);
void sys_closefile(struct register_frame *stack);
void sys_opendirectory(struct register_frame *stack);
void sys_openfile(struct register_frame *stack);
void sys_scanning(struct register_frame *stack);
void sys_allocheap(struct register_frame *stack);
void sys_execute(struct register_frame *stack);
void sys_waitingwithpid(struct register_frame *stack);
void sys_waiting(struct register_frame *stack);
void sys_forking(struct register_frame *stack);
void sys_sleep(struct register_frame *stack);
void sys_write(struct register_frame *stack);
void sys_brk(struct register_frame *stack);
void sys_exit(struct register_frame *stack);
void sys_getcurrentdir(struct register_frame *stack);
void sys_psid(struct register_frame *stack);
void sys_printps(struct register_frame *stack);
void sys_readdirectorycall(struct register_frame *stack);
#endif
