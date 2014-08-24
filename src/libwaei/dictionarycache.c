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
//! @file dictionarycache.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>

#include <libwaei/dictionarycache-private.h>

G_DEFINE_TYPE (LwDictionaryCache, lw_dictionarycache, G_TYPE_OBJECT)


GQuark
lw_dictionarycache_error_quark ()
{
    return g_quark_from_static_string ("lw-dictionarycache-error");
}


LwDictionaryCache*
lw_dictionarycache_new (const gchar *NAME,
                        LwUtf8Flag   flags)
{
    //Declarations
    LwDictionaryCache *self = NULL;

    //Initializations
    self = LW_DICTIONARYCACHE (g_object_new (LW_TYPE_DICTIONARYCACHE,
      "name", NAME,
      "normalization-flags", flags,
      NULL
    ));

    return self;
}


static void 
lw_dictionarycache_init (LwDictionaryCache *self)
{
    self->priv = LW_DICTIONARYCACHE_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwDictionaryCachePrivate));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;
}


static void 
lw_dictionarycache_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LwDictionaryCache *self = NULL;
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYCACHE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_NAME:
        lw_dictionarycache_set_name (self, g_value_get_string (value));
        break;
      case PROP_FLAGS:
        lw_dictionarycache_set_flags (self, g_value_get_flags (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarycache_get_property (GObject    *object,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
    //Declarations
    LwDictionaryCache *self = NULL;
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYCACHE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_NAME:
        g_value_set_string (value, lw_dictionarycache_get_name (self));
        break;
      case PROP_CONTENTS:
        g_value_set_static_string (value, lw_dictionarycache_get_contents (self));
        break;
      case PROP_LENGTH:
        g_value_set_long (value, lw_dictionarycache_length (self));
        break;
      case PROP_FLAGS:
        g_value_set_flags (value, lw_dictionarycache_get_flags (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarycache_finalize (GObject *object)
{
    //Declarations
    LwDictionaryCache *self = NULL;
    LwDictionaryCachePrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARYCACHE (object);
    priv = self->priv;

    G_OBJECT_CLASS (lw_dictionarycache_parent_class)->finalize (object);
}


static void
lw_dictionarycache_dispose (GObject *object)
{
    //Declarations
    LwDictionaryCache *self = NULL;

    //Initializations
    self = LW_DICTIONARYCACHE (object);

    G_OBJECT_CLASS (lw_dictionarycache_parent_class)->dispose (object);
}


static void
lw_dictionarycache_class_init (LwDictionaryCacheClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwDictionaryCacheClassPrivate, 1);
    object_class->set_property = lw_dictionarycache_set_property;
    object_class->get_property = lw_dictionarycache_get_property;
    object_class->dispose = lw_dictionarycache_dispose;
    object_class->finalize = lw_dictionarycache_finalize;

    g_type_class_add_private (object_class, sizeof (LwDictionaryCachePrivate));

    LwDictionaryCacheClassPrivate *klasspriv = klass->priv;

    klasspriv->pspec[PROP_NAME] = g_param_spec_string (
        "name",
        gettext("Name"),
        gettext("The base name of the data cache.  This is used to build the filename."),
        "unnamed",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS, klasspriv->pspec[PROP_CONTENTS]);

    klasspriv->pspec[PROP_CONTENTS] = g_param_spec_string (
        "buffer",
        gettext("Buffer"),
        gettext("Internal String buffer"),
        "",
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS, klasspriv->pspec[PROP_CONTENTS]);

    klasspriv->pspec[PROP_FLAGS] = g_param_spec_flags (
        "normalization-flags",
        gettext("Normalization Flags"),
        gettext("The flags that are set on normalizations"),
        LW_TYPE_UTF8FLAG, 
        0,
        G_PARAM_CONSTRUCT | G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_FLAGS, klasspriv->pspec[PROP_FLAGS]);

    klasspriv->pspec[PROP_PATH] = g_param_spec_string (
        "path",
        gettext("Path"),
        gettext("The file path of the dictionary cache."),
        "",
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_PATH, klasspriv->pspec[PROP_PATH]);
}


void
lw_dictionarycache_clear (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    lw_cachefile_unref (priv->cache_file); priv->cache_file = NULL;

    lw_dictionarylines_free (priv->lines);
    priv->lines = NULL;
}


gsize
lw_dictionarycache_length (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), 0);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwCacheFile *cache_file = NULL;
    gsize length = 0;

    //Initializations
    priv = self->priv;
    cache_file = priv->cache_file;
    if (cache_file == NULL) goto errored;
    length = lw_cachefile_length (priv->cache_file);

errored:

    return length;
}


void
lw_dictionarycache_write (LwDictionaryCache *self,
                          const gchar        *CHECKSUM,
                          const gchar        *CONTENTS,
                          gint                content_length,
                          LwProgress         *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (CHECKSUM != NULL);
    g_return_if_fail (CONTENTS != NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClass *klass = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;
    const char *C = CONTENTS;
    gint bytes_read = -1;
    LwUtf8Flag flags = 0;
    gssize left = -1;
    LwCacheFile *cache_file = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYCACHE_CLASS (self);
    klasspriv = klass->priv;
    flags = priv->flags;
    left = content_length;
    if (left < 1) goto errored;
    cache_file = priv->cache_file;
    if (cache_file == NULL) goto errored;

    while (*C != '\0' && C - CONTENTS < content_length)
    {
      gchar *normalized = NULL;
      bytes_read = lw_utf8_normalize_chunk (&normalized, C, flags, left);
      if (normalized != NULL)
      {
        lw_cachefile_write (cache_file, CHECKSUM, C, left, progress);
        g_free (normalized); normalized = NULL;
      }
      C += bytes_read;
      left -= bytes_read;
    }

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CONTENTS]);

errored:

    return;
}


gboolean
lw_dictionarycache_validate (LwDictionaryCache *self,
                             const gchar *EXPECTED_CHECKSUM,
                             LwProgress  *progress)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (EXPECTED_CHECKSUM == NULL, FALSE);
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    gboolean is_valid = TRUE;
    LwCacheFile *cache_file = NULL;
    const gchar *CONTENTS = NULL;
    const gchar *PATH = NULL;

    //Initializations
    priv = self->priv;
    cache_file = priv->cache_file;
    if (cache_file ==  NULL)
    {
      is_valid = FALSE;
      goto errored;
    }
    CONTENTS = lw_cachefile_get_contents (cache_file);
    PATH = lw_dictionarycache_get_path (self);

    if (CONTENTS == NULL)
    {
      if (progress != NULL)
      {
        lw_progress_set_error (progress, g_error_new (
          LW_DICTIONARYCACHE_ERROR,
          LW_DICTIONARYCACHE_ERRORCODE_CORRUPT_CONTENTS,
          "The contents of the dictionary cache file are missing %s",
          PATH
        ));
      }
      is_valid = FALSE;
      goto errored;
    }

    if (!lw_utf8_validate (CONTENTS, -1, progress))
    {
      if (progress != NULL)
      {
        lw_progress_set_error (progress, g_error_new (
          LW_DICTIONARYCACHE_ERROR,
          LW_DICTIONARYCACHE_ERRORCODE_CORRUPT_CONTENTS,
          "The dictionary cache file %s is corrupt. It should be valid utf8.",
          PATH
        ));
      }
      is_valid = FALSE;
      goto errored;
    }

errored:

    if (is_valid == FALSE)
    {
      lw_dictionarycache_clear (self);
    }

    return is_valid;
}


const gchar*
lw_dictionarycache_read (LwDictionaryCache          *self,
                         const gchar                *EXPECTED_CHECKSUM,
                         LwDictionaryCacheParseFunc  func,
                         gpointer                    data,
                         LwProgress                 *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, FALSE);
    if (progress != NULL && lw_progress_should_abort (progress)) return NULL;

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwCacheFile *cache_file = NULL;
    LwDictionaryLines *lines = NULL;
    gchar *contents = NULL;
    gsize length = 0;

    //Initializations
    priv = self->priv;
    cache_file = priv->cache_file;
    if (cache_file == NULL) goto errored;
    contents = lw_cachefile_read (cache_file, EXPECTED_CHECKSUM, progress);
    if (contents == NULL) goto errored;
    length = lw_cachefile_length (cache_file);
    if (!lw_dictionarycache_validate (self, EXPECTED_CHECKSUM, progress)) goto errored;

    lw_dictionarycache_set_contents (self, EXPECTED_CHECKSUM, contents, length, func, data, progress);

errored:

    return contents;
}


const gchar*
lw_dictionarycache_get_contents (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwCacheFile *cache_file = NULL;
    const gchar *CONTENTS = NULL;

    //Initializations
    priv = self->priv;
    cache_file = priv->cache_file;
    if (cache_file == NULL) goto errored;
    CONTENTS = lw_cachefile_get_contents (cache_file);

errored:
    
    return CONTENTS;
}


void lw_dictionarycache_set_contents (LwDictionaryCache          *self, 
                                      const gchar                *CHECKSUM,
                                      gchar                      *contents,
                                      gint                        length,
                                      LwDictionaryCacheParseFunc  func,
                                      gpointer                    data,
                                      LwProgress                 *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    if (progress != NULL && lw_progress_should_abort (progress)) return NULL;

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryLines *lines = NULL;

    //Initializations
    priv = self->priv;
    priv->contents = contents;
    priv->length = (contents != NULL && length < 1) ? priv->length = strlen(contents) : length;

    lines = func (self, data);
    lw_dictionarycache_set_lines (self, lines, num_lines);
}


void
lw_dictionarycache_set_flags (LwDictionaryCache  *self,
                               LwUtf8Flag  flags)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClass *klass = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYCACHE_CLASS (self);
    klasspriv = klass->priv;
    flags = lw_utf8flags_clean (flags);
    if (flags == priv->flags) goto errored;
    priv->flags = flags;

    lw_dictionarycache_sync_path (self);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_FLAGS]);

errored:

    return;
}


LwUtf8Flag
lw_dictionarycache_get_flags (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    priv->flags;
}


void
lw_dictionarycache_set_name (LwDictionaryCache *self,
                             const gchar       *NAME)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self, NULL);

    //Declarations    
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClass *klass = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYCACHE_CLASS (self);
    klasspriv = klass->priv;
    if (g_strcmp0 (NAME, priv->name) == 0) goto errored;

    g_free (priv->name);
    priv->name = g_strdup (NAME);

    lw_dictionarycache_sync_path (self);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_NAME]);

errored:

    return
}


const gchar*
lw_dictionarycache_get_name (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self, NULL);

    //Declarations    
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->name;
}


gboolean
lw_dictionarycache_is_modifiable (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), FALSE);

    //Declarations    
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return (priv->path == NULL || !g_file_test (priv->path, G_FILE_TEST_IS_REGULAR));
}


gchar*
lw_dictionarycache_build_filename (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), FALSE);

    //Declarations    
    LwDictionaryCachePrivate *priv = NULL;
    GFlagsClass *flags_class = NULL;
    const gchar *NAME = NULL;
    gchar **names = NULL;
    gint i = 0;

    //Initializations
    priv = self->priv;
    flags_class = g_type_class_ref (LW_TYPE_UTF8FLAG)
    if (flags_class == NULL) goto errored;
    NAME = priv->name;
    if (NAMe == NULL) goto errored;
    names = g_new0 (gchar*, flags_class->n_values + 1);
    if (names == NULL) goto errored;

    names[i++] = name;

    {
      LwUtf8Flag flag = 0;
      for (flag = 0x1; flag != 0; flag >> 1)
      {
        GFlagsValue *value = g_flags_get_first_value (flags_class, flag);
        if (value != NULL)
        {
          names[i++] = value->value_nick;
        }
      }
      names[i++] = NULL;
    }

    filename = g_strjoinv (".", names);

errored:

    g_free (flag_names); flag_names = NULL;
    if (flags_class != NULL) g_type_class_unref (flags_class);
    flags_class = NULL;

    return filename;
}


gchar*
lw_dictionarycache_build_path (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    
    gchar *filename = NULL;
    gchar *path = NULL;

    filename = lw_dictionarycache_build_filename (self);
    if (filename == NULL) goto errored;
    path = lw_util_build_filename (LW_PATH_CACHE, filename);
    if (path == NULL) goto errored;

errored:

    return path;
}


void
lw_dictionarycache_sync_path (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    
    //Declarations
    gchar *path = NULL;

    //Initializations
    path = lw_dictionarycache_build_path (self);

    lw_dictionarycache_set_path (self, path);

errored:

    g_free (path); path = NULL;
}


void
lw_dictionarycache_set_path (LwDictionaryCache *self,
                             const gchar       *PATH)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    
    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClass *klass = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    klasspriv = klass->priv;
    if (g_strcmp0 (PATH, priv->path) == 0) goto errored;

    g_free (priv->path);
    priv->path = path;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PATH]);

errored:

    return;
}


const gchar*
lw_dictionarycache_get_path (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initialization
    priv = self->priv;

    return priv->path;
}



