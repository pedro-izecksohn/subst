/* subst - Substitutes the first occurance of a string in a binary file for another string.
   This program is useful to change pre-compiled ELF files, without needing to re-compile.
   By: Pedro Izecksohn , izecksohn@yahoo.com
   Version: 2021/September/09 18:07
   Copyright: 1) This program is free to use.
              2) You may modify and distribute this source code if you grant the receiver the right to receive this source code
              in a machine readable media.
              3) I'm not responsible for illegal usage or wrong usage of the executable derived from this source code.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t length;
    char mem [];
} MemBuf;

MemBuf *new_MemBuf ()
{
    MemBuf *ret = malloc (sizeof(MemBuf));
    if (!ret)
    {
        fprintf (stderr, "new_MemBuf: malloc failed.\n");
        exit (EXIT_FAILURE);
    }
    ret->length = 0;
    return ret;
}

MemBuf *append_MemBuf_char (MemBuf *old, const char c)
{
    if (!old)
    {
        old = new_MemBuf();
    }
    MemBuf *ret = realloc (old, sizeof(MemBuf)+1+old->length);
    if (!ret)
    {
        fprintf (stderr, "append_MemBuf_char: realloc failed.\n");
        exit (EXIT_FAILURE);
    }
    ret->mem[ret->length] = c;
    ret->length++;
    return ret;
}

char *search_MemBuf_str (MemBuf *buf, char *str)
{
    if (!buf)
    {
        fprintf (stderr, "search_MemBuf_str: The MemBuf* argument is NULL.\n");
        exit (EXIT_FAILURE);
    }
    if (buf->length == 0) return NULL;
    if (!str)
    {
        fprintf (stderr, "search_MemBuf_str: The char* argument is NULL.\n");
        exit (EXIT_FAILURE);
    }
    char *ret = NULL, *strc = &str[0];
    size_t bufi = 0;
    for (;bufi<buf->length; bufi++)
    {
        if (!*strc) break;
        if (buf->mem[bufi]==*strc)
        {
            if (!ret)
            {
                ret = &buf->mem[bufi];
            }
            strc++;
            continue;
        }
        else
        {
            ret = NULL;
            strc = &str[0];
        }
    }
    return ret;
}

void replace_str_str (char *destiny, char *origin)
{
    char *di = &destiny[0];
    char *oi = &origin[0];
    while (*di && *oi)
    {
        *di = *oi;
        di++;
        oi++;
    }
    *di = *oi; // The NUL character.
}

int main (int argc, char **argv)
{
  if (argc!=3)
  {
      fprintf (stderr, "Usage: infile | %s oldstr newstr > outfile\n", argv[0]);
      exit (EXIT_FAILURE);
  }
  char *oldstr = argv[1];
  char *newstr = argv[2];
  if (strlen(oldstr)<strlen(newstr))
  {
      fprintf (stderr, "\"%s\" is shorter than \"%s\".\n", oldstr, newstr);
      exit (EXIT_FAILURE);
  }
  MemBuf *in = new_MemBuf();
  int C = 0;
  while (1)
  {
      C = fgetc (stdin);
      if (EOF==C)
      {
          if (ferror(stdin))
          {
              fprintf (stderr, "main: fgetc failed.\n");
              exit (EXIT_FAILURE);
          }
          if (feof(stdin)) break;
      }
      in = append_MemBuf_char (in, (char)C);
  }
  char *place = search_MemBuf_str (in, oldstr);
  if (place)
  {
    replace_str_str (place, newstr);
  }
  if (fwrite (in->mem, 1, in->length, stdout)!=in->length)
  {
    fprintf (stderr, "main: fwrite failed.\n");
    exit (EXIT_FAILURE);
  }
  exit (EXIT_SUCCESS);
}
