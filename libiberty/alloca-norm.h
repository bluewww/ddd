/* "Normal" configuration for alloca.  */

#ifdef __GNUC__

#define alloca __builtin_alloca

#else /* not __GNUC__ */

#ifdef sparc
#include <alloca.h>
/* Stupid include file doesn't declare it */
#ifdef __STDC__
extern PTR __builtin_alloca(size_t);
#else
extern PTR __builtin_alloca();
#endif

#else /* not sparc */

#ifdef __STDC__
PTR alloca (size_t);
#else
PTR alloca ();			/* must agree with functions.def */
#endif

#endif /* not sparc */
#endif /* not __GNUC__ */
