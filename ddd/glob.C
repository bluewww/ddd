/* File-name wildcard pattern matching for GNU.
   Copyright (C) 1985, 1988, 1989, 1991, 2001 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.
*/


/* To whomever it may concern: I have never seen the code which most
   Unix programs use to perform this function.  I wrote this from scratch
   based on specifications for the pattern matching.  --RMS.  */

// Arranged for C++ and autoconf by Andreas Zeller
// C++ comments indicate changes to Stallman's original C code.

#include "my-alloca.h"

// Yep, `#pragma alloca' must come even before this simple stuff.  Sigh.
char glob_rcsid[] = 
    "$Id$";

#include "my-dirent.h"

#include "glob.h"

// Other headers, hopefully more portable
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if HAVE_MEMCPY && !HAVE_MEMCPY_DECL
extern "C" void *memcpy(void *to, const void *from, size_t size);
#endif

// These should be checked by autoconf, but we prefer to play it safe
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif

#ifndef OPENDIR_NOT_ROBUST
#define OPENDIR_NOT_ROBUST
#endif

#if defined (_POSIX_SOURCE)
/* Posix does not require that the d_ino field be present, and some
   systems do not provide it. */
#define REAL_DIR_ENTRY(dp) 1
#else
#define REAL_DIR_ENTRY(dp) (dp->d_ino != 0)
#endif /* _POSIX_SOURCE */


/* Global variable which controls whether or not * matches .*.
   Non-zero means don't match .*.  */
int noglob_dot_filenames = 1;


static int glob_match_after_star (const char *pattern, const char *text);

/* Return nonzero if PATTERN has any special globbing chars in it.  */
int
glob_pattern_p (const char *pattern)
{
  register const char *p = pattern;
  register char c;
  int	open = 0;

  while ((c = *p++) != '\0')
    switch (c)
      {
      case '?':
      case '*':
	return 1;

      case '[':		/* Only accept an open brace if there is a close */
	open++;		/* brace to match it.  Bracket expressions must be */
	continue;	/* complete, according to Posix.2 */
      case ']':
	if (open)
	  return 1;
	continue;      

      case '\\':
	if (*p++ == '\0')
	  return 0;
      }

  return 0;
}

/* Match the pattern PATTERN against the string TEXT;
   return 1 if it matches, 0 otherwise.

   A match means the entire string TEXT is used up in matching.

   In the pattern string, `*' matches any sequence of characters,
   `?' matches any character, [SET] matches any character in the specified set,
   [!SET] matches any character not in the specified set.

   A set is composed of characters or ranges; a range looks like
   character hyphen character (as in 0-9 or A-Z).
   [0-9a-zA-Z_] is the set of characters allowed in C identifiers.
   Any other character in the pattern must be matched exactly.

   To suppress the special syntactic significance of any of `[]*?!-\',
   and match the character exactly, precede it with a `\'.

   If DOT_SPECIAL is nonzero,
   `*' and `?' do not match `.' at the beginning of TEXT.  */
int
glob_match (const char *pattern, const char *text, int dot_special)
{
  register char const *p = pattern, *t = text;
  register char c;

  while ((c = *p++) != '\0')
    switch (c)
      {
      case '?':
	if (*t == '\0' || (dot_special && t == text && *t == '.'))
	  return 0;
	else
	  ++t;
	break;

      case '\\':
	if (*p++ != *t++)
	  return 0;
	break;

      case '*':
	if (dot_special && t == text && *t == '.')
	  return 0;
	return glob_match_after_star (p, t);

      case '[':
	{
	  register char c1 = *t++;
	  int invert;

	  if (!c1)
	    return (0);

	  invert = ((*p == '!') || (*p == '^'));
	  if (invert)
	    p++;

	  c = *p++;
	  while (1)
	    {
	      register char cstart = c, cend = c;

	      if (c == '\\')
		{
		  cstart = *p++;
		  cend = cstart;
		}

	      if (c == '\0')
		return 0;

	      c = *p++;
	      if (c == '-' && *p != ']')
		{
		  cend = *p++;
		  if (cend == '\\')
		    cend = *p++;
		  if (cend == '\0')
		    return 0;
		  c = *p++;
		}
	      if (c1 >= cstart && c1 <= cend)
		goto match;
	      if (c == ']')
		break;
	    }
	  if (!invert)
	    return 0;
	  break;

	match:
	  /* Skip the rest of the [...] construct that already matched.  */
	  while (c != ']')
	    { 
	      if (c == '\0')
		return 0;
	      c = *p++;
	      if (c == '\0')
		return 0;
	      else if (c == '\\')
		++p;
	    }
	  if (invert)
	    return 0;
	  break;
	}

      default:
	if (c != *t++)
	  return 0;
      }

  return *t == '\0';
}

/* Like glob_match, but match PATTERN against any final segment of TEXT.  */

static int
glob_match_after_star (const char *pattern, const char *text)
{
  register const char *p = pattern, *t = text;
  register char c, c1;

  while ((c = *p++) == '?' || c == '*')
    if (c == '?' && *t++ == '\0')
      return 0;

  if (c == '\0')
    return 1;

  if (c == '\\')
    c1 = *p;
  else
    c1 = c;

  while (1)
    {
      if ((c == '[' || *t == c1) && glob_match (p - 1, t, 0))
	return 1;
      if (*t++ == '\0')
	return 0;
    }
}

/* Return a vector of names of files in directory DIR
   whose names match glob pattern PAT.
   The names are not in any particular order.
   Wildcards at the beginning of PAT do not match an initial period.

   The vector is terminated by an element that is a null pointer.

   To free the space allocated, first free the vector's elements,
   then free the vector.

   Return 0 if cannot get enough memory to hold the pointer
   and the names.

   Return -1 if cannot access directory DIR.
   Look in errno for more information.  */

char **
glob_vector (const char *pat, const char *dir)
{
  struct globval
    {
      struct globval *next;
      char *name;
    };

  DIR *d;
  register struct dirent *dp;
  struct globval *lastlink;
  register struct globval *nextlink;
  register char *nextname;
  unsigned int count;
  int lose;
  register char **name_vector = 0;
  register unsigned int i;
#if defined (OPENDIR_NOT_ROBUST)
  struct stat finfo;

  if (stat (dir, &finfo) < 0)
    return ((char **)-1);

  if (!S_ISDIR (finfo.st_mode))
    return ((char **)-1);
#endif /* OPENDIR_NOT_ROBUST */

  d = opendir (dir);
  if (d == 0)
    return (char **) -1;

  lastlink = 0;
  count = 0;
  lose = 0;

  /* Scan the directory, finding all names that match.
     For each name that matches, allocate a struct globval
     on the stack and store the name in it.
     Chain those structs together; lastlink is the front of the chain.  */
  while (1)
    {
      dp = readdir (d);
      if (dp == 0)
	break;

      /* If this directory entry is not to be used, try again. */
      if (!REAL_DIR_ENTRY (dp))
	continue;

      /* If a dot must be explicity matched, check to see if they do. */
      if (noglob_dot_filenames && dp->d_name[0] == '.' && pat[0] != '.')
	continue;

      if (glob_match (pat, dp->d_name, noglob_dot_filenames))
	{
          int len = NAMLEN(dp);
	  nextlink = (struct globval *) alloca (sizeof (struct globval));
	  nextlink->next = lastlink;
	  nextname = (char *) malloc (len + 1);
	  if (nextname == 0)
	    {
	      lose = 1;
	      break;
	    }
	  lastlink = nextlink;
	  nextlink->name = nextname;
	  memcpy (nextname, dp->d_name, len);
	  nextname[len] = '\0';
	  ++count;
	}
    }
  (void) closedir (d);

  if (!lose)
    {
      name_vector = (char **) malloc ((count + 1) * sizeof (char *));
      lose |= name_vector == 0;
    }

  /* Have we run out of memory?	 */
  if (lose)
    {
      /* Here free the strings we have got.  */
      while (lastlink)
	{
	  free (lastlink->name);
	  lastlink = lastlink->next;
	}
      return 0;
    }

  /* Copy the name pointers from the linked list into the vector.  */
  for (i = 0; i < count; ++i)
    {
      name_vector[i] = lastlink->name;
      lastlink = lastlink->next;
    }

  name_vector[count] = 0;
  return name_vector;
}

/* Return a new array which is the concatenation
   of each string in ARRAY to DIR. */

static char **
glob_dir_to_array (char *dir, char **array)
{
  register unsigned int i, l;
  int add_slash;
  char **result;

  l = strlen (dir);
  if (l == 0)
    return array;

  add_slash = dir[l - 1] != '/';

  i = 0;
  while (array[i] != 0)
    ++i;

  result = (char **) malloc ((i + 1) * sizeof (char *));
  if (result == 0)
    return 0;

  for (i = 0; array[i] != 0; i++)
    {
      result[i] = (char *) malloc (l + (add_slash ? 1 : 0)
				   + strlen (array[i]) + 1);
      if (result[i] == 0)
	return 0;
      sprintf (result[i], "%s%s%s", dir, add_slash ? "/" : "", array[i]);
    }
  result[i] = 0;

  /* Free the input array.  */
  for (i = 0; array[i] != 0; i++)
    free (array[i]);
  free ((char *) array);

  return result;
}

/* Do globbing on PATHNAME.  Return an array of pathnames that match,
   marking the end of the array with a null-pointer as an element.
   If no pathnames match, then the array is empty (first element is null).
   If there isn't enough memory, then return 0.
   If a file system error occurs, return -1; `errno' has the error code.  */
char **
glob_filename (const char *pathname)
{
  char **result;
  unsigned int result_size;
  char *directory_name; 
  const char *filename;
  unsigned int directory_len;

  result = (char **) malloc (sizeof (char *));
  result_size = 1;
  if (result == 0)
    return 0;

  result[0] = 0;

  /* Find the filename.  */
  filename = strrchr (pathname, '/');
  if (filename == 0)
    {
      filename = pathname;
      directory_name = (char *)"";
      directory_len = 0;
    }
  else
    {
      directory_len = (filename - pathname) + 1;
      directory_name = (char *) alloca (directory_len + 1);

      memcpy (directory_name, pathname, directory_len);
      directory_name[directory_len] = '\0';
      ++filename;
    }

  /* If directory_name contains globbing characters, then we
     have to expand the previous levels.  Just recurse. */
  if (glob_pattern_p (directory_name))
    {
      char **directories;
      register unsigned int i;

      if (directory_name[directory_len - 1] == '/')
	directory_name[directory_len - 1] = '\0';

      directories = glob_filename (directory_name);

      if (directories == 0)
	goto memory_error;
      else if (directories == (char **)-1)
        {
	  if (result)
	      free(result);
	  return (char **) -1;
        }
      else if (*directories == 0)
	{
	  if (result)
	      free(result);
	  free ((char *) directories);
	  return (char **) -1;
	}

      /* We have successfully globbed the preceding directory name.
	 For each name in DIRECTORIES, call glob_vector on it and
	 FILENAME.  Concatenate the results together.  */
      for (i = 0; directories[i] != 0; ++i)
	{
	  char **temp_results = glob_vector (filename, directories[i]);

	  /* Handle error cases. */
	  if (temp_results == 0)
	    goto memory_error;
	  else if (temp_results == (char **)-1)
	    /* This filename is probably not a directory.  Ignore it.  */
	    ;
	  else
	    {
	      char **array = glob_dir_to_array (directories[i], temp_results);
	      register unsigned int l;

	      l = 0;
	      while (array[l] != 0)
		++l;

	      result =
		(char **)realloc ((char *)result, 
				  (result_size + l) * sizeof (char *));

	      if (result == 0)
		goto memory_error;

	      for (l = 0; array[l] != 0; ++l)
		result[result_size++ - 1] = array[l];

	      result[result_size - 1] = 0;

	      /* Note that the elements of ARRAY are not freed.  */
	      free ((char *) array);
	    }
	}
      /* Free the directories.  */
      for (i = 0; directories[i]; i++)
	free (directories[i]);

      free ((char *) directories);

      return result;
    }

  /* If there is only a directory name, return it. */
  if (*filename == '\0')
    {
      result = (char **) realloc ((char *) result, 2 * sizeof (char *));
      if (result == 0)
	return 0;
      result[0] = (char *) malloc (directory_len + 1);
      if (result[0] == 0)
	goto memory_error;
      memcpy (result[0], directory_name, directory_len + 1);
      result[1] = 0;
      return result;
    }
  else
    {
      /* Otherwise, just return what glob_vector
	 returns appended to the directory name. */
      char **temp_results = glob_vector (filename,
					 (char *)(directory_len == 0
					  ? (char *)"." : directory_name));

      if (result)
	free(result);
      if (temp_results == 0 || temp_results == (char **)-1)
	return temp_results;

      return (glob_dir_to_array (directory_name, temp_results));
    }

  /* We get to memory error if the program has run out of memory, or
     if this is the shell, and we have been interrupted. */
 memory_error:
  if (result != 0)
    {
      register unsigned int i;
      for (i = 0; result[i] != 0; ++i)
	free (result[i]);
      free ((char *) result);
    }
  return 0;
}

#ifdef TEST

main (int argc, char *argv[])
{
  unsigned int i;

  for (i = 1; i < argc; ++i)
    {
      char **value = glob_filename (argv[i]);
      if (value == 0)
	puts ("Out of memory.");
      else if ((int) value == -1)
	perror (argv[i]);
      else
	for (i = 0; value[i] != 0; i++)
	  puts (value[i]);
    }

  exit (0);
}
#endif	/* TEST.  */
