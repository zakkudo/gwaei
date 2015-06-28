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
#ifdef G_OS_WIN32
#include <windows.h>
#endif

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>


/**
 * lw_io_error_quark:
 *
 * Returns: a #GQuark representing a standard lw io domain error
 */
GQuark
lw_io_error_quark ()
{
    return g_quark_from_static_string ("lw-io-error");
}


/**
 * lw_io_get_pagesize:
 *
 * Tries to find the the page size of the os.  If it fails,
 * it defaults to 1024 * 4 bytes.
 * 
 * Returns: The page size of the system
 */
gsize
lw_io_get_pagesize ()
{
    gsize pagesize = 0;

#ifdef G_OS_UNIX
    pagesize = sysconf(_SC_PAGESIZE);
#endif
#ifdef G_OS_WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    pagesize = si.dwPageSize;
#endif

    if (pagesize <= 0)
    {
      pagesize = 1024 * 4;
    }

    return pagesize;
}


/**
 * lw_io_fwrite_chunk:
 * @stream: #FILE to write to
 * @CHUNK: Text to write to the @stream
 * @chunk_length: The length of @CHUNK in bytes
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
lw_io_fwrite_chunk (FILE        * stream,
                    gchar const * CHUNK,
                    gsize         chunk_length,
                    GError     ** error)
{
		//Sanity checks
    g_return_val_if_fail (stream != NULL, 0);
		g_return_val_if_fail (CHUNK != NULL, 0);
		if (chunk_length < 1) return 0;
		if (error != NULL && *error != NULL) return 0;

    //Declarations
    gsize bytes_written = 0;

    //Initialization
		bytes_written = fwrite(CHUNK, sizeof(gchar), chunk_length, stream);

		if (bytes_written != chunk_length || ferror(stream) != 0)
		{
			*error = g_error_new (
							G_FILE_ERROR,
							g_file_error_from_errno (ferror(stream)),
							"Could not write the file\n"
			);
      goto errored;
		}

errored:

		return bytes_written;
}


/**
 * lw_io_fwrite_chunked:
 * @stream: A #FILE stream to write to
 * @CONTENTS: (no transfer): The chunk to write
 * @content_length: The number of bytes to write of the @CONTENTS
 * @progress: (transfer none) (allow-none): A #LwProgress to track write progress or %NULL to ignore it
 * Returns: The number of bytes written
 */
gsize
lw_io_fwrite_chunked (FILE             * stream,
                      gchar const      * CONTENTS,
                      gsize              content_length,
                      LwProgress       * progress)
{
		//Sanity checks
		g_return_val_if_fail (CONTENTS != NULL, 0);
    if (content_length == 0) return 0;
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, 0);

    gsize max_chunk = LW_PROGRESS_START (progress, content_length);
    gsize chunk = 0;
    gsize total_bytes_written = 0;
    gsize bytes_written = 0;
    gsize bytes_handled = 0;
    gsize current = 0;
    gsize bytes_to_write = 0;
    gsize left = 0;
    gchar const * C = CONTENTS;
    GError * error = NULL;

    while (current < content_length)
    {
      left = content_length - current;
      bytes_to_write = (left > max_chunk) ? max_chunk : left;
      bytes_written = lw_io_fwrite_chunk (stream, C, bytes_to_write, &error);
      total_bytes_written += bytes_written;
      chunk += bytes_written;
      C += bytes_written;
      current = C - CONTENTS;

      LW_PROGRESS_UPDATE (progress, current, chunk, max_chunk, error);
    }

    LW_PROGRESS_FINISH (progress, current);

errored:

    return total_bytes_written;
}


/**
 * lw_io_write_file:
 * @PATH: (transfer none) (type filename): The file path to write to
 * @MODE: (transfer none): The mode of the file
 * @CONTENTS: (transfer none): The content to write
 * @content_length: The byte length of @CONTENTS
 * @progress: (transfer none) (allow-none): A #LwProgress to track progress or %NULL to ignore it
 *
 * A convenience method to write to a file path.
 *
 * Returns: The number of bytes written
 */
gsize
lw_io_write_file (const gchar * PATH, 
                  const gchar * MODE,
                  const gchar * CONTENTS, 
                  gsize         content_length,
                  LwProgress  * progress)
{
    //Sanity checks
    g_return_val_if_fail (PATH != NULL, 0);
    g_return_val_if_fail (MODE != NULL, 0);
    g_return_val_if_fail (CONTENTS != NULL, 0);

    //Declarations
    FILE *stream = NULL;
    gsize bytes_written = 0;

    //Initializations
    stream = g_fopen (PATH, "wb");
    if (stream == NULL) goto errored;
    bytes_written = lw_io_fwrite_chunked (stream, CONTENTS, content_length, progress);
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);

errored:

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
    if (progress != NULL) chunk_size = lw_progress_get_chunk_size (progress);
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
        if (progress != NULL && chunk_size > 0)
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
    
    if (progress != NULL)
    {
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
    }

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
    if (progress != NULL) chunk_size = lw_progress_get_chunk_size (progress);

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
    gsize chunk_size = 0;
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
    if (progress != NULL) chunk_size = lw_progress_get_chunk_size (progress);

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


/**
 * lw_io_get_file_size:
 * @PATH: (transfer none) (type filename): The path of the file to calculate the size of
 * Returns: The size of the file or 0 if there was an error
 */
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


/**
 * lw_io_allocate_temporary_file:
 * @bytes_length: The number of bytes to allocate
 * @progress: (transfer none) (allow-none): A #LwProgress to track allocation progress or %NULL to ignore it
 * Returns: (transfer full): Returns the file path of the allocated temporary file.  The string should be freed with g_free() when finished.
 */
gchar *
lw_io_allocate_temporary_file (gsize        bytes_length,
                               LwProgress * progress)
{
    //Sanity checks
    g_return_val_if_fail (bytes_length > 0, NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);

    //Initializations
    gchar *path = NULL;
    gsize max_chunk = 0;
    gint fd = -1;
    gchar *buffer = NULL;
    gboolean has_error = FALSE;
    FILE *stream = NULL;
    gsize current = 0;
    GError * error = NULL;

    //Declarations
    max_chunk = LW_PROGRESS_START (progress, bytes_length);
    path = g_build_filename (g_get_tmp_dir (), "gwaei.XXXXXX", NULL);
    if (path == NULL) goto errored;
    fd = g_mkstemp (path);
    if (fd < 0) goto errored;
    stream = fdopen (fd, "w");
    if (stream == NULL) goto errored;
    fd = -1;

    buffer = g_new0 (gchar, max_chunk);
    if (buffer == NULL) goto errored;
    
    {
      gsize bytes_written = 0;
      gsize bytes_to_write = 0;
      gsize chunk = 0;
      while (bytes_length > 0)
      {
        bytes_to_write = (bytes_length > max_chunk) ? max_chunk : bytes_length;
        bytes_written = lw_io_fwrite_chunk (stream, buffer, bytes_to_write, &error);
        chunk += bytes_written;
        current += bytes_written;

        LW_PROGRESS_UPDATE (progress, current, chunk, max_chunk, error);

        bytes_length -= bytes_written;
      }
    }

    LW_PROGRESS_FINISH (progress, current);

errored:

    if (has_error && path != NULL)
    {
      g_remove (path);
      g_free (path);
      path = NULL;
    }

    //close(fd); fd = -1;
    if (stream != NULL) fclose(stream);
    stream = NULL;

    g_free (buffer);

    return path;
}
