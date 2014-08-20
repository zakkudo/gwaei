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
#ifdef G_OS_UNIX
#include <unistd.h>
#endif

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>


GQuark
lw_io_read_error_quark ()
{
  return g_quark_from_static_string ("lw-io-read-error");
}


GQuark
lw_io_write_error_quark ()
{
  return g_quark_from_static_string ("lw-io-write-error");
}


glong
lw_io_get_pagesize ()
{
    gsize pagesize = 0;

#ifdef G_OS_UNIX
    pagesize = sysconf(_SC_PAGESIZE);
#endif

    if (pagesize <= 0)
    {
      pagesize = 1024 * 4;
    }

    return pagesize;
}


void
lw_io_fwrite (FILE        *stream,
                    const gchar *TEXT,
                    gint         length,
                    LwProgress  *progress)
{
    //Sanity checks
    g_return_if_fail (PATH != NULL);
    g_return_if_fail (mode != NULL);
    g_return_if_fail (text != NULL);
    g_return_if_fail (progress != NULL);
    if (lw_progress_should_abort (progress)) return;

    //Declarations
    glong pagesize = 0;
    gsize chunk = 0;
    gsize offset = 0;
    gsize left = 0;

    //Initializations
    pagesize = lw_io_get_pagesize ()
    if (pagesize < 1) goto errored;

    if (length < 0)
    {
      length = strlen(TEXT);
    }
    if (length < 1) goto errored;

    if (progress != NULL) lw_progress_set_total (progress, length);

    while (length > 0 && feof(stream) == 0 && ferror(stream) == 0)
    {
      left = length - offset;
      chunk = (pagesize > left) ? left : pagesize;
      offset += fwrite(TEXT + offset, 1, chunk, stream);
      if (progress != NULL) lw_progress_set_current (self, offset);
    }

errored:

    return;
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
                  const gchar *MODE,
                  const gchar *TEXT, 
                  gint         length,
                  LwProgress  *progress)
{
    //Sanity checks
    g_return_if_fail (PATH != NULL);
    g_return_if_fail (MODE != NULL);
    g_return_if_fail (TEXT != NULL);
    g_return_if_fail (LW_IS_PROGRESS (progress));

    //Declarations
    FILE *stream = NULL;

    //Initializations
    stream = g_fopen (PATH, "wb");
    if (stream == NULL) goto errored;

    lw_io_fwrite (stream, TEXT, length, progress);

errored:

    if (progress != NULL && ferror(stream))
    {
      gint code = ferror(stream);
      lw_progress_take_error (progress, g_error_new (
          LW_IO_WRITE_ERROR,
          code,
          "There was an error code %d while writing the file:\n'%s'"
          code,
          PATH
        )
      );
    }

    if (stream != NULL) fclose(stream); stream = NULL;

    return;
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

      lw_progress_set_total (progress, filesize);
      lw_progress_set_current (progress, position);
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
      lw_progress_set_total (progress, dltotal);
      lw_progress_set_current (progress, dlnow);
    }
    else
    {
      lw_progress_set_total (progress, dltotal);
      lw_progress_set_current (progress, 0.0);
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

    REDO THIS!

    //Initalizations
    infd = g_fopen (SOURCE_PATH, "rb");
    outfd = g_fopen (TARGET_PATH, "wb");
    chunk = 1;
    end = lw_io_get_filesize (SOURCE_PATH);
    curpos = 0;

    while (chunk)
    {
      if (lw_progress_should_abort (progress)) break; 

      lw_progress_set_total (progress, end);
      lw_progress_set_current (progress, curpos);
      chunk = fread(buffer, sizeof(char), MAX, infd);
      chunk = fwrite(buffer, sizeof(char), chunk, outfd);
      curpos += chunk;
    }

    lw_progress_set_total (progress, end);
    lw_progress_set_current (progress, curpos);

    //Cleanup
    fclose(infd); infd = NULL;
    fclose(outfd); outfd = NULL;

    return (!lw_progress_errored (progress));
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
REDO THIS FOR PAGE SIZE
    //Declarations
    gzFile source;
    FILE *target;
    int read;
    const int MAX = 1024 * 4;
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
            lw_progress_set_total (progress, filesize);
            lw_progress_set_current (progress, position);
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
//! @brief Deletes a file from the filesystem
//! @param URI The path to the file to delet
//! @param error A pointer to a GError object to write errors to or NULL
//!
gboolean 
lw_io_remove (const gchar   *URI, 
              LwProgress    *progress)
{
    //Sanity checks
    if (URI == NULL) return;
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    gboolean is_removed = FALSE;

    if (progress != NULL)
    {
      lw_progress_set_secondary_message (progress, URI);
      lw_progress_set_total (progress, 1.0);
      lw_progress_set_current (progress, 0.0);
      lw_progress_set_complete (progress, FALSE);
    }

    g_remove (URI);

    is_removed = (g_file_test (URI, G_FILE_TEST_IS_REGULAR));

    if (!is_removed && progress != NULL)
    {
      lw_progress_take_error (progress, g_error_new (
        LW_IO_ERROR,
        LW_IO_ERRORCODE_FAILED_REMOVING_FILE,
        "Could not remove %s\n"
        uri
      );
    }

    if (progress != NULL)
    {
      lw_progress_set_current (progress, 1.0);
      lw_progress_set_complete (progress, TRUE);
    }

    return is_removed;
}


//!
//! @brief A quick way to get the number of lines in a file for use in progress functions
//! @param FILENAME The path to the file to see how many lines it has
//!
gsize
lw_io_get_file_size (const char *PATH)
{
    //Declarations
    FILE *file = NULL;
    gsize length = 0;

    //Initializations
    file = g_fopen (URI, "r");
    length = 0;

    if (file != NULL)
    {
      fseek (file, 0, SEEK_END);
      length = ftell (file);
      fclose(file);
      file = NULL;
    }
   
    return length;
}



