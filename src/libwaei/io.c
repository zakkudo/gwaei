/*****************************************************************************
    AUTHOR:
    File written and Copyrighted by Zachary Dovel. All Rights Reserved.

    LICENSE:
    This file is part of gWaei.

    gWaei is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    gWaei is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with gWaei.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

//!
//! @file io.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <gio/gio.h>
#include <curl/curl.h>
#include <zlib.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>


static gchar *_savepath = NULL;


//!
//! @brief Creates a savepath that is used with the save/save as functions
//! @param PATH a path to save to
//!
void 
lw_io_set_savepath (const gchar *PATH)
{
    if (_savepath != NULL)
    {
      g_free (_savepath);
      _savepath = NULL;
    }

    if (PATH != NULL)
      _savepath = g_strdup (PATH);
}


//!
//! @brief Gets the savepath used with the save/save as functions
//! @returns A constant path string that is not to be freed
//!
const gchar* 
lw_io_get_savepath ()
{
  return _savepath;
}


//!
//! @brief Writes a file using the given text and write mode
//! @param PATH The Path to write the file to
//! @param mode A constant char representing the write mode to be used (w,a)
//! @param text A char pointer to some text to write to the file.
//! @param cb A LwProgressCallback function to give progress feedback or NULL
//! @param data A generic pointer to data to pass to the callback
//! @param error A pointer to a GError object to write errors to or NULL
//!
void 
lw_io_write_file (const gchar *PATH, 
                  const gchar *mode, 
                  gchar       *text, 
                  LwProgress  *progress)
{
    //Sanity checks
    g_return_if_fail (PATH != NULL);
    g_return_if_fail (mode != NULL);
    g_return_if_fail (text != NULL);
    g_return_if_fail (progress != NULL);
    if (lw_progress_should_abort (progress)) return;

    //Declarations
    gchar *ptr;
    FILE* file;

    //Initializations
    ptr = &text[0];
    file = g_fopen (_savepath, mode);

    while (*ptr != '\0' && feof(file) == 0 && ferror(file) == 0)
    {
      fputc(*ptr, file);
      ptr++;
    }

    if (feof(file) == 0 && ferror(file) == 0)
    {
      fputc('\n', file);
    }

    //Cleanup
    fclose(file);
    file = NULL;
    ptr = NULL;
}


//!
//! @brief Copies a file and creates a new one using the new encoding
//! @param SOURCE_PATH The source file to change the encoding on.
//! @param TARGET_PATH The place to save the new file with the new encoding.
//! @param SOURCE_ENCODING The encoding of the source file.
//! @param TARGET_ENCODING THe wanted encoding in the new file to be created.
//! @param cb A LwProgressCallback to use to give progress feedback or NULL
//! @param data A gpointer to data to pass to the LwProgressCallback
//! @param error pointer to a GError to write errors to
//! @return The status of the conversion opertaion
//!
gboolean 
lw_io_copy_with_encoding (const gchar *SOURCE_PATH, 
                          const gchar *TARGET_PATH,
                          const gchar *SOURCE_ENCODING, 
                          const gchar *TARGET_ENCODING,
                          LwProgress  *progress)
{
    if (lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    FILE* readfd = NULL;
    FILE* writefd = NULL;
    const gint MAX = 1024 * 2;
    gchar source_buffer[MAX];
    gchar target_buffer[MAX];
    gchar *sptr, *tptr;
    size_t read, source_bytes_left, target_bytes_left;
    gdouble fraction;
    size_t position, filesize;
    GIConv conv;

    filesize = lw_io_get_filesize (SOURCE_PATH);
    position = 0;

    //Initializations
    readfd = g_fopen (SOURCE_PATH, "rb");
    writefd = g_fopen (TARGET_PATH, "wb");
    conv = g_iconv_open (TARGET_ENCODING, SOURCE_ENCODING);

    //Read a chunk
    while (ferror(readfd) == 0 && feof(readfd) == 0)
    {
      read = fread(source_buffer, sizeof(gchar), MAX, readfd);
      source_bytes_left = read;
      sptr = source_buffer;

      //Try to convert and write the chunk
      while (source_bytes_left > 0 && ferror(writefd) == 0 && feof(writefd) == 0)
      {
        target_bytes_left = MAX;
        tptr = target_buffer;

        g_iconv (conv, &sptr, &source_bytes_left, &tptr, &target_bytes_left);
        if (MAX != target_bytes_left) //Bytes were converted!
        {
          fwrite(target_buffer, sizeof(gchar), MAX - target_bytes_left, writefd); 
        }
        else if (source_bytes_left == MAX && target_bytes_left == MAX)
        {
          fprintf(stderr, "The file you are converting may be corrupt! Trying to skip a character...\n");
          fseek(readfd, 1L - source_bytes_left, SEEK_CUR);
        }
        else if (source_bytes_left > 0) //Bytes failed to convert!
        {
          fseek(readfd, -source_bytes_left, SEEK_CUR);
          source_bytes_left = 0;
        }
      }
      position = ftell(readfd);

      lw_progress_set_fraction (progress, position, filesize);
    }

    //Cleanup
    g_iconv_close (conv);
    fclose(readfd);
    fclose(writefd);

    return TRUE;
}


//!
//! @brief Private function made to be used with lw_io_download
//! @param ptr TBA
//! @param size TBA
//! @param nmemb TBA
//! @param stream TBA
//!
static size_t _libcurl_write_func (void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fwrite(ptr, size, nmemb, stream);
}
 

//!
//! @brief Private function made to be used with lw_io_download
//! @param ptr TBA
//! @param size TBA
//! @param nmemb TBA
//! @param stream TBA
//!
static size_t _libcurl_read_func (void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fread (ptr, size, nmemb, stream);
}

//!
//! @brief Private struct made to be used with lw_io_download
//! @param ptr TBA
//! @param size TBA
//! @param nmemb TBA
//! @param stream TBA
//!
static int _libcurl_update_progress (LwProgress *progress,
                                     double      dltotal,
                                     double      dlnow,
                                     double      ultotal,
                                     double      ulnow)
{
    //Sanity checks
    g_return_val_if_fail (progress != NULL, 1);
    
    if (dltotal > 0.0 && dlnow > 0.0)
    {
      lw_progress_set_fraction (progress, dlnow, dltotal);
    }
    else
    {
      lw_progress_set_fraction (progress, 0.0, dltotal);
    }

    //Update the interface
    if (lw_progress_should_abort (progress)) return 1;

    return 0;
}




//!
//! @brief Downloads a file using libcurl
//! @param source_path String of the source url
//! @param target_path String of the path to save the file locally
//! @param cb A LwProgressCallback to use to give progress feedback or NULL
//! @param data gpointer to data to pass to the function pointer
//! @param error Error handling
//!
gboolean 
lw_io_download (const gchar *SOURCE_PATH, 
                const gchar *TARGET_PATH, 
                LwProgress  *progress)
{
    g_return_val_if_fail (SOURCE_PATH != NULL, FALSE);
    g_return_val_if_fail (TARGET_PATH != NULL, FALSE);
    g_return_val_if_fail (progress != NULL, FALSE);
    if (lw_progress_should_abort (progress)) return FALSE;

    curl_global_init (CURL_GLOBAL_ALL);

    //Declarations
    GQuark quark;
    CURL *curl = NULL;
    CURLcode res = 0;
    FILE *outfile = NULL;
    const gchar *message = NULL;

    //Initializations
    curl = curl_easy_init ();
    outfile = g_fopen (TARGET_PATH, "wb");
    res = 0;

    if (curl != NULL || outfile != NULL)
    {
      curl_easy_setopt(curl, CURLOPT_URL, SOURCE_PATH);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _libcurl_write_func);
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, _libcurl_read_func);

      curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
      curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, _libcurl_update_progress);
      curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, progress);

      res = curl_easy_perform(curl);
    }
     
    fclose(outfile);
    curl_easy_cleanup(curl);

    if (lw_progress_should_abort (progress)) return 1;

    if (res != 0 && !lw_progress_is_cancelled (progress))
    {
      g_remove (TARGET_PATH);

      message = gettext(curl_easy_strerror(res));
      quark = g_quark_from_string (LW_IO_ERROR);
      lw_progress_set_error (progress, g_error_new_literal (quark, LW_IO_DOWNLOAD_ERROR, message));
    }

  curl_global_cleanup ();

    return (res == 0);
}


//!
//! @brief Copies a local file to another local location
//! @param SOURCE_PATH String of the source url
//! @param TARGET_PATH String of the path to save the file locally
//! @param cb A LwProgressCallback to use to give progress feedback or NULL
//! @param data A gpointer to data to pass to the LwProgressCallback
//! @param error Error handling
//!
gboolean 
lw_io_copy (const gchar *SOURCE_PATH, 
            const gchar *TARGET_PATH, 
            LwProgress  *progress)
{
    if (lw_progress_should_abort (progress)) return FALSE;
    if (!g_file_test (SOURCE_PATH, G_FILE_TEST_IS_REGULAR)) return FALSE;

    //Declarations
    FILE *infd;
    FILE *outfd;
    size_t chunk;
    size_t end;
    size_t curpos;
    const int MAX = 1024;
    char buffer[MAX];
    gboolean is_cancelled;

    //Initalizations
    infd = g_fopen (SOURCE_PATH, "rb");
    outfd = g_fopen (TARGET_PATH, "wb");
    chunk = 1;
    end = lw_io_get_filesize (SOURCE_PATH);
    curpos = 0;

    while (chunk)
    {
      if (lw_progress_should_abort (progress)) break; 

      lw_progress_set_fraction (progress, curpos, end);
      chunk = fread(buffer, sizeof(char), MAX, infd);
      chunk = fwrite(buffer, sizeof(char), chunk, outfd);
      curpos += chunk;
    }

    lw_progress_set_fraction (progress, curpos, end);

    //Cleanup
    fclose(infd); infd = NULL;
    fclose(outfd); outfd = NULL;

    return (!lw_progress_errored (progress));
}


//!
//! @brief Creates a single dictionary containing both the radical dict and kanji dict
//! @param output_path Mix dictionary path to write to
//! @param kanji_dictionary_path Kanjidic dictionary path
//! @param radicals_dictionary_path raddic dictionary path
//! @param cb A LwProgressCallback to use to give progress feedback or NULL
//! @param data A gpointer to data to pass to the LwProgressCallback
//! @param error pointer to a GError to write errors to
//!
gboolean 
lw_io_create_mix_dictionary (const gchar  *OUTPUT_PATH, 
                             const gchar  *KANJI_DICTIONARY_PATH, 
                             const gchar  *RADICALS_DICTIONARY_PATH, 
                             LwProgress   *progress)
{
    //Sanity check
    if (lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    FILE *output_file, *kanji_file, *radicals_file;
    char radicals_input[LW_IO_MAX_FGETS_LINE];
    char kanji_input[LW_IO_MAX_FGETS_LINE];
    char output[LW_IO_MAX_FGETS_LINE * 2];
    char *radicals_ptr, *kanji_ptr, *output_ptr, *temp_ptr;

    size_t curpos;
    size_t end;

    //Initializations
    kanji_file =  g_fopen (KANJI_DICTIONARY_PATH, "r");
    radicals_file = g_fopen (RADICALS_DICTIONARY_PATH, "r");
    output_file = g_fopen (OUTPUT_PATH, "w");
    radicals_ptr = NULL;
    kanji_ptr = NULL;
    output_ptr = NULL;

    curpos = 0;
    end = lw_io_get_filesize (KANJI_DICTIONARY_PATH);

    //Loop through the kanji file
    while (fgets(kanji_input, LW_IO_MAX_FGETS_LINE, kanji_file) != NULL)
    {
      if (lw_progress_should_abort (progress)) break;

      lw_progress_set_fraction (progress, curpos, end);

      curpos += strlen (kanji_input);

      if (kanji_input[0] == '#') continue;

      kanji_ptr = kanji_input;
      output_ptr = output;

      //1. Copy the kanji character from the kanji line
      while (*kanji_ptr != ' ')
      {
        *output_ptr = *kanji_ptr;
        output_ptr++;
        kanji_ptr++;
      }

      //2. Find the relevent radical line and insert it if available
      rewind (radicals_file);
      while (fgets(radicals_input, LW_IO_MAX_FGETS_LINE, radicals_file) != NULL)
      {
        //Check for a match
        temp_ptr = kanji_input;
        radicals_ptr = radicals_input;
        while (*radicals_ptr != ' ' && *radicals_ptr == *temp_ptr)
        {
          temp_ptr++;
          radicals_ptr++;
        }

        //If a match is found...
        if (*radicals_ptr == ' ')
        {
          //Skip over the colon
          radicals_ptr++;
          radicals_ptr++;
   
          //Copy the data
          while (*(radicals_ptr + 1) != '\0')
          {
            *output_ptr = *radicals_ptr;
            output_ptr++;
            radicals_ptr++;
          }

          break;
        }
      }

      //3. Copy the rest of the kanji line to output
      while (*kanji_ptr != '\0')
      {
        *output_ptr = *kanji_ptr;
        output_ptr++;
        kanji_ptr++;
      }

      //4. Close off the string and write it to the file
      *output_ptr = '\0';
      fputs(output, output_file);
      output[0] = '\0';
    }

    lw_progress_set_fraction (progress, curpos, end);

    //Cleanup
    fclose(kanji_file); kanji_file = NULL;
    fclose(output_file); output_file = NULL;
    fclose(radicals_file); radicals_file = NULL;

    return TRUE;
}


//!
//! @brief Splits the Names 
//! @param OUTPUT_NAMES_PATH The path to write the new Names dictionary to
//! @param OUTPUT_PLACES_PATH The path to write the new Places dictionary to
//! @param INPUT_NAMES_PLACES_PATH The file to use to generate the split dictionaries
//! @param cb A LwProgressCallback to use to give progress feedback or NULL
//! @param data A gpointer to data to pass to the LwProgressCallback
//! @param error pointer to a GError to write errors to
//!
gboolean 
lw_io_split_places_from_names_dictionary (const gchar *OUTPUT_NAMES_PATH, 
                                          const gchar *OUTPUT_PLACES_PATH,
                                          const gchar *INPUT_NAMES_PLACES_PATH,
                                          LwProgress  *progress)
{
    if (lw_progress_should_abort (progress)) return FALSE;

    /*
      Current composition of the Enamdic dictionary
      ----------------------------------------------
      s - surname (138,500)
      p - place-name (99,500)
      u - person name, either given or surname, as-yet unclassified (139,000) 
      g - given name, as-yet not classified by sex (64,600)
      f - female given name (106,300)
      m - male given name (14,500)
      h - full (family plus given) name of a particular person (30,500)
      pr - product name (55)
      co - company name (34)
      ---------------------------------------------
    */

    //Declarations
    char buffer[LW_IO_MAX_FGETS_LINE];
    FILE *inputf;
    size_t curpos;
    size_t end;
    double fraction;
    gboolean is_cancelled;
    GError *error = NULL;

    FILE *placesf;
    GRegex *re_place;
    const gchar *place_pattern = "([\\(,])((p)|(st))([\\),])";
    int  place_write_error;

    FILE *namesf;
    GRegex *re_name;
    const gchar *name_pattern = "([\\(,])((s)|(u)|(g)|(f)|(m)|(h)|(pr)|(co))([\\),])";
    int  name_write_error;

    //Initializations
    inputf = g_fopen (INPUT_NAMES_PLACES_PATH, "r");
    curpos = 0;
    end = lw_io_get_filesize (INPUT_NAMES_PLACES_PATH);
    fraction = 0.0;

    re_place = g_regex_new (place_pattern, G_REGEX_OPTIMIZE, 0, &error);
    if (error != NULL) goto errored;
    placesf = g_fopen (OUTPUT_PLACES_PATH, "w");
    place_write_error = 0;

    re_name = g_regex_new (name_pattern, G_REGEX_OPTIMIZE, 0, &error);
    if (error != NULL) goto errored;
    namesf = g_fopen (OUTPUT_NAMES_PATH, "w");
    name_write_error  = 0;


    //Start writing the child files
    while (fgets(buffer, LW_IO_MAX_FGETS_LINE, inputf) != NULL &&
           place_write_error != EOF &&
           name_write_error  != EOF &&
          !lw_progress_should_abort (progress))
    {
      if (lw_progress_should_abort (progress)) break;

      lw_progress_set_fraction (progress, curpos, end);

      if (placesf != NULL && g_regex_match (re_place, buffer, 0, NULL))
        place_write_error = fputs(buffer, placesf);
      if (namesf != NULL && g_regex_match(re_name, buffer, 0, NULL))
        name_write_error =  fputs(buffer, namesf);
      curpos += strlen(buffer);
    }

    lw_progress_set_fraction (progress, curpos, end);

errored:

    //Cleanup
    if (inputf != NULL) fclose(inputf);
    if (placesf != NULL) fclose(placesf);
    if (namesf != NULL) fclose(namesf);
    if (re_place != NULL) g_regex_unref (re_place);
    if (re_name != NULL) g_regex_unref (re_name);

    if (progress != NULL)
    {
      lw_progress_take_error (progress, &error);
    }
    else
    {
      g_clear_error (&error);
    }

    return (place_write_error != EOF && name_write_error != EOF);
}


//!
//! @brief Decompresses a gzip file
//! @param SOURCE_PATH The path to the file that is gzipped
//! @param TARGET_PATH The path to write the uncompressed file to
//! @param cb A LwProgressCallback function to give progress feedback or NULL
//! @param data A generic pointer to data to pass to the LwProgressCallback
//! @param error A pointer to a GError object to write an error to or NULL
//!
gboolean 
lw_io_gunzip_file (const gchar *SOURCE_PATH, 
                   const gchar *TARGET_PATH,
                   LwProgress  *progress)
{
    if (lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    gzFile source;
    FILE *target;
    int read;
    const int MAX = 1024;
    char buffer[MAX];
    size_t filesize, position;
    position = 0;

    source = gzopen (SOURCE_PATH, "rb");
    if (source != NULL)
    {
      filesize = lw_io_get_filesize (SOURCE_PATH);

      target = g_fopen (TARGET_PATH, "wb");
      if (target != NULL)
      {
        do {
          read = gzread (source, buffer, MAX);
          if (read > 0) 
          {
            position += MAX;
            if (position > filesize) position = filesize;
            lw_progress_set_fraction (progress, position, filesize);
            fwrite(buffer, sizeof(char), read, target);
          }
        } while (read > 0);

        fclose(target); target = NULL;
      }
      gzclose(source); source = NULL;
    }

    return (!lw_progress_errored (progress));
} 


//!
//! @brief Decompresses a zip file
//! @param SOURCE_PATH The path to the file that is gzipped
//! @param cb A LwProgressCallback function to give progress feedback or NULL
//! @param data A generic pointer to data to pass to the LwProgressCallback
//! @param error A pointer to a GError object to write an error to or NULL
//!
gboolean 
lw_io_unzip_file (const gchar *SOURCE_PATH, 
                  LwProgress  *progress)
{
    return TRUE;
}


//!
//! @brief Gets the size of a file in bytes
//! @param URI The path to the file to calculate the size of
//! @returns The size of the file in bytes
//!
size_t 
lw_io_get_filesize (const gchar *URI)
{
    //Sanity check
    g_assert (g_file_test (URI, G_FILE_TEST_IS_REGULAR));

    //Declarations
    const int MAX_CHUNK = 128;
    char buffer[MAX_CHUNK];
    FILE *file;
    size_t size;

    //Initializations
    file = g_fopen (URI, "rb");
    size = 0;

    while (file != NULL && ferror(file) == 0 && feof(file) == 0)
        size += fread(buffer, sizeof(char), MAX_CHUNK, file);

    //Cleanup
    fclose(file);

    return size;
}


//!
//! @brief Deletes a file from the filesystem
//! @param URI The path to the file to delet
//! @param error A pointer to a GError object to write errors to or NULL
//!
gboolean 
lw_io_remove (const gchar   *URI, 
              LwProgress    *progress)
{
  if (lw_progress_should_abort (progress)) return FALSE;

  lw_progress_set_fraction (progress, 0.0, 1.0);

  g_remove (URI);

  lw_progress_set_fraction (progress, 1.0, 1.0);

  return (!lw_progress_errored (progress));
}


//!
//! @brief A quick way to get the number of lines in a file for use in progress functions
//! @param FILENAME The path to the file to see how many lines it has
//!
long 
lw_io_get_size_for_uri (const char *URI)
{
    //Declarations
    FILE *file;
    long length;

    //Initializations
    file = g_fopen (URI, "r");
    length = 0L;

    if (file != NULL)
    {
      fseek (file, 0L, SEEK_END);
      length = ftell (file);
      fclose(file);
    }
   
    return length;
}


