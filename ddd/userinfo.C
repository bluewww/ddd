// $Id$ -*- C++ -*-
// Issue name and e-mail address of building user

// Copyright (C) 1995, 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char userinfo_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include "hostname.h"
#include "bool.h"

extern "C" {
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
}

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#include <pwd.h>

#define NO_UNAME_AGENTS
#include "hostname.C"

// Return true if C is a letter found in real user names
inline bool is_letter(char c)
{
    return c && (isalpha(c) || isspace(c) || c == '.' || c == '-');
}

// Return e-mail address from preferences file DOTRC in HOME dir.
// Format of preferences line is `TAG E-MAIL-ADDRESS'.
static char *email_from_preferences(char *home, char *dotrc, char *tag)
{
    char preferences[BUFSIZ];
    strcpy(preferences, home);
    strcat(preferences, "/");
    strcat(preferences, dotrc);

    FILE *fp = fopen(preferences, "r");
    if (fp != NULL)
    {
	char line[BUFSIZ];
	while (fgets(line, sizeof(line), fp) != NULL)
	{
	    if (strncmp(line, tag, strlen(tag)) == 0)
	    {
		static char buffer[BUFSIZ];

		char *s = line + strlen(tag);
		char *t = buffer;
		while (isspace(*s))
		    s++;
		while (!isspace(*s) && *s != '\0')
		    *t++ = tolower(*s++);
		*t++ = '\0';
		fclose(fp);
		return buffer;
	    }
	}

	fclose(fp);
    }

    return 0;
}


// Return e-mail address from signature file DOTRC in HOME dir.  Find
// the first `@' and return the surrounding string.
static char *email_from_signature(char *home, char *dotrc)
{
    char signature[BUFSIZ];
    strcpy(signature, home);
    strcat(signature, "/");
    strcat(signature, dotrc);

    FILE *fp = fopen(signature, "r");
    if (fp != NULL)
    {
	char line[BUFSIZ];
	while (fgets(line, sizeof(line), fp) != NULL)
	{
	    char *s = strchr(line, '@');
	    if (s != 0)
	    {
		static char buffer[BUFSIZ];
		char *t = buffer;

		while (!isspace(*s) && s != line)
		    s--;
		while (isspace(*s))
		    s++;
		while (!isspace(*s) && *s != '\0')
		    *t++ = tolower(*s++);
		*t++ = '\0';
		fclose(fp);
		return buffer;
	    }
	}

	fclose(fp);
    }

    return 0;
}

// Return e-mail address from PWD at current host.
static char *email_from_pwd(struct passwd *pwd)
{
    static char buffer[BUFSIZ];
    strcpy(buffer, pwd->pw_name);
    strcat(buffer, "@");
    strcat(buffer, fullhostname());

    return buffer;
}

// Check if S looks like an e-mail address.  S must contain a `@' and
// a `.', neither at the beginning nor at the end.
static bool is_email(char *s)
{
    if (s == 0)
	return false;

    char *at = strchr(s, '@');
    if (at == 0 || at == s || at[1] == '\0')
	return false;

    char *dot = strchr(s, '.');
    if (dot == 0 || dot == s || dot[1] == '\0')
	return false;

    return true;
}

// Return an e-mail address for user PWD.
static char *email(struct passwd *pwd)
{
    char *s = 0;

    // Try Netscape and Lynx preferences
    if (!is_email(s))
	s = email_from_preferences(pwd->pw_dir, ".netscape-preferences", 
				   "EMAIL_ADDRESS:");
    if (!is_email(s))
	s = email_from_preferences(pwd->pw_dir, ".netscape/preferences", 
				   "EMAIL_ADDRESS:");
    if (!is_email(s))
	s = email_from_preferences(pwd->pw_dir, ".lynxrc", 
				   "personal_mail_address=");

    // Try .dot files used for e-mail and fingering
    if (!is_email(s))
	s = email_from_signature(pwd->pw_dir, ".signature");
    if (!is_email(s))
	s = email_from_signature(pwd->pw_dir, ".Sig");
    if (!is_email(s))
	s = email_from_signature(pwd->pw_dir, ".project");
    if (!is_email(s))
	s = email_from_signature(pwd->pw_dir, ".plan");

    // Try e-mail address from current host name
    if (!is_email(s))
	s = email_from_pwd(pwd);

    // Tried it all, and failed :-(
    if (!is_email(s))
	s = "unknown";

    return s;
}

// Write user information for given ARG
int userinfo(char *arg = 0)
{
    struct passwd *pwd = 0;

    if (arg == 0)
    {
	// Get info for current user
	pwd = getpwuid(getuid());
    }
    else if (isdigit(arg[0]))
    {
	// Get info for numerical id
	pwd = getpwuid(atoi(arg));
    }
    else
    {
	// Get info for user name
	pwd = getpwnam(arg);
    }

    if (pwd == 0)
    {
	fputs(arg, stderr);
	fputs(": no such passwd entry\n", stderr);
	return 1;
    }

    // Issue real name
    char *s = pwd->pw_gecos;
    while (is_letter(*s))
	putchar(*s++);

    if (s == pwd->pw_gecos)
    {
	// No real name given; try user id
	if (pwd->pw_name[0])
	{
	    s = pwd->pw_name;
	    while (is_letter(*s++))
		;

	    if (*s)
	    {
		// User id contains non-letters and is probably a
		// symbolic name => leave it unchanged.
		fputs(pwd->pw_name + 1, stdout);
	    }
	    else
	    {
		// User id is a real name => capitalize it.
		fputc(toupper(pwd->pw_name[0]), stdout);
		fputs(pwd->pw_name + 1, stdout);
	    }
	}
	else
	{
	    // No user id.  This is weird.
	    fputs("Unknown ", stdout);
	}
    }

    // Lookup preferences files for the e-mail address.
    fputs(" <", stdout);
    fputs(email(pwd), stdout);
    fputs(">\n", stdout);

    return 0;
}


// Issue the name of the building user, in the format
// ``REALNAME <USERNAME@HOSTNAME>''
int main(int argc, char *argv[])
{
    if (argc == 1)
	return userinfo();

    int ret = 0;
    for (int i = 1; i < argc; i++)
	ret |= userinfo(argv[i]);

    return ret;
}
