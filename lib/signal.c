#include "../include/lib.h"
#include "../include/signal.h"

int (*vectab[NR_SIGS])();	/* array of functions to catch signals */

/* The definition of signal really should be 
 *  PUBLIC int (*signal(signr, func))()
 * but some compilers refuse to accept this, even though it is correct.
 * The only thing to do if you are stuck with such a defective compiler is
 * change it to
 *  PUBLIC int *signal(signr, func)
 * and change ../h/signal.h accordingly.
 */

PUBLIC int (*signal(signr, func))()
int signr;			/* which signal is being set */
int (*func)();			/* pointer to function that catches signal */
{
  int r,(*old)();

  old = vectab[signr - 1];
  vectab[signr - 1] = func;
  M.m6_i1 = signr;
  M.m6_f1 = ( (func == SIG_IGN || func == SIG_DFL) ? func : begsig);
  r = callx(MM, SIGNAL);
  return( (r < 0 ? (int (*)()) r : old) );
}
