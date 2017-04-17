#ifndef TERMINAL_H
#define TERMINAL_H

#include "types.h"

/*does nothing (shouldn't be called)*/
extern int32_t terminal_read(void* buf, int32_t nbytes);

/*writes the contents of the buffer to the terminal*/
extern int32_t terminal_write(void* buf, int32_t nbytes);

extern int32_t terminal_open();
extern int32_t terminal_close();


#endif
