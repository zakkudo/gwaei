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
      case PROP_LINES:
        lw_dictionarycache_set_lines (self, g_value_get_boxed (value));
        break;
      case PROP_CACHEFILE_PARSED:
        lw_dictionarycache_set_parsed_cachefile (self, g_value_get_boxed (value));
        break;
      case PROP_CACHEFILE_LINES:
        lw_dictionarycache_set_lines_cachefile (self, g_value_get_boxed (value));
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
      case PROP_LINES:
        g_value_set_boxed (value, lw_dictionarycache_get_lines (self));
        break;
      case PROP_CACHEFILE_PARSED:
        g_value_set_boxed (value, lw_dictionarycache_get_parsed_cachefile (self));
        break;
      case PROP_CACHEFILE_LINES:
        g_value_set_boxed (value, lw_dictionarycache_get_lines_cachefile (self));
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
    g_object_class_install_property (object_class, PROP_NAME, klasspriv->pspec[PROP_NAME]);

    klasspriv->pspec[PROP_FLAGS] = g_param_spec_flags (
        "normalization-flags",
        gettext("Normalization Flags"),
        gettext("The flags that are set on normalizations"),
        LW_TYPE_UTF8FLAG, 
        0,
        G_PARAM_CONSTRUCT | G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_FLAGS, klasspriv->pspec[PROP_FLAGS]);
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

    lw_dictionarycache_set_parsed_cachefile (self, NULL);
    lw_dictionarycache_set_lines_cachefile (self, NULL);

    if (priv->lines != NULL) lw_parseddictionary_unref (priv->lines);
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
    cache_file = priv->parsed_cachefile;
    if (cache_file == NULL) goto errored;
    length = lw_cachefile_length (cache_file);

errored:

    return length;
}


gchar*
_make_normalized_temporary_file (LwDictionaryCache *self,
                                 const gchar       *CONTENTS,
                                 gsize              content_length,
                                 LwProgress        *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (CONTENTS != NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    FILE *stream = NULL;
    GMappedFile *mapped_file = NULL;
    gchar *path = NULL;
    gchar *tmpl = NULL;
    const gchar *TMPDIR = NULL;
    gint fd = -1;
    gchar *name = NULL;
    LwUtf8Flag flags = 0;
    GError *error = NULL;
    gsize page_size = -1;
    gboolean has_error = FALSE;

    //Initializations
    priv = self->priv;
    tmpl = g_strdup_printf ("%s.normalized.XXXX", lw_dictionarycache_get_name (self));
    if (tmpl == NULL) goto errored;
    TMPDIR = g_get_tmp_dir ();
    if (TMPDIR == NULL) goto errored;
    fd = g_file_open_tmp (tmpl, &name, &error);
    if (error != NULL)
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, g_error_new (
          G_FILE_ERROR,
          g_file_error_from_errno (ferror(stream)),
          "Could not create the dictionary cache file, %s",
          name
        ));
      }
      has_error = TRUE;
      goto errored;
    }
    if (fd == -1) goto errored;
    flags = priv->flags;
    stream = fdopen(fd, "w+");
    if (stream == NULL) goto errored;
    page_size = lw_io_get_pagesize ();
    if (page_size < 1) goto errored;

    //Normalize
    {
      gint bytes_read = -1;
      gsize bytes_written = 0;
      gsize bytes_normalized = 0;
      const char *C = CONTENTS;
      gsize left = content_length;
      gchar *normalized = NULL;
      if (left < 1) goto errored;
      while (*C != '\0' && C - CONTENTS < content_length)
      {
        bytes_read = lw_utf8_normalize_chunk (&normalized, C, flags, page_size);
        bytes_normalized = strlen(normalized);
        if (normalized != NULL)
        {
          bytes_written = fwrite(C, sizeof(gchar), bytes_normalized, stream);
          g_free (normalized); normalized = NULL;
          if (bytes_written != bytes_normalized && ferror(stream) != 0)
          {
            if (progress != NULL)
            {
              lw_progress_take_error (progress, g_error_new (
                G_FILE_ERROR,
                g_file_error_from_errno (ferror(stream)),
                "Could not write the dictionary cache temporary file, \"%s\"\n", 
                name
              ));
            }
            has_error = TRUE;
            goto errored;
          }
        }
        C += bytes_read;
        left -= bytes_read;
      }
    }

    fclose(stream); fd = -1;  stream = NULL;

    path = g_build_path (TMPDIR, name, NULL);

errored:

    g_free (tmpl); tmpl = NULL;

    return path;
}



static void
_write_parsed_cachefile (LwDictionaryCache *self,
                         gchar const       *CHECKSUM,
                         gchar const       *CONTENTS, 
                         gsize              content_length,
                         LwProgress        *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (CHECKSUM != NULL);
    g_return_if_fail (CONTENTS != NULL);

    //Declarations
    gchar *path = NULL;
    LwCacheFile *cachefile = NULL;

    //Initializations
    path = lw_dictionarycache_build_path (self, "parsed");
    cachefile = lw_cachefile_new (path);

    lw_cachefile_write (cachefile, CHECKSUM, CONTENTS, content_length, progress);

errored:

    g_free (path); path = NULL;
}


static void
_write_lines_cachefile (LwDictionaryCache  *self,
                        gchar const        *CHECKSUM,
                        LwParsedDictionary *lines,
                        LwProgress         *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (CHECKSUM != NULL);
    g_return_if_fail (lines != NULL);

    //Declarations
    gsize length = 0;
    gchar *path = NULL;
    gchar *tmp_path = NULL;
    GMappedFile *mapped_file = NULL;
    gchar *contents = NULL;
    GError *error = NULL;
    gboolean has_error = FALSE;
    LwCacheFile *cachefile = NULL;

    //Initializations
    path = lw_dictionarycache_build_path (self, "lines");
    length = lw_parseddictionary_get_serialized_length (lines, progress);

    tmp_path = lw_io_allocate_temporary_file (length, progress);
    mapped_file = g_mapped_file_new (tmp_path, TRUE, &error);
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
    contents = g_mapped_file_get_contents (mapped_file);
    cachefile = lw_cachefile_new (path);

    lw_parseddictionary_serialize (lines, contents, progress);
    lw_cachefile_write (cachefile, CHECKSUM, contents, length, progress);

errored:

    g_free (path); path = NULL;
    g_free (tmp_path); tmp_path = NULL;
    g_mapped_file_unref (mapped_file); mapped_file = NULL;
    contents = NULL;
    lw_cachefile_unref (cachefile); cachefile = NULL;
}


void
lw_dictionarycache_write (LwDictionaryCache          *self,
                          gchar const                *CHECKSUM,
                          gchar const                *CONTENTS,
                          gsize                       content_length,
                          LwDictionaryCacheParseFunc  parse,
                          gpointer                    data,
                          LwProgress                 *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (CHECKSUM != NULL);
    g_return_if_fail (CONTENTS != NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClass *klass = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;
    gchar *temporary_file_path = NULL;
    GMappedFile *temporary_mapped_file = NULL;
    gchar *temporary_mapped_contents = NULL;
    gsize temporary_mapped_content_length = 0;
    gboolean has_error = FALSE;
    GError *error = NULL;
    LwParsedDictionary *lines = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYCACHE_CLASS (self);
    klasspriv = klass->priv;
    if (content_length < 1) content_length = strlen(CONTENTS);
    temporary_file_path = _make_normalized_temporary_file (self, CONTENTS, content_length, progress);
    if (temporary_file_path == NULL) goto errored;
    temporary_mapped_file = g_mapped_file_new (temporary_file_path, TRUE, &error);
    if (error != NULL)
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, error);
        error = NULL;
      }
      has_error = TRUE;
      goto errored;
    }
    temporary_mapped_contents = g_mapped_file_get_contents (temporary_mapped_file);
    if (temporary_mapped_contents == NULL) goto errored;
    temporary_mapped_content_length = g_mapped_file_get_length (temporary_mapped_file);

    //Parse the vanilla contents 
    lines = parse (temporary_mapped_contents, temporary_mapped_content_length, data);

    _write_parsed_cachefile (self, CHECKSUM, temporary_mapped_contents, temporary_mapped_content_length, progress);
    _write_lines_cachefile (self, CHECKSUM, lines, progress);

errored:

    //TODO there is a memory leak here
    g_free (temporary_file_path); temporary_file_path = NULL;
    g_mapped_file_unref (temporary_mapped_file); temporary_mapped_file = NULL;
    lw_parseddictionary_unref (lines); lines = NULL;

    return;
}


void
lw_dictionarycache_set_parsed_cachefile (LwDictionaryCache *self,
                                         LwCacheFile       *cachefile)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClass *klass = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYCACHE_CLASS (self);
    klasspriv = klass->priv;
    if (cachefile == priv->parsed_cachefile) goto errored;

    if (cachefile != NULL) lw_cachefile_ref (cachefile);
    if (priv->parsed_cachefile != NULL) lw_cachefile_unref (priv->parsed_cachefile);
    priv->parsed_cachefile = cachefile;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CACHEFILE_PARSED]);

errored:
    
    return;
}


LwCacheFile*
lw_dictionarycache_get_parsed_cachefile (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->parsed_cachefile;
}


void
lw_dictionarycache_set_lines_cachefile (LwDictionaryCache *self,
                                        LwCacheFile       *cachefile)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClass *klass = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYCACHE_CLASS (self);
    klasspriv = klass->priv;
    if (cachefile == priv->lines_cachefile) goto errored;

    if (cachefile != NULL) lw_cachefile_ref (cachefile);
    if (priv->lines_cachefile != NULL) lw_cachefile_unref (priv->lines_cachefile);
    priv->lines_cachefile = cachefile;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CACHEFILE_LINES]);

errored:
    
    return;
}


LwCacheFile*
lw_dictionarycache_get_lines_cachefile (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    
    //Initializations
    priv = self->priv;

    return priv->lines_cachefile;
}


static LwCacheFile*
_read_parsed_cachefile (LwDictionaryCache *self,
                        gchar const       *EXPECTED_CHECKSUM,
                        LwProgress        *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, FALSE);
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    gchar *path = NULL;
    LwCacheFile *cachefile = NULL;
    const gchar *contents = NULL;

    //Initializations
    path = lw_dictionarycache_build_path (self, "parsed");
    cachefile = lw_cachefile_new (path);
    contents = lw_cachefile_read (cachefile, EXPECTED_CHECKSUM, progress);

    lw_dictionarycache_set_parsed_cachefile (self, cachefile);

errored:

    g_free (path); path = NULL;

    return cachefile;
}


static LwCacheFile*
_read_lines_cachefile (LwDictionaryCache *self,
                       gchar const       *EXPECTED_CHECKSUM,
                       LwProgress        *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, FALSE);
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    gchar *path = NULL;
    LwCacheFile *cachefile = NULL;
    const gchar *contents = NULL;

    //Initializations
    path = lw_dictionarycache_build_path (self, "lines");
    cachefile = lw_cachefile_new (path);
    contents = lw_cachefile_read (cachefile, EXPECTED_CHECKSUM, progress);

    lw_dictionarycache_set_parsed_cachefile (self, cachefile);

errored:

    g_free (path); path = NULL;

    return cachefile;
}


gboolean
lw_dictionarycache_read (LwDictionaryCache *self,
                         gchar const       *EXPECTED_CHECKSUM,
                         LwProgress        *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, FALSE);
    if (progress != NULL && lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    gboolean has_error = FALSE;
    LwCacheFile *parsed_cachefile = NULL;
    gchar *parsed_serialized_data = NULL;
    LwCacheFile *lines_cachefile = NULL;
    gchar *lines_serialized_data = NULL;
    LwParsedDictionary *parsed_dictionary = NULL;
    gsize bytes_read = 0;

    //Initializations
    priv = self->priv;

    parsed_cachefile = _read_parsed_cachefile (self, EXPECTED_CHECKSUM, progress);
    if (parsed_cachefile == NULL)
    {
      has_error = TRUE;
      goto errored;
    }
    parsed_serialized_data = (gchar*) lw_cachefile_get_contents (parsed_cachefile);
    if (parsed_serialized_data == NULL) goto errored;

    lines_cachefile = _read_lines_cachefile (self, EXPECTED_CHECKSUM, progress);
    if (lines_cachefile == NULL)
    {
      has_error = TRUE;
      goto errored;
    }
    lines_serialized_data = (gchar*) lw_cachefile_get_contents (lines_cachefile);
    if (lines_serialized_data == NULL) goto errored;

    parsed_dictionary = lw_parseddictionary_new (parsed_serialized_data);
    if (parsed_dictionary == NULL) goto errored;

    bytes_read = lw_parseddictionary_deserialize_into (parsed_dictionary, lines_serialized_data, progress);
    if (bytes_read != lw_cachefile_length (lines_cachefile))
    {
      has_error = TRUE;
      goto errored;
    }

    lw_dictionarycache_set_parsed_cachefile (self, parsed_cachefile);
    parsed_cachefile = NULL;
    lw_dictionarycache_set_lines_cachefile (self, lines_cachefile);
    lines_cachefile = NULL;
    lw_dictionarycache_set_lines (self, parsed_dictionary);
    parsed_dictionary = NULL;

errored:

    if (parsed_cachefile != NULL) g_object_unref (parsed_cachefile); parsed_cachefile = NULL;
    if (lines_cachefile != NULL) g_object_unref (lines_cachefile); lines_cachefile = NULL;
    if (parsed_dictionary  != NULL) lw_parseddictionary_unref (parsed_dictionary); parsed_dictionary = NULL;

    return !has_error;
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
    cache_file = priv->parsed_cachefile;
    if (cache_file == NULL) goto errored;
    CONTENTS = lw_cachefile_get_contents (cache_file);

errored:
    
    return CONTENTS;
}


void
lw_dictionarycache_set_contents (LwDictionaryCache          *self, 
                                 gchar const                *CHECKSUM,
                                 gchar const                *CONTENTS,
                                 gsize                       content_length,
                                 LwDictionaryCacheParseFunc  parse,
                                 gpointer                    data,
                                 LwProgress                 *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (CONTENTS != NULL);
    g_return_if_fail (parse == NULL);
    if (progress != NULL && lw_progress_should_abort (progress)) return;

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwParsedDictionary *lines = NULL;

    //Initializations
    priv = self->priv;
    if (content_length < 1)
    {
      content_length = strlen(CONTENTS);
    }

    if (lw_dictionarycache_read (self, CHECKSUM, progress) == FALSE)
    {
      if (progress != NULL) lw_progress_set_error (progress, NULL);
      lw_dictionarycache_write (self, CHECKSUM, CONTENTS, content_length, parse, data, progress);
      lw_dictionarycache_read (self, CHECKSUM, progress);
    }
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
    flags = lw_utf8flag_clean (flags);
    if (flags == priv->flags) goto errored;
    priv->flags = flags;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_FLAGS]);

errored:

    return;
}


LwUtf8Flag
lw_dictionarycache_get_flags (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), 0);

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
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));

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

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_NAME]);

errored:

    return;
}


const gchar*
lw_dictionarycache_get_name (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

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
lw_dictionarycache_build_filename (LwDictionaryCache *self,
                                   gchar const       *TYPE) 
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), FALSE);

    //Declarations    
    LwDictionaryCachePrivate *priv = NULL;
    GFlagsClass *flags_class = NULL;
    const gchar *NAME = NULL;
    gchar const * * flag_names = NULL;
    gchar *filename = NULL;
    gint i = 0;

    //Initializations
    priv = self->priv;
    flags_class = g_type_class_ref (LW_TYPE_UTF8FLAG);
    if (flags_class == NULL) goto errored;
    NAME = priv->name;
    if (NAME == NULL) goto errored;
    flag_names = g_new0 (gchar const*, flags_class->n_values + 2);
    if (flag_names == NULL) goto errored;

    flag_names[i++] = NAME;
    if (TYPE != NULL)
    {
      flag_names[i++] = TYPE;
    }

    {
      LwUtf8Flag flag = 0;
      for (flag = 0x1; flag != 0; flag >> 1)
      {
        GFlagsValue *value = g_flags_get_first_value (flags_class, flag);
        if (value != NULL)
        {
          flag_names[i++] = value->value_nick;
        }
      }
      flag_names[i++] = NULL;
    }

    filename = g_strjoinv (".", (gchar**) flag_names);

errored:

    g_free (flag_names); flag_names = NULL;
    if (flags_class != NULL) g_type_class_unref (flags_class);
    flags_class = NULL;

    return filename;
}


gchar*
lw_dictionarycache_build_path (LwDictionaryCache *self,
                               gchar const       *TYPE)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    
    gchar *filename = NULL;
    gchar *path = NULL;

    filename = lw_dictionarycache_build_filename (self, TYPE);
    if (filename == NULL) goto errored;
    path = lw_util_build_filename (LW_PATH_CACHE, filename);
    if (path == NULL) goto errored;

errored:

    return path;
}


void
lw_dictionarycache_set_lines (LwDictionaryCache *self,
                              LwParsedDictionary *lines)
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
    if (lines == priv->lines) goto errored;

    if (lines != NULL)
    {
      lw_parseddictionary_ref (priv->lines);
    }

    if (priv->lines != NULL)
    {
      lw_parseddictionary_unref (priv->lines);
      priv->lines = NULL;
    }

    priv->lines = lines;
  
    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_LINES]);

errored:

    return;
}


LwParsedDictionary*
lw_dictionarycache_get_lines (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->lines;
}



