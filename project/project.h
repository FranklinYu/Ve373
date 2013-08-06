/**********************************************************
 *      Project header                                    *
 **********************************************************/
#include <p32xxxx.h>

unsigned char key_F, key_E;

void key_init(); // to initialize the keyboard
int get_num(int row, int column); // returns value on keyboard
