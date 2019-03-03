/* signal.h standard header */
#ifndef _SIGNAL
#define _SIGNAL
#ifndef _YVALS
#include <yvals.h>
#endif
		/* type definitions */
typedef int sig_atomic_t;
typedef void _Sigfun(int);
		/* signal codes */
#define	SIGHUP	           1	/* hangup */
#define SIGINT             2	/* interrupt (DEL) */
#define SIGQUIT            3	/* quit (ASCII FS) */
#define SIGILL             4	/* illegal instruction (not reset when caught)*/
#define SIGTRAP            5	/* trace trap (not reset when caught) */
#define SIGIOT             6	/* IOT instruction */
#define SIGEMT             7	/* EMT instruction */
#define SIGFPE             8	/* floating point exception */
#define SIGKILL            9	/* kill (cannot be caught or ignored) */
#define SIGBUS            10	/* bus error */
#define SIGSEGV           11	/* segmentation violation */
#define SIGSYS            12	/* bad argument to system call */
#define SIGPIPE           13	/* write on a pipe with no one to read it */
#define SIGALRM           14	/* alarm clock */
#define SIGTERM           15	/* software termination signal from kill */

#define STACK_FAULT       16    /* used by kernel to signal stack fault */

#define SIGABRT	    _SIGABRT
#define _NSIG	     _SIGMAX    /* one more than last code */
		/* signal return values */
#define SIG_DFL	(_Sigfun *)0
#define SIG_ERR	(_Sigfun *)-1
#define SIG_IGN	(_Sigfun *)1
		/* declarations */
int raise(int);
_Sigfun *signal(int, _Sigfun *);
#endif
