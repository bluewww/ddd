/* MPW-Unix compatibility library.
   Copyright (C) 1993, 1994 Free Software Foundation, Inc.

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* This should only be compiled and linked under MPW. */

#include <stdio.h>
#include <stdlib.h>
#include <files.h>
#include <ioctl.h>
#include <ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <Types.h>

#include <spin.h>

#ifndef R_OK
#define R_OK 4
#define W_OK 2
#define X_OK 1
#endif

#define ENOSYS 99

extern int errno;

#ifndef IN_GCC

extern int sys_nerr;

extern char **sys_errlist;

#endif

int DebugPI = 0;

void mpw_abort (void);

void
mpwify_filename(char *unixname, char *macname)
{
  int i, j, in_middle, terminate = 0;

  /* (should truncate 255 chars from end of name, not beginning) */
  if (strlen (unixname) > 255)
    {
      fprintf (stderr, "Pathname \"%s\" is too long for Macs, truncating\n",
	       unixname);
      terminate = 1;
    }
  /* Abs Unix path to abs Mac path. */
  if (*unixname == '/')
    {
      if (strncmp (unixname, "/tmp/", 6) == 0)
	{
	  /* A temporary name, make a more Mac-flavored tmpname. */
	  /* A better choice would be {Boot}Trash:foo, but that would
	     require being able to identify the boot disk's and trashcan's
	     name.  Another option would be to have an env var, so user
	     can point it at a ramdisk. */
	  strncpy (macname, unixname, 255);
	  if (terminate)
	    macname[255] = '\0';
	  macname[0] = ':';
	  macname[4] = '_';
	}
      else
	{
	  /* Assume that the leading component is a valid disk name. */
	  strncpy (macname, unixname + 1, 255);
	}
    }
  else
    {
      /* If this is a "Unix-only" pathname, assume relative. */
      if (strchr (unixname, '/') && ! strchr (unixname, ':'))
	{
	  macname[0] = ':';
	  strncpy (macname + 1, unixname, 255);
	}
      else
	{
	  /* Otherwise copy it verbatim. */
	  /* ... but if of the form ":/foo", lose the extra colon;
	     the slash will be made into a colon shortly. */
	  if (unixname[0] == ':' && unixname[1] == '/')
	    ++unixname;
	  strncpy (macname, unixname, 255);
	}
    }
  if (terminate)
    macname[255] = '\0';
  for (i = 0; macname[i] != '\0'; ++i)
    {
      if (macname[i] == '/')
	macname[i] = ':';
    }
  in_middle = 0;
  j = 0;
  for (i = 0; macname[i] != '\0'; ++i)
    {
      /* We're in the middle of the name when a char is not a colon. */
      if (macname[i] != ':')
	in_middle = 1;
      /* Copy chars verbatim, *unless* the char is the first of a pair
	 of colons in the middle of a pathname. */
      if (!(in_middle && macname[i] == ':' && macname[i+1] == ':'))
	macname[j++] = macname[i];
    }
  macname[j] = '\0';
  /* If we have a trailing ":.", make it into a ":". */
  if (j >= 2 && macname[j-2] == ':' && macname[j-1] == '.')
    macname[j-1] = '\0';
  if (DebugPI)
    {
      fprintf (stderr, "# Made \"%s\"\n", unixname);
      fprintf (stderr, "# into \"%s\"\n", macname);
    }
}

/* MPW-flavored basename finder. */

char *
mpw_basename (name)
  char *name;
{
  char *base = name;

  while (*name)
    {
      if (*name++ == ':')
	{
	  base = name;
	}
    }
  return base;
}

/* Mixed MPW/Unix basename finder.  This can be led astray by
   filenames with slashes in them and come up with a basename that
   either corresponds to no file or (worse) to some other file, so
   should only be tried if other methods of finding a file via a
   basename have failed.  */

char *
mpw_mixed_basename (name)
  char *name;
{
  char *base = name;

  while (*name)
    {
      if (*name == '/' || *name == ':')
	{
	  base = name + 1;
	}
      ++name;
    }
  return base;
}

/* This function is fopen() modified to create files that are type TEXT
   or 'BIN ', and always of type 'MPS '.  */

FILE *
mpw_fopen (char *name, char *mode)
{
#undef fopen
  int errnum;
  FILE *fp;
  char tmpname[256];

  mpwify_filename (name, tmpname);
  PROGRESS (1);
  fp = fopen (tmpname, mode);
  errnum = errno;

  /* If writing, need to set type and creator usefully. */
  if (strchr (mode, 'w'))
    {
      char *pname = (char *) malloc (strlen (tmpname) + 2);
      OSErr e;
      struct FInfo fi;

      pname[0] = strlen (tmpname);
      strcpy (pname+1, tmpname);
	
      e = GetFInfo (pname, 0, &fi);
      /* should do spiffier error handling */
      if (e != 0)
	fprintf(stderr, "GetFInfo returns %d\n", e);
      if (strchr (mode, 'b'))
	{
	  fi.fdType = (OSType) 'BIN ';
	}
      else
	{
	  fi.fdType = (OSType) 'TEXT';
	}
      fi.fdCreator = (OSType) 'MPS ';
      e = SetFInfo (pname, 0, &fi);
      if (e != 0)
	fprintf(stderr, "SetFInfo returns %d\n", e);
      free (pname);
    }
  if (fp == NULL)
    errno = errnum;
  return fp;
}

/* This is a version of fseek() modified to fill the file with zeros
   if seeking past the end of it.  */

#define ZEROBLKSIZE 4096

char zeros[ZEROBLKSIZE];

int
mpw_fseek (FILE *fp, int offset, int whence)
{
  int cursize, numleft;

  PROGRESS (1);
  if (whence == SEEK_SET)
    {
      fseek (fp, 0, SEEK_END);
      cursize = ftell (fp);
      if (offset > cursize)
	{
	  numleft = offset - cursize;
	  while (numleft > ZEROBLKSIZE)
	    {
	      /* This might fail, should check for that. */
	      PROGRESS (1);
	      fwrite (zeros, 1, ZEROBLKSIZE, fp);
	      numleft -= ZEROBLKSIZE;
	    }
	  PROGRESS (1);
	  fwrite (zeros, 1, numleft, fp);
	  fflush (fp);
	}
    }
  return fseek (fp, offset, whence);
}

int
mpw_fread (char *ptr, int size, int nitems, FILE *stream)
{
#undef fread
  int rslt;

  PROGRESS (1);
  rslt = fread (ptr, size, nitems, stream);
  PROGRESS (1);
  return rslt;
}

int
mpw_fwrite (char *ptr, int size, int nitems, FILE *stream)
{
#undef fwrite
  int rslt;

  PROGRESS (1);
  rslt = fwrite (ptr, size, nitems, stream);
  PROGRESS (1);
  return rslt;
}

int
fork ()
{
  fprintf (stderr, "fork not available!\n");
  mpw_abort ();
}

int
vfork ()
{
  fprintf (stderr, "vfork not available!\n");
  mpw_abort ();
  return (-1);
}

int
pipe (int *fd)
{
  fprintf (stderr, "pipe not available!\n");
  mpw_abort ();
  return (-1);
}

int
execvp (char *file, char **argv)
{
  fprintf (stderr, "execvp not available!\n");
  mpw_abort ();
  return (-1);
}

int
execv (char *path, char **argv)
{
  fprintf (stderr, "execv not available!\n");
  mpw_abort ();
  return (-1);
}

int
kill (int pid, int sig)
{
  fprintf (stderr, "kill not available!\n");
  mpw_abort ();
  return (-1);
}

int
wait (int *status)
{
  *status = 0;
  return 0;
}

int
sleep (int seconds)
{
  unsigned long start_time, now;

  time (&start_time);

  while (1)
    {
      PROGRESS (1);
      time (&now);
      if (now > start_time + seconds)
	return 0;
    }
}

void
putenv (char *str)
{
  /* The GCC driver calls this to do things for collect2, but we
     don't care about collect2. */
}

int
chmod (char *path, int mode)
{
  /* Pretend it was all OK. */
  return 0;
}

int
getuid ()
{
  /* One value is as good as another... */
  return 0;
}

int
getgid ()
{
  /* One value is as good as another... */
  return 0;
}

/* Instead of coredumping, which is not a normal Mac facility, we
   drop into Macsbug.  If we then "g" from Macsbug, the program will
   exit cleanly. */

void
mpw_abort ()
{
  /* Make sure no output still buffered up, then zap into MacsBug. */
  fflush(stdout);
  fflush(stderr);
  printf("## Abort! ##\n");
#ifdef MPW_SADE
  SysError(8005);
#else 
  Debugger();
#endif
  /* "g" in MacsBug will then cause a regular error exit. */
  exit (1);
}

/* Imitation getrusage based on the ANSI clock() function. */

int
getrusage (int who, struct rusage *rusage)
{
  int clk = clock ();

  rusage->ru_utime.tv_sec = clk / CLOCKS_PER_SEC;
  rusage->ru_utime.tv_usec = ((clk * 1000) / CLOCKS_PER_SEC) * 1000;
  rusage->ru_stime.tv_sec = 0;
  rusage->ru_stime.tv_usec = 0;
}

int
sbrk ()
{
  return 0;
}

char **environ = NULL;

int
isatty (int fd)
{
  return 0;
}

/* link.c  --  Un*x like function to hard link one file to another.
 *          by  ||ugh Daniel <hugh@cygnus.com> 1994/1/3  */

/*
 *  We have two choices here, make an Alias or duplacate the file.
 * The only place I see this being used unlinks the orignal file right
 * after makeing the link, so I think we copy the file.
 *
 *  We presume that we are copying a Mac style file (as it might be a
 * mac binary executable), so we use funney Macintosh style functions
 * insted of fopen() or something.
 *
 */

int link (char *path1, char *path2)
{
#ifdef    never
  OSErr macretval;
  HParamBlockRec workorder;

  workorder = (HParamBlockRec) malloc(sizeof(union HParamBlockRec));
  workorder->CopyParam.
    macretval = PBHCopyFile(workorder, false);
  if ( macretval ) {
  }
#else  /* never */
  return (-1);  /* link does not fit the macintosh file system model */
#endif /* never */
}

/* This is inherited from Timothy Murray's Posix library. */

#include "utime.h"

int
utime (char *filename, struct utimbuf *times)
{
  CInfoPBRec cipbr;
  HFileInfo *fpb = (HFileInfo *) &cipbr;
  DirInfo *dpb = (DirInfo *) &cipbr;
  unsigned char pname[256];
  short err;
  
  strcpy ((char *) pname, filename);
  c2pstr (pname);

  dpb->ioDrDirID = 0L;
  fpb->ioNamePtr = pname;
  fpb->ioVRefNum = 0;
  fpb->ioFDirIndex = 0;
  fpb->ioFVersNum = 0;
  err = PBGetCatInfo (&cipbr, 0);
  if (err != noErr) {
    errno = ENOENT;
    return -1;
  }
  dpb->ioDrDirID = 0L;
  fpb->ioFlMdDat = times->modtime;
  fpb->ioFlCrDat = times->actime;
  err = PBSetCatInfo (&cipbr, 0);
  if (err != noErr) {
    errno = EACCES;
    return -1;
  }
  return 0;
}

int
mkdir (char *path, int mode)
{
  errno = ENOSYS;
  return -1;
}

int
rmdir ()
{
  errno = ENOSYS;
  return -1;
}

chown ()
{
  errno = ENOSYS;
  return -1;
}

/* Minimal 'stat' emulation: tells directories from files and
   gives length and mtime.

   Derived from code written by Guido van Rossum, CWI, Amsterdam
   and placed by him in the public domain.  */

char *myenviron[] = {NULL};

char **environ = myenviron;

extern int __uid, __gid;

int __uid = 0;
int __gid = 0;

/* Bits in ioFlAttrib: */
#define LOCKBIT	(1<<0)		/* File locked */
#define DIRBIT	(1<<4)		/* It's a directory */

/* Macified "stat" in which filename is given relative to a directory,
   specified by long DirID.  */

static int
_stat (char *name, long dirid, struct stat *buf)
{
  CInfoPBRec cipbr;
  HFileInfo *fpb = (HFileInfo*) &cipbr;
  DirInfo *dpb = (DirInfo*) &cipbr;
  Str255 pname;
  short err;

  /* Make a temp copy of the name and pascalize. */
  strcpy ((char *) pname, name);
  c2pstr (pname);
  
  cipbr.dirInfo.ioDrDirID = dirid;
  cipbr.hFileInfo.ioNamePtr = pname;
  cipbr.hFileInfo.ioVRefNum = 0;
  cipbr.hFileInfo.ioFDirIndex = 0;
  cipbr.hFileInfo.ioFVersNum = 0;
  err = PBGetCatInfo (&cipbr, 0);
  if (err != noErr)
    {
      errno = ENOENT;
      return -1;
    }
  /* Mac files are readable if they can be accessed at all. */
  buf->st_mode = 0444;
  /* Mark unlocked files as writeable. */
  if (!(fpb->ioFlAttrib & LOCKBIT))
    buf->st_mode |= 0222;
  if (fpb->ioFlAttrib & DIRBIT)
    {
      /* Mark directories as "executable". */
      buf->st_mode |= 0111 | S_IFDIR;
      buf->st_size = dpb->ioDrNmFls;
      buf->st_rsize = 0;
    }
  else
    {
      buf->st_mode |= S_IFREG;
      /* Mark apps as "executable". */
      if (fpb->ioFlFndrInfo.fdType == 'APPL')
	buf->st_mode |= 0111;
      /* Fill in the sizes of data and resource forks. */
      buf->st_size = fpb->ioFlLgLen;
      buf->st_rsize = fpb->ioFlRLgLen;
    }
  /* Fill in various times. */
  buf->st_atime = fpb->ioFlCrDat;
  buf->st_mtime = fpb->ioFlMdDat;
  buf->st_ctime = fpb->ioFlCrDat;
  /* Set up an imitation inode number. */
  buf->st_ino = (unsigned short) fpb->ioDirID;
  /* Set up an imitation device. */
  GetVRefNum (buf->st_ino, &buf->st_dev);
  buf->st_uid = __uid;
  buf->st_gid = __gid;
/*  buf->st_FlFndrInfo = fpb->ioFlFndrInfo;  */
  return 0;
}

/* stat() sets up an empty dirid. */

int
stat (char *path, struct stat *buf)
{
  long rslt, errnum;
  char tmpname[256];

  mpwify_filename (path, tmpname);
  if (DebugPI)
    fprintf (stderr, "# stat (%s, %x)", tmpname, buf);
  PROGRESS (1);
  rslt = _stat (tmpname, 0L, buf);
  errnum = errno;
  if (DebugPI)
    {
      fprintf (stderr, " -> %d", rslt);
      if (rslt != 0)
	fprintf (stderr, " (errno is %d)", errnum);
      fprintf (stderr, "\n");
      fflush (stderr);
    }
  if (rslt != 0)
    errno = errnum;
  return rslt;
}

int
fstat (int fd, struct stat *buf)
{
  FCBPBRec fcb;
  Str255 pathname;
  long dirid = 0L, temp;
  long rslt, errnum;
  short err;

  if (DebugPI)
    fprintf (stderr, "# fstat (%d, %x)", fd, buf);
  PROGRESS (1);
#if 0
  /* fdopen() gives FILE entry with name of file, as well as RefNum
     of containing directory. */
	
/*  FILE *fp = fdopen(fd, "");  */
  
  /* Use PBGetFCBInfo() to convert short RefNum to long dirid. */

  ioctl (fd, FIOREFNUM, &temp);
  fcb.ioRefNum = temp;
#if 0
  fcb.ioRefNum = fp->refnum;
#endif
  fcb.ioVRefNum = 0;
  fcb.ioFCBIndx = 0;
  fcb.ioNamePtr = pathname;
  err = PBGetFCBInfo (&fcb, 0);
  if (0 /*err != noErr*/)
    {
      errnum = ENOENT;
      rslt = -1;
    }
#endif
  dirid = 0L /* fcb.ioFCBParID */ ;
  ioctl (fd, FIOFNAME, (long *) pathname); 
  if (DebugPI)
    fprintf (stderr, " (name is %s)", pathname);
  rslt = _stat ((char *) pathname, dirid, buf);
  errnum = errno;
  if (DebugPI)
    {
      fprintf (stderr, " -> %d", rslt);
      if (rslt != 0)
	fprintf (stderr, " (errno is %d)", errnum);
      fprintf (stderr, "\n");
      fflush (stderr);
    }
  if (rslt != 0)
    errno = errnum;
  return rslt;
}

/* This should probably be more elaborate for MPW. */

char *
getpwd ()
{
  return ":";
}

int
mpw_open (char *filename, int arg2, int arg3)
{
#undef open

  char tmpname[256];

  mpwify_filename (filename, tmpname);
  return open (tmpname, arg2);
}

int
mpw_access (char *filename, unsigned int cmd)
{
#undef access

  int rslt, errnum = 0;
  struct stat st;
  char tmpname[256];

  mpwify_filename (filename, tmpname);
  if (DebugPI)
    fprintf (stderr, "# mpw_access (%s, %d)", tmpname, cmd);
  if (cmd & R_OK || cmd & X_OK)
    {
      rslt = stat (tmpname, &st);
      errnum = errno;
      if (rslt >= 0)
	{
	  if (((st.st_mode & 004 == 0) && (cmd & R_OK))
	      || ((st.st_mode & 002 == 0) && (cmd & W_OK))
	      || ((st.st_mode & 001 == 0) && (cmd & X_OK)))
	    {
	      rslt = -1;
	      errnum = EACCES;
	    }
	}
    }
  if (DebugPI)
    {
      fprintf (stderr, " -> %d", rslt);
      if (rslt != 0)
	fprintf (stderr, " (errno is %d)", errnum);
      fprintf (stderr, "\n");
    }
  if (rslt != 0)
    errno = errnum;
  return rslt;
}

chdir ()
{
  errno = ENOSYS;
  return (-1);
}

char *
getcwd (char *buf, int size)
{
  if (buf == NULL)
    buf = (char *) malloc (size);
  strcpy(buf, ":");
  return buf;
}

/* This is a hack to get control in an MPW tool before it crashes the
   machine.  */

mpw_special_init (name)
     char *name;
{
  if (strstr (name, "DEBUG"))
    DebugStr("\pat beginning of program");
}

static int current_umask;

int
umask(int mask)
{
  int oldmask = current_umask;

  current_umask = mask;
  return oldmask;
}

#ifdef IN_GCC

#define NEED_sys_errlist

/* Extended support for using errno values.
   Copyright (C) 1992 Free Software Foundation, Inc.
   Written by Fred Fish.  fnf@cygnus.com

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#ifndef NEED_sys_errlist
/* Note that errno.h (not sure what OS) or stdio.h (BSD 4.4, at least)
   might declare sys_errlist in a way that the compiler might consider
   incompatible with our later declaration, perhaps by using const
   attributes.  So we hide the declaration in errno.h (if any) using a
   macro. */
#define sys_errlist sys_errlist__
#endif

#include <stdio.h>
#include <errno.h>

#ifndef NEED_sys_errlist
#undef sys_errlist
#endif

/*  Routines imported from standard C runtime libraries. */

#ifdef __STDC__
#include <stddef.h>
extern void *malloc (size_t size);				/* 4.10.3.3 */
extern void *memset (void *s, int c, size_t n);			/* 4.11.6.1 */
#else	/* !__STDC__ */
#ifndef const
#define const
#endif
extern char *malloc ();		/* Standard memory allocater */
extern char *memset ();
#endif	/* __STDC__ */

#ifndef MAX
#  define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

/* Translation table for errno values.  See intro(2) in most UNIX systems
   Programmers Reference Manuals.

   Note that this table is generally only accessed when it is used at runtime
   to initialize errno name and message tables that are indexed by errno
   value.

   Not all of these errnos will exist on all systems.  This table is the only
   thing that should have to be updated as new error numbers are introduced.
   It's sort of ugly, but at least its portable. */

struct error_info
{
  int value;		/* The numeric value from <errno.h> */
  const char *name;	/* The equivalent symbolic value */
#ifdef NEED_sys_errlist
  const char *msg;	/* Short message about this value */
#endif
};

#ifdef NEED_sys_errlist
#   define ENTRY(value, name, msg)	{value, name, msg}
#else
#   define ENTRY(value, name, msg)	{value, name}
#endif

static const struct error_info error_table[] =
{
#if defined (EPERM)
  ENTRY(EPERM, "EPERM", "Not owner"),
#endif
#if defined (ENOENT)
  ENTRY(ENOENT, "ENOENT", "No such file or directory"),
#endif
#if defined (ESRCH)
  ENTRY(ESRCH, "ESRCH", "No such process"),
#endif
#if defined (EINTR)
  ENTRY(EINTR, "EINTR", "Interrupted system call"),
#endif
#if defined (EIO)
  ENTRY(EIO, "EIO", "I/O error"),
#endif
#if defined (ENXIO)
  ENTRY(ENXIO, "ENXIO", "No such device or address"),
#endif
#if defined (E2BIG)
  ENTRY(E2BIG, "E2BIG", "Arg list too long"),
#endif
#if defined (ENOEXEC)
  ENTRY(ENOEXEC, "ENOEXEC", "Exec format error"),
#endif
#if defined (EBADF)
  ENTRY(EBADF, "EBADF", "Bad file number"),
#endif
#if defined (ECHILD)
  ENTRY(ECHILD, "ECHILD", "No child processes"),
#endif
#if defined (EWOULDBLOCK)	/* Put before EAGAIN, sometimes aliased */
  ENTRY(EWOULDBLOCK, "EWOULDBLOCK", "Operation would block"),
#endif
#if defined (EAGAIN)
  ENTRY(EAGAIN, "EAGAIN", "No more processes"),
#endif
#if defined (ENOMEM)
  ENTRY(ENOMEM, "ENOMEM", "Not enough space"),
#endif
#if defined (EACCES)
  ENTRY(EACCES, "EACCES", "Permission denied"),
#endif
#if defined (EFAULT)
  ENTRY(EFAULT, "EFAULT", "Bad address"),
#endif
#if defined (ENOTBLK)
  ENTRY(ENOTBLK, "ENOTBLK", "Block device required"),
#endif
#if defined (EBUSY)
  ENTRY(EBUSY, "EBUSY", "Device busy"),
#endif
#if defined (EEXIST)
  ENTRY(EEXIST, "EEXIST", "File exists"),
#endif
#if defined (EXDEV)
  ENTRY(EXDEV, "EXDEV", "Cross-device link"),
#endif
#if defined (ENODEV)
  ENTRY(ENODEV, "ENODEV", "No such device"),
#endif
#if defined (ENOTDIR)
  ENTRY(ENOTDIR, "ENOTDIR", "Not a directory"),
#endif
#if defined (EISDIR)
  ENTRY(EISDIR, "EISDIR", "Is a directory"),
#endif
#if defined (EINVAL)
  ENTRY(EINVAL, "EINVAL", "Invalid argument"),
#endif
#if defined (ENFILE)
  ENTRY(ENFILE, "ENFILE", "File table overflow"),
#endif
#if defined (EMFILE)
  ENTRY(EMFILE, "EMFILE", "Too many open files"),
#endif
#if defined (ENOTTY)
  ENTRY(ENOTTY, "ENOTTY", "Not a typewriter"),
#endif
#if defined (ETXTBSY)
  ENTRY(ETXTBSY, "ETXTBSY", "Text file busy"),
#endif
#if defined (EFBIG)
  ENTRY(EFBIG, "EFBIG", "File too large"),
#endif
#if defined (ENOSPC)
  ENTRY(ENOSPC, "ENOSPC", "No space left on device"),
#endif
#if defined (ESPIPE)
  ENTRY(ESPIPE, "ESPIPE", "Illegal seek"),
#endif
#if defined (EROFS)
  ENTRY(EROFS, "EROFS", "Read-only file system"),
#endif
#if defined (EMLINK)
  ENTRY(EMLINK, "EMLINK", "Too many links"),
#endif
#if defined (EPIPE)
  ENTRY(EPIPE, "EPIPE", "Broken pipe"),
#endif
#if defined (EDOM)
  ENTRY(EDOM, "EDOM", "Math argument out of domain of func"),
#endif
#if defined (ERANGE)
  ENTRY(ERANGE, "ERANGE", "Math result not representable"),
#endif
#if defined (ENOMSG)
  ENTRY(ENOMSG, "ENOMSG", "No message of desired type"),
#endif
#if defined (EIDRM)
  ENTRY(EIDRM, "EIDRM", "Identifier removed"),
#endif
#if defined (ECHRNG)
  ENTRY(ECHRNG, "ECHRNG", "Channel number out of range"),
#endif
#if defined (EL2NSYNC)
  ENTRY(EL2NSYNC, "EL2NSYNC", "Level 2 not synchronized"),
#endif
#if defined (EL3HLT)
  ENTRY(EL3HLT, "EL3HLT", "Level 3 halted"),
#endif
#if defined (EL3RST)
  ENTRY(EL3RST, "EL3RST", "Level 3 reset"),
#endif
#if defined (ELNRNG)
  ENTRY(ELNRNG, "ELNRNG", "Link number out of range"),
#endif
#if defined (EUNATCH)
  ENTRY(EUNATCH, "EUNATCH", "Protocol driver not attached"),
#endif
#if defined (ENOCSI)
  ENTRY(ENOCSI, "ENOCSI", "No CSI structure available"),
#endif
#if defined (EL2HLT)
  ENTRY(EL2HLT, "EL2HLT", "Level 2 halted"),
#endif
#if defined (EDEADLK)
  ENTRY(EDEADLK, "EDEADLK", "Deadlock condition"),
#endif
#if defined (ENOLCK)
  ENTRY(ENOLCK, "ENOLCK", "No record locks available"),
#endif
#if defined (EBADE)
  ENTRY(EBADE, "EBADE", "Invalid exchange"),
#endif
#if defined (EBADR)
  ENTRY(EBADR, "EBADR", "Invalid request descriptor"),
#endif
#if defined (EXFULL)
  ENTRY(EXFULL, "EXFULL", "Exchange full"),
#endif
#if defined (ENOANO)
  ENTRY(ENOANO, "ENOANO", "No anode"),
#endif
#if defined (EBADRQC)
  ENTRY(EBADRQC, "EBADRQC", "Invalid request code"),
#endif
#if defined (EBADSLT)
  ENTRY(EBADSLT, "EBADSLT", "Invalid slot"),
#endif
#if defined (EDEADLOCK)
  ENTRY(EDEADLOCK, "EDEADLOCK", "File locking deadlock error"),
#endif
#if defined (EBFONT)
  ENTRY(EBFONT, "EBFONT", "Bad font file format"),
#endif
#if defined (ENOSTR)
  ENTRY(ENOSTR, "ENOSTR", "Device not a stream"),
#endif
#if defined (ENODATA)
  ENTRY(ENODATA, "ENODATA", "No data available"),
#endif
#if defined (ETIME)
  ENTRY(ETIME, "ETIME", "Timer expired"),
#endif
#if defined (ENOSR)
  ENTRY(ENOSR, "ENOSR", "Out of streams resources"),
#endif
#if defined (ENONET)
  ENTRY(ENONET, "ENONET", "Machine is not on the network"),
#endif
#if defined (ENOPKG)
  ENTRY(ENOPKG, "ENOPKG", "Package not installed"),
#endif
#if defined (EREMOTE)
  ENTRY(EREMOTE, "EREMOTE", "Object is remote"),
#endif
#if defined (ENOLINK)
  ENTRY(ENOLINK, "ENOLINK", "Link has been severed"),
#endif
#if defined (EADV)
  ENTRY(EADV, "EADV", "Advertise error"),
#endif
#if defined (ESRMNT)
  ENTRY(ESRMNT, "ESRMNT", "Srmount error"),
#endif
#if defined (ECOMM)
  ENTRY(ECOMM, "ECOMM", "Communication error on send"),
#endif
#if defined (EPROTO)
  ENTRY(EPROTO, "EPROTO", "Protocol error"),
#endif
#if defined (EMULTIHOP)
  ENTRY(EMULTIHOP, "EMULTIHOP", "Multihop attempted"),
#endif
#if defined (EDOTDOT)
  ENTRY(EDOTDOT, "EDOTDOT", "RFS specific error"),
#endif
#if defined (EBADMSG)
  ENTRY(EBADMSG, "EBADMSG", "Not a data message"),
#endif
#if defined (ENAMETOOLONG)
  ENTRY(ENAMETOOLONG, "ENAMETOOLONG", "File name too long"),
#endif
#if defined (EOVERFLOW)
  ENTRY(EOVERFLOW, "EOVERFLOW", "Value too large for defined data type"),
#endif
#if defined (ENOTUNIQ)
  ENTRY(ENOTUNIQ, "ENOTUNIQ", "Name not unique on network"),
#endif
#if defined (EBADFD)
  ENTRY(EBADFD, "EBADFD", "File descriptor in bad state"),
#endif
#if defined (EREMCHG)
  ENTRY(EREMCHG, "EREMCHG", "Remote address changed"),
#endif
#if defined (ELIBACC)
  ENTRY(ELIBACC, "ELIBACC", "Can not access a needed shared library"),
#endif
#if defined (ELIBBAD)
  ENTRY(ELIBBAD, "ELIBBAD", "Accessing a corrupted shared library"),
#endif
#if defined (ELIBSCN)
  ENTRY(ELIBSCN, "ELIBSCN", ".lib section in a.out corrupted"),
#endif
#if defined (ELIBMAX)
  ENTRY(ELIBMAX, "ELIBMAX", "Attempting to link in too many shared libraries"),
#endif
#if defined (ELIBEXEC)
  ENTRY(ELIBEXEC, "ELIBEXEC", "Cannot exec a shared library directly"),
#endif
#if defined (EILSEQ)
  ENTRY(EILSEQ, "EILSEQ", "Illegal byte sequence"),
#endif
#if defined (ENOSYS)
  ENTRY(ENOSYS, "ENOSYS", "Operation not applicable"),
#endif
#if defined (ELOOP)
  ENTRY(ELOOP, "ELOOP", "Too many symbolic links encountered"),
#endif
#if defined (ERESTART)
  ENTRY(ERESTART, "ERESTART", "Interrupted system call should be restarted"),
#endif
#if defined (ESTRPIPE)
  ENTRY(ESTRPIPE, "ESTRPIPE", "Streams pipe error"),
#endif
#if defined (ENOTEMPTY)
  ENTRY(ENOTEMPTY, "ENOTEMPTY", "Directory not empty"),
#endif
#if defined (EUSERS)
  ENTRY(EUSERS, "EUSERS", "Too many users"),
#endif
#if defined (ENOTSOCK)
  ENTRY(ENOTSOCK, "ENOTSOCK", "Socket operation on non-socket"),
#endif
#if defined (EDESTADDRREQ)
  ENTRY(EDESTADDRREQ, "EDESTADDRREQ", "Destination address required"),
#endif
#if defined (EMSGSIZE)
  ENTRY(EMSGSIZE, "EMSGSIZE", "Message too long"),
#endif
#if defined (EPROTOTYPE)
  ENTRY(EPROTOTYPE, "EPROTOTYPE", "Protocol wrong type for socket"),
#endif
#if defined (ENOPROTOOPT)
  ENTRY(ENOPROTOOPT, "ENOPROTOOPT", "Protocol not available"),
#endif
#if defined (EPROTONOSUPPORT)
  ENTRY(EPROTONOSUPPORT, "EPROTONOSUPPORT", "Protocol not supported"),
#endif
#if defined (ESOCKTNOSUPPORT)
  ENTRY(ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT", "Socket type not supported"),
#endif
#if defined (EOPNOTSUPP)
  ENTRY(EOPNOTSUPP, "EOPNOTSUPP", "Operation not supported on transport endpoint"),
#endif
#if defined (EPFNOSUPPORT)
  ENTRY(EPFNOSUPPORT, "EPFNOSUPPORT", "Protocol family not supported"),
#endif
#if defined (EAFNOSUPPORT)
  ENTRY(EAFNOSUPPORT, "EAFNOSUPPORT", "Address family not supported by protocol"),
#endif
#if defined (EADDRINUSE)
  ENTRY(EADDRINUSE, "EADDRINUSE", "Address already in use"),
#endif
#if defined (EADDRNOTAVAIL)
  ENTRY(EADDRNOTAVAIL, "EADDRNOTAVAIL","Cannot assign requested address"),
#endif
#if defined (ENETDOWN)
  ENTRY(ENETDOWN, "ENETDOWN", "Network is down"),
#endif
#if defined (ENETUNREACH)
  ENTRY(ENETUNREACH, "ENETUNREACH", "Network is unreachable"),
#endif
#if defined (ENETRESET)
  ENTRY(ENETRESET, "ENETRESET", "Network dropped connection because of reset"),
#endif
#if defined (ECONNABORTED)
  ENTRY(ECONNABORTED, "ECONNABORTED", "Software caused connection abort"),
#endif
#if defined (ECONNRESET)
  ENTRY(ECONNRESET, "ECONNRESET", "Connection reset by peer"),
#endif
#if defined (ENOBUFS)
  ENTRY(ENOBUFS, "ENOBUFS", "No buffer space available"),
#endif
#if defined (EISCONN)
  ENTRY(EISCONN, "EISCONN", "Transport endpoint is already connected"),
#endif
#if defined (ENOTCONN)
  ENTRY(ENOTCONN, "ENOTCONN", "Transport endpoint is not connected"),
#endif
#if defined (ESHUTDOWN)
  ENTRY(ESHUTDOWN, "ESHUTDOWN", "Cannot send after transport endpoint shutdown"),
#endif
#if defined (ETOOMANYREFS)
  ENTRY(ETOOMANYREFS, "ETOOMANYREFS", "Too many references: cannot splice"),
#endif
#if defined (ETIMEDOUT)
  ENTRY(ETIMEDOUT, "ETIMEDOUT", "Connection timed out"),
#endif
#if defined (ECONNREFUSED)
  ENTRY(ECONNREFUSED, "ECONNREFUSED", "Connection refused"),
#endif
#if defined (EHOSTDOWN)
  ENTRY(EHOSTDOWN, "EHOSTDOWN", "Host is down"),
#endif
#if defined (EHOSTUNREACH)
  ENTRY(EHOSTUNREACH, "EHOSTUNREACH", "No route to host"),
#endif
#if defined (EALREADY)
  ENTRY(EALREADY, "EALREADY", "Operation already in progress"),
#endif
#if defined (EINPROGRESS)
  ENTRY(EINPROGRESS, "EINPROGRESS", "Operation now in progress"),
#endif
#if defined (ESTALE)
  ENTRY(ESTALE, "ESTALE", "Stale NFS file handle"),
#endif
#if defined (EUCLEAN)
  ENTRY(EUCLEAN, "EUCLEAN", "Structure needs cleaning"),
#endif
#if defined (ENOTNAM)
  ENTRY(ENOTNAM, "ENOTNAM", "Not a XENIX named type file"),
#endif
#if defined (ENAVAIL)
  ENTRY(ENAVAIL, "ENAVAIL", "No XENIX semaphores available"),
#endif
#if defined (EISNAM)
  ENTRY(EISNAM, "EISNAM", "Is a named type file"),
#endif
#if defined (EREMOTEIO)
  ENTRY(EREMOTEIO, "EREMOTEIO", "Remote I/O error"),
#endif
  ENTRY(0, NULL, NULL)
};

/* Translation table allocated and initialized at runtime.  Indexed by the
   errno value to find the equivalent symbolic value. */

static const char **error_names;
static int num_error_names = 0;

/* Translation table allocated and initialized at runtime, if it does not
   already exist in the host environment.  Indexed by the errno value to find
   the descriptive string.

   We don't export it for use in other modules because even though it has the
   same name, it differs from other implementations in that it is dynamically
   initialized rather than statically initialized. */

#ifdef NEED_sys_errlist

int sys_nerr;
const char **sys_errlist;

#else

extern int sys_nerr;
extern char *sys_errlist[];

#endif


/*

NAME

	init_error_tables -- initialize the name and message tables

SYNOPSIS

	static void init_error_tables ();

DESCRIPTION

	Using the error_table, which is initialized at compile time, generate
	the error_names and the sys_errlist (if needed) tables, which are
	indexed at runtime by a specific errno value.

BUGS

	The initialization of the tables may fail under low memory conditions,
	in which case we don't do anything particularly useful, but we don't
	bomb either.  Who knows, it might succeed at a later point if we free
	some memory in the meantime.  In any case, the other routines know
	how to deal with lack of a table after trying to initialize it.  This
	may or may not be considered to be a bug, that we don't specifically
	warn about this particular failure mode.

*/

static void
init_error_tables ()
{
  const struct error_info *eip;
  int nbytes;

  /* If we haven't already scanned the error_table once to find the maximum
     errno value, then go find it now. */

  if (num_error_names == 0)
    {
      for (eip = error_table; eip -> name != NULL; eip++)
	{
	  if (eip -> value >= num_error_names)
	    {
	      num_error_names = eip -> value + 1;
	    }
	}
    }

  /* Now attempt to allocate the error_names table, zero it out, and then
     initialize it from the statically initialized error_table. */

  if (error_names == NULL)
    {
      nbytes = num_error_names * sizeof (char *);
      if ((error_names = (const char **) malloc (nbytes)) != NULL)
	{
	  memset (error_names, 0, nbytes);
	  for (eip = error_table; eip -> name != NULL; eip++)
	    {
	      error_names[eip -> value] = eip -> name;
	    }
	}
    }

#ifdef NEED_sys_errlist

  /* Now attempt to allocate the sys_errlist table, zero it out, and then
     initialize it from the statically initialized error_table. */

  if (sys_errlist == NULL)
    {
      nbytes = num_error_names * sizeof (char *);
      if ((sys_errlist = (const char **) malloc (nbytes)) != NULL)
	{
	  memset (sys_errlist, 0, nbytes);
	  sys_nerr = num_error_names;
	  for (eip = error_table; eip -> name != NULL; eip++)
	    {
	      sys_errlist[eip -> value] = eip -> msg;
	    }
	}
    }

#endif

}

/*

NAME

	errno_max -- return the max errno value

SYNOPSIS

	int errno_max ();

DESCRIPTION

	Returns the maximum errno value for which a corresponding symbolic
	name or message is available.  Note that in the case where
	we use the sys_errlist supplied by the system, it is possible for
	there to be more symbolic names than messages, or vice versa.
	In fact, the manual page for perror(3C) explicitly warns that one
	should check the size of the table (sys_nerr) before indexing it,
	since new error codes may be added to the system before they are
	added to the table.  Thus sys_nerr might be smaller than value
	implied by the largest errno value defined in <errno.h>.

	We return the maximum value that can be used to obtain a meaningful
	symbolic name or message.

*/

int
errno_max ()
{
  int maxsize;

  if (error_names == NULL)
    {
      init_error_tables ();
    }
  maxsize = MAX (sys_nerr, num_error_names);
  return (maxsize - 1);
}

#ifdef NEED_strerror

/*

NAME

	strerror -- map an error number to an error message string

SYNOPSIS

	char *strerror (int errnoval)

DESCRIPTION

	Maps an errno number to an error message string, the contents of
	which are implementation defined.  On systems which have the external
	variables sys_nerr and sys_errlist, these strings will be the same
	as the ones used by perror().

	If the supplied error number is within the valid range of indices
	for the sys_errlist, but no message is available for the particular
	error number, then returns the string "Error NUM", where NUM is the
	error number.

	If the supplied error number is not a valid index into sys_errlist,
	returns NULL.

	The returned string is only guaranteed to be valid only until the
	next call to strerror.

*/

char *
strerror (errnoval)
  int errnoval;
{
  char *msg;
  static char buf[32];

#ifdef NEED_sys_errlist

  if (error_names == NULL)
    {
      init_error_tables ();
    }

#endif

  if ((errnoval < 0) || (errnoval >= sys_nerr))
    {
      /* Out of range, just return NULL */
      msg = NULL;
    }
  else if ((sys_errlist == NULL) || (sys_errlist[errnoval] == NULL))
    {
      /* In range, but no sys_errlist or no entry at this index. */
      sprintf (buf, "Error %d", errnoval);
      msg = buf;
    }
  else
    {
      /* In range, and a valid message.  Just return the message. */
      msg = (char *) sys_errlist[errnoval];
    }
  
  return (msg);
}

#endif	/* NEED_strerror */


/*

NAME

	strerrno -- map an error number to a symbolic name string

SYNOPSIS

	const char *strerrno (int errnoval)

DESCRIPTION

	Given an error number returned from a system call (typically
	returned in errno), returns a pointer to a string containing the
	symbolic name of that error number, as found in <errno.h>.

	If the supplied error number is within the valid range of indices
	for symbolic names, but no name is available for the particular
	error number, then returns the string "Error NUM", where NUM is
	the error number.

	If the supplied error number is not within the range of valid
	indices, then returns NULL.

BUGS

	The contents of the location pointed to are only guaranteed to be
	valid until the next call to strerrno.

*/

const char *
strerrno (errnoval)
  int errnoval;
{
  const char *name;
  static char buf[32];

  if (error_names == NULL)
    {
      init_error_tables ();
    }

  if ((errnoval < 0) || (errnoval >= num_error_names))
    {
      /* Out of range, just return NULL */
      name = NULL;
    }
  else if ((error_names == NULL) || (error_names[errnoval] == NULL))
    {
      /* In range, but no error_names or no entry at this index. */
      sprintf (buf, "Error %d", errnoval);
      name = (const char *) buf;
    }
  else
    {
      /* In range, and a valid name.  Just return the name. */
      name = error_names[errnoval];
    }

  return (name);
}

/*

NAME

	strtoerrno -- map a symbolic errno name to a numeric value

SYNOPSIS

	int strtoerrno (char *name)

DESCRIPTION

	Given the symbolic name of a error number, map it to an errno value.
	If no translation is found, returns 0.

*/

int
strtoerrno (name)
     const char *name;
{
  int errnoval = 0;

  if (name != NULL)
    {
      if (error_names == NULL)
	{
	  init_error_tables ();
	}
      for (errnoval = 0; errnoval < num_error_names; errnoval++)
	{
	  if ((error_names[errnoval] != NULL) &&
	      (strcmp (name, error_names[errnoval]) == 0))
	    {
	      break;
	    }
	}
      if (errnoval == num_error_names)
	{
	  errnoval = 0;
	}
    }
  return (errnoval);
}


/* A simple little main that does nothing but print all the errno translations
   if MAIN is defined and this file is compiled and linked. */

#ifdef MAIN

#include <stdio.h>

int
main ()
{
  int errn;
  int errnmax;
  const char *name;
  char *msg;
  char *strerror ();

  errnmax = errno_max ();
  printf ("%d entries in names table.\n", num_error_names);
  printf ("%d entries in messages table.\n", sys_nerr);
  printf ("%d is max useful index.\n", errnmax);

  /* Keep printing values until we get to the end of *both* tables, not
     *either* table.  Note that knowing the maximum useful index does *not*
     relieve us of the responsibility of testing the return pointer for
     NULL. */

  for (errn = 0; errn <= errnmax; errn++)
    {
      name = strerrno (errn);
      name = (name == NULL) ? "<NULL>" : name;
      msg = strerror (errn);
      msg = (msg == NULL) ? "<NULL>" : msg;
      printf ("%-4d%-18s%s\n", errn, name, msg);
    }

  return 0;
}

#endif

#endif

/* Cursor-spinning stuff that includes metering of spin rate and delays.  */

/* Nonzero when cursor spinning has been set up properly.  */

int cursor_inited;

/* Nonzero if spin should be measured and excessive delays reported.  */

int measure_spin;

/* Nonzero if spin histogram and rate data should be written out.  */

int dump_spin_data;

long warning_threshold = 400000;

long bucket_size = 1024;

long bucket_power = 10;

long numbuckets = 300;

int *delay_counts;

int overflow_count;

char *current_progress;

#ifdef MPW

/* Note the MPW-specific syntax here.  */

/* Also note that we're only collecting the low half of a 64-bit
   value, the other half being in A0.  This means that every 20
   minutes or so, a difference with the last value from here will
   be spectacularly wrong.  */

long Microseconds () = 0xa193;

#endif

static long last_microseconds;

static char *last_spin_file = "";

static int last_spin_line;

void
warn_if_spin_delay (char *file, int line)
{
  long diff, ix;
  long now = Microseconds();

  diff = now - last_microseconds;

  if (diff > warning_threshold)
    fprintf (stderr, "# %s: %ld.%06ld sec delay getting from %s:%d to %s:%d\n",
	     (current_progress ? current_progress : ""),
	     diff / 1000000, diff % 1000000,
	     last_spin_file, last_spin_line, file, line);
  if (dump_spin_data)
    {
      if (diff >= 0)
	{
	  ix = diff >> bucket_power;
	  if (ix >= 0 && ix < numbuckets && delay_counts != NULL)
	    ++delay_counts[ix];
	  else
	    ++overflow_count;
	}
      else
	fprintf (stderr, "raw diff is %ld (?)\n", diff);
    }
}

void
record_for_spin_delay (char *file, int line)
{
  last_microseconds = Microseconds();
  last_spin_file = file;
  last_spin_line = line;
}

void
mpw_start_progress (char *str, int n, char *file, int line)
{
  int i;
  char *measure, *threshold;

  if (!cursor_inited)
    {
      InitCursorCtl (nil);
      cursor_inited = 1;
      record_for_spin_delay (file, line);
      measure = getenv ("MEASURE_SPIN");
      if (measure != NULL && measure[0] != '\0')
	{
	  measure_spin = 1;
	  if (strcmp (measure, "all") == 0)
	    dump_spin_data = 1;
	}
      threshold = getenv ("SPIN_WARN_THRESHOLD");
      if (threshold != NULL && threshold[0] != '\0')
	warning_threshold = atol (threshold);
      if (dump_spin_data)
	{
	  if (delay_counts == NULL)
	    delay_counts = (int *) malloc (numbuckets * sizeof (int));
	  for (i = 0; i < numbuckets; ++i)
	    delay_counts[i] = 0;
	  overflow_count = 0;
	}
    }
  current_progress = str;

  mpw_special_init (str);
}

void
mpw_progress (int n)
{
  SpinCursor (32);
}

void
mpw_progress_measured (int n, char *file, int line)
{
  if (measure_spin)
    warn_if_spin_delay (file, line);
  SpinCursor (32);
  if (measure_spin)
    record_for_spin_delay (file, line);
}

void
mpw_end_progress (char *str, char *file, int line)
{
  long i, delay, count = 0, sum = 0, avgdelay, spinrate;
  long curpower = 0, curgroup = 0;

  /* Warn if it's been a while since the last spin.  */
  if (measure_spin)
    warn_if_spin_delay (file, line);

  /* Dump all the nonzero delay counts and an approximation of the delay.  */
  if (dump_spin_data && delay_counts != NULL)
    {
      for (i = 0; i < numbuckets; ++i)
	{
	  delay = (i + 1) * bucket_size;
	  sum += delay_counts[i] * (i + 1);
	  count += delay_counts[i];
	  if (delay <= (1 << curpower))
	    {
	      curgroup += delay_counts[i];
	    }
	  else
	    {
	      if (curgroup > 0)
		fprintf (stderr,
			 "# %s: %d delays between %ld.%06ld and %ld.%06ld sec\n",
			 (str ? str : ""),
			 curgroup,
			 (1 << curpower) / 1000000,
			 (1 << curpower) % 1000000,
			 (1 << (curpower + 1)) / 1000000,
			 (1 << (curpower + 1)) % 1000000);
	      ++curpower;
	      curgroup = 0;
	    }
	}
      if (count > 0)
	{
	  avgdelay = (sum * bucket_size) / count;
	  spinrate = 1000000 / avgdelay;
	  fprintf (stderr, "# %s: Average spin rate is %d times/sec\n",
		   (str ? str : ""), spinrate);
	}
    }
}

#ifdef PROGRESS_TEST

/* Test program.  */

main ()
{
  int i, j;
  double x = 1.0, y = 2.4;
  long start = Microseconds (), tm;

  START_PROGRESS ("hi", 0);

  for (i = 0; i < 1000; ++i)
    {
      PROGRESS (1);

      for (j = 0; j < (i * 100); ++j)
	{
	  x += (x * y) / j;
	}
    }
  
  END_PROGRESS ("hi");
  
  tm = Microseconds () - start;

  printf ("Total time is %d.%d secs\n", tm / 1000000, tm % 1000000);
}

#endif
