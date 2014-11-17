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
#include <errno.h>

#include <glib.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>

#include <libwaei/dictionarycache-private.h>

G_DEFINE_TYPE (LwDictionaryCache, lw_dictionarycache, G_TYPE_OBJECT)


struct _LwWriteChunkData {
  FILE *stream;
  gsize bytes_written;
}


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
      case PROP_PARSED:
        lw_dictionarycache_set_parsed (self, g_value_get_boxed (value));
        break;
      case PROP_INDEXED:
        lw_dictionarycache_set_indexed (self, g_value_get_boxed (value));
        break;
      case PROP_CACHEFILE_NORMALIZED:
        lw_dictionarycache_set_normalized_cachefile (self, g_value_get_boxed (value));
        break;
      case PROP_CACHEFILE_PARSED:
        lw_dictionarycache_set_parsed_cachefile (self, g_value_get_boxed (value));
        break;
      case PROP_CACHEFILE_INDEXED:
        lw_dictionarycache_set_indexed_cachefile (self, g_value_get_boxed (value));
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
      case PROP_PARSED:
        g_value_set_boxed (value, lw_dictionarycache_get_parsed (self));
        break;
      case PROP_INDEXED:
        g_value_set_boxed (value, lw_dictionarycache_get_indexed (self));
        break;
      case PROP_CACHEFILE_NORMALIZED:
        g_value_set_boxed (value, lw_dictionarycache_get_normalized_cachefile (self));
        break;
      case PROP_CACHEFILE_PARSED:
        g_value_set_boxed (value, lw_dictionarycache_get_parsed_cachefile (self));
        break;
      case PROP_CACHEFILE_INDEXED:
        g_value_set_boxed (value, lw_dictionarycache_get_indexed_cachefile (self));
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

    lw_dictionarycache_clear (self);

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

    lw_dictionarycache_set_normalized_cachefile (self, NULL);
    lw_dictionarycache_set_parsed_cachefile (self, NULL);
    lw_dictionarycache_set_indexed_cachefile (self, NULL);

    lw_dictionarycache_set_parsed (self, NULL);
    lw_dictionarycache_set_indexed (self, NULL);
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




static void
_write_mmapped (LwDictionaryCache *self,
                gchar const       *NAME,
                gchar const       *CHECKSUM,
                GMappedFile       *mapped_file,
                LwProgress        *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (CHECKSUM != NULL);
    g_return_if_fail (mapped_file != NULL);

    //Declarations
    gchar *path = NULL;
    LwCacheFile *cachefile = NULL;
    gchar *contents = NULL;
    gsize content_length = 0;

    //Initializations
    contents = g_mapped_file_get_contents(mapped_file);
    if (contents == NULL) goto errored;
    content_length = g_mapped_file_get_length (mapped_file);
    if (content_length < 1) goto errored;
    path = lw_dictionarycache_build_path (self, "normalized");
    if (path == NULL) goto errored;
    cachefile = lw_cachefile_new (path);
    if (cachefile == NULL) goto errored;

    lw_cachefile_write (cachefile, CHECKSUM, contents, content_length, progress);

errored:

    g_free (path); path = NULL;
}


LwCacheFile*
_write (LwDictionaryCache *self,
        gchar const        *NAME,
        gchar const        *CHECKSUM,
        LwSerializable     *serializable,
        LwProgress         *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    g_return_val_if_fail (CHECKSUM != NULL, NULL);
    if (lw_progress_should_abort (progress)) return NULL;

    //Declarations
    gchar *path = NULL;
    LwCacheFile *cachefile = NULL;
    gsize bytes_written = 0;

    //Initializations
    path = lw_dictionarycache_build_path (self, NAME);
    if (path == NULL) goto errored;
    cachefile = lw_cachefile_new (path);
    bytes_written = lw_serializable_serialize_to_cachefile (serializable, CHECKSUM, cachefile, progress);

errored:

    return cachefile;
}


static LwMappedFile*
_map (LwDictionaryCache *   self,
      gchar const       *   CONTENTS,
      gsize                 content_length,
			gchar             * * path_out,
      LwProgress        *   progress)
{
		//Sanity checks
		g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL)
		g_return_val_if_fail (CONTENTS != NULL, NULL)
		g_return_val_if_fail (content_length > 0, NULL)
    g_return_val_if_fail (LW_IS_PROGRESS (progress))
    if (lw_progress_should_abort (progress)) return NULL;

		//Declarations
		gchar *path = NULL;
		GMappedFile *mapped_file = NULL;
		GError *error = NULL;
		gboolean has_error = FALSE;

    //Copy and normalize the dictionary contents
    path = lw_dictionarycache_write_normalized_temporary_file (self, CONTENTS, content_length, progress);
    if (path == NULL) goto errored;
    mapped_file = lw_mappedfile_new (path, TRUE, &error);
    if (error != NULL)
    {
      lw_progress_take_error (progress, error);
      error = NULL;
      has_error = TRUE;
      goto errored;
    }

errored:
  
    g_free (path);
    path = NULL;
    g_clear_error (error);

		return mapped_file;
}


static LwIndexed*
_index (LwDictionaryCache *self,
        LwParsed          *parsed,
        LwProgress        *progress)
{
    //Sanity checks
    g_return_val_if_fail (parsed != NULL, NULL);

    //Declarations
    LwIndexed *indexed = NULL;

    //Initializations
    indexed = lw_indexed_new (parsed);

    //index things here! TODO

    return indexed;
}

static LwParsed*
_parse (LwDictionaryCache *cache,
        GMappedFile       *mapped_file,
        LwProgress        *progress)
{
    TODO
    parsed = parse (temporary_mapped_contents, temporary_mapped_content_length, data);
    if (parsed == NULL) goto errored;
    return parsed;
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
    LwMappedFile *mapped = NULL;
    LwParsed *parsed = NULL;
    LwIndexed *indexed = NULL;

    //Map the dictionary contents to a normalized file
		mapped = _map (self, CHECKSUM, CONTENTS, content_length, progress)
		if (mapped == NULL) goto errored;

    //Parse the dictionary, tokenizing the contents inline in the mapped file
    parsed = _parse (self, temporary_mapped_file, progress)
    if (parsed == NULL) goto errored;

    //Create an index from the parsed data
    indexed = _index (self, parsed, progress);
    if (indexed == NULL) goto errored;

    //Write the files perminently
    normalized_cachefile = _write_mmapped (self, "normalized", CHECKSUM, mapped, progress);
    if (normalized_cachefile == NULL) goto errored;
    parsed_cachefile = _write (self, "parsed", CHECKSUM, LW_SERIALIZABLE (parsed), progress);
    if (parsed_cachefile == NULL) goto errored;
    indexed_cachefile = _write (self, "indexed", CHECKSUM, LW_SERIALIZABLE (indexed), progress);
    if (indexed_cachefile == NULL) goto errored;

errored:

    //Cleanup the temporary file
    if (mapped != NULL)
    {
      gchar *path = lw_mappedfile_free (mapped, FALSE);
      g_remove (path);
      path = NULL;
      mapped = NULL;
    }

    //Unreference the cachefiles
    g_mapped_file_unref (normalized_cachefile); normalized_cachefile = NULL;

    lw_parsed_unref (parsed); parsed = NULL;
    g_mapped_file_unref (parsed_cachefile); parsed_cachefile = NULL;

    lw_indexed_unref (indexed); indexed = NULL;
    g_mapped_file_unref (indexed_cachefile); indexed_cachefile = NULL;
}


void
lw_dictionarycache_set_normalized_cachefile (LwDictionaryCache *self,
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
    if (cachefile == priv->normalized_cachefile) goto errored;

    if (cachefile != NULL) lw_cachefile_ref (cachefile);
    if (priv->normalized_cachefile != NULL) lw_cachefile_unref (priv->normalized_cachefile);
    priv->normalized_cachefile = cachefile;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CACHEFILE_CONTENTS]);

errored:
    
    return;
}


LwCacheFile*
lw_dictionarycache_get_normalized_cachefile (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->normalized_cachefile;
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
lw_dictionarycache_set_indexed_cachefile (LwDictionaryCache *self,
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
    if (cachefile == priv->indexed_cachefile) goto errored;

    if (cachefile != NULL) lw_cachefile_ref (cachefile);
    if (priv->indexed_cachefile != NULL) lw_cachefile_unref (priv->indexed_cachefile);
    priv->indexed_cachefile = cachefile;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CACHEFILE_INDEXED]);

errored:
    
    return;
}


LwCacheFile*
lw_dictionarycache_get_indexed_cachefile (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->indexed_cachefile;
}


static void
_read_into_serializable (LwDictionaryCache *self,
                         gchar const       *NAME,
                         gchar const       *EXPECTED_CHECKSUM,
                         LwSerializable    *serializable,
                         LwProgress        *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, NULL);
    g_return_val_if_fail (LW_IS_PROGRESS (progress), NULL);
    if (lw_progress_should_abort (progress)) return NULL;

    //Declarations
    gchar *path = NULL;
    gchar *cachefile_key = NULL;
    gchar *serializable_key = NULL;
    LwCacheFile *cachefile = NULL;

    //Initializations
    path = lw_dictionarycache_build_path (self, NAME);
    if (path == NULL) goto errored;
    cachefile_key = g_strdup_printf()
    if (cachefile == NULL) goto errored;
    serializable_key = g_strdup_printf()
    if (serializable_key == NULL) goto errored;
    cachefile = lw_cachefile_new (path);
    lw_serializable_read_from_cachefile_into (serializable, EXPECTED_CHECKSUM, cachefile, progress);

errored:

    return;
}


LwCacheFile*
_read_cachefile (self, "normalized")
{
}


gboolean
lw_dictionarycache_read (LwDictionaryCache *self,
                         gchar const       *EXPECTED_CHECKSUM,
                         LwProgress        *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), FALSE);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, FALSE);
    g_return_val_if_fail (LW_IS_PROGRESS (progress), FALSE);
    if (lw_progress_should_abort (progress)) return FALSE;

    //Initializations
    lw_dictionarycache_clear (self);

    //Load the data
    normalized_cachfile = _read_cachefile (self, "normalized")
    parsed_cachefile = _read_cachefile (self, "parsed")
    indexed_cachefile = _read_cachefile (self, "indexed")
    
    //Parse it
    _read_into_serializable (self, "parsed", EXPECTED_CHECKSUM, progress);
ERROR CHECK
    _read_into_serializable (self, "indexed", EXPECTED_CHECKSUM, progress);
ERROR CHECK

    //Finalized
    lw_dictionarycache_set_normalized (self, normalized);
    lw_dictionarycache_set_parsed_cachefile( self, parsed_cachefile);
    lw_dictionarycache_set_indexed_cachefile (self, indexed_cachefile);

    lw_dictionarycache_set_parsed (self, parsed);
    lw_dictionarycache_set_indexed (self, indexed);

errored:

    return FALSE; //TODO
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
    cache_file = priv->contents_cachefile;
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
    g_return_if_fail (LW_IS_PROGRESS (progress));
    if (lw_progress_should_abort (progress)) return;

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwParsed *parsed = NULL;

    //Initializations
    priv = self->priv;
    if (content_length < 1)
    {
      content_length = strlen(CONTENTS);
    }

    if (lw_dictionarycache_read (self, CHECKSUM, progress) == FALSE)
    {
      lw_progress_set_error (progress, NULL);
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
lw_dictionarycache_set_parsed (LwDictionaryCache *self,
                               LwParsed          *parsed)
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
    if (parsed == priv->parsed) goto errored;

    if (parsed != NULL)
    {
      lw_parsed_ref (priv->parsed);
    }

    if (priv->parsed != NULL)
    {
      lw_parsed_unref (priv->parsed);
      priv->parsed = NULL;
    }

    priv->parsed = parsed;
  
    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PARSED]);

errored:

    return;
}


LwParsed*
lw_dictionarycache_get_parsed (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->parsed;
}


void
lw_dictionarycache_set_indexed (LwDictionaryCache *self,
                                LwIndexed         *indexed)
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
    if (indexed == priv->indexed) goto errored;

    if (indexed != NULL)
    {
      lw_indexed_ref (priv->indexed);
    }

    if (priv->indexed != NULL)
    {
      lw_indexed_unref (priv->indexed);
      priv->indexed = NULL;
    }

    priv->indexed = indexed;
  
    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_INDEXED]);

errored:

    return;
}


LwIndexed*
lw_dictionarycache_get_indexed (LwDictionaryCache *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->indexed;
}


static gsize
_write_chunk (gchar              *chunk,
              gsize               chunk_length,
              _LwWriteChunkData  *data,
              GError            **error)
{
		//Sanity checks
		g_return_val_if_fail (chunk != NULL);
		if (chunk_length < 1) return 0;
		if (error != NULL && *error != NULL) return 0;

		bytes_written = fwrite(chunk, sizeof(gchar), chunk_length, stream);
    data.bytes_written += bytes_written;
		if (bytes_written != chunk_length && ferror(stream) != 0)
		{
			*error = g_error_new (
							G_FILE_ERROR,
							g_file_error_from_errno (ferror(stream)),
							"Could not write the dictionary cache temporary file, \"%s\"\n", 
							name
			)
		}

		return bytes_written;
}


gchar*
_create_normalized_temporary_file (LwDictionaryCache  *self,
                                   GError            **error)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL)
    if (error != NULL && *error != NULL) return NULL;

    //Declarations
    gchar *path = NULL;
    gchar *tmpl = NULL;
    const gchar *TMPDIR = NULL;
    gint fd = -1;
    gchar const * NAME = NULL;
    gchar *filename = NULL;
    gboolean has_error = FALSE;
    gchar const * CONTENT = "uninitialized";

    //Initializations
    NAME = lw_dictionarycache_get_name (self);
    if (NAME == NULL) goto errored;
    tmpl = g_strdup_printf ("%s.normalized.XXXX", NAME);
    if (tmpl == NULL) goto errored;
    TMPDIR = g_get_tmp_dir ();
    if (TMPDIR == NULL) goto errored;
    fd = g_file_open_tmp (tmpl, &filename, &error);
    if (error != NULL)
    {
      *error = error;
      error = NULL;
      has_error = TRUE;
      goto errored;
    }

    write(fd, CONTENT, strlen(CONTENT));

    if (fsync(fd) != 0)
    {
      *error = g_error_new (
        G_FILE_ERROR,
        g_file_error_from_errno (errno),
        "Could not create the temp file for parsing the dictionary %s",
        filename
      )
      has_error = TRUE;
      goto errored;
    }

    if (close(fd) != 0)
    {
      *error = g_error_new (
        G_FILE_ERROR,
        g_file_error_from_errno (errno),
        "Could not close temp file for parsing the dictionary %s",
        filename
      )
      has_error = TRUE;
      goto errored;
    }

    path = g_build_path (TMPDIR, filename, NULL);

errored:

    if (fd != -1)
    {
      close (fd); 
    }
    fd = -1;
    g_free (tmpl);
    tmpl = NULL;
    g_free (filename);
    filename = NULL;

    return path;
}


gchar*
lw_dictionarycache_write_normalized_temporary_file (LwDictionaryCache *self,
                                                    const gchar *CONTENTS,
                                                    gsize        content_length,
                                                    LwProgress  *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (CONTENTS != NULL);
    g_return_if_fail (LW_IS_PROGRESS (progress))
    if (lw_progress_should_abort (progress)) return NULL;

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwUtf8Flag flags = 0;
    LwWriteChunkData data = { 0 };

    //Initializations
    priv = self->priv;
    flags = priv->flags;

    //Create the temporary file
    path = _create_normalized_temporary_file (self, &error);
    if (error != NULL)
    {
      lw_progress_take_error (progress, error);
      error = NULL;
      has_error = TRUE;
      goto errored;
    }

    //Write to it
    data.stream = g_fopen (path, "w+");
    lw_utf8_normalize_chunked (CONTENTS, content_length, flags, _write_chunk, data, progress)
    if (lw_progress_should_abort (progress)) goto errored;

errored:

    if (data.stream != NULL) fclose(data.stream);
    data.stream = NULL;
    g_free (tmpl); tmpl = NULL;

    return path;
}
