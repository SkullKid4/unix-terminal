#include "keyboard.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "syscall.h"
#include "files.h"

volatile unsigned lock = 0;       //used to lock the thread when writing keyboard output to the screen
volatile unsigned shift = 0;      //2a or 36 on press;
volatile unsigned caps = 0;
volatile unsigned ctrl = 0;



static int keyboard_idx;
static int last_idx;



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

/*
void keyboard_init()
  INPUT: none
  Return Value: none
  Function: sets the 0-15, 16-31 bits to point to the keyboard handler we defined
*/
void keyboard_init(){
  memset(keyboard_buf, ' ', 128);
  keyboard_idx = 0;
  last_idx = 0;
  enter = 0;
  SET_IDT_ENTRY(idt[KEYBOARD_IDT_IDX], (keyboard_handler));
}

/*
void keyboard_handler()
  INPUT: none
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
        if((keyboard_idx) != 0){
          int screen_y_temp = screen_y;
          if(screen_x == 0 && screen_y != 0){
            int catch = find_last_char(screen_y-1);
            if(catch == -1){
              screen_y--;
              sti();
              lock = 0;
              return;
            }
            screen_x = catch+2;           //because we decriment screen x after this (for regualr case) we need to offest the returned value from find_lastr_char
            screen_y_temp = --screen_y;
          }
          int screen_temp = --screen_x;
          keyboard_buf[keyboard_idx-1] = '\0';
          last_idx--;
          write(STDOUT, keyboard_buf, 128);
          screen_x = screen_temp;
          screen_y = screen_y_temp;
          keyboard_idx--;
        }
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
        memset(keyboard_buf, ' ', 128);
        keyboard_idx = 0;
        last_idx = 0;
        sti();
        lock = 0;
        return;
      }
	/************************************/
	//system files call written by Joann
	int i;
	uint32_t file_size;
	 if(ctrl && ascii == '1'){
		clear();
		for(i=0;i<(int)my_boot_block.num_dentries;i++){
			memcpy(&file_size,inodes+(my_dentry[i].inode)*BLOCK_ADDR_SIZE,4);
			printf("file name:%s,    file_type: %u, file_size: %u\n",my_dentry[i].file_name,my_dentry[i].file_type,file_size);
		}
		sti(); 
		lock=0;
		return;		
	 }
	
	
	/************************************/
      if(keyboard_idx == 128){
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
      write(STDOUT, keyboard_buf, 128);
      last_idx++;

      if(ascii == '\n'){
        enter = 1;
      }
          //write the value of the ascii char to the screen

    }
    sti();          //enable interrupts and unlock
    lock = 0;
  }
}

void get_keyboard_idx(int* data){
  data[0] = last_idx;
  data[1] = keyboard_idx;
}
