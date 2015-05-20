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
 * SECTION:cachefile
 * @short_description: A mapped file that includes a checksum and is writable
 * @title: LwCacheFile
 *
 * This is a mapped file designed two have two modes, making it ideal for
 * handling data cached by the application that is read many times,
 * but only written once. When lw_cachefile_write() or
 * lw_cachefile_write_cachefile() is called, the #LwCacheFile is set to
 * writing mode, where the checksum is written first, then any contents
 * are written after.  Consecutive calls append to this file.  Once you
 * call lw_cachefile_read(), the #FILE steam is closed if it is open
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
#include <libwaei/cachefile.h>

#include <libwaei/cachefile-private.h>

G_DEFINE_TYPE (LwCacheFile, lw_cachefile, LW_TYPE_MAPPEDFILE)

/**
 * lw_cachefile_error_quark:
 * The cachefile error domain
 * Returns: The #GQuark representing a #LwCacheFile error
 */
GQuark
lw_cachefile_error_quark ()
{
    return g_quark_from_static_string ("lw-cachefile-error");
}


/**
 * lw_cachefile_new:
 * @PATH: The path to load the cachefile from or write to
 * Returns: (transfer full): A new #LwCacheFile that can be freed with g_object_unref()
 */
LwCacheFile *
lw_cachefile_new (gchar const * PATH)
{
    //Sanity checks
    g_return_val_if_fail (PATH != NULL, FALSE);

    //Declarations
    LwCacheFile *self = NULL;

    //Initializations
    self = LW_CACHEFILE (g_object_new (LW_TYPE_CACHEFILE,
      "path", PATH,
      "writable", TRUE,
      "delete-on-free", FALSE,
      NULL
    ));

    return self;
}


static void 
lw_cachefile_init (LwCacheFile * self)
{
    self->priv = LW_CACHEFILE_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwCacheFilePrivate));
}


static void 
lw_cachefile_finalize (GObject * object)
{
    //Declarations
    LwCacheFile * self = NULL;
    LwCacheFilePrivate * priv = NULL;

    //Initalizations
    self = LW_CACHEFILE (object);
    priv = self->priv;

    _ensure_fclose (self, NULL);

    memset(self->priv, 0, sizeof(LwCacheFilePrivate));

    G_OBJECT_CLASS (lw_cachefile_parent_class)->finalize (object);
}


static void 
lw_cachefile_set_property (GObject      * object,
                           guint          property_id,
                           const GValue * value,
                           GParamSpec   * pspec)
{
    //Declarations
    LwCacheFile * self = NULL;
    LwCacheFilePrivate * priv = NULL;

    //Initializations
    self = LW_CACHEFILE (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_cachefile_get_property (GObject      * object,
                           guint          property_id,
                           GValue       * value,
                           GParamSpec   * pspec)
{
    //Declarations
    LwCacheFile * self = NULL;
    LwCacheFilePrivate * priv = NULL;

    //Initializations
    self = LW_CACHEFILE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_CHECKSUM:
        g_value_set_string (value, lw_cachefile_get_checksum (self));
        break;
      case PROP_CONTENTS:
        g_value_set_string (value, lw_cachefile_get_contents (self));
        break;
      case PROP_CONTENT_LENGTH:
        g_value_set_ulong (value, lw_cachefile_length (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_cachefile_class_finalize (LwCacheFileClass *klass)
{
    memset(klass->priv, 0, sizeof(LwCacheFileClassPrivate));
    g_free (klass->priv);
    klass->priv = NULL;
}


static void
lw_cachefile_class_init (LwCacheFileClass * klass)
{
    //Declarations
    GObjectClass * object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwCacheFileClassPrivate, 1);
    object_class->set_property = lw_cachefile_set_property;
    object_class->get_property = lw_cachefile_get_property;
    object_class->finalize = lw_cachefile_finalize;

    g_type_class_add_private (object_class, sizeof (LwCacheFilePrivate));

    klass->priv->pspec[PROP_CHECKSUM] = g_param_spec_string (
      "checksum",
      gettext("Checksum"),
      "Checksum of the file",
      NULL,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CHECKSUM, klass->priv->pspec[PROP_CHECKSUM]);

    g_object_class_override_property (object_class, PROP_CONTENTS, "contents");
    g_object_class_override_property (object_class, PROP_CONTENT_LENGTH, "content-length");
}


/**
 * lw_cachefile_clear:
 * @self: A #LwCacheFile
 */
static void
lw_cachefile_clear (LwCacheFile * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_CACHEFILE (self));

    //Declarations
    LwCacheFilePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    lw_mappedfile_set_path (LW_MAPPEDFILE (self), NULL);
    if (priv->stream != NULL) fclose (priv->stream);
    priv->stream = NULL;
    priv->CHECKSUM = NULL;
    priv->contents = NULL;
    priv->content_length = 0;
}


/**
 * lw_cachefile_write:
 * @self: A #LwCacheFile
 * @CHECKSUM: The checksum string to write
 * @CONTENTS: The contents to write
 * @content_length: The length of the CONTENTS string + \0 or 0 to have it calculated automatically.
 * @progress: A #LwProgress to track progress or %NULL to ignore it
 * Returns: The number of bytes written
 */
gsize
lw_cachefile_write (LwCacheFile * self,
                    const gchar * CHECKSUM,
                    const gchar * CONTENTS,
                    gssize        content_length,
                    LwProgress  * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHEFILE (self), 0);
    g_return_val_if_fail (CHECKSUM != NULL, 0);
    g_return_val_if_fail (CONTENTS != NULL, 0);

    //Declarations
    LwCacheFilePrivate *priv = NULL;
    gboolean is_new_file = FALSE;
    FILE* stream = NULL;
    gsize bytes_written = 0;

    //Initializations
    priv = self->priv;
    is_new_file = (priv->stream == NULL);
    stream = _ensure_fopen (self, NULL);
    if (priv->stream == NULL) goto errored;

    if (is_new_file)
    {
      fwrite(CHECKSUM, 1, strlen(CHECKSUM) + 1, stream);
    }

    if (content_length < 0)
    {
      content_length = strlen (CONTENTS) + 1;
    }

    bytes_written = lw_io_fwrite (stream, CONTENTS, content_length, progress);

errored:

    return bytes_written;
}


/**
 * lw_cachefile_write_cachefile:
 * @self: A #LwCacheFile
 * @CHECKSUM: The checksum to write
 * @cache_file: Another cachefile to copy of the contents of
 * @progress: A #LwProgress to track progress or %NULL to ignore it
 * Returns: The number of bytes written
 */
gsize
lw_cachefile_write_cachefile (LwCacheFile  * self,
                              const gchar  * CHECKSUM,
                              LwCacheFile  * cache_file,
                              LwProgress   * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHEFILE (self), 0);
    g_return_val_if_fail (LW_IS_CACHEFILE (cache_file), 0);

    //Declarations
    gchar * contents = NULL;
    gsize content_length = 0;

    //Initializations
    contents = lw_cachefile_get_contents (cache_file);
    content_length = lw_cachefile_length (cache_file);

    return lw_cachefile_write (self, CHECKSUM, contents, content_length, progress);
}


/**
 * lw_cachefile_validate:
 * @self: A #LwCacheFile
 * @EXPECTED_CHECKSUM: The checksum that the cachefile should have
 * @progress: A #LwProgress to track progress or %NULL to ignore it
 * Returns: %TRUE if the cachefile is valid with the checksum
 */
gboolean
lw_cachefile_validate (LwCacheFile * self,
                       gchar const * EXPECTED_CHECKSUM,
                       LwProgress  * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHEFILE (self), FALSE);
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
    priv = self->priv;
    PATH = lw_mappedfile_get_path (LW_MAPPEDFILE (self));
    CHECKSUM = priv->CHECKSUM;
    contents = priv->contents;
    length = priv->content_length;

    if (CHECKSUM == NULL)
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, g_error_new (
          LW_CACHEFILE_ERROR,
          LW_CACHEFILE_ERRORCODE_INVALID_CHECKSUM,
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
          LW_CACHEFILE_ERROR,
          LW_CACHEFILE_ERRORCODE_INVALID_CHECKSUM,
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
          LW_CACHEFILE_ERROR,
          LW_CACHEFILE_ERRORCODE_CORRUPT_CONTENTS,
          "The contents of the cachefile are missing %s",
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
 * lw_cachefile_read:
 * @self: A #LwCacheFile
 * @EXPECTED_CHECKSUM: The checksum the read file should contain
 * @progress: A #LwProgress to track progress or %NULL to ignore it
 * Returns: The number of bytes read
 */
gsize
lw_cachefile_read (LwCacheFile * self,
                   const gchar * EXPECTED_CHECKSUM,
                   LwProgress  * progress)
{
    g_return_val_if_fail (LW_IS_CACHEFILE (self), NULL);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, NULL);
    if (progress != NULL && lw_progress_should_abort (progress)) return NULL;

    //Declarations
    LwCacheFilePrivate *priv = NULL;
    GError * error = NULL;
    gsize length = 0;

    //Initializations
    priv = self->priv;

    _ensure_fclose (self, &error);
    if (error != NULL)
    {
      if (error != NULL && progress != NULL)
      {
        lw_progress_take_error (progress, error);
        error = NULL;
      }
      g_clear_error (&error);
      goto errored;
    }

    priv->content_length = lw_mappedfile_length (LW_MAPPEDFILE (self));
    if (priv->content_length == 0) goto errored;
    priv->contents = lw_mappedfile_get_contents (LW_MAPPEDFILE (self));
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

    if (!lw_cachefile_validate (self, EXPECTED_CHECKSUM, progress))
    {
      lw_cachefile_clear (self);
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
    g_return_val_if_fail (LW_IS_CACHEFILE (self), NULL);

    //Declarations
    LwCacheFilePrivate * priv = NULL;
    LwMappedFile * mapped_file = NULL;
    gchar const * PATH = NULL;

    //Initializations
    priv = self->priv;
    mapped_file = LW_MAPPEDFILE (self);
    PATH = lw_mappedfile_get_path (mapped_file);
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
    g_return_if_fail (LW_IS_CACHEFILE (self));

    //Declarations
    LwCacheFilePrivate *priv = NULL;
    LwMappedFile *mapped_file = NULL;
    gchar const * PATH = NULL;

    //Initializations
    priv = self->priv;
    mapped_file = LW_MAPPEDFILE (self);
    PATH = lw_mappedfile_get_path (mapped_file);

    if (priv->stream != NULL)
    {
      fclose (priv->stream);
      priv->stream = NULL;
    }
    if (PATH != NULL)
    {
      if (g_file_test (PATH, G_FILE_TEST_IS_REGULAR))
      {
        if (!lw_mappedfile_is_writable (mapped_file)) g_chmod (PATH, 0444);
      }
    }
}


//Properties


/**
 * lw_cachefile_get_checksum:
 * @self: A #LwCacheFile
 * Returns: The checksum of the cachefile
 */
gchar const *
lw_cachefile_get_checksum (LwCacheFile * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHEFILE (self), NULL);

    //Declarations
    LwCacheFilePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->CHECKSUM;
}


/**
 * lw_cachefile_get_contents:
 * @self: A #LwCacheFile
 * Returns: (no transfer): The contents of the cachefile.  This string is owned by the #LwCacheFile and should not be freed
 */
gchar *
lw_cachefile_get_contents (LwCacheFile * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHEFILE (self), NULL);

    //Declarations
    LwCacheFilePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->contents;
}


/**
 * lw_cachefile_length:
 * @self: A #LwCacheFile
 * Returns: The number of bytes in the cachefile content body.  Use lw_mappedfile_length() if you want the full size of the mapped file.
 */
gsize
lw_cachefile_length (LwCacheFile * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHEFILE (self), 0);

    //Declarations
    LwCacheFilePrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (priv->CHECKSUM >= priv->contents, 0) return 0;

    return priv->content_length;
}
