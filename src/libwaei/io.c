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

/**
 * SECTION:io
 * @short description: IO Helper methods
 * @Title: IO Utility Methods
 * @include: libwaei/io.h
 */

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


/**
 * lw_io_fwrite:
 * @stream: #FILE to write to
 * @TEXT: Text to write to the @stream
 * @length: The length of @TEXT in bytes or 0 to have it calculated
 * @progress: A #LwProgress to track progress or #NULL to ignore it
 *
 * A convenience method to write a string to a file while reporting
 * progress. The chunk size defaults to the page file size of your
 * system, which can be overridden by setting prefered chunk size on
 * the #LwProgress object.
 *
 * Returns: The number of bytes written
 */
gsize
lw_io_fwrite (FILE        * stream,
              const gchar * TEXT,
              gint          length,
              LwProgress  * progress)
{
    //Sanity checks
    g_return_val_if_fail (stream != NULL, 0);
    g_return_val_if_fail (TEXT != NULL, 0);
    if (progress != NULL && lw_progress_should_abort (progress)) return 0;

    //Declarations
    gsize chunk_size = 0;
    gsize bytes_written = 0;
    gsize chunk = 0;
    gsize offset = 0;
    gsize left = 0;

    //Initializations

    if (length < 1) length = strlen(TEXT);
    if (length < 1) goto errored;

    chunk_size = length;
    if (progress != NULL) chunk_size = lw_progress_get_chunk_size (progress);

    if (progress != NULL) lw_progress_set_total (progress, length);
    left = length;

    while (left > 0 && feof(stream) == 0 && ferror(stream) == 0)
    {
      if (chunk_size > left) chunk_size = left;
      bytes_written = fwrite(TEXT + offset, 1, chunk_size, stream);
      chunk += bytes_written;
      offset += bytes_written;
      left -= bytes_written;
      if (chunk >= chunk_size)
      {
        if (progress != NULL)
        {
          if (lw_progress_should_abort (progress)) goto errored;
          lw_progress_set_current (progress, offset);
        }
        chunk = 0;
      }
    }

    if (progress != NULL)
    {
      lw_progress_set_current (progress, length);
    }

errored:

    return offset;
}


gsize
lw_io_write_file (const gchar *PATH, 
                  const gchar *MODE,
                  const gchar *TEXT, 
                  gint         length,
                  LwProgress  *progress)
{
    //Sanity checks
    g_return_val_if_fail (PATH != NULL, 0);
    g_return_val_if_fail (MODE != NULL, 0);
    g_return_val_if_fail (TEXT != NULL, 0);

    //Declarations
    FILE *stream = NULL;
    gsize bytes_written = 0;

    //Initializations
    stream = g_fopen (PATH, "wb");
    if (stream == NULL) goto errored;
    bytes_written = lw_io_fwrite (stream, TEXT, length, progress);

errored:

    if (progress != NULL && ferror(stream))
    {
      gint code = g_file_error_from_errno (ferror(stream));
      lw_progress_take_error (progress, g_error_new (
          G_FILE_ERROR,
          code,
          "There was an error code %d while writing the file:\n'%s'",
          code,
          PATH
        )
      );
    }

    if (stream != NULL) fclose(stream); stream = NULL;

    return bytes_written;
}


static gchar const *
_convert_encoding (GIConv       conv,
                   gchar const *CONTENTS,
                   gsize        content_length,
                   gchar       *buffer,
                   gsize       *buffer_size)
{
    //Sanity checks
    g_return_val_if_fail (CONTENTS != NULL, NULL);
    g_return_val_if_fail (buffer != NULL, NULL);
    g_return_val_if_fail (buffer_size != NULL, NULL);
    if (CONTENTS == NULL || buffer == NULL) return NULL;
    if (content_length < 1) return NULL;
    if (*buffer_size < 1) return NULL;

    //Declarations
    gsize conversion_count = 0;
    gsize left = 0;
    gsize offset = 0;

    //Initializations
    left = content_length;
    conversion_count = g_iconv (conv, (gchar**)&CONTENTS, &left, &buffer, buffer_size);
    offset = content_length - left;

    if (offset == 0)
    {
      offset++;
    }

    return CONTENTS + offset;
}


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
    FILE *stream = NULL;
    gchar *buffer = NULL;
    gsize chunk_size = -1;
    GIConv conv = { 0 };
    gboolean has_error = FALSE;
    GError *error = NULL;

    //Initializations
    mapped_file = g_mapped_file_new (SOURCE_PATH, FALSE, &error);
    if (error != NULL)
    {
      if (progress != NULL) lw_progress_take_error (progress, error);
      error = NULL;
      g_clear_error (&error);
      has_error = TRUE;
      goto errored;
    }
    CONTENTS = g_mapped_file_get_contents (mapped_file);
    if (CONTENTS == NULL) goto errored;
    length = g_mapped_file_get_length (mapped_file);
    if (length < 1) goto errored;
    stream = g_fopen (TARGET_PATH, "wb");
    if (stream == NULL) goto errored;
    chunk_size = lw_progress_get_chunk_size (progress);
    if (chunk_size < 1) goto errored;
    buffer = g_new (gchar, chunk_size);
    if (buffer == NULL) goto errored;
    conv = g_iconv_open (TARGET_ENCODING, SOURCE_ENCODING);

    if (progress != NULL)
    {
      if (lw_progress_get_secondary_message (progress) == NULL)
      {
        lw_progress_set_secondary_message_printf (progress, "Converting encoding to %s...", TARGET_ENCODING);
      }
      lw_progress_set_total (progress, length);
      lw_progress_set_current (progress, 0);
    }


    {
      gchar const *C = CONTENTS;
      gsize bytes_written = 0;
      gsize offset = 0;
      gsize buffer_size = NULL;
      while (C != NULL && C - CONTENTS < length)
      {
        buffer_size = chunk_size;
        offset = C - CONTENTS;
        C = _convert_encoding (conv, C, length - offset, buffer, &buffer_size);
        if (buffer_size > 0)
        {
          bytes_written = fwrite(buffer, sizeof(gchar), buffer_size, stream);
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
        has_error = TRUE;
        goto errored;
      }
    }

errored:

    if (progress != NULL)
    {
      lw_progress_set_current (progress, length);
    }

    //Cleanup
    if (stream != NULL) fclose(stream); stream = NULL;
    g_iconv_close (conv);
    g_free (buffer); buffer = NULL;

    if (has_error)
    {
      g_remove (TARGET_PATH);
    }

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
    gboolean has_error = FALSE;

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
        lw_progress_set_secondary_message_printf (progress, "Downloading %s...", SOURCE_PATH);
      }
      lw_progress_set_current (progress, 0.0);
      lw_progress_set_total (progress, 0.0);
    }

    if (res != 0 && !lw_progress_is_cancelled (progress))
    {
      lw_progress_set_error (progress, g_error_new (
        LW_IO_ERROR,
        LW_IO_ERRORCODE_DOWNLOAD_ERROR,
        "%s",
        curl_easy_strerror(res)
      ));
      has_error = TRUE;
    }

errored:

    if (progress != NULL)
    {
      lw_progress_set_current (progress, lw_progress_get_total (progress));
    }

    if (stream != NULL) fclose(stream); stream = NULL;
    if (curl != NULL) curl_easy_cleanup(curl); curl = NULL;
    curl_global_cleanup ();

    if (has_error)
    {
      g_remove (TARGET_PATH);
    }

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
    gchar *contents = NULL;
    gsize content_length = 0;
    FILE *stream = NULL;
    gsize chunk = 0;
    gsize chunk_size = 0;
    gsize offset = 0;
    GError *error = NULL;
    gboolean has_error = FALSE;

    //Initalizations
    mapped_file = g_mapped_file_new (SOURCE_PATH, FALSE, &error);
    if (error != NULL)
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, error);
        error = NULL;
      }
      g_clear_error (&error);
      has_error = TRUE;
      goto errored;
    }
    if (mapped_file == NULL) goto errored;
    contents = g_mapped_file_get_contents (mapped_file);
    content_length = g_mapped_file_get_length (mapped_file);
    if (content_length < 1) goto errored;
    stream = g_fopen (TARGET_PATH, "wb");
    if (stream == NULL) goto errored;
    chunk_size = lw_progress_get_chunk_size (progress);

    if (progress != NULL)
    {
      if (lw_progress_get_secondary_message (progress) == NULL)
      {
        lw_progress_set_secondary_message_printf (progress, "Copying %s\n", SOURCE_PATH);
      }
      lw_progress_set_total (progress, content_length);
      lw_progress_set_current (progress, offset);
    }

    while (chunk > 0)
    {
      chunk = fwrite(contents + offset, sizeof(gchar), chunk_size, stream);
      offset += chunk;

      if (progress != NULL)
      {
        lw_progress_set_current (progress, offset);
      }
    }

    if (progress != NULL)
    {
      lw_progress_set_current (progress, content_length);
    }

    if (ferror(stream))
    {
      if (progress != NULL)
      {
        gint code = g_file_error_from_errno (ferror(stream));
        lw_progress_set_error (progress, g_error_new (
          G_FILE_ERROR,
          code,
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
    if (stream != NULL) fclose(stream); stream = NULL;
    if (mapped_file != NULL) g_mapped_file_unref (mapped_file); mapped_file = NULL;

    if (has_error)
    {
      g_remove (TARGET_PATH);
    }

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
    gint chunk_size = lw_progress_get_chunk_size (progress);
    gchar *buffer = NULL;
    gsize content_length = 0;
    gboolean has_error = FALSE;

    //Initializations
    content_length = lw_io_get_file_size (SOURCE_PATH);
    if (content_length < 1) goto errored;
    source = gzopen (SOURCE_PATH, "rb");
    if (source == NULL) goto errored;
    target = g_fopen (TARGET_PATH, "wb");
    if (target == NULL) goto errored;

    if (progress != NULL)
    {
      if (lw_progress_get_secondary_message (progress) == NULL)
      {
        lw_progress_set_secondary_message_printf (progress, "Decompressing %s\n", SOURCE_PATH);
      }
      lw_progress_set_total (progress, content_length);
      lw_progress_set_current (progress, 0);
    }


    {
      gsize uncompressed_bytes_read = 0;
      gsize offset = 0;

      while ((uncompressed_bytes_read = gzread (source, buffer, chunk_size)) > 0)
      {
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
            lw_progress_set_current (progress, offset);
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
      lw_progress_set_current (progress, content_length);
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
    if (URI == NULL) return FALSE;
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    gboolean is_removed = FALSE;

    if (progress != NULL)
    {
      lw_progress_set_secondary_message (progress, URI);
      lw_progress_set_total (progress, 1.0);
      lw_progress_set_current (progress, 0.0);
    }

    g_remove (URI);

    is_removed = (g_file_test (URI, G_FILE_TEST_IS_REGULAR));

    if (!is_removed && progress != NULL)
    {
      lw_progress_take_error (progress, g_error_new (
        LW_IO_ERROR,
        LW_IO_ERRORCODE_FAILED_REMOVING_FILE,
        "Could not remove %s\n",
        URI
      ));
    }

    if (progress != NULL)
    {
      lw_progress_set_current (progress, 1.0);
    }

    return is_removed;
}


//!
//! @brief A quick way to get the number of lines in a file for use in progress functions
//! @param FILENAME The path to the file to see how many lines it has
//!
gsize
lw_io_get_file_size (char const * PATH)
{
    //Declarations
    FILE *stream = NULL;
    gsize size = 0;

    //Initializations
    stream = g_fopen (PATH, "rb");
    if (stream == NULL) goto errored;

    fseek (stream, 0, SEEK_END);
    size = ftell (stream);

errored:

    if (stream != NULL) fclose(stream); stream = NULL;
   
    return size;
}


gchar*
lw_io_allocate_temporary_file (gsize        bytes_length,
                               LwProgress  *progress)
{
    //Sanity checks
    g_return_val_if_fail (bytes_length > 0, NULL);

    //Initializations
    gchar *tmpl = NULL;
    gchar *path = NULL;
    gsize chunk_size = 0;
    gint fd = -1;
    gchar *buffer = NULL;
    gboolean has_error = FALSE;
    FILE *stream = NULL;

    //Declarations
    tmpl = g_strdup ("gwaei.XXXXXX");
    chunk_size = lw_progress_get_chunk_size (progress);
    if (chunk_size == 0) goto errored;
    path = g_build_path (g_get_tmp_dir (), tmpl, NULL);
    if (path == NULL) goto errored;
    fd = g_mkstemp (tmpl);
    if (fd < 0) goto errored;
    stream = fdopen (fd, "w+");
    if (stream == NULL) goto errored;
    fd = -1;
    buffer = g_new0 (gchar, chunk_size);
    if (buffer == NULL) goto errored;

    {
      gsize bytes_written = 0;
      gsize bytes_to_write = 0;
      while (bytes_length > 0)
      {
        bytes_to_write = (bytes_length > chunk_size) ? chunk_size : bytes_length;
        if (bytes_written != chunk_size && ferror(stream) != 0)
        {
          if (progress != NULL)
          {
            lw_progress_take_error (progress, g_error_new (
              G_FILE_ERROR,
              g_file_error_from_errno (ferror(stream)),
              "There was an error allocating the temporary file\n"
            ));
          }
          has_error = TRUE;
          goto errored;
        }
        bytes_length -= bytes_to_write;
      }
    }

errored:

    if (has_error && path != NULL)
    {
      g_remove (path);
      g_free (path);
      path = NULL;
    }

    g_free (tmpl); tmpl = NULL;

    fclose(stream); stream = NULL;

    return path;
}


gsize
lw_io_write_chunk_with_data (gchar               *chunk,
                             gsize                chunk_length,
                             LwIoWriteChunkData  *data,
                             GError             **error)
{
		//Sanity checks
		g_return_val_if_fail (chunk != NULL, 0);
		if (chunk_length < 1) return 0;
		if (error != NULL && *error != NULL) return 0;

    //Declarations
    gsize bytes_written = 0;
    FILE *stream = NULL;

    //Initialization
    stream = data->stream;
    if (stream == NULL) goto errored;
		bytes_written = fwrite(chunk, sizeof(gchar), chunk_length, stream);

    data->bytes_written += bytes_written;
		if (bytes_written != chunk_length && ferror(stream) != 0)
		{
			*error = g_error_new (
							G_FILE_ERROR,
							g_file_error_from_errno (ferror(stream)),
							"Could not write the dictionary cache temporary file\n"
			);
		}

errored:

		return bytes_written;
}
