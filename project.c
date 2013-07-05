/***********************************************************
 *      Project code
 **********************************************************/

#include <p32xxxx.h>

int main ()
{
    initialTimer ();
    initialPWM ();
    initialSpeaker ()
    startAll ();
    while (1);
    return 0;
}
