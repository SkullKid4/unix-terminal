#include "keyboard.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"

volatile unsigned lock = 0;       //used to lock the thread when writing keybored output to the screen
volatile unsigned shift = 0;      //2a or 36 on press;
volatile unsigned caps = 0;
volatile unsigned ctrl = 0;

/*
this is a map I retrived from https://github.com/arjun024/mkeykernel/blob/master/keyboard_map.h
All it does is converts the raw output from the keyboared into the ascii chars they are associated with
*/
//128
unsigned char keyboard_map[128][2] =
{
    {0, 0},  {27, 27}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'},  /* 9 */
  {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'}, {'\b', '\b'}, /* Backspace */
  {'\t', '\t'},     /* Tab */
  {'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'}, /* 19 */
  {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'}, {'[', '{'}, {']', '}'}, {'\n', '\n'}, /* Enter key */
    {0, 0},     /* 29   - Control */
  {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'}, /* 39 */
 {'`', '"'}, {'`', '~'},   {0, 0},    /* Left shift */
 {'\\', '|'}, {'z', 'Z'}, {'x', 'X'}, {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'},     /* 49 */
  {'m', 'M'}, {',', '<'}, {'.', '>'}, {'/', '?'},   {0, 0},       /* Right shift */
  {0, 0},
    {0, 0}, /* Alt */
  {' ', ' '}, /* Space bar */
    {0, 0}, /* Caps lock */
    {0, 0}, /* 59 - F1 key ... > */
    {0, 0},   {0, 0},   {0, 0},   {0, 0},   {0, 0},   {0, 0},   {0, 0},   {0, 0},
    {0, 0}, /* < ... F10 */
    {0, 0}, /* 69 - Num lock*/
    {0, 0}, /* Scroll Lock */
    {0, 0}, /* Home key */
    {0, 0}, /* Up Arrow */
    {0, 0}, /* Page Up */
  {'-', '-'},
    {0, 0}, /* Left Arrow */
    {0, 0},
    {0, 0}, /* Right Arrow */
  {'+', '+'},
    {0, 0}, /* 79 - End key*/
    {0, 0}, /* Down Arrow */
    {0, 0}, /* Page Down */
    {0, 0}, /* Insert Key */
    {0, 0}, /* Delete Key */
    {0, 0},   {0, 0},   {0, 0},
    {0, 0}, /* F11 Key */
    {0, 0}, /* F12 Key */
    {0, 0}, /* All other keys are undefined */
};

/*
void keyboard_init()
  INPUT: none
  Return Value: none
  Function: sets the 0-15, 16-31 bits to point to the keyboard handler we defined
*/
void keyboard_init(){
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
      if(keycode < 0 || keycode > MAX_PRESS_CODE){    //if this is a button release code, unlock and turn on interrupts
        if(keycode == SHIFT_UP_L || keycode == SHIFT_UP_R){
          shift = 0;
        } else if(keycode == CTRL_UP){
          ctrl = 0;
        }
        lock = 0;
        sti();
        return;
      }
      if(keycode == SHIFT_DOWN_L || keycode == SHIFT_DOWN_R){
        shift = 1;
        lock = 0;
        sti();
        return;
      }
      if(keycode == CAPS_DOWN){
        if(caps == 1){
          caps = 0;
        } else{
          caps = 1;
        }
        lock = 0;
        sti();
        return;
      }
      if(keycode == CTRL_DOWN){
        ctrl = 1;
        lock = 0;
        sti();
        return;
      }

      ascii = keyboard_map[keycode][shift];

      if(ctrl && ascii == 'l'){
        clear();
        ctrl = 1;
        lock = 0;
        sti();
        return;
      }

      if((ascii >= 'a' && ascii <= 'z' && caps) || (caps && ascii >= 'A' && ascii <= 'Z')){
        putc(keyboard_map[keycode][!(shift)]);
      } else{
        putc(ascii);
      }

          //write the value of the ascii char to the screen

    }
    sti();          //enable interrupts and unlock
    lock = 0;
  }
}
