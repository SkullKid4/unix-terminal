#ifndef SYSCALL_LINK_H
#define SYSCALL_LINK_H

int syscall_link(int cmd, int arg1, int arg2, int arg3);

void jump_user_space(int entry_point);

#endif
