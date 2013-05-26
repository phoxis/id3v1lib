/* VERSION: 1.0 */
/* Program: Code to read/write ID3v1.x tag from an mp3 file */
/* to use with id3v1.h */
/* This code is a part of http://phoxis.org/2010/05/02/an-id3v1-tag-parsing-library/ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "id3v1.h"


/* Allocate a tag, blank it and return allocated, if failed return NULL */

id3v1_tag *
id3v1_tag_allocate (void)
{
  id3v1_tag *tag;

  tag = malloc (sizeof (id3v1_tag));
  if (tag != NULL)
    {
      /* Clear the allocated tag */
      memset (tag->track_name, NUL, TRACK_SIZE);
      memset (tag->artist, NUL, ARTIST_SIZE);
      memset (tag->album, NUL, ALBUM_SIZE);
      memset (tag->comment, NUL, COMMENT_SIZE);
      memset (tag->year, NUL, YEAR_SIZE);
      tag->zero = 0;
      tag->trackno = 0;
      tag->genre = 0;
    }

  return tag;
}

/* Free the tag */
void
id3v1_tag_free (id3v1_tag * tag)
{
  if (tag)
  {
    free (tag);
    tag = NULL;
  }
}

/* NOTE: do not terminate the strings with NUL */
/* Open the file, read and populate the id3v1_tag structure */
id3v1_tag *
id3v1_tag_read (char *file_name)
{
  char raw[TAG_SIZE];
  FILE *tag_ptr;
  id3v1_tag *tag;

  tag = id3v1_tag_allocate ();
  if (tag == NULL)
    return NULL;

  tag_ptr = fopen (file_name, "rb");
  if (tag_ptr == NULL)
    {
      perror ("Error");
      return NULL;
    }

  fseek (tag_ptr, -TAG_SIZE, SEEK_END);
  fread (raw, TAG_SIZE, 1, tag_ptr);

  fclose (tag_ptr);

  /* If ID3v1.1 tag exists then populate the tag */
  if (memcmp (raw, "TAG", 3) == 0)
    {
      memcpy (tag->track_name, raw + 3, TRACK_SIZE);
      memcpy (tag->artist, raw + 33, ARTIST_SIZE);
      memcpy (tag->album, raw + 63, ALBUM_SIZE);
      memcpy (tag->year, raw + 93, YEAR_SIZE);
      memcpy (tag->comment, raw + 97, COMMENT_SIZE);
      tag->trackno = raw[126];
      tag->genre = raw[127];
    }
  /* Else free the allocated tag */
  else
    {
      id3v1_tag_free (tag);
      tag = NULL;
    }

  return tag;
}


/* Write a tag into a file */
int
id3v1_tag_write (char *file_name, id3v1_tag * tag)
{
  FILE *tag_ptr;
  char raw[TAG_SIZE];

  if (tag == NULL)
    return FALSE;

  tag_ptr = fopen (file_name, "rb+");
  if (tag_ptr == NULL)
    {
      perror ("Error");
      return FALSE;
    }

  /* If a tag exists then overwrite it, else append new */
  fseek (tag_ptr, -TAG_SIZE, SEEK_END);
  fread (raw, sizeof (char), 3, tag_ptr);

  /* If tag exists then only modify the tag data */
  if (memcmp (raw, "TAG", 3) == 0)
    {
//       printf ("\nOverwriting");
      fseek (tag_ptr, -TAG_SIZE+3, SEEK_END);
      fwrite (tag, sizeof (id3v1_tag), 1, tag_ptr);
    }
  /* If the tag does not exists, then append the whole tag */
  else
    {
//       printf ("\nAppending");
      fseek (tag_ptr, 0, SEEK_END);
      fwrite ("TAG", sizeof (char), 3, tag_ptr);
      fwrite (tag, sizeof (id3v1_tag), 1, tag_ptr);
    }

  fclose (tag_ptr);
  return TRUE;
}

/* if tag exists , rip it returns TRUE, else FALSE, if error then FALSE */
int
id3v1_tag_rip (char *file_name)
{
  FILE *tag_ptr;
  char raw[TAG_SIZE], null[TAG_SIZE];

  memset (null, 0, 128);
  
  tag_ptr = fopen (file_name, "rb+");
  if (tag_ptr == NULL)
    {
      perror ("Error");
      return FALSE;
    }

  fseek (tag_ptr, -TAG_SIZE, SEEK_END);
  fread (raw, sizeof (char), 3, tag_ptr);

  if (memcmp (raw, "TAG", 3) == 0)
    {
      /*TODO: Make this rewrite file without the tag */
      fseek (tag_ptr, -128, SEEK_END);
      fwrite (null, sizeof (char), 128, tag_ptr);
    }
  else
    {
      return FALSE;
    }

  fclose (tag_ptr);
  return TRUE;
}

/* NOTE: do not append a NUL at the last char, as the tags are fixed length
 * when using the get functions append a NULL and return 
 */

/* id3v1_tag_set_* functions START */
int
id3v1_tag_set_track (char *track_name, id3v1_tag * tag)
{
  if (tag == NULL)
    return FALSE;

  memcpy (tag->track_name, track_name, TRACK_SIZE);
  return TRUE;
}

int
id3v1_tag_set_artist (char *artist, id3v1_tag * tag)
{
  if (tag == NULL)
    return FALSE;

  memcpy (tag->artist, artist, ARTIST_SIZE);
  return TRUE;
}

int
id3v1_tag_set_album (char *album, id3v1_tag * tag)
{
  if (tag == NULL)
    return FALSE;

  memcpy (tag->album, album, ALBUM_SIZE);

  return TRUE;
}

int
id3v1_tag_set_year (char *year, id3v1_tag * tag)
{
  if (tag == NULL)
    return FALSE;

  memcpy (tag->year, year, YEAR_SIZE);

  return TRUE;
}

int
id3v1_tag_set_trackno (unsigned char track_no, id3v1_tag * tag)
{
  if (tag == NULL)
    return FALSE;

  tag->trackno = track_no;

  return TRUE;
}

int
id3v1_tag_set_comment (char *comment, id3v1_tag * tag)
{
  if (tag == NULL)
    return FALSE;

  memcpy (tag->comment, comment, COMMENT_SIZE);

  return TRUE;
}

/* to set through string use id3v1_tag_set_genre ( get_genre_number ("Classical") ); */
int
id3v1_tag_set_genre (char genre, id3v1_tag * tag)
{
  if (tag == NULL)
    return FALSE;

  tag->genre = genre;

  return TRUE;
}
/* id3v1_tag_set_* functions END */

/* NOTE: No need to explicitely do track_name[30]=NUL because the alloc_str is already making it clear and atmost 30 chars 0 to 29 is being copied so it is automatically \0 terminated. So make the change ando not explicitely assign a \0 to the last element
 */
/* Send a NUL terminated copy of the strings of the tag strings */

/* id3v1_tag_get_* functions START */
char *
id3v1_tag_get_track (id3v1_tag * tag)
{
  char *track_name;

  if (tag == NULL)
    return NULL;

  track_name = alloc_str (TRACK_SIZE+1);
  memcpy (track_name, tag->track_name, TRACK_SIZE);

  return track_name;
}

char *
id3v1_tag_get_artist (id3v1_tag * tag)
{
  char *artist;

  if (tag == NULL)
    return NULL;

  artist = alloc_str (ARTIST_SIZE+1);
  memcpy (artist, tag->artist, ARTIST_SIZE);

  return artist;
}

char *
id3v1_tag_get_album (id3v1_tag * tag)
{
  char *album;

  if (tag == NULL)
    return NULL;

  album = alloc_str (ALBUM_SIZE+1);
  memcpy (album, tag->album, ALBUM_SIZE);

  return album;
}

char *
id3v1_tag_get_year (id3v1_tag * tag)
{
  char *year;

  if (tag == NULL)
    return NULL;

  year = alloc_str (YEAR_SIZE+1);
  memcpy (year, tag->year, YEAR_SIZE);

  return year;
}

char
id3v1_tag_get_trackno (id3v1_tag * tag)
{
  char trackno;

  if (tag == NULL)
    return FALSE;

  trackno = tag->trackno;
  return trackno;
}

char *
id3v1_tag_get_comment (id3v1_tag * tag)
{
  char *comment;

  if (tag == NULL)
    return NULL;

  comment = alloc_str (COMMENT_SIZE+1);
  memcpy (comment, tag->comment, COMMENT_SIZE);
  
  return comment;
}

/* to get the string use get_genre_string (id3v1_tag_get_genre ( tag )); */
unsigned char
id3v1_tag_get_genre (id3v1_tag * tag)
{
  char genre;

  if (tag == NULL)
    return FALSE;

  genre = tag->genre;
  return genre;
}
/* id3v1_tag_get_* functions END */


/* Genre no to string lookup function. */
char *
get_genre_string (unsigned char genre_no)
{

  char *genre_array[] = {
    "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge",
    "Hip-Hop", "Jazz", "Metal",
    "New Age", "Oldies", "Other", "Pop", "R&B", "Rap", "Reggae", "Rock",
    "Techno", "Industrial", "Alternative",
    "Ska", "Death Metal", "Pranks", "Soundtrack", "Euro-Techno", "Ambient",
    "Trip-Hop", "Vocal", "Jazz+Funk",
    "Fusion", "Trance", "Classical", "Instrumental", "Acid", "House", "Game",
    "Sound Clip", "Gospel", "Noise",
    "AlternRock", "Bass", "Soul", "Punk", "Space", "Meditative",
    "Instrumental Pop", "Instrumental Rock", "Ethnic",
    "Gothic", "Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk",
    "Eurodance", "Dream", "Southern Rock",
    "Comedy", "Cult", "Gangsta", "Top 40", "Christian Rap", "Pop/Funk",
    "Jungle", "Native American", "Cabaret",
    "New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer", "Lo-Fi",
    "Tribal", "Acid Punk", "Acid Jazz", "Polka",
    "Retro", "Musical", "Rock & Roll", "Hard Rock", "Folk", "Folk-Rock",
    "National Folk", "Swing", "Fast Fusion",
    "Bebob", "Latin", "Revival", "Celtic", "Bluegrass", "Avantgarde",
    "Gothic Rock", "Progressive Rock",
    "Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band", "Chorus",
    "Easy Listening", "Acoustic", "Humour",
    "Speech", "Chanson", "Opera", "Chamber Music", "Sonata", "Symphony",
    "Booty Bass", "Primus", "Porn Groove",
    "Satire", "Slow Jam", "Club", "Tango", "Samba", "Folklore", "Ballad",
    "Power Ballad", "Rhythmic Soul", "Freestyle",
    "Duet", "Punk Rock", "Drum Solo", "Acapella", "Euro-House", "Dance Hall",
    "Goa", "Drum & Bass", "Club-House",
    "Hardcore", "Terror", "Indie", "BritPop", "Negerpunk", "Polsk Punk",
    "Beat", "Christian Gangsta", "Heavy Metal",
    "Black Metal", "Crossover", "Contemporary C", "Christian Rock",
    "Merengue", "Salsa", "Thrash Metal", "Anime",
    "JPop", "SynthPop", "None"
  };
  return genre_array[genre_no];
}


/* Genre string to genre no lookup function */
unsigned char
get_genre_number (char *genre_string)
{

  char *genre_array[] = {
    "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge",
    "Hip-Hop", "Jazz", "Metal",
    "New Age", "Oldies", "Other", "Pop", "R&B", "Rap", "Reggae", "Rock",
    "Techno", "Industrial", "Alternative",
    "Ska", "Death Metal", "Pranks", "Soundtrack", "Euro-Techno", "Ambient",
    "Trip-Hop", "Vocal", "Jazz+Funk",
    "Fusion", "Trance", "Classical", "Instrumental", "Acid", "House", "Game",
    "Sound Clip", "Gospel", "Noise",
    "AlternRock", "Bass", "Soul", "Punk", "Space", "Meditative",
    "Instrumental Pop", "Instrumental Rock", "Ethnic",
    "Gothic", "Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk",
    "Eurodance", "Dream", "Southern Rock",
    "Comedy", "Cult", "Gangsta", "Top 40", "Christian Rap", "Pop/Funk",
    "Jungle", "Native American", "Cabaret",
    "New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer", "Lo-Fi",
    "Tribal", "Acid Punk", "Acid Jazz", "Polka",
    "Retro", "Musical", "Rock & Roll", "Hard Rock", "Folk", "Folk-Rock",
    "National Folk", "Swing", "Fast Fusion",
    "Bebob", "Latin", "Revival", "Celtic", "Bluegrass", "Avantgarde",
    "Gothic Rock", "Progressive Rock",
    "Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band", "Chorus",
    "Easy Listening", "Acoustic", "Humour",
    "Speech", "Chanson", "Opera", "Chamber Music", "Sonata", "Symphony",
    "Booty Bass", "Primus", "Porn Groove",
    "Satire", "Slow Jam", "Club", "Tango", "Samba", "Folklore", "Ballad",
    "Power Ballad", "Rhythmic Soul", "Freestyle",
    "Duet", "Punk Rock", "Drum Solo", "Acapella", "Euro-House", "Dance Hall",
    "Goa", "Drum & Bass", "Club-House",
    "Hardcore", "Terror", "Indie", "BritPop", "Negerpunk", "Polsk Punk",
    "Beat", "Christian Gangsta", "Heavy Metal",
    "Black Metal", "Crossover", "Contemporary C", "Christian Rock",
    "Merengue", "Salsa", "Thrash Metal", "Anime",
    "JPop", "SynthPop", "None"
  };
  int i, j, flag;

  /* FIXME: please fix this loop, the numbers look ugly */
  for (i = 0; i < 149; i++)
    {
      for (j = 0, flag = 1;
	   genre_array[i][j] != NUL && genre_string[j] != NUL; j++)
	{
	  if (tolower (genre_array[i][j]) != tolower (genre_string[j]))
	    {
	      flag = 0;
	      break;
	    }
	}
      /* The genre_string is has 0x10 at the end before the 0x00 */
      if ((flag == 1) && (genre_array[i][j] == NUL)
	  && !isalpha (genre_string[j]))
	return i;
    }
  return 148;
}

/* Allocate a string and clear it */
/* FIXME: Shall we exit ?*/
char *
alloc_str (int size)
{
  char *string;

  string = malloc (size);
  if (string == NULL)
    {
      perror ("Error");
      exit (1);
    }
  memset (string, NUL, size);
  return string;
}
 
