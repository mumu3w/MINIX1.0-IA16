/* errno.h standard header */
#ifndef _ERRNO
#define _ERRNO
#ifndef _YVALS
#include <yvals.h>
#endif
		/* error codes */
#define EDOM	_EDOM
#define ERANGE	_ERANGE
#define EFPOS	_EFPOS
#define OK                 0
#define ERROR              1
#define EPERM              1
#define ENOENT             2
#define ESRCH              3
#define EINTR              4
#define EIO                5
#define ENXIO              6
#define E2BIG              7
#define ENOEXEC            8
#define EBADF              9
#define ECHILD            10
#define EAGAIN            11
#define ENOMEM            12
#define EACCES            13
#define EFAULT            14
#define ENOTBLK           15
#define EBUSY             16
#define EEXIST            17
#define EXDEV             18
#define ENODEV            19
#define ENOTDIR           20
#define EISDIR            21
#define EINVAL            22
#define ENFILE            23
#define EMFILE            24
#define ENOTTY            25
#define ETXTBSY           26
#define EFBIG             27
#define ENOSPC            28
#define ESPIPE            29
#define EROFS             30
#define EMLINK            31
#define EPIPE             32
/* #define EDOM              33 */
/* #define ERANGE            34 */

#define E_LOCKED         101
#define E_BAD_CALL       102
#define E_LONG_STRING    103
	/* ADD YOURS HERE */
#define _NERR	_ERRMAX	/* one more than last code */
		/* declarations */
extern int errno;
#endif
