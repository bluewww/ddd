/* "Normal" configuration for alloca.  */

#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not __GNUC__ */
#ifdef sparc
#include <alloca.h>
/* Stupid include file doesn't declare it */
extern PTR __builtin_alloca();
#else
#ifdef __STDC__
PTR alloca (size_t);
#else
PTR alloca ();			/* must agree with functions.def */
#endif
#endif /* sparc */
#endif /* not __GNUC__ */
