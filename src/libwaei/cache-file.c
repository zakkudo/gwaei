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


/**
 * SECTION:cache_file
 * @short_description: A mapped file that includes a checksum and is writable
 * @title: LwCacheFile
 *
 * This is a mapped file designed two have two modes, making it ideal for
 * handling data cached by the application that is read many times,
 * but only written once. When lw_cache_file_write() or
 * lw_cache_file_write_cache_file() is called, the #LwCacheFile is set to
 * writing mode, where the checksum is written first, then any contents
 * are written after.  Consecutive calls append to this file.  Once you
 * call lw_cache_file_read(), the #FILE steam is closed if it is open
 * and the file is verified as matching the supplied checksum in the
 * header and that the contents are valid utf8.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include <stdlib.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/io.h>
#include <libwaei/utf8.h>
#include <libwaei/cache-file.h>

#define lw_cache_file_class_get_private() G_TYPE_CLASS_GET_PRIVATE(g_type_class_peek(LW_TYPE_CACHE_FILE), LW_TYPE_CACHE_FILE, LwCacheFileClassPrivate)

typedef enum
{
  PROP_0,
  PROP_PATH,
  PROP_CHECKSUM,
  PROP_CONTENTS,
  PROP_CONTENT_LENGTH,
  TOTAL_PROPS
} LwCacheFileProps;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwCacheFile {
    LwMappedFile parent;
};

typedef struct {
  FILE * stream;
  gchar const * CHECKSUM;
  gsize content_length;
  gchar * contents;
} LwCacheFilePrivate;

typedef struct {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];
} LwCacheFileClassPrivate;

static FILE* _ensure_fopen (LwCacheFile *self, GError **error);
void static _ensure_fclose (LwCacheFile *self, GError **error);

G_DEFINE_TYPE_WITH_CODE (LwCacheFile, lw_cache_file, G_TYPE_OBJECT, G_ADD_PRIVATE(LwCacheFile) g_type_add_class_private(LW_TYPE_CACHE_FILE, sizeof(LwCacheFileClassPrivate)) )

/**
 * lw_cache_file_error_quark:
 * The cache_file error domain
 * Returns: The #GQuark representing a #LwCacheFile error
 */
GQuark
lw_cache_file_error_quark ()
{
    return g_quark_from_static_string ("lw-cache_file-error");
}


/**
 * lw_cache_file_new:
 * @PATH: (type filename) (transfer none): The path to load the cache_file from or write to
 * Returns: (transfer full): A new #LwCacheFile that can be freed with g_object_unref()
 */
LwCacheFile *
lw_cache_file_new (gchar const * PATH)
{
    //Sanity checks
    g_return_val_if_fail (PATH != NULL, FALSE);

    //Declarations
    LwCacheFile *self = NULL;

    //Initializations
    self = LW_CACHE_FILE (g_object_new (LW_TYPE_CACHE_FILE,
      "path", PATH,
      "writable", TRUE,
      "delete-on-free", FALSE,
      NULL
    ));

    return self;
}


static void 
lw_cache_file_init (LwCacheFile * self)
{
}


static void 
lw_cache_file_finalize (GObject * object)
{
    //Declarations
    LwCacheFile * self = NULL;
    LwCacheFilePrivate * priv = NULL;

    //Initalizations
    self = LW_CACHE_FILE (object);
    priv = lw_cache_file_get_instance_private (self);

    _ensure_fclose (self, NULL);

    G_OBJECT_CLASS (lw_cache_file_parent_class)->finalize (object);
}


static void 
lw_cache_file_set_property (GObject      * object,
                           guint          property_id,
                           const GValue * value,
                           GParamSpec   * pspec)
{
    //Declarations
    LwCacheFile * self = NULL;
    LwCacheFilePrivate * priv = NULL;

    //Initializations
    self = LW_CACHE_FILE (object);
    priv = lw_cache_file_get_instance_private (self);

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_cache_file_get_property (GObject      * object,
                           guint          property_id,
                           GValue       * value,
                           GParamSpec   * pspec)
{
    //Declarations
    LwCacheFile * self = NULL;
    LwCacheFilePrivate * priv = NULL;

    //Initializations
    self = LW_CACHE_FILE (object);
    priv = lw_cache_file_get_instance_private (self);

    switch (property_id)
    {
      case PROP_CHECKSUM:
        g_value_set_string (value, lw_cache_file_get_checksum (self));
        break;
      case PROP_CONTENTS:
        g_value_set_string (value, lw_cache_file_get_contents (self));
        break;
      case PROP_CONTENT_LENGTH:
        g_value_set_ulong (value, lw_cache_file_length (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_cache_file_class_init (LwCacheFileClass * klass)
{
    //Declarations
    GObjectClass * object_class = NULL;
    LwCacheFileClassPrivate * klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klasspriv = lw_cache_file_class_get_private ();

    object_class->set_property = lw_cache_file_set_property;
    object_class->get_property = lw_cache_file_get_property;
    object_class->finalize = lw_cache_file_finalize;

    klasspriv->pspec[PROP_CHECKSUM] = g_param_spec_string (
      "checksum",
      gettext("Checksum"),
      "Checksum of the file",
      NULL,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CHECKSUM, klasspriv->pspec[PROP_CHECKSUM]);

    g_object_class_override_property (object_class, PROP_CONTENTS, "contents");
    g_object_class_override_property (object_class, PROP_CONTENT_LENGTH, "content-length");
}


/**
 * lw_cache_file_clear:
 * @self: A #LwCacheFile
 */
static void
lw_cache_file_clear (LwCacheFile * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_CACHE_FILE (self));

    //Declarations
    LwCacheFilePrivate *priv = NULL;

    //Initializations
    priv = lw_cache_file_get_instance_private (self);

    lw_mapped_file_set_path (LW_MAPPED_FILE (self), NULL);
    if (priv->stream != NULL) fclose (priv->stream);
    priv->stream = NULL;
    priv->CHECKSUM = NULL;
    priv->contents = NULL;
    priv->content_length = 0;
}


/**
 * lw_cache_file_write:
 * @self: A #LwCacheFile
 * @CHECKSUM: (transfer none): The checksum string to write
 * @CONTENTS: (transfer none): The contents to write
 * @content_length: The length of the CONTENTS string + \0 or 0 to have it calculated automatically.
 * @progress: (allow-none) (transfer none): A #LwProgress to track progress or %NULL to ignore it
 * Returns: The number of bytes written
 */
gsize
lw_cache_file_write (LwCacheFile * self,
                    const gchar * CHECKSUM,
                    const gchar * CONTENTS,
                    gssize        content_length,
                    LwProgress  * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHE_FILE (self), 0);
    g_return_val_if_fail (CHECKSUM != NULL, 0);
    g_return_val_if_fail (CONTENTS != NULL, 0);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, 0);

    //Declarations
    LwCacheFilePrivate *priv = NULL;
    gboolean is_new_file = FALSE;
    FILE* stream = NULL;
    gsize bytes_written = 0;
    gboolean has_error = FALSE;

    //Initializations
    priv = lw_cache_file_get_instance_private (self);
    is_new_file = (priv->stream == NULL);
    stream = _ensure_fopen (self, NULL);
    if (priv->stream == NULL) goto errored;

    if (is_new_file)
    {
      fwrite(CHECKSUM, 1, strlen(CHECKSUM) + 1, stream);
      if (ferror(stream))
      {
        GError * error = g_error_new (
          G_FILE_ERROR,
          g_file_error_from_errno (ferror(stream)),
          "Could not write cache_file checksum"
        );
        LW_PROGRESS_TAKE_ERROR (progress, error);
      }
    }

    if (content_length < 1)
    {
      content_length = strlen (CONTENTS) + 1;
    }

    bytes_written = lw_io_fwrite_chunked (stream, CONTENTS, content_length, progress);
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);

errored:

    return bytes_written;
}


/**
 * lw_cache_file_write_cache_file:
 * @self: A #LwCacheFile
 * @CHECKSUM: (transfer none): The checksum to write
 * @cache_file: (transfer none): Another cache_file to copy of the contents of
 * @progress: (transfer none) (allow-none):A #LwProgress to track progress or %NULL to ignore it
 * Returns: The number of bytes written
 */
gsize
lw_cache_file_write_cache_file (LwCacheFile  * self,
                              const gchar  * CHECKSUM,
                              LwCacheFile  * cache_file,
                              LwProgress   * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHE_FILE (self), 0);
    g_return_val_if_fail (LW_IS_CACHE_FILE (cache_file), 0);

    //Declarations
    gchar * contents = NULL;
    gsize content_length = 0;

    //Initializations
    contents = lw_cache_file_get_contents (cache_file);
    content_length = lw_cache_file_length (cache_file);

    return lw_cache_file_write (self, CHECKSUM, contents, content_length, progress);
}


/**
 * lw_cache_file_validate:
 * @self: A #LwCacheFile
 * @EXPECTED_CHECKSUM: (transfer none): The checksum that the cache_file should have
 * @progress: (transfer none) (allow-none): A #LwProgress to track progress or %NULL to ignore it
 * Returns: %TRUE if the cache_file is valid with the checksum
 */
gboolean
lw_cache_file_validate (LwCacheFile * self,
                       gchar const * EXPECTED_CHECKSUM,
                       LwProgress  * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHE_FILE (self), FALSE);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, FALSE);
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    LwCacheFilePrivate *priv = NULL;
    gboolean is_valid = TRUE;
    gchar const * PATH = NULL;
    gchar const * CHECKSUM = NULL;
    gchar const * CONTENTS = NULL;
    gchar * contents = NULL;
    gsize length = 0;

    //Initializations
    priv = lw_cache_file_get_instance_private (self);
    PATH = lw_mapped_file_get_path (LW_MAPPED_FILE (self));
    CHECKSUM = priv->CHECKSUM;
    contents = priv->contents;
    length = priv->content_length;

    if (CHECKSUM == NULL)
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, g_error_new (
          LW_CACHE_FILE_ERROR,
          LW_CACHE_FILE_ERRORCODE_INVALID_CHECKSUM,
          "The checksum of the cache file %s is corrupt. It should be valid utf8",
          PATH
        ));
      }
      is_valid = FALSE;
      goto errored;
    }

    if (!lw_utf8_validate (CHECKSUM, length, progress))
    {
      is_valid = FALSE;
      goto errored;
    }

    if (g_strcmp0 (EXPECTED_CHECKSUM, CHECKSUM) != 0)
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, g_error_new (
          LW_CACHE_FILE_ERROR,
          LW_CACHE_FILE_ERRORCODE_INVALID_CHECKSUM,
          "The checksum for %s was different from exected\n",
          PATH
        ));
      }
      is_valid = FALSE;
      goto errored;
    }

    if (priv->contents == NULL)
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, g_error_new (
          LW_CACHE_FILE_ERROR,
          LW_CACHE_FILE_ERRORCODE_CORRUPT_CONTENTS,
          "The contents of the cache_file are missing %s",
          PATH
        ));
      }
      is_valid = FALSE;
      goto errored;
    }

errored:

    return is_valid;
}


/**
 * lw_cache_file_read:
 * @self: A #LwCacheFile
 * @EXPECTED_CHECKSUM: (transfer none): The checksum the read file should contain
 * @progress: (transfer none) (allow-none): A #LwProgress to track progress or %NULL to ignore it
 * Returns: The number of bytes read
 */
gsize
lw_cache_file_read (LwCacheFile * self,
                   const gchar * EXPECTED_CHECKSUM,
                   LwProgress  * progress)
{
    g_return_val_if_fail (LW_IS_CACHE_FILE (self), 0);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, 0);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, 0);

    //Declarations
    LwCacheFilePrivate *priv = NULL;
    GError * error = NULL;
    gsize length = 0;
    gboolean has_error = FALSE;

    //Initializations
    priv = lw_cache_file_get_instance_private (self);

    _ensure_fclose (self, &error);
    LW_PROGRESS_TAKE_ERROR (progress, error);

    priv->content_length = lw_mapped_file_length (LW_MAPPED_FILE (self));
    if (priv->content_length == 0) goto errored;
    priv->contents = lw_mapped_file_get_contents (LW_MAPPED_FILE (self));
    if (priv->contents == NULL) goto errored;
    priv->CHECKSUM = priv->contents;
    if (priv->CHECKSUM == NULL) goto errored;

    {
      gint i = 0;
      while (i < priv->content_length && priv->contents[i] != '\0') i++;
      if (priv->contents[i] == '\0' && i < priv->content_length)
      {
        priv->contents = priv->contents + i + 1;
        priv->content_length = priv->content_length - (priv->contents - priv->CHECKSUM);
      }
      else
      {
        priv->contents = NULL;
        priv->content_length = 0;
      }
    }

    if (!lw_cache_file_validate (self, EXPECTED_CHECKSUM, progress))
    {
      lw_cache_file_clear (self);
      goto errored;
    }

errored:

    return priv->content_length;
}



static FILE *
_ensure_fopen (LwCacheFile  * self,
               GError      ** error)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHE_FILE (self), NULL);

    //Declarations
    LwCacheFilePrivate * priv = NULL;
    LwMappedFile * mapped_file = NULL;
    gchar const * PATH = NULL;

    //Initializations
    priv = lw_cache_file_get_instance_private (self);
    mapped_file = LW_MAPPED_FILE (self);
    PATH = lw_mapped_file_get_path (mapped_file);
    priv->contents = NULL;
    priv->content_length = 0;

    if (g_file_test (PATH, G_FILE_TEST_IS_REGULAR))
    {
      g_chmod (PATH, 0644);
    }

    if (priv->stream == NULL)
    {
      priv->stream = fopen(PATH, "w+");
    }

    return priv->stream;
}


void static
_ensure_fclose (LwCacheFile  * self,
                GError      ** error)
{
    //Sanity checks
    g_return_if_fail (LW_IS_CACHE_FILE (self));

    //Declarations
    LwCacheFilePrivate *priv = NULL;
    LwMappedFile *mapped_file = NULL;
    gchar const * PATH = NULL;

    //Initializations
    priv = lw_cache_file_get_instance_private (self);
    mapped_file = LW_MAPPED_FILE (self);
    PATH = lw_mapped_file_get_path (mapped_file);

    if (priv->stream != NULL)
    {
      fclose (priv->stream);
      priv->stream = NULL;
    }
    if (PATH != NULL)
    {
      if (g_file_test (PATH, G_FILE_TEST_IS_REGULAR))
      {
        if (!lw_mapped_file_is_writable (mapped_file)) g_chmod (PATH, 0444);
      }
    }
}


//Properties


/**
 * lw_cache_file_get_checksum:
 * @self: A #LwCacheFile
 * Returns: (transfer none): The checksum of the cache_file.  This string is owned by the cache_file and it should not be modified or freed.
 */
gchar const *
lw_cache_file_get_checksum (LwCacheFile * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHE_FILE (self), NULL);

    //Declarations
    LwCacheFilePrivate *priv = NULL;

    //Initializations
    priv = lw_cache_file_get_instance_private (self);

    return priv->CHECKSUM;
}


/**
 * lw_cache_file_get_contents:
 * @self: A #LwCacheFile
 * Returns: (transfer none): The contents of the #LwCacheFile.
 */
gchar *
lw_cache_file_get_contents (LwCacheFile * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHE_FILE (self), NULL);

    //Declarations
    LwCacheFilePrivate *priv = NULL;

    //Initializations
    priv = lw_cache_file_get_instance_private (self);

    return priv->contents;
}


/**
 * lw_cache_file_length:
 * @self: A #LwCacheFile
 * Returns: The number of bytes in the cache_file content body.  Use lw_mapped_file_length() if you want the full size of the mapped file.
 */
gsize
lw_cache_file_length (LwCacheFile * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHE_FILE (self), 0);

    //Declarations
    LwCacheFilePrivate *priv = NULL;

    //Initializations
    priv = lw_cache_file_get_instance_private (self);
    if (priv->CHECKSUM >= priv->contents, 0) return 0;

    return priv->content_length;
}
