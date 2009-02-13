// $Id$ -*- C++ -*-
// An agent interface using ptys (pseudo ttys)

// Copyright (C) 1999 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001, 2003 Free Software Foundation, Inc.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char TTYAgent_rcsid[] = 
    "$Id$";

// Most of this code is based on `calldbx.c' from `xxgdb', by Pierre
// Willard which in turn is based on `calldbx.c' from `xdbx', by Po
// Cheung.  The open_master() and open_slave() routines are based on
// the pty routines from the GNU `expect' library.
//
// I have not been able to test all possible configurations and
// architectures.  If someone finds problems in this code, please
// enlighten me.  I also apologize for the mess of `#ifdef's in this
// file; please tell me about superfluous ones if you find some.
//
//                                                 (Andreas Zeller)


// Options

// If 1, synchronize parent and child by sending an initialization sequence
#define SYNCHRONIZE_PARENT_AND_CHILD 0


#include "TTYAgent.h"
#include "config.h"

DEFINE_TYPE_INFO_1(TTYAgent, LiterateAgent)

extern "C" {

#if HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#if sun
// On SunOS 4.1.4, <sys/ioctl.h> defines lots of symbols which are
// redefined in <termios.h>.  We want the <termios.h> ones.
#if defined(ECHO) && defined(O_ECHO) && ECHO == O_ECHO
#undef ECHO
#endif

#if defined(NL0) && defined(O_NL0) && NL0 == O_NL0
#undef NL0
#endif

#if defined(NL1) && defined(O_NL1) && NL1 == O_NL1
#undef NL1
#endif

#if defined(TAB0) && defined(O_TAB0) && TAB0 == O_TAB0
#undef TAB0
#endif

#if defined(TAB1) && defined(O_TAB1) && TAB1 == O_TAB1
#undef TAB1
#endif

#if defined(TAB2) && defined(O_TAB2) && TAB2 == O_TAB2
#undef TAB2
#endif

#if defined(XTABS) && defined(O_XTABS) && XTABS == O_XTABS
#undef XTABS
#endif

#if defined(CR0) && defined(O_CR0) && CR0 == O_CR0
#undef CR0
#endif

#if defined(CR1) && defined(O_CR1) && CR1 == O_CR1
#undef CR1
#endif

#if defined(CR2) && defined(O_CR2) && CR2 == O_CR2
#undef CR2
#endif

#if defined(CR3) && defined(O_CR3) && CR3 == O_CR3
#undef CR3
#endif

#if defined(FF0) && defined(O_FF0) && FF0 == O_FF0
#undef FF0
#endif

#if defined(FF1) && defined(O_FF1) && FF1 == O_FF1
#undef FF1
#endif

#if defined(BS0) && defined(O_BS0) && BS0 == O_BS0
#undef BS0
#endif

#if defined(BS1) && defined(O_BS1) && BS1 == O_BS1
#undef BS1
#endif

#if defined(TOSTOP) && defined(O_TOSTOP) && TOSTOP == O_TOSTOP
#undef TOSTOP
#endif

#if defined(FLUSHO) && defined(O_FLUSHO) && FLUSHO == O_FLUSHO
#undef FLUSHO
#endif

#if defined(PENDIN) && defined(O_PENDIN) && PENDIN == O_PENDIN
#undef PENDIN
#endif

#if defined(NOFLSH) && defined(O_NOFLSH) && NOFLSH == O_NOFLSH
#undef NOFLSH
#endif
#endif // sun

// Nico van Waes <nico@yegal.njit.edu> says: under Solaris 2.6, one
// must include <sys/types.h> and <sys/int_types.h> before
// <termios.h>.
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if HAVE_SYS_INT_TYPES_H
#include <sys/int_types.h>
#endif

#ifdef __osf__
// DEC OSF has some special treatment in this file; I hope these
// `#ifdef __osf__' flags will be deleted by an OSF expert some day. - AZ
#include <termio.h>
#else // !__osf__
#if HAVE_TCGETATTR && HAVE_TCSETATTR
#if HAVE_TERMIOS_H
#include <termios.h>
#endif
#else // !HAVE_TCGETATTR || !HAVE_TCSETATTR
#if HAVE_TERMIO_H
#include <termio.h>
#endif
#endif // !HAVE_TCGETATTR || !HAVE_TCSETATTR
#endif // !__osf__

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#ifndef S_ISDIR
#define S_ISDIR ((m) & S_IFMT == S_IFDIR)
#endif
#endif

#if HAVE_SYS_STROPTS_H
#include <sys/stropts.h>
#endif

#if HAVE_STROPTS_H
#include <stropts.h>
#endif

#if HAVE_SYS_SYSMACROS_H
#include <sys/sysmacros.h>
#ifndef minor
#define minor(x)        ((int)((x)&0377))
#endif
#endif

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if HAVE_SYS_VTY_H
#include <sys/vty.h>

#ifndef MAXPTYNAMELEN
#define MAXPTYNAMELEN 255
#endif

#endif
}


#include <stdio.h>
#include <errno.h>
#include <signal.h>


extern "C" {
#if HAVE_SETPGID && !HAVE_SETPGID_DECL && !defined(setpgid)
    int setpgid(pid_t pid, pid_t pgrp);
#endif
#if !HAVE_SETPGID && HAVE_SETPGRP2 && !defined(setpgid)
#if !HAVE_SETPGRP2_DECL && !defined(setpgrp2)
    int setpgrp2(pid_t pid, pid_t pgrp);
#define setpgid setpgrp2
#define HAVE_SETPGID 1
#endif
#endif
#if !HAVE_SETPGID && HAVE_SETPGRP && !defined(setpgid)
#if !HAVE_SETPGRP_DECL && !defined(setpgrp)
#if HAVE_SETPRGP_VOID
    int setpgrp();
#else
    int setpgrp(pid_t pid, pid_t pgrp);
#endif // HAVE_SETPRGP_VOID
#define setpgid setpgrp
#define HAVE_SETPGID 1
#endif
#endif
#if HAVE_TCGETATTR && !HAVE_TCGETATTR_DECL && !defined(tcgetattr)
    int tcgetattr(int fd, struct termios *termios_p);
#endif
#if HAVE_TCSETATTR && !HAVE_TCSETATTR_DECL && !defined(tcsetattr)
    int tcsetattr(int fd, int when, const struct termios *termios_p);
#endif
#if HAVE_TCGETPGRP && !HAVE_TCGETPGRP_DECL && !defined(tcgetpgrp)
    pid_t tcgetpgrp(int fd);
#endif
#if HAVE_TCSETPGRP && !HAVE_TCSETPGRP_DECL && !defined(tcsetpgrp)
    int tcsetpgrp(int fd, pid_t pgid);
#endif
#if HAVE_IOCTL && !HAVE_IOCTL_DECL && !defined(ioctl)
#if defined(__GLIBC__) && \
    (__GLIBC__ >= 3 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 1))
    // Christian Meder <meder@isr.uni-stuttgart.de> says there is an
    // ioctl() decl in glibc 2.1 and later.
#else
    int ioctl(int fd, int request, ...);
#endif // __GLIBC__
#endif // HAVE_IOCTL
#if HAVE_FCNTL && !HAVE_FCNTL_DECL && !defined(fcntl)
    int fcntl(int fd, int command, ...);
#endif
#if HAVE_GETPTY && !HAVE_GETPTY_DECL && !defined(getpty)
    int getpty(char *line, char *sline, int mode);
#endif
#if HAVE__GETPTY && !HAVE__GETPTY_DECL && !defined(_getpty)
    char *_getpty(int *fildes, int oflag, mode_t mode, int nofork);
#endif
#if HAVE_SETSID && !HAVE_SETSID_DECL && !defined(setsid)
    pid_t setsid(void);
#endif
#if !HAVE_SETSID && HAVE_SETPGRP && defined(SETPGRP_VOID)
#define setsid() setpgrp()
#define HAVE_SETSID 1
#endif
}

// getpt() should be defined in <stdlib.h> if we define _GNU_SOURCE
#if HAVE_GETPT && !HAVE_GETPT_DECL && !defined(getpt)
extern "C" int getpt();
#endif

// Streams won't work on DEC OSF because there isn't any "ttcompat"
// module and I don't know enough about any of this stuff to try to
// figure it out now.  -- phil_brooks@MENTORG.COM (Phil Brooks)
#if !defined(__osf__) && \
    HAVE_PTSNAME && HAVE_GRANTPT && HAVE_UNLOCKPT && HAVE_IOCTL
#define HAVE_UNIX98PTYS 1
#define HAVE_STREAMS_IMPLEMENTATION 1
// FIXME: We should have a proper test for this.
// For instance, GNU libc 2.1 on Linux (and probably HURD too) has UNIX98-style
// pty allocation functions, but does not have a STREAMS-based implementation.
// -- Ray Dassen // <jdassen@wi.LeidenUniv.nl>
#endif // !defined(__osf__) && HAVE_PTSNAME && ...

// Provide C++ declarations
#if HAVE_PTSNAME && !HAVE_PTSNAME_DECL && !defined(ptsname)
extern "C" char *ptsname(int master);
#endif
#if HAVE_GRANTPT && !HAVE_GRANTPT_DECL && !defined(grantpt)
extern "C" int grantpt(int fd);
#endif
#if HAVE_UNLOCKPT && !HAVE_UNLOCKPT_DECL && !defined(unlockpt)
extern "C" int unlockpt(int fd);
#endif

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

#ifndef R_OK
/* 3b2 doesn't define these according to jthomas@nmsu.edu. */
#define R_OK 04
#define W_OK 02
#endif

#if !defined(O_NONBLOCK) && defined(O_NDELAY)
#define O_NONBLOCK O_NDELAY
#endif

#if !defined(O_NONBLOCK) && defined(FNDELAY)
#define O_NONBLOCK FNDELAY
#endif

#if !defined(O_APPEND) && defined(FAPPEND)
#define O_APPEND FAPPEND
#endif

// Open a tty.
// Like open(TTY, FLAGS), but care for EAGAIN and EWOULDBLOCK conditions
int TTYAgent::open_tty(const char *tty, int flags)
{
    int fd = open(tty, flags);
    if (fd >= 0)
	return fd;

    if (false
#ifdef EAGAIN
	|| errno == EAGAIN
#endif
#ifdef EWOULDBLOCK
	|| errno == EWOULDBLOCK
#endif
	)
    {
	// Resource temporarily unavailable: an operation that would
	// block was attempted on an object that has non-blocking mode
	// selected.  Trying the same operation again will block until
	// some external condition makes it possible to read, write,
	// or connect (whatever the operation).  So, we just try again.
	_raiseIOWarning(string(tty) + " is temporary unavailable");
	fd = open(tty, flags);
    }

    return fd;
}

// Check whether a TTY is usable
bool TTYAgent::tty_ok(const char *tty)
{
    if (access(tty, R_OK | W_OK) != 0)
	return false;		// No access

    int fd = open_tty(tty);
    if (fd < 0)
	return false;		// Cannot open

    close(fd);
    return true;
}


// Open master side of pty.
// Depending on the host features, we try a large variety of possibilities.
void TTYAgent::open_master()
{
    char *line;
    struct stat sb;

    // Setup defaults
    push = false;

#if HAVE_GETPTY
    // getpty() - an UTS feature
    static char master_line[MAXPTYNAMELEN];
    static char slave_line[MAXPTYNAMELEN];
    master = getpty(master_line, slave_line, O_RDWR);
    if (master >= 0)
    {
	// Verify slave side is usable
	if (tty_ok(slave_line))
	{
	    _master_tty  = master_line;
	    _slave_tty   = slave_line;
	    return;
	}

	close(master);
    }
#endif

#if HAVE__GETPTY
    // _getpty() - an SGI/IRIX feature
    //
    // Ferdinand Contreras <fcontre@clear.co.nz> reports: The pty man
    // page says to set the O_NDELAY bit in the call to _getpty().
    line = _getpty(&master, O_RDWR | O_NDELAY, 0600, 0);
    if (line != 0 && master >= 0)
    {
	// Verify slave side is usable
	//
	// We don't use TTY_OK() here - it seems this breaks SGI TTY setup.
	// The error messages generated are as follows:
	// 
	// dbx: cannot open /dev/ttyq<n>: I/O error
	// dbx: child communication setup failed
	//
	// Ferdinand Contreras <fcontre@clear.co.nz> says:
	//
	// I've traced the execution and it seems that the slave tty
	// doesn't like to be opened twice.  That is, the tty_ok(line)
	// function call opens the tty and closes it after
	// successfully opening it.  However, the next time it is
	// opened via open_tty() in the function open_slave(), it gets
	// a return code of EIO (errno=5). I've decided to just skip
	// the call to tty_ok() since the slave tty will eventually be
	// opened by open_slave().  Any errors would still be caught
	// at that point.
	if (access(line, R_OK | W_OK) == 0)
	{
	    char *t = ttyname(master);
	    if (t)
		_master_tty = t;
	    _slave_tty   = line;
	    return;
	}
	close(master);
    }
#endif

    if (stat("/dev/ptc", &sb) == 0)
    {
	// Try /dev/ptc - an AIX and SGI3 feature
	master = open_tty("/dev/ptc");
	if (master >= 0)
	{
	    line = ttyname(master);
	    if (line != 0)
	    {
		if (tty_ok(line))
		{
#if defined(SGI3) || defined(sgi3)
		    int ptynum = minor(sb.st_rdev);
		    char buffer[BUFSIZ];
		    sprintf(buffer, "/dev/ttyq%d", ptynum);
		    if (tty_ok(buffer))
		    {
			_master_tty = line;
			_slave_tty  = buffer;
			return;
		    }
#else
		    _master_tty = line;
		    _slave_tty  = line;
		    return;
#endif
		}
	    }
	    close(master);
	}
    }


#if HAVE_UNIX98PTYS
    // pty handling as described in "The Single UNIX Specification, Version 2"
    // ("UNIX98"). Based on SVR4, and thus possibly streams based.
    master = -1;

#if HAVE_GETPT
    // On systems with GNU libc 2.1, getpt() returns a new file
    // descriptor for the next available master pseudo-terminal.  This
    // function is a GNU extension.
    // The GNU libc documentation explicitly mentions that its ptsname() result
    // may be STREAMS-based (even if obtained via a getpt() result).
    master = getpt();
    if (master < 0)
	_raiseIOMsg("getpt");
#endif

    if (master < 0 && stat("/dev/ptmx", &sb) == 0)
    {
	// On other systems, we try /dev/ptmx - a SVR4 feature
	master = open_tty("/dev/ptmx");
	if (master < 0)
	    _raiseIOMsg("cannot open /dev/ptmx");
    }

    if (master >= 0)
    {
	// Finish setup
	line = ptsname(master);
	if (line == 0)
	    _raiseIOMsg("ptsname");
	else if (grantpt(master) < 0)
	    _raiseIOMsg("grantpt " + string(line));
	else if (unlockpt(master) < 0)
	    _raiseIOMsg("unlockpt " + string(line));
	else if (!tty_ok(line))
	    _raiseIOMsg("access " + string(line));
	else
	{
	    // Everything ok - proceed
	    char *t = ttyname(master);
	    if (t)
		_master_tty = t;
	    _slave_tty  = line;
#ifdef TIOCFLUSH
	    ioctl(master, TIOCFLUSH, (char *)0);
#endif
#if HAVE_STREAMS_IMPLEMENTATION
	    push = true;
#endif
	    return;
	}

	close(master);
    }
#endif // HAVE_UNIX98PTYS

    // Try PTY's
    if (stat("/dev/pty/000", &sb) == 0)
    {
	// Try PTY's in /dev/pty/XXX and /dev/ttypXXX -- a UNICOS feature
#if defined(CRAY) && defined(_SC_CRAY_NPTY)
	int highpty = sysconf(_SC_CRAY_NPTY);
#else
	int highpty = 128;
#endif
	for (int i = 0; i < highpty; i++)
	{
	    char nr[32];
	    sprintf(nr, "%03d", i);
	    const string pty = string("/dev/pty/") + nr;
	    const string tty = string("/dev/ttyp") + nr;
		
	    master = open_tty(pty.chars());
	    if (master >= 0)
	    {
		// Verify slave tty is usable
		if (!tty_ok(tty.chars()))
		{
		    close(master);
		    continue;
		}

		_master_tty = pty;
		_slave_tty  = tty;
		return;
	    }
	}
    }

    const string p1 = "pqrstuvwxyzPQRSTUVWXYZ";
    const string p2 = "0123456789abcdefghijklmnopqrstuvwxyz";

    if (stat("/dev/ptym", &sb) == 0 && S_ISDIR(sb.st_mode))
    {
	// Try PTY's in /dev/ptym/ptyXX and /dev/pty/ttyXX -- a HP-UX feature
	for (int i = 0; i < int(p1.length()); i++)
	    for (int j = 0; j < int(p2.length()); j++)
	    {
		const string nr  = string(p1[i]) + p2[j];
		const string pty = "/dev/ptym/pty" + nr;
		const string tty = "/dev/pty/tty" + nr;
		
		master = open_tty(pty.chars());
		if (master >= 0)
		{
		    // Verify slave tty is usable
		    if (!tty_ok(tty.chars()))
		    {
			close(master);
			continue;
		    }

		    _master_tty = pty;
		    _slave_tty  = tty;
		    return;
		}
	    }
    }

    // Try PTY's in /dev/ptyXX -- a BSD and USG feature
    // Slackware 3.0 wants [/zip]/dev/ptyXX, as
    // Jim Van Zandt <jrv@vanzandt.mv.com> suggests.
    for (int k = 0; k < 2; k++)
    {
	const string prefix = (k == 0 ? "" : "/zip");

	for (int i = 0; i < int(p1.length()); i++)
	{
	    for (int j = 0; j < int(p2.length()); j++)
	    {
		const string nr  = string(p1[i]) + p2[j];
		const string pty = prefix + "/dev/pty" + nr;
		const string tty = prefix + "/dev/tty" + nr;

		master = open_tty(pty.chars());
		if (master >= 0)
		{
		    // Verify slave tty is usable
		    if (!tty_ok(tty.chars()))
		    {
			close(master);
			continue;
		    }

		    _master_tty = pty;
		    _slave_tty  = tty;
		    return;
		}
	    }
	}
    }

    _raiseIOMsg("cannot open master pty");
    return;
}


// Open slave side of the pty.  The slave pty name has been set
// in slave_tty by open_master.
void TTYAgent::open_slave()
{
    slave = open_tty(slave_tty_c());
    if (slave < 0)
    {
	_raiseIOMsg("cannot open " + slave_tty());
	return;
    }

#if HAVE_IOCTL && defined(TIOCSCTTY)
    if (!push)
    {
	// This is required on some boxes (notably DEC OSF and
	// FreeBSD); if it fails, don't complain.
	if (ioctl(slave, TIOCSCTTY, 1) < 0)
	{
	    // _raiseIOWarning("cannot allocate controlling terminal");
	}
    }
#endif

#if HAVE_STREAMS_IMPLEMENTATION && defined(I_PUSH)
    if (push)
    {
	if (isastream(slave)) {
	    // FIXME: the test around this block should not be necessary, but
	    // we don't have a proper test yet to see if the pty implementation
	    // is STREAMS-based. - Ray

	    // Finish STREAMS setup by pushing TTY compatibility modules.
	    // These calls may fail if the modules do not exist.  For
	    // instance, HP-UX has no `ttcompat' module; Linux has no
	    // modules at all.  To avoid confusion, we do not give a
	    // warning if these calls fail due to invalid module names.

	    if (ioctl(slave, I_PUSH, "ptem") < 0 && errno != EINVAL)
		_raiseIOWarning("ioctl ptem " + slave_tty());
	    
	    if (ioctl(slave, I_PUSH, "ldterm") < 0 && errno != EINVAL)
		_raiseIOWarning("ioctl ldterm " + slave_tty());

	    if (ioctl(slave, I_PUSH, "ttcompat") < 0 && errno != EINVAL)
		_raiseIOWarning("ioctl ttcompat " + slave_tty());
	}
    }
#endif // HAVE_STREAMS_IMPLEMENTATION && defined(I_PUSH)

    return;
}


int TTYAgent::setupCommunication()
{
    open_master();
    if (master < 0)
	return -1;

    return 0;
}

#if SYNCHRONIZE_PARENT_AND_CHILD
static const char const TTY_INIT[] = { 'A', '\n' };
#endif


int TTYAgent::setupParentCommunication()
{
#if SYNCHRONIZE_PARENT_AND_CHILD
    // Read initialization sequence from TTY (synchronizing)
    char buf[BUFSIZ];
    int ret = ::read(master, buf, sizeof(TTY_INIT));
    if (ret <= 0)
	raiseIOMsg("cannot read initialization sequence from child");
#endif


#if HAVE_FCNTL && defined(O_NONBLOCK)
    // Set the child file descriptor to nonblocking mode
    int flags = fcntl(master, F_GETFL, 0);
    if (flags == -1)
    {
	_raiseIOWarning("cannot get file descriptor status flags");
    }
    else
    {
	if (fcntl(master, F_SETFL, flags | O_NONBLOCK) == -1)
	    _raiseIOWarning("cannot set file to non-blocking mode");
    }
#endif
	
    // Open file pointer with read/write access to child process
    _inputfp = fdopen(master, "r+");
    if (inputfp() == 0)
    {
	raiseIOMsg("cannot associate input stream with pty master");
	terminate();
	return -1;
    }

    _outputfp = _inputfp;
    _errorfp  = 0;

#if HAVE_SETBUF
    // Set unbuffered mode
    setbuf(_outputfp, (char *)0);
#elif HAVE_SETVBUF && defined(_IONBF)
    // According to lee@champion.tcs.co.jp (Lee Hounshell), this
    // won't work on Linux ELF systems:
    setvbuf(_outputfp, (char *)0, _IONBF, BUFSIZ);
#endif

#if SYNCHRONIZE_PARENT_AND_CHILD
    // Echo initialization sequence again
    ::write(master, TTY_INIT, sizeof(TTY_INIT));
#endif

    return 0;
}


int TTYAgent::setupChildCommunication()
{
    // Close master side of pty
    close(master);

    int result = 0;
    pid_t pid = getpid();

    // Make child a process leader:
    // Set the process group of the pty and of us to our process id.

#if HAVE_SETSID || defined(setsid)
    result = int(setsid());
#else // !HAVE_SETSID

    // Clear controlling tty.  This means that we will not have a
    // controlling tty until we open another terminal device.
#if HAVE_IOCTL && defined(TIOCNOTTY)
    int fd;

#if HAVE_TCGETSID && HAVE_TCGETPGRP
    if ((tcgetsid(STDIN_FILENO) != tcgetpgrp(STDIN_FILENO)) &&
	(fd = open_tty("/dev/tty", O_RDWR | O_NONBLOCK)) > 0)
#else  // !HAVE_TCGETSID
    if ((fd = open_tty("/dev/tty")) > 0)
#endif // !HAVE_TCGETSID
    {
  	result = ioctl(fd, TIOCNOTTY, 0);
  	close(fd);
    }
    if (result < 0)
	_raiseIOMsg("cannot clear controlling tty");
#endif // HAVE_IOCTL && defined(TIOCNOTTY)

    // Create a new process group.
#if HAVE_SETPGID
    result = setpgid(pid, pid);
#endif // HAVE_SETPGID
#endif // !HAVE_SETSID

    if (result < 0)
	_raiseIOMsg("cannot create new process group");

    if (slave < 0)
    {
	// Open slave such that it becomes the controlling terminal.
	open_slave();
    }

    if (slave < 0)
	return -1;


    // Make this process the foreground process in the slave pty.
    result = 0;
#if HAVE_TCSETPGRP
    result = tcsetpgrp(slave, pid);
#elif HAVE_IOCTL && defined(TIOCSPGRP)
    result = ioctl(slave, TIOCSPGRP, &pid);
#endif

    if (result < 0)
	_raiseIOMsg("cannot set terminal foreground process group");

    // Modify local and output mode of slave pty
#if HAVE_TCGETATTR && HAVE_TCSETATTR
    // Method 1.  Use termios, tcgetattr(), and tcsetattr().
    struct termios settings;
    result = tcgetattr(slave, &settings);
    if (result < 0)
	_raiseIOMsg("cannot get slave terminal settings");
    else
    {
#ifdef ICANON
	settings.c_lflag |= ICANON;     // Canonical mode
#endif
#ifdef ECHO
	settings.c_lflag &= ~ECHO;      // No echo
#endif
#ifdef ECHONL
	settings.c_lflag &= ~ECHONL;    // No echo of newlines
#endif
#ifdef ISIG
	settings.c_lflag |= ISIG;       // Enable signal characters
#endif
#ifdef TOSTOP
	settings.c_lflag &= ~TOSTOP;    // No interrupt on background processes
#endif
#ifdef NOFLSH
	settings.c_lflag &= ~NOFLSH;    // Clear queues upon interrupt
#endif
#ifdef OPOST
	settings.c_oflag &= ~OPOST;     // Do not process output data
#endif
#ifdef ONLCR
	settings.c_oflag &= ~ONLCR;     // Do not map NL to CR-NL on output
#endif
#ifdef VDSUSP
	settings.c_cc[VSUSP] = '\031';  // Set DELAYED SUSPEND to `^Y'
#endif
#ifdef VEOF
	settings.c_cc[VEOF] = '\004';   // Set EOF character to `^D'
#endif
#ifdef VERASE
	settings.c_cc[VERASE] = '\010'; // Set ERASE character to `^H'
#endif
#ifdef VINTR
	settings.c_cc[VINTR] = '\003';  // Set INTERRUPT to `^C'
#endif
#ifdef VKILL
	settings.c_cc[VKILL] = '\025';  // Set KILL character to `^U'
#endif
#ifdef VQUIT
	settings.c_cc[VQUIT] = '\034';  // Set QUIT to `^\'
#endif
#ifdef VSUSP
	settings.c_cc[VSUSP] = '\032';  // Set SUSPEND to `^Z'
#endif
#ifdef VREPRINT
	settings.c_cc[VREPRINT] = '\022'; // Set REPRINT character to `^R'
#endif
#ifdef VWERASE
	settings.c_cc[VWERASE] = '\027'; // Set WERASE character to `^W'
#endif
	result = tcsetattr(slave, TCSANOW, &settings);
	if (result < 0)
	    _raiseIOMsg("cannot set slave terminal settings");
    }
#elif HAVE_IOCTL && defined(TIOCGETP) && defined(TIOCSETP)
    // Method 2.  Use sgttyb, ioctl(TIOCGETP), and ioctl(TIOCSETP).
    struct sgttyb settings;
    result = ioctl(slave, TIOCGETP, &settings);
    if (result < 0)
	_raiseIOMsg("cannot get slave terminal settings");
    else
    {
#ifdef ECHO
	settings.sg_flags &= ~ECHO;	// No echo
#endif
#ifdef ISIG
	settings.sg_flags |= ISIG;      // Enable signals
#endif
#ifdef CRMOD
	settings.sg_flags &= ~CRMOD;	// Do not map NL to CR-NL on output
#endif
	result = ioctl(slave, TIOCSETP, &settings);
	if (result < 0)
	    _raiseIOMsg("cannot set slave terminal settings");
    }
#elif HAVE_IOCTL && defined(TCGETA) && defined(TCSETA)
    // Method 3.  Use termio, ioctl(TCGETA), and ioctl(TCSETA).
    struct termio settings;
    result = ioctl(slave, TCGETA, &settings);
    if (result < 0)
	_raiseIOMsg("cannot get slave terminal settings");
    else
    {
#ifdef ICANON
	settings.c_lflag |= ICANON;     // Canonical mode
#endif
#ifdef ECHO
	settings.c_lflag &= ~ECHO;      // No echo
#endif
#ifdef ISIG
	settings.c_lflag |= ISIG;       // Enable signal characters
#endif
#ifdef OPOST
	settings.c_oflag &= ~OPOST;     // Do not process output data
#endif
#ifdef ONLCR
	settings.c_oflag &= ~ONLCR;	// Do not map NL to CR-NL on output
#endif
#ifdef VDSUSP
	settings.c_cc[VSUSP] = '\031';  // Set DELAYED SUSPEND to `^Y'
#endif
#ifdef VEOF
	settings.c_cc[VEOF] = '\004';   // Set EOF character to `^D'
#endif
#ifdef VERASE
	settings.c_cc[VERASE] = '\010'; // Set ERASE character to `^H'
#endif
#ifdef VINTR
	settings.c_cc[VINTR] = '\003';  // Set INTERRUPT to `^C'
#endif
#ifdef VKILL
	settings.c_cc[VKILL] = '\025';  // Set KILL character to `^U'
#endif
#ifdef VQUIT
	settings.c_cc[VQUIT] = '\034';  // Set QUIT to `^\'
#endif
#ifdef VSUSP
	settings.c_cc[VSUSP] = '\032';  // Set SUSPEND to `^Z'
#endif
#ifdef VREPRINT
	settings.c_cc[VREPRINT] = '\022'; // Set REPRINT character to `^R'
#endif
#ifdef VWERASE
	settings.c_cc[VWERASE] = '\027'; // Set WERASE character to `^W'
#endif
	result = ioctl(slave, TCSETA, &settings);
	if (result < 0)
	    _raiseIOMsg("cannot set slave terminal settings");
    }
#else // !HAVE_TCGETATTR && !HAVE_IOCTL
    // No method left.  There is probably something wrong with
    // config.h -- HAVE_IOCTL may be missing, for instance.
    // We actually might compile and run here, but without
    // being able to reset echo mode, things will be real bad.
#error no way to set child terminal mode -- please check the settings of
#error HAVE_TCSETATTR, HAVE_IOCTL, HAVE_TERMIOS_H, HAVE_TERMIO_H in config.h
#endif // !HAVE_IOCTL

    // Redirect stdin, stdout, stderr of child to pty
    if (dup2(slave, STDIN_FILENO) < 0)
	_raiseIOMsg("cannot redirect standard input to slave pty");
    if (dup2(slave, STDOUT_FILENO) < 0)
	_raiseIOMsg("cannot redirect standard output to slave pty");
    if (dup2(slave, STDERR_FILENO) < 0)
	_raiseIOMsg("cannot redirect standard error to slave pty");

    if (slave > STDERR_FILENO)
	close(slave);

    // Unbuffer output data from child
    fcntl(STDOUT_FILENO, F_SETFL, O_APPEND);
    setbuf(stdout, (char *)0);

#if SYNCHRONIZE_PARENT_AND_CHILD
    // Send an initialization sequence ...
    ::write(STDOUT_FILENO, TTY_INIT, sizeof(TTY_INIT));

    // ... and read it back again
    char buf[BUFSIZ];
    int ret = ::read(STDIN_FILENO, buf, sizeof(TTY_INIT));
    if (ret <= 0)
	raiseIOMsg("cannot read initialization sequence from parent");
#endif

    return 0;
}
