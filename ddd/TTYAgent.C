// $Id$ -*- C++ -*-
// An agent interface using ptys (pseudo ttys)

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

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


#ifdef __GNUG__
#pragma implementation
#endif

// Options

// If 1, synchronize parent and child by sending an initialization sequence
#define SYNCHRONIZE_PARENT_AND_CHILD 0


#include "TTYAgent.h"
#include "config.h"

DEFINE_TYPE_INFO_1(TTYAgent, LiterateAgent)

extern "C" {

// Nico van Waes <nico@yegal.njit.edu> says: under Solaris 2.6, one
// must include <sys/types.h> before <termios.h>.
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
#else
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

#if HAVE_SYS_SYSMACROS_H
#include <sys/sysmacros.h>
#ifndef minor
#define minor(x)        ((int)((x)&0377))
#endif
#endif

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
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
    int ioctl(int fd, int request, ...);
#endif
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

// Streams won't work on DEC OSF because there isn't any "ttcompat"
// module and I don't know enough about any of this stuff to try to
// figure it out now.  -- phil_brooks@MENTORG.COM (Phil Brooks)
#if !defined(__osf__) && HAVE_PTSNAME && HAVE_GRANTPT \
    && HAVE_UNLOCKPT && HAVE_IOCTL

#define HAVE_STREAMS 1

// Provide C++ declarations
extern "C" {
#if !HAVE_PTSNAME_DECL && !defined(ptsname)
    char *ptsname(int master);
#endif
#if !HAVE_UNLOCKPT_DECL && !defined(unlockpt)
    int unlockpt(int fd);
#endif
#if !HAVE_GRANTPT_DECL && !defined(grantpt)
    int grantpt(int fd);
#endif
}

#endif // !defined(__osf__) && HAVE_PTSNAME && ...

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

#if HAVE_STREAMS
    if (stat("/dev/ptmx", &sb) == 0)
    {
	// Try STREAMS - a SVR4 feature
	master = open_tty("/dev/ptmx");
	if (master >= 0)
	{
	    line = ptsname(master);
	    if (line == NULL)
		_raiseIOMsg("ptsname");
	    else if (grantpt(master))
		_raiseIOMsg("grantpt " + string(line));
	    else if (unlockpt(master))
		_raiseIOMsg("unlockpt " + string(line));
	    else if (!tty_ok(line))
		_raiseIOMsg("access " + string(line));
	    else
	    {
		// Everything ok - proceed
		_master_tty = ttyname(master);
		_slave_tty  = line;
#ifdef TIOCFLUSH
		ioctl(master, TIOCFLUSH, (char *)0);
#endif
		push = true;
		return;
	    }

	    close(master);
	}
	else
	    _raiseIOMsg("cannot open /dev/ptmx");
    }
#endif

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
	    string pty = string("/dev/pty/") + nr;
	    string tty = string("/dev/ttyp") + nr;
		
	    master = open_tty(pty.chars());
	    if (master >= 0)
	    {
		// Verify slave tty is usable
		if (!tty_ok(tty))
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
		string nr  = string(p1[i]) + p2[j];
		string pty = "/dev/ptym/pty" + nr;
		string tty = "/dev/pty/tty" + nr;
		
		master = open_tty(pty.chars());
		if (master >= 0)
		{
		    // Verify slave tty is usable
		    if (!tty_ok(tty))
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

    // Try PTY's in /dev/pty?? -- a BSD and USG feature
    // Slackware 3.0 wants [/zip]/dev/pty??, as
    // Jim Van Zandt <jrv@vanzandt.mv.com> suggests.
    for (int k = 0; k < 2; k++)
    {
	string prefix = (k == 0 ? "" : "/zip");

	for (int i = 0; i < int(p1.length()); i++)
	{
	    for (int j = 0; j < int(p2.length()); j++)
	    {
		string nr  = string(p1[i]) + p2[j];
		string pty = prefix + "/dev/pty" + nr;
		string tty = prefix + "/dev/tty" + nr;

		master = open_tty(pty.chars());
		if (master >= 0)
		{
		    // Verify slave tty is usable
		    if (!tty_ok(tty))
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
    slave = open_tty(slave_tty().chars());
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

#if HAVE_STREAMS && defined(I_PUSH)
    if (push)
    {
	// Finish STREAMS setup.
	if (ioctl(slave, I_PUSH, "ptem"))
	    _raiseIOWarning("ioctl ptem " + slave_tty());
	if (ioctl(slave, I_PUSH, "ldterm"))
	    _raiseIOWarning("ioctl ldterm " + slave_tty());
	if (ioctl(slave, I_PUSH, "ttcompat"))
	{
	    // On HP-UX and other systems, this call always fails.
	    // Fortunately, it seems we can live without as well.  Hence,
	    // we suppress the warning message to avoid confusion.
#if 0
	    _raiseIOWarning("ioctl ttcompat " + slave_tty());
#endif
	}
    }
#endif // I_PUSH

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
static char TTY_INIT[] = { 'A', '\n' };
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
    if (inputfp() == NULL)
    {
	raiseIOMsg("cannot associate input stream with pty master");
	terminate();
	return -1;
    }

    _outputfp = _inputfp;
    _errorfp  = NULL;

#if HAVE_SETBUF
    // Set unbuffered mode
    setbuf(_outputfp, NULL);
#elif HAVE_SETVBUF && defined(_IONBF)
    // According to lee@champion.tcs.co.jp (Lee Hounshell), this
    // won't work on Linux ELF systems:
    setvbuf(_outputfp, NULL, _IONBF, BUFSIZ);
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
    setbuf(stdout, NULL);

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
