/* This code is a part of http://phoxis.org/2010/05/02/an-id3v1-tag-parsing-library/ */
/* To be used with id3v1_lib.c and id3v1.h */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "id3v1.h"

#define UNSET 0
#define READ 1
#define WRITE 2
#define RIPP 3

void id3v1_tag_display (id3v1_tag * tag);
void id3v1_tag_prompt (id3v1_tag * tag);
void show_help (void);


int
main (int argc, char *argv[])
{
  id3v1_tag *tag;
  int status, file_count;
  char file_name[FILENAME_MAX];
  int opt, todo = UNSET;

  while ((opt = getopt (argc, argv, "rwx")) != -1)
    {
      switch (opt)
	{
	case 'r':
	  todo = READ;
	  break;
	case 'w':
	  todo = WRITE;
	  break;
	case 'x':
	  todo = RIPP;
	  break;
	default:
	  show_help ();
	  exit (0);
	}
    }

  if (todo == UNSET)
    {
      printf ("\nOperation Unset");
      show_help ();
      exit (0);
    }

  switch (todo)
    {
    case READ:
      for (file_count = optind; file_count < argc; file_count++)
	{
	  strcpy (file_name, argv[file_count]);
	  tag = id3v1_tag_read (file_name);
	  if (tag != NULL)
	    {
	      printf ("\nID3v1.x tag found is file \"%s\"\n", file_name);
	      id3v1_tag_display (tag);
	    }
	  else
	    printf ("\nID3v1.x tag is not present in file \"%s\"\n",
		    file_name);
	}
      break;
    case WRITE:
      for (file_count = optind; file_count < argc; file_count++)
	{
	  strcpy (file_name, argv[file_count]);
	  tag = id3v1_tag_allocate ();
	  printf ("\nEnter ID3v1 Tag Details for \"%s\"", file_name);
	  id3v1_tag_prompt (tag);
	  printf ("\nWriting ID3v1 tag to \"%s\"", file_name);
	  status = id3v1_tag_write (file_name, tag);
	  if (status == FALSE)
	    printf ("\t[FAIL]\n");
	  else
	    printf ("\t[DONE]\n");
	  id3v1_tag_free (tag);
	}
      break;
    case RIPP:
      for (file_count = optind; file_count < argc; file_count++)
	{
	  strcpy (file_name, argv[file_count]);
	  printf ("\nRipping ID3v1 Tag from \"%s\"", file_name);
	  id3v1_tag_rip (file_name);
	  printf ("\t [DONE]\n");
	}
      break;
    }

  return 0;
}

void
id3v1_tag_prompt (id3v1_tag * tag)
{
  char buffer[128];
  int track_no;

  memset (buffer, 0, 128);
  printf ("\nTrack Name: ");
  scanf (" %[^\n]", buffer);
  id3v1_tag_set_track (buffer, tag);

  memset (buffer, 0, 128);
  printf ("\nAlbum Name: ");
  scanf (" %[^\n]", buffer);
  id3v1_tag_set_album (buffer, tag);

  memset (buffer, 0, 128);
  printf ("\nArtist Name: ");
  scanf (" %[^\n]", buffer);
  id3v1_tag_set_artist (buffer, tag);

  memset (buffer, 0, 128);
  printf ("\nComment: ");
  scanf (" %[^\n]", buffer);
  id3v1_tag_set_comment (buffer, tag);

  memset (buffer, 0, 128);
  printf ("\nYear: ");
  scanf (" %[^\n]", buffer);
  id3v1_tag_set_year (buffer, tag);

  printf ("\nTrack Number: ");
  scanf (" %d", &track_no);
  id3v1_tag_set_trackno ((unsigned char) track_no, tag);
  getchar ();
  /* TODO: FIX THIS */
  memset (buffer, 0, 128);
  printf ("\nGenre: ");
  scanf (" %[^\n]", buffer);
  id3v1_tag_set_genre (get_genre_number (buffer), tag);

}

void
id3v1_tag_display (id3v1_tag * tag)
{
  printf ("Track Name : %s\n", id3v1_tag_get_track (tag));
  printf ("Artist     : %s\n", id3v1_tag_get_artist (tag));
  printf ("Album      : %s\n", id3v1_tag_get_album (tag));
  printf ("Year       : %s\n", id3v1_tag_get_year (tag));
  printf ("Comment    : %s\n", id3v1_tag_get_comment (tag));
  printf ("Track No.  : %d\n", id3v1_tag_get_trackno (tag));
  printf ("Genre      : %s\n", get_genre_string (id3v1_tag_get_genre (tag)));
}

void
show_help (void)
{
  printf
    ("\nUsage: use options -r|-w|-x file1.mp3 <file2.mp3> ...  \n-r : Read\n-w: Write\n-x: Ripp\n");
  return;
}
