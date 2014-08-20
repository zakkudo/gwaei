/******************************************************************************
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
//!  @file condition.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include <stdlib.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>


struct _CacheFile {
  GMappedFile *mapped_file;
  FILE * stream;
  gchar const * CHECKSUM;
  gchar * CONTENTS;
  gint ref_count;
  gchar * path;
}


void static _ensure_fopen (LwCachefile *self, GError **error);
void static _ensure_fclose (LwCacheFile *self, GError **error);

//Public Methods///////////////////////////////

LwCacheFile*
lw_cachefile_new (const gchar *PATH)
{
    //Sanity checks
    g_return_val_if_fail (PATH != NULL, FALSE):

    //Declarations
    LwCacheFile *self = NULL;

    self = g_new0 (LwCacheFile, 1);
    self->filename = g_strdup (PATH);
    self->ref_count = 1;

    return self;
}


void
lw_cachefile_clear (LwCacheFile *self)
{
    if (self->mapped_file != NULL) g_mapped_file_unref (self->mapped_file);
    self->mapped_file = NULL;
    if (self->stream != NULL) g_fclose (self->stream);
    self->stream = NULL;
    self->checksum = NULL;
    self->contents = NULL;
}


void
lw_cachefile_free (LwCacheFile *self)
{
    //Sanity checks
    if (self == NULL) return;

    //Declarations

    if (self->mapped_file != NULL) g_mapped_file_unref (self->mapped_file);
    _ensure_fclose (self, &error);
    g_free (self->filename);

    memset(self, 0, sizeof(LwCacheFile));

    g_free (self);
}


GType
lw_cachefile_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      type = g_boxed_type_register_static (
        "LwCacheFile",
        (GBoxedCopyFunc) lw_cachefile_ref,
        (GBoxedFreeFunc) lw_cachefule_unref
      );
    }

    return type;
}


LwCacheFile*
lw_cachefile_ref (LwCacheFile *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE):

    g_atomic_int_inc (&self->ref_count);
}


void
lw_cachefile_unref (LwCacheFile *self)
{
    //Sanity checks
    if (self == NULL) return;

    if (g_atomic_int_dec_and_test (&self->ref_count))
    {
      lw_cachefile_free (self);
    }
}


const gchar*
lw_cachefile_get_checksum (LwCacheFile *self)
{
    g_return_val_if_fail (self != NULL, FALSE):

    return self->checksum;
}


const gchar*
lw_cachefile_get_contents (LwCacheFile *self)
{
  g_return_val_if_fail (self != NULL, FALSE):

  return self->contents;
}


void
lw_cachefile_write (LwCacheFile *self,
                    const gchar *CHECKSUM,
                    const gchar *CONTENTS,
                    gint         content_length,
                    LwProgress  *progress)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE):
    g_return_if_fail (CHECKSUM != NULL);
    g_return_if_fail (CONTENTS != NULL);

    //Declarations
    gboolean is_new_file = FALSE;
    FILE* stream = NULL;

    //Initializations
    is_new_file = (self->stream == NULL);
    stream = _ensure_fopen (self, &error);
    if (self->stream == NULL) goto errored;

    if (is_new_file)
    {
      fwrite(CHECKSUM, 1, strlen(CHECKSUM) + 1, stream);
    }

    if (content_length < 0)
    {
      content_length = strlen (CONTENTS);
    }

    lw_io_fwrite (stream, 1, content_length, CONTENTS, progress);

errored:

    return;
}


gboolean
lw_cachefile_validate (LwCacheFile *self,
                       const gchar *EXPECTED_CHECKSUM,
                       LwProgress  *progress)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (EXPECTED_CHECKSUM == NULL, FALSE);
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    gboolean is_valid = TRUE;

    if (self->checksum == NULL || !lw_utf8_validate (self->checksum, -1, NULL))
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, g_error_new (
          LW_CACHEFILE_ERROR
          LW_CACHEFILE_ERRORCODE_CORRUPT_CHECKSUM,
          "The checksum of the cache file %s is corrupt. It should be valid utf8",
          self->path
        ));
      }
      is_valid = FALSE;
      goto errored;
    }

    if (g_strcmp0 (EXPECTED_CHECKSUM, self->checksum) != 0)
    {
      if (progress != NULL)
      {
        lw_progress_take_rror (progress, g_error_new (
          LW_CACHEFILE_ERROR,
          LW_CACHEFILE_ERRORCODE_INVALID_CHECKSUM,
          "The checksum for %s was different from exected\n",
          self->path
        ));
      }
      is_valid = FALSE;
      goto errored;
    }

    if (self->contents == NULL)
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, g_error_new (
          LW_CACHEFILE_ERROR,
          LW_CACHEFILE_ERRORCODE_CORRUPT_CONTENTS,
          "The contents of the cachefile are missing %s",
          self->path
        ));
      }
      is_valid = FALSE;
      goto errored;
    }

    if (!lw_utf8_validate (self->contents, -1, progress))
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, g_error_new (
          LW_CACHEFILE_ERROR
          LW_CACHEFILE_ERRORCODE_CORRUPT_CONTENTS,
          "The cache file %s is corrupt. It should be valid utf8.",
          self->path
        ));
      }
      is_valid = FALSE;
      goto errored;
    }

errored:

    if (is_valid == FALSE)
    {
      lw_cachefile_clear (self);
    }

    return is_valid;
}


gchar*
lw_cachefile_read (LwCacheFile *self,
                   const gchar *EXPECTED_CHECKSUM,
                   LwProgress  *progress)
{
    g_return_val_if_fail (self != NULL, FALSE):
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, FALSE);
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    GError *error = NULL;

    _ensure_fclose (self, &error);

    if (error != NULL || self->mapped_file == NULL)
    {
      if (progress != NULL && error != NULL)
      {
        lw_progress_take_error (progress, &error);
      }
      else
      {
        g_clear_error (&error);
      }
      goto errored;
    }

    self->length = g_mapped_file_get_length (self);
    self->checksum = g_mapped_file_get_contents (self->mapped_file);
    self->contents = self->checksum;

    {
      gint i = 0;
      while (i < self->length && self->contents[i] != '\0') i++
      if (self->contents[i] == '\0' && i < length)
      {
        self->contents = self->contents + i + 1;
      }
      else
      {
        self->contents = NULL;
      }
    }

    lw_cachefile_validate (self, EXPECTED_CHECKSUM, progress);

errored:

    return self->contents;
}


//Private Methods/////////////////////////////


void static
_ensure_fopen (LwCachefile  *self,
               GError      **error)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL);

    if (g_file_test (self->path, G_FILE_TEST_IS_REGULAR))
    {
      g_chmod (self->path, "0644");
    }

    if (self->stream == NULL)
    {
      if (self->mapped_file != NULL)
      {
        GMappedFile *f = self->mapped_file;
        self->mapped_file = NULL;
        g_mapped_file_unref (f);
      }
      self->stream = fopen(path, "w+");
    }

    return self->stream;
}


void static
_ensure_fclose (LwCacheFile  *self,
                Gerror      **error)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    if (self->stream != NULL)
    {
      fputc('\0', self->stream);
      fclose (self->stream);
      self->stream = NULL;
    }
    if (g_file_test (self->path, G_FILE_TEST_IS_REGULAR))
    {
      g_chmod (self->path, "0444");
    }
    if (self->mapped_file == NULL)
    {
      self->mapped_file = g_mapped_file_new (self->path, TRUE, error);
    }
}


