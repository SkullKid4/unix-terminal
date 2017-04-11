#include "keyboard.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "syscall.h"
#include "files.h"
#include "rtc.h"
#include "types.h"
#include "terminal.h"

volatile unsigned lock = 0;       //used to lock the thread when writing keyboard output to the screen
volatile unsigned shift = 0;      //2a or 36 on press;
volatile unsigned caps = 0;
volatile unsigned ctrl = 0;


static int keyboard_idx;
static int last_idx;
uint32_t count=0;	//index to count for switching files 
int i;
int copied; 		// number of bytes copied from read data
uint32_t file_size; // current file size
char my_file_name[MAX_FILE_CHAR+1]; // current file name
dentry_t curr_dentry; // current directory entry



/*
this is a map I retrived from https://github.com/arjun024/mkeykernel/blob/master/keyboard_map.h
All it does is converts the raw output from the keyboard into the ascii chars they are associated with
*/
//128
unsigned char keyboard_map[128][2] =
{
    {'\0', '\0'},  {27, 27}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'},  /* 9 */
  {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'}, {' ', ' '},//{'\b', '\b'}, /* Backspace */
  {'\t', '\t'},     /* Tab */
  {'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'}, /* 19 */
  {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'}, {'[', '{'}, {']', '}'}, {'\n', '\n'}, /* Enter key */
    {'\0', '\0'},     /* 29   - Control */
  {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'}, /* 39 */
 {'`', '"'}, {'`', '~'},   {'\0', '\0'},    /* Left shift */
 {'\\', '|'}, {'z', 'Z'}, {'x', 'X'}, {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'},     /* 49 */
  {'m', 'M'}, {',', '<'}, {'.', '>'}, {'/', '?'},   {'\0', '\0'},       /* Right shift */
  {'\0', '\0'},
    {'\0', '\0'}, /* Alt */
  {' ', ' '}, /* Space bar */
    {'\0', '\0'}, /* Caps lock */
    {'\0', '\0'}, /* 59 - F1 key ... > */
    {'\0', '\0'},   {'\0', '\0'},   {'\0', '\0'},   {'\0', '\0'},   {'\0', '\0'},   {'\0', '\0'},   {'\0', '\0'},   {'\0', '\0'},
    {'\0', '\0'}, /* < ... F10 */
    {'\0', '\0'}, /* 69 - Num lock*/
    {'\0', '\0'}, /* Scroll Lock */
    {'\0', '\0'}, /* Home key */
    {'\0', '\0'}, /* Up Arrow */
    {'\0', '\0'}, /* Page Up */
  {'-', '-'},
    {'\0', '\0'}, /* Left Arrow */
    {'\0', '\0'},
    {'\0', '\0'}, /* Right Arrow */
  {'+', '+'},
    {'\0', '\0'}, /* 79 - End key*/
    {'\0', '\0'}, /* Down Arrow */
    {'\0', '\0'}, /* Page Down */
    {'\0', '\0'}, /* Insert Key */
    {'\0', '\0'}, /* Delete Key */
    {'\0', '\0'},   {'\0', '\0'},   {'\0', '\0'},
    {'\0', '\0'}, /* F11 Key */
    {'\0', '\0'}, /* F12 Key */
    {'\0', '\0'}, /* All other keys are undefined */
};

int32_t keyboard_read(void* buf, int32_t nbytes) {
	while(enter == 0){        //wait until enter is pressed
      //wait
    };

    int idx[2];
    int i;
    int j = 0;            //holds the index of the first char to copy to the buffer
    int count = 0;          //hold the number of new lines seen so far
    get_keyboard_idx(idx);      //get the idecies of the keyboard

    for(i = (idx[1]-1); i >= 0; i--){ //start at the right end of the buffer and go until 0
      if(keyboard_buf[i] == '\n'){
        count++;
        if(count == 2){     //if the new line count is 2 then youre done
          j = i+1;
          break;
        }
      }
    }
    for(i = j; keyboard_buf[i] != '\0'; i++){   //copys the keyboard buffer to the given buffer
      if((i-j) == nbytes){
        break;
      }
      ((char *)buf)[i] = keyboard_buf[i];
    }
    enter = 0;          //set the volatile enter to zero
    return (i-j);       //the number of bytes read
}


int32_t keyboard_write(void* buf, int32_t nbytes) {
  int i;
  int idx[2];
  get_keyboard_idx(idx);
    if(idx[0] != idx[1]){           //this
      for(i = idx[0]; i < idx[1]; i++){
       char data = ((char *)buf)[i];
       putc(data);
      }
     return(idx[1] - idx[0]);
    }
	return -1;
}

int32_t keyboard_close() {
  disable_irq(KEYBOARD_IRQ);
  return 0;
}

/*
void keyboard_init()
  Input: none
  Return Value: none
  Function: sets the 0-15, 16-31 bits to point to the keyboard handler we defined
*/
void keyboard_init(){
  memset(keyboard_buf, ' ', MAX_BUF_SIZE);   //fill buffer with blank spaces
  keyboard_idx = 0;
  last_idx = 0;
  enter = 0;
  SET_IDT_ENTRY(idt[KEYBOARD_IDT_IDX], (keyboard_handler)); //init idt entry and irq signal
  enable_irq(KEYBOARD_IRQ);
}

/*
void keyboard_handler()
  Input: none
  Return Value: none 
  Function: When a putton press or release occurs this function is called and the response is
               handled appropriatly
*/
void keyboard_handler(){

  unsigned char status;    //used to check keyboard status
  int keycode;             //holds the raw output of the keyboard
  char ascii;
  if(lock == 0){
    lock = 1;             //lock the thread and blocks intrrupts
    cli();

    send_eoi(KEYBOARD_IRQ);

    status = inb(KEYBOARD_STATUS_PORT);
  
    if (status & 0x01) {                    //if the status is set, get the code from the keyboard port
      keycode = inb(KEYBOARD_DATA_PORT);
      //printf("Keycode is: %d", keycode);
      if(keycode == BACKSPACE && ((screen_x + screen_y) != 0)){
        handle_backspace();
        sti();
        lock = 0;
        return;
      }

      if(keycode < 0 || keycode > MAX_PRESS_CODE){    //if this is a button release code, unlock and turn on interrupts
        if(keycode == SHIFT_UP_L || keycode == SHIFT_UP_R){
          shift = 0;
        } else if(keycode == CTRL_UP){
          ctrl = 0;
        }
        sti();
        lock = 0;
        return;
      }

      /* checks for shift, caps, ctrl */
      if(keycode == SHIFT_DOWN_L || keycode == SHIFT_DOWN_R){
        shift = 1;
        sti();
        lock = 0;
        return;
      }
      if(keycode == CAPS_DOWN){
        if(caps == 1){
          caps = 0;
        } else{
          caps = 1;
        }
        sti();
        lock = 0;
        return;
      }
      if(keycode == CTRL_DOWN){
        ctrl = 1;
        sti();
        lock = 0;
        return;
      }

      ascii = keyboard_map[keycode][shift];

      //Crtl button cases

      if(ctrl && ascii == 'l'){
        clear();
        memset(keyboard_buf, ' ', MAX_BUF_SIZE);
        keyboard_idx = 0;
        last_idx = 0;
        sti();
        lock = 0;
        return;
      }
	 if(ctrl && ascii == '1'){   //list all files
		clear();
		/*for(i=0;i<(int)my_boot_block.num_dentries;i++){
			memcpy(&file_size,inodes+(my_dentry[i].inode)*BLOCK_ADDR_SIZE,4);
			write(STDOUT,"file_name:",strlen("file_name:"));
			write(STDOUT,my_dentry[i].file_name,strlen((int8_t*)my_dentry[i].file_name));
			write(STDOUT,"         ",strlen("          "));
			write(STDOUT,"file_type:",strlen("file_type:"));
			itoa(my_dentry[i].file_type, one_line_buf, 10);//10 for decimal system 
			write(STDOUT,one_line_buf,strlen(one_line_buf));
			write(STDOUT,"      ",strlen("      "));
			strcpy(one_line_buf,"");
			write(STDOUT,"file_size:",strlen("file_size:"));
			itoa(file_size, one_line_buf, 10);//10 for decimal system
			write(STDOUT,one_line_buf,strlen(one_line_buf));
			putc('\n');						
		}*/
		for(i=0;i<(int)my_boot_block.num_dentries;i++){
			strcpy(one_line_buf,"");
			dir_read(one_line_buf);
			keyboard_write(one_line_buf,strlen(one_line_buf));
		}
		sti(); 
		lock=0;
		return;		
	 }
	 if(ctrl && ascii == '2'){   //read file by name
		 clear();
		 strcpy(my_file_name,"frame0.txt");
		 if(read_dentry_by_name((uint8_t*)(my_file_name),&curr_dentry)==0){
			memcpy(&file_size,inodes+(curr_dentry.inode)*BLOCK_ADDR_SIZE,4);
			uint8_t buf[file_size+1];
			copied=read_data(curr_dentry.inode,0,buf,file_size);
			if(copied==0)
				copied=file_size;
			for(i=0;i<copied;i++){
				putc(buf[i]);
			}		
			putc('\n');
			terminal_write("file_name:",strlen("file_name:"));
			terminal_write(curr_dentry.file_name,strlen((int8_t*)curr_dentry.file_name));
		}
		 sti();
		 lock=0;
		 return;
	 }
	 if(ctrl&&ascii == '3'){   //read file by index, repeat press to switch file
		clear();
		if(count>=my_boot_block.num_dentries)
			count=0;
		if(read_dentry_by_index(count,&curr_dentry)==0){
			memcpy(&file_size,inodes+(curr_dentry.inode)*BLOCK_ADDR_SIZE,4);
			uint8_t buf[file_size+1];
			copied=read_data(curr_dentry.inode,0,buf,file_size);
			if(copied==0)
				copied=file_size;
			for(i=0;i<copied;i++){
				putc(buf[i]);
			}
			putc('\n');
			terminal_write("file_name:",strlen("file_name:"));
			terminal_write(curr_dentry.file_name,strlen((int8_t*)curr_dentry.file_name));			
			count++;
		}
		sti();
		lock=0;
		return;
	 }
	

  if(ctrl&&ascii == '4'){   //test rtc, repeat press to change frequency
    clear();
    toggle_freq();    //test_rtc is started in kernel.c, so only have to change freq
    sti();
    lock=0;
    return;
  }

  if(ctrl&&ascii == '5') {    //end rtc test
    clear();
    set_rate(0);       //set rate to 0 to end test
    sti();
    lock=0;
    return;
  }
      if(keyboard_idx == MAX_BUF_SIZE){
        sti();
        lock = 0;
        return;
      }

      if((screen_x == (NUM_COLS-1) && screen_y == (NUM_ROWS-1)) || (screen_y == (NUM_ROWS-1) && ascii == '\n')){
        vert_scroll();
        if(screen_y == (NUM_ROWS-1) && ascii == '\n'){
          sti();
          lock = 0;
          return;
        }
      }

      if((ascii >= 'a' && ascii <= 'z' && caps) || (caps && ascii >= 'A' && ascii <= 'Z')){
        keyboard_buf[keyboard_idx] = keyboard_map[keycode][!(shift)];
      } else if(ascii != '\0'){
        keyboard_buf[keyboard_idx] = ascii;
      }

      keyboard_idx++;
      //char x[] = "terminal_write";
      //uint8_t s = STDIN;
      //uint8_t b = MAX_BUF_SIZE;
      //DO_CALL(x, 4, 0, keyboard_buf, 128);
      //DO_CALL(x, 4, s, keyboard_buf, b);

      //x[0] = 'f';
      //s++;
      //b++;
      keyboard_write(keyboard_buf, MAX_BUF_SIZE);
      last_idx++;

      if(ascii == '\n'){
        enter = 1;
        memset(keyboard_buf, ' ', MAX_BUF_SIZE);   //<------------------------ **** DOES NOT WORK ALL OF THE TIME PLEASE INVESTIGATE
        keyboard_idx = 0;
        last_idx = 0;
      }
          //write the value of the ascii char to the screen

    }
    sti();          //enable interrupts and unlock
    lock = 0;
  }
}




/*
void get_keyboard_idx
  Input: data - pointer to array to copy to
  Return Value: none 
  Function: Puts the index of the last written character and the current keyboard cursor in the given array
*/
void get_keyboard_idx(int* data){
  data[0] = last_idx;
  data[1] = keyboard_idx;
}

/*
void keyboard_backspace
  Input: none
  Return Value: none 
  Function: Helper function called by handler for backspacing
*/
void handle_backspace(){
  if((keyboard_idx) != 0){
    int screen_y_temp = screen_y;
    if(screen_x == 0 && screen_y != 0){
      int catch = find_last_char(screen_y-1);
      if(catch == -1){
        screen_y--;
        return;
      }
      screen_x = catch+2;           //because we decrement screen x after this (for regualr case) we need to offest the returned value from find_lastr_char
      screen_y_temp = --screen_y;
    }
    int screen_temp = --screen_x;
    keyboard_buf[keyboard_idx-1] = '\0';
    last_idx--;
    keyboard_write(keyboard_buf, MAX_BUF_SIZE);
    screen_x = screen_temp;
    screen_y = screen_y_temp;
    keyboard_idx--;
  }
  update_cursor(screen_y, screen_x);
}

/*int32_t keyboard_read(void* buf, int32_t nbytes) {
	while(enter == 0){        //wait until enter is pressed
      //wait
    };

    int idx[2];
    int i;
    int j = 0;            //holds the index of the first char to copy to the buffer
    int count = 0;          //hold the number of new lines seen so far
    get_keyboard_idx(idx);      //get the idecies of the keyboard

    for(i = (idx[1]-1); i >= 0; i--){ //start at the right end of the buffer and go until 0
      if(keyboard_buf[i] == '\n'){
        count++;
        if(count == 2){     //if the new line count is 2 then youre done
          j = i+1;
          break;
        }
      }
    }
    for(i = j; keyboard_buf[i] != '\0'; i++){   //copys the keyboard buffer to the given buffer
      if((i-j) == nbytes){
        break;
      }
      ((char *)buf)[i] = keyboard_buf[i];
    }
    enter = 0;          //set the volatile enter to zero
    return (i-j);       //the number of bytes read
}

int32_t keyboard_write(void* buf, int32_t nbytes) {
  int i;
  int idx[2];
  get_keyboard_idx(idx);
    if(idx[0] != idx[1]){           //this
      for(i = idx[0]; i < idx[1]; i++){
       char data = ((char *)buf)[i];
       putc(data);
      }
     return(idx[1] - idx[0]);
    }
	return -1;
}*/
