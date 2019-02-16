#include "../h/const.h"
#include "../h/type.h"
#include "../h/callnr.h"
#include "../h/com.h"
#include "../h/error.h"

#define FS          FS_PROC_NR
#define MM          MMPROCNR

extern int errno;
extern message M;


/*----------------------------------------------------------------------------
			Messages to systask (special calls)
----------------------------------------------------------------------------*/

PUBLIC sys_xit(parent, proc)
int parent;			/* parent of exiting proc. */
int proc;			/* which proc has exited */
{
/* A proc has exited.  Tell the kernel. */

  callm1(SYSTASK, SYS_XIT, parent, proc, 0, NIL_PTR, NIL_PTR, NIL_PTR);
}


PUBLIC sys_getsp(proc, newsp)
int proc;			/* which proc has enabled signals */
vir_bytes *newsp;		/* place to put sp read from kernel */
{
/* Ask the kernel what the sp is. */


  callm1(SYSTASK, SYS_GETSP, proc, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
  *newsp = (vir_bytes) M.STACK_PTR;
}


PUBLIC sys_sig(proc, sig, sighandler)
int proc;			/* which proc has exited */
int sig;			/* signal number: 1 - 16 */
int (*sighandler)();		/* pointer to signal handler in user space */
{
/* A proc has to be signaled.  Tell the kernel. */

  M.m6_i1 = proc;
  M.m6_i2 = sig;
  M.m6_f1 = sighandler;
  callx(SYSTASK, SYS_SIG);
}


PUBLIC sys_fork(parent, child, pid)
int parent;			/* proc doing the fork */
int child;			/* which proc has been created by the fork */
int pid;			/* process id assigned by MM */
{
/* A proc has forked.  Tell the kernel. */

  callm1(SYSTASK, SYS_FORK, parent, child, pid, NIL_PTR, NIL_PTR, NIL_PTR);
}


PUBLIC sys_exec(proc, ptr)
int proc;			/* proc that did exec */
char *ptr;			/* new stack pointer */
{
/* A proc has exec'd.  Tell the kernel. */

  callm1(SYSTASK, SYS_EXEC, proc, 0, 0, ptr, NIL_PTR, NIL_PTR);
}

PUBLIC sys_newmap(proc, ptr)
int proc;			/* proc whose map is to be changed */
char *ptr;			/* pointer to new map */
{
/* A proc has been assigned a new memory map.  Tell the kernel. */


  callm1(SYSTASK, SYS_NEWMAP, proc, 0, 0, ptr, NIL_PTR, NIL_PTR);
}

PUBLIC sys_copy(mptr)
message *mptr;			/* pointer to message */
{
/* A proc wants to use local copy. */

  /* Make this routine better.  Also check other guys' error handling -DEBUG */
  mptr->m_type = SYS_COPY;
  if (sendrec(SYSTASK, mptr) != OK) panic("sys_copy can't send", NO_NUM);
}

PUBLIC sys_times(proc, ptr)
int proc;			/* proc whose times are needed */
real_time ptr[4];			/* pointer to time buffer */
{
/* Fetch the accounting info for a proc. */

  callm1(SYSTASK, SYS_TIMES, proc, 0, 0, ptr, NIL_PTR, NIL_PTR);
  ptr[0] = M.USER_TIME;
  ptr[1] = M.SYSTEM_TIME;
  ptr[2] = M.CHILD_UTIME;
  ptr[3] = M.CHILD_STIME;
}





PUBLIC sys_abort()
{
/* Something awful has happened.  Abandon ship. */

  callm1(SYSTASK, SYS_ABORT, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
}


PUBLIC int tell_fs(what, p1, p2, p3)
int what, p1, p2, p3;
{
/* This routine is only used by MM to inform FS of certain events:
 *      tell_fs(CHDIR, slot, dir, 0)
 *      tell_fs(EXIT, proc, 0, 0)
 *      tell_fs(FORK, parent, child, 0)
 *      tell_fs(SETGID, proc, realgid, effgid)
 *      tell_fs(SETUID, proc, realuid, effuid)
 *      tell_fs(SYNC, 0, 0, 0)
 *      tell_fs(UNPAUSE, proc, signr, 0)
 */
  callm1(FS, what, p1, p2, p3, NIL_PTR, NIL_PTR, NIL_PTR);
}
