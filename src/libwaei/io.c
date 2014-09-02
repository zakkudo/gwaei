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
lw_io_error_quark ()
{
  return g_quark_from_static_string ("lw-io-error");
}


gsize
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


static gchar const *
_convert_encoding (GIConv  conv,
                   gchar  *contents,
                   gsize   length,
                   gchar  *buffer,
                   gsize  *buffer_size)
{
    //Sanity checks
    g_return_val_if_fail (contents != NULL, NULL);
    g_return_val_if_fail (buffer != NULL, NULL);
    g_return_val_if_fail (buffer_size != NULL, NULL);
    if (contents == NULL || buffer == NULL) return NULL;
    if (*buffer_size < 1) return NULL;
    if (length < 1) return NULL;

    //Declarations
    gsize conversion_count = 0;
    gsize left = 0;
    gsize offset = 0;

    //Initializations
    left = length;
    outbuf = buffer;
    conversion_count = g_iconv (conv, &contents, &left, &buffer, buffer_size);
    offset = length - left;

    if (offset == 0)
    {
      offset++;
    }

    return contents + offset;
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
    //Sanity checks
    g_return_val_if_fail (SOURCE_PATH != NULL, FALSE);
    g_return_val_if_fail (TARGET_PATH != NULL, FALSE);
    g_return_val_if_fail (SOURCE_ENCODING != NULL, FALSE);
    g_return_val_if_fail (TARGET_ENCODING != NULL, FALSE);
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    GMappedFile *mapped_file = NULL;
    const gchar *CONTENTS = NULL;
    gsize length = 0;
    FILE *out_stream = NULL;
    gsize page_size = -1;
    GIConv conv = { 0 };
    gboolean has_error = NULL;

    //Initializations
    mapped_file = g_mapped_file_new (SOURCE_PATH, FALSE, &error);
    if (error != NULL)
    {
      lw_progress_take_error (progress, &error);
      has_error = TRUE;
      goto errored;
    }
    contents = g_mapped_file_get_contents (mapped_file);
    if (contents == NULL) goto errored;
    length = g_mapped_file_get_length (mapped_file);
    if (length < 1) goto errored;
    out_stream = g_fopen (TARGET_PATH, "wb");
    if (out_stream == NULL) goto errored;
    page_size = lw_io_get_pagesize ();
    if (page_size < 1) goto errored;
    outbuf = g_new (gchar, page_size);
    if (outbuf == NULL) goto errored;
    conv = g_iconv_open (TARGET_ENCODING, SOURCE_ENCODING);

    if (progress != NULL)
    {
      if (lw_progress_get_secondary_message (progress) == NULL)
      {
        lw_progres_set_secondary_message (progress, "Converting encoding to %s...", TARGET_ENCODING);
      }
      lw_progress_set_total (progress, length);
      lw_progress_set_current (progress, 0);
      lw_progress_set_complete (progress, FALSE);
    }


    {
      const gchar C = CONTENTS;
      gsize bytes_written = 0;
      gsize offset = 0;
      while (C != NULL && C -  < length)
      {
        buffer_size = page_size;
        offset = C - TEXT;
        C = _convert_encoding (conv, C, length - offset, buffer, &buffer_size);
        if (buffer_size > 0)
        {
          bytes_written = fwrite(outbuf, sizeof(gchar), buffer_size, out_stream);
          if (bytes_written == 0) goto errored;
        }
        if (progress != NULL)
        {
          lw_progress_set_current (progress, offset);
        }
      }

      if (bytes_written < 1)
      {
        if (progress != NULL)
        {
          lw_progress_set_error (progress, g_error_new (
            LW_IO_ERROR,
            LW_IO_ERRORCODE_TEXT_ENCODING_CONVERSION_ERROR,
            "There was an error converting the text encoding to %s from %s\n",
            SOURCE_ENCODING,
            TARGET_ENCODING
          ));
        }
        has_error = true;
        goto errored;
      }
    }

errored:

    if (progress != NULL)
    {
      lw_progress_set_current (progress, length);
      lw_progress_set_complete (progress, TRUE);
    }

    //Cleanup
    g_iconv_close (conv);
    g_free (buffer); buffer = NULL;

    return !has_error;
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
    g_return_val_if_fail (LW_IS_PROGRESS (progress), 1);
    
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
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    curl_global_init (CURL_GLOBAL_ALL);

    //Declarations
    CURL *curl = NULL;
    CURLcode res = 0;
    FILE *stream = NULL;
    gint res = 0;

    //Initializations
    curl = curl_easy_init ();
    if (curl == NULL) goto errored;
    stream = g_fopen (TARGET_PATH, "wb");
    if (stream == NULL) goto errored;

    curl_easy_setopt(curl, CURLOPT_URL, SOURCE_PATH);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _libcurl_write_func);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, _libcurl_read_func);

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, _libcurl_update_progress);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, progress);

    res = curl_easy_perform(curl);

    if (progress != NULL)
    {
      if (lw_progress_get_secondary_message (progress) == NULL)
      {
        lw_progress_set_secondary_message (progress, "Downloading %s...", SOURCE_PATH);
      }
      lw_progress_set_current (progress, 0.0);
      lw_progress_set_total (progress, 0.0);
      lw_progress_set_complete (progress, FALSE);
    }

    if (res != 0 && !lw_progress_is_cancelled (progress))
    {
      fclose(stream); stream = NULL;
      g_remove (TARGET_PATH);

      lw_progress_set_error (progress, g_error_new (
        LW_IO_ERROR,
        LW_IO_ERRORCODE_DOWNLOAD_ERROR,
        "%s",
        curl_easy_strerror(res)
      ));
    }

errored:

    if (progress != NULL)
    {
      lw_progress_set_current (progress, lw_progress_get_total (progress));
      lw_progress_set_complete (progress, TRUE);
    }

    if (stream != NULL) fclose(stream); stream = NULL;
    if (curl != NULL) curl_easy_cleanup(curl); curl = NULL;
    curl_global_cleanup ();

    return !has_error;
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
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;
    if (!g_file_test (SOURCE_PATH, G_FILE_TEST_IS_REGULAR)) return FALSE;

    //Declarations
    GMappedFile *mapped_file = NULL;
    FILE *out_stream = NULL;
    gsize chunk = 0;
    gsize pagesize = 0;
    gsize offset = 0;
    gboolean has_error = FALSE;

    //Initalizations
    mapped_file = g_mapped_file_new (SOURCE_PATH, FALSE, &error);
    if (error != NULL)
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, &error);
        error = NULL;
      }
      has_error = TRUE;
      goto errored;
    }
    length = g_mapped_file_get_length (mapped_file);
    out_stream = g_fopen (TARGET_PATH, "wb");
    if (out_stream == NULL) goto errored;
    pagesize = lw_io_get_pagesize

    if (progress != NULL)
    {
      if (lw_progress_get_secondary_message (progress) == NULL)
      {
        lw_progress_set_secondary_message (progress, "Copying %s\n", SOURCE_PATH);
      }
      lw_progress_set_complete (progress, FALSE);
      lw_progress_set_total (progress, length);
      lw_progress_set_current (progress, current);
    }

    while (chunk > 0)
    {
      chunk = fwrite(contents + offset, sizeof(gchar), pagesize, out_stream);
      offset += chunk;

      if (progress != NULL)
      {
        lw_progress_set_current (progress, offset);
      }
    }

    if (progress != NULL)
    {
      lw_progress_set_current (progress, length);
      lw_progress_set_complete (progress, TRUE);
    }

    if (ferror(out_stream))
    {
      if (progress != NULL)
      {
        lw_progress_set_error (progress, g_error_new (
          LW_IO_ERROR,
          LW_IO_ERRORCODE_COPY_ERROR,
          "There was an error copying from %s to %s\n",
          SOURCE_PATH,
          TARGET_PATH
        ));
      }
      has_error = TRUE;
      goto errored;
    }

errored:

    //Cleanup
    if (out_stream != NULL) fclose(out_stream); out_stream = NULL;
    if (mapped_file != NULL) g_mapped_file_unref (mapped_file); mapped_file = NULL;

    return has_error;
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
    //Sanity checks
    g_return_val_if_fail (SOURCE_PATH != NULL, FALSE);
    g_return_val_if_fail (TARGET_PATH != NULL, FALSE);
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    gzFile source = NULL;
    FILE *target = NULL;
    gint read = 0;
    gint page_size = lw_io_get_pagesize ();
    gchar buffer* = NULL;
    gsize length = 0;

    //Initializations
    source = gzopen (SOURCE_PATH, "rb");
    if (source == NULL) goto errored;
    filesize = lw_io_get_filesize (SOURCE_PATH);
    if (filesize < 1) goto errored;
    target = g_fopen (TARGET_PATH, "wb");
    if (target == NULL) goto errored;

    if (progress != NULL)
    {
      if (lw_progress_get_secondary_message (progress) == NULL)
      {
        lw_progress_set_secondary_message (progress, "Decompressing %s\n", %s, SOURCEPATH);
      }
      lw_progress_set_complete (progress, FALSE);
      lw_progress_set_total (progress, length);
      lw_progress_set_current (progress, 0);
    }

    {
      gsize uncompressed_bytes_read = 0;
      gsize offset = 0;
      while ((uncompressed_bytes_read = gzread (source, buffer, page_size)) > 0)
        offset = gztell(source);

        if (fwrite(buffer, sizeof(char), uncompressed_bytes_read, target) == 0)
        {
          if (progress != NULL)
          {
            lw_progress_set_error (progress, g_error_new (
              LW_IO_ERROR,
              LW_IO_ERRORCODE_WRITE_ERROR,
              "Unabled to write the decompressed file at %s\n",
              TARGET_PATH
            ));
          }
          has_error = TRUE;
          goto errored;
        }
        else
        {
          if (progress != NULL)
          {
            lw_progress_set_current (progress, position);
          }
        }
      }
    }

    {
      gint errnum = 0;
      const gchar *message = gzerror(source, &errnum);
      if (errnum == Z_BUF_ERROR)
      {
        if (progress != NULL)
        {
          lw_progress_set_error (progress, g_error_new (
            LW_IO_ERROR,
            LW_IO_ERRORCODE_CORRUPT_ARCHIVE,
            "gzip file is possibly corrupt %s",
            SOURCE_PATH
          ));
        }
        has_error = TRUE;
        goto errored;
      }
    }

errored:

    if (progress != NULL)
    {
      lw_progress_set_current (progress, length);
      lw_progress_set_complete (progress, TRUE);
    }

    if (target != NULL) fclose(target); target = NULL;
    if (source != NULL) gzclose(source); source = NULL;
    g_free (buffer); buffer = NULL;

    if (has_error)
    {
      g_remove (TARGET_PATH);
    }

    return !has_error;
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


void
lw_io_allocate_file (FILE       *stream,
                     gsize       byte_length,
                     LwProgress *progress)
{
    TODO
}

gchar*
lw_io_allocate_temporary_file (gsize       bytes_length,
                               LwProgress *progress)
{
    TODO
}
                                

