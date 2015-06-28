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
 * SECTION: dictionarycache
 * @short_description: Searchable normalized parsed dictionary information
 * @title: LwDictionaryCache
 * @include: libwaei/dictionarycache.h
 *
 * Used for storing and loading parsed dictionary information
 * from cachefiles.  This is done by serializing/deserializing
 * each of the internal #LwParsed and #LwIndexed objects. Usually
 * a different #LwDictionaryCache will be created for each normalized
 * dictionary form used, and are indexed into a tree indexed by its
 * #LwUtf8Flags.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>

#include <libwaei/dictionarycache-private.h>

G_DEFINE_TYPE (LwDictionaryCache, lw_dictionarycache, G_TYPE_OBJECT)


GQuark
lw_dictionarycache_error_quark ()
{
    return g_quark_from_static_string ("lw-dictionarycache-error");
}


/**
 * lw_dictionarycache_new:
 * @NAME: (transfer none): Base name of the cache on the filesystem
 * @flags: Normalization flags for the dictionary cache that will be applied to written data
 * Returns: (transfer full): A new #LwDictionaryCache that must be freed with g_object_unref()
 */
LwDictionaryCache *
lw_dictionarycache_new (gchar const * DICTIONARY_NAME,
                        GType         dictionary_type,
                        LwUtf8Flag    flags)
{
    //Declarations
    LwDictionaryCache *self = NULL;

    //Initializations
    self = LW_DICTIONARYCACHE (g_object_new (LW_TYPE_DICTIONARYCACHE,
      "name", DICTIONARY_NAME,
      "dictionary-type", dictionary_type,
      "normalization-flags", flags,
      NULL
    ));

    return self;
}


static void 
lw_dictionarycache_init (LwDictionaryCache * self)
{
    self->priv = LW_DICTIONARYCACHE_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwDictionaryCachePrivate));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;
}


static void 
lw_dictionarycache_set_property (GObject      * object,
                                 guint          property_id,
                                 const GValue * value,
                                 GParamSpec   * pspec)
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
      case PROP_DICTIONARY_TYPE:
        lw_dictionarycache_set_dictionary_type (self, g_value_get_gtype (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarycache_get_property (GObject     * object,
                                  guint        property_id,
                                  GValue     * value,
                                  GParamSpec * pspec)
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
      case PROP_FLAGS:
        g_value_set_flags (value, lw_dictionarycache_get_flags (self));
        break;
      case PROP_PARSED:
        g_value_set_boxed (value, lw_dictionarycache_get_parsed (self));
        break;
      case PROP_INDEXED:
        g_value_set_boxed (value, lw_dictionarycache_get_indexed (self));
        break;
      case PROP_DICTIONARY_TYPE:
        g_value_set_gtype (value, lw_dictionarycache_get_dictionary_type (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarycache_finalize (GObject * object)
{
    //Declarations
    LwDictionaryCache *self = NULL;
    LwDictionaryCachePrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARYCACHE (object);
    priv = self->priv;

    g_free (priv->name);

    if (priv->parsed) g_object_unref (priv->parsed);
    if (priv->indexed) g_object_unref (priv->indexed);

    memset(priv, 0, sizeof(LwDictionaryCachePrivate));

    G_OBJECT_CLASS (lw_dictionarycache_parent_class)->finalize (object);
}


static void
lw_dictionarycache_dispose (GObject * object)
{
    //Declarations
    LwDictionaryCache *self = NULL;

    //Initializations
    self = LW_DICTIONARYCACHE (object);

    lw_dictionarycache_clear (self);

    G_OBJECT_CLASS (lw_dictionarycache_parent_class)->dispose (object);
}


static void
lw_dictionarycache_class_init (LwDictionaryCacheClass * klass)
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
        NULL,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_NAME, klasspriv->pspec[PROP_NAME]);

    klasspriv->pspec[PROP_FLAGS] = g_param_spec_flags (
        "normalization-flags",
        gettext("Normalization Flags"),
        gettext("The flags that are set on normalizations"),
        LW_TYPE_UTF8FLAG, 
        0,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_FLAGS, klasspriv->pspec[PROP_FLAGS]);

    klasspriv->pspec[PROP_INDEXED] = g_param_spec_object (
        "indexed",
        gettext("Indexed"),
        gettext("Indexed information for the dictionary"),
        LW_TYPE_INDEXED, 
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_INDEXED, klasspriv->pspec[PROP_INDEXED]);

    klasspriv->pspec[PROP_PARSED] = g_param_spec_object (
        "parsed",
        gettext("Parsed"),
        gettext("Parsed information for the dictionary"),
        LW_TYPE_PARSED, 
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_PARSED, klasspriv->pspec[PROP_PARSED]);

    klasspriv->pspec[PROP_DICTIONARY_TYPE] = g_param_spec_gtype (
        "dictionary-type",
        gettext("Dictionary Type"),
        gettext("The GType of the dictionary that his cache is for"),
        LW_TYPE_DICTIONARY, 
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY
    );
    g_object_class_install_property (object_class, PROP_DICTIONARY_TYPE, klasspriv->pspec[PROP_DICTIONARY_TYPE]);
}


static void
lw_dictionarycache_clear (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    lw_dictionarycache_set_parsed (self, NULL);
    lw_dictionarycache_set_indexed (self, NULL);
}


static LwCacheFile*
_write_mapped (LwDictionaryCache * self,
               gchar const       * NAME,
               gchar const       * CHECKSUM,
               LwCacheFile       * mapped,
               LwProgress        * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    g_return_val_if_fail (CHECKSUM != NULL, NULL);
    g_return_val_if_fail (LW_IS_CACHEFILE (mapped), NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);

    //Declarations
    gchar *path = NULL;
    LwCacheFile *cachefile = NULL;

    //Initializations
    path = lw_dictionarycache_build_path (self, "normalized");
    if (path == NULL) goto errored;
    cachefile = lw_cachefile_new (path);
    if (cachefile == NULL) goto errored;

    lw_cachefile_write_cachefile (cachefile, CHECKSUM, mapped, progress);

errored:

    g_free (path); path = NULL;

    return cachefile;
}


LwCacheFile*
_write_serializable (LwDictionaryCache * self,
                     gchar const       * NAME,
                     gchar const       * CHECKSUM,
                     LwSerializable    * serializable,
                     LwProgress        * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    g_return_val_if_fail (CHECKSUM != NULL, NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);

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


static LwCacheFile *
_map_contents (LwDictionaryCache * self,
               gchar const       * CHECKSUM,
               gchar const       * CONTENTS,
               gsize               content_length,
               LwProgress        * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    g_return_val_if_fail (CHECKSUM != NULL, NULL);
    g_return_val_if_fail (CONTENTS != NULL, NULL);
    g_return_val_if_fail (content_length > 0, NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);

    //Declarations
    gchar * path = NULL;
    LwCacheFile * cache_file = NULL;

    //Copy and normalize the dictionary contents
    path = lw_dictionarycache_write_normalized_temporary_file (self, CHECKSUM, CONTENTS, content_length, progress);
    if (path == NULL) goto errored;

    cache_file = lw_cachefile_new (path);
    if (cache_file == NULL) goto errored;

    lw_mappedfile_set_delete_on_free (LW_MAPPEDFILE (cache_file), TRUE);
    lw_cachefile_read (cache_file, CHECKSUM, progress);

errored:
  
    g_free (path);
    path = NULL;

    return cache_file;
}


static LwParsed *
_parse (LwDictionaryCache          * self,
        LwCacheFile                * mapped,
        LwDictionaryCacheParseFunc   parse,
        gpointer                     data)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    g_return_val_if_fail (LW_IS_MAPPEDFILE (mapped), NULL);
    g_return_val_if_fail (parse != NULL, NULL);

    //Declarations
    LwParsed *parsed = NULL;

    //Initializations
    parsed = parse (mapped, data);

    return parsed;
}


static LwIndexed *
_index (LwDictionaryCache * self,
        LwParsed          * parsed,
        LwProgress        * progress)
{
    //Sanity checks
    g_return_val_if_fail (parsed != NULL, NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);
    return NULL;

    //Declarations
    LwIndexed *indexed = NULL;

    //Initializations
    indexed = lw_indexed_new (parsed);

    //index things here! TODO

    return indexed;
}


/**
 * lw_dictionarycache_write:
 * @self: A #LwDictionaryCache
 * @CHECKSUM: (transfer none): A checksum to write to disk
 * @CONTENTS: (transfer none): The contents to write to disk
 * @content_length: The byte length of @CONTENTS
 * @parse: A function pointer for parsing the data before it is written
 * @data: Data to pass to @parse when it is called
 * @progress: A #LwProgress to track parsing progress or %NULL to ignore it
 */
void
lw_dictionarycache_write (LwDictionaryCache          * self,
                          gchar const                * CHECKSUM,
                          gchar const                * CONTENTS,
                          gsize                        content_length,
                          LwDictionaryCacheParseFunc   parse,
                          gpointer                     data,
                          LwProgress                 * progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (CHECKSUM != NULL);
    g_return_if_fail (CONTENTS != NULL);
    LW_PROGRESS_RETURN_IF_SHOULD_ABORT (progress);

    //Declarations
    LwCacheFile *mapped_contents = NULL;
    LwParsed *parsed = NULL;
    LwIndexed *indexed = NULL;
    LwCacheFile *normalized_cachefile = NULL;
    LwCacheFile *parsed_cachefile = NULL;
    LwCacheFile *indexed_cachefile = NULL;

    //Map the dictionary contents to a normalized file
    mapped_contents = _map_contents (self, CHECKSUM, CONTENTS, content_length, progress);
    if (mapped_contents == NULL) goto errored;

    //Parse the dictionary, tokenizing the contents inline in the mapped file
    parsed = _parse (self, mapped_contents, parse, data);
    if (parsed == NULL) goto errored;

    //Create an index from the parsed data
    indexed = _index (self, parsed, progress);

    //Write the files perminently if we made it this far
    normalized_cachefile = _write_mapped (self, "normalized", CHECKSUM, mapped_contents, progress);
    if (normalized_cachefile == NULL) goto errored;

    if (parsed != NULL)
    {
      parsed_cachefile = _write_serializable (self, "parsed", CHECKSUM, LW_SERIALIZABLE (parsed), progress);
      if (parsed_cachefile == NULL) goto errored;
    }

    if (indexed != NULL)
    {
      indexed_cachefile = _write_serializable (self, "indexed", CHECKSUM, LW_SERIALIZABLE (indexed), progress);
      if (indexed_cachefile == NULL) goto errored;
    }

errored:

    if (indexed != NULL)
    {
      g_object_unref (indexed);
      indexed = NULL;
    }

    if (parsed != NULL)
    {
      g_object_unref (parsed);
      parsed = NULL;
    }

    //Cleanup the temporary file
    if (mapped_contents != NULL)
    {
      g_object_unref (mapped_contents);
      mapped_contents = NULL;
    }

    //Unreference the cachefiles
    if (normalized_cachefile != NULL)
    {
      g_object_unref (normalized_cachefile);
      normalized_cachefile = NULL;
    }

    if (parsed != NULL)
    {
      g_object_unref (parsed);
      parsed = NULL;
    }

    if (parsed_cachefile != NULL)
    {
      g_object_unref (parsed_cachefile);
      parsed_cachefile = NULL;
    }

    if (indexed != NULL)
    {
      g_object_unref (indexed);
      indexed = NULL;
    }

    if (indexed_cachefile != NULL)
    {
      g_object_unref (indexed_cachefile);
      indexed_cachefile = NULL;
    }
}


static LwCacheFile*
_read_cachefile (LwDictionaryCache * self,
                 gchar const       * NAME,
                 gchar const       * EXPECTED_CHECKSUM,
                 LwProgress        * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);

    //Declarations
    gchar *path = NULL;
    LwCacheFile *cachefile = NULL;
    gsize bytes_written = 0;

    //Initializations
    path = lw_dictionarycache_build_path (self, NAME);
    if (path == NULL) goto errored;
    cachefile = lw_cachefile_new (path);
    lw_cachefile_read (cachefile, EXPECTED_CHECKSUM, progress);

errored:

    return cachefile;
}


/**
 * lw_dictionarycache_read:
 * @self: A #LwDictionaryCache
 * @EXPECTED_CHECKSUM: (transfer none): The expected checksum of the data that will be read
 * @progress: A #LwProgress to track read and validation progress or %NULL to ignore it
 */
void
lw_dictionarycache_read (LwDictionaryCache * self,
                         gchar const       * EXPECTED_CHECKSUM,
                         LwProgress        * progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (EXPECTED_CHECKSUM != NULL);
    LW_PROGRESS_RETURN_IF_SHOULD_ABORT (progress);

    //Declarations
    LwCacheFile *normalized_cachefile = NULL;
    LwCacheFile *parsed_cachefile = NULL;
    LwCacheFile *indexed_cachefile = NULL;
    LwParsed *parsed = NULL;
    LwIndexed *indexed = NULL;

    //Initializations
    lw_dictionarycache_clear (self);

    //Load the dictionary file
    normalized_cachefile = _read_cachefile (self, "normalized", EXPECTED_CHECKSUM, progress);
    if (normalized_cachefile == NULL) goto errored;
    
    //Load the parsed information
    parsed_cachefile = _read_cachefile (self, "parsed", EXPECTED_CHECKSUM, progress);
    if (parsed_cachefile == NULL) goto errored;
    parsed = lw_parsed_new (normalized_cachefile); 
    if (parsed == NULL) goto errored;
    lw_serializable_deserialize_from_cachefile_into (LW_SERIALIZABLE (parsed), EXPECTED_CHECKSUM, parsed_cachefile, progress);
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);

    //Load the indexed information
    indexed_cachefile = _read_cachefile (self, "indexed", EXPECTED_CHECKSUM, progress);
    if (indexed_cachefile == NULL) goto errored;
    indexed = lw_indexed_new (parsed); 
    if (indexed == NULL) goto errored;
    lw_serializable_deserialize_from_cachefile_into (LW_SERIALIZABLE (indexed), EXPECTED_CHECKSUM, indexed_cachefile, progress);
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);

    //Finalize
    lw_dictionarycache_set_parsed (self, parsed);
    lw_dictionarycache_set_indexed (self, indexed);

errored:

    if (parsed != NULL) g_object_unref (parsed);
    if (indexed != NULL) g_object_unref (indexed);

    if (normalized_cachefile != NULL) g_object_unref (normalized_cachefile);
    if (parsed_cachefile != NULL) g_object_unref (parsed_cachefile);
    if (indexed_cachefile != NULL) g_object_unref (indexed_cachefile);
}


GType
lw_dictionarycache_get_dictionary_type (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), G_TYPE_INVALID);

    //Declarations
    LwDictionaryCachePrivate * priv = NULL;

    //Initializatons
    priv = self->priv;

    return priv->dictionary_type;
}


void
lw_dictionarycache_set_dictionary_type (LwDictionaryCache * self,
                                        GType               dictionary_type)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (dictionary_type != G_TYPE_NONE);
    g_return_if_fail (dictionary_type != G_TYPE_INVALID);

    //Declarations
    LwDictionaryCachePrivate * priv = NULL;
    LwDictionaryCacheClass * klass = NULL;

    //Initializatons
    priv = self->priv;
    klass = LW_DICTIONARYCACHE_GET_CLASS (self);
    if (dictionary_type == priv->dictionary_type) goto errored;

    priv->dictionary_type = dictionary_type;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_DICTIONARY_TYPE]);

errored:

    return;
}


static void
lw_dictionarycache_set_flags (LwDictionaryCache * self,
                              LwUtf8Flag          flags)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClass *klass = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYCACHE_GET_CLASS (self);
    klasspriv = klass->priv;
    flags = lw_utf8flag_clean (flags);
    if (flags == priv->flags) goto errored;
    priv->flags = flags;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_FLAGS]);

errored:

    return;
}


/**
 * lw_dictionarycache_get_flags:
 * @self: A #LwDictionaryCache
 * Returns: The #LwUtf8Flags that were set on construction
 */
LwUtf8Flag
lw_dictionarycache_get_flags (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), 0);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    priv->flags;
}


static void
lw_dictionarycache_set_name (LwDictionaryCache * self,
                             gchar const       * NAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));

    //Declarations    
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClass *klass = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYCACHE_GET_CLASS (self);
    klasspriv = klass->priv;
    if (g_strcmp0 (NAME, priv->name) == 0) goto errored;

    g_free (priv->name);
    priv->name = g_strdup (NAME);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_NAME]);

errored:

    return;
}


/**
 * lw_dictionarycache_get_name:
 * @self: A #LwDictionaryCache
 * Returns: (transfer none): The name of the dictionary cache set on consruction. This string is owned by the dictionary cache and so it should not be modified or freed.
 */
gchar const *
lw_dictionarycache_get_name (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations    
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->name;
}


gchar *
lw_dictionarycache_build_filename (LwDictionaryCache * self,
                                   gchar const       * CACHETYPE) 
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), FALSE);

    //Declarations    
    LwDictionaryCachePrivate *priv = NULL;
    GFlagsClass *flags_class = NULL;
    gchar const *NAME = NULL;
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
    if (CACHETYPE != NULL)
    {
      flag_names[i++] = CACHETYPE;
    }

    //Collect the names for the enabled flags
    {
      LwUtf8Flag flag = 0;
      for (flag = 0x1; flag != 0; flag <<= 1)
      {
        if (flag & priv->flags)
        {
          GFlagsValue *value = g_flags_get_first_value (flags_class, flag);
          if (value != NULL)
          {
            flag_names[i++] = value->value_nick;
          }
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


gchar *
lw_dictionarycache_build_directory (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    
    //Declarations
    gchar const * NAME = NULL;
    gchar const * CACHE_DIR = NULL;
    gchar * path = NULL;
    GType dictionary_type = G_TYPE_NONE;

    //Initializations
    NAME = lw_dictionarycache_get_name (self);
    if (NAME == NULL) goto errored;
    dictionary_type = lw_dictionarycache_get_dictionary_type (self);
    if (dictionary_type == G_TYPE_NONE) goto errored;
    if (dictionary_type == G_TYPE_INVALID) goto errored;
    CACHE_DIR = g_getenv("DICTIONARYCACHEDIR");
    if (CACHE_DIR == NULL) CACHE_DIR = g_get_user_cache_dir ();
    if (CACHE_DIR == NULL) goto errored;
    path = g_build_filename (CACHE_DIR, "libwaei", "dictionary", g_type_name (dictionary_type), NULL);
    if (path == NULL) goto errored;

    g_mkdir_with_parents (path, 0700);

errored:

    return path;
}


gchar *
lw_dictionarycache_build_path (LwDictionaryCache * self,
                               gchar const       * CACHETYPE)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    
    //Declarations
    gchar * filename = NULL;
    gchar * path = NULL;
    gchar * directory = NULL;

    //Initializations
    directory = lw_dictionarycache_build_directory (self);
    if (directory == NULL) goto errored;
    filename = lw_dictionarycache_build_filename (self, CACHETYPE);
    if (filename == NULL) goto errored;
    path = g_build_filename (directory, filename, NULL);
    if (path == NULL) goto errored;

errored:

    g_free (directory); directory = NULL;
    g_free (filename); filename = NULL;

    return path;
}


static void
lw_dictionarycache_set_parsed (LwDictionaryCache * self,
                               LwParsed          * parsed)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClass *klass = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYCACHE_GET_CLASS (self);
    klasspriv = klass->priv;
    if (parsed == priv->parsed) goto errored;

    if (parsed != NULL)
    {
      g_object_ref (parsed);
    }

    if (priv->parsed != NULL)
    {
      g_object_unref (priv->parsed);
      priv->parsed = NULL;
    }

    priv->parsed = parsed;
  
    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PARSED]);

errored:

    return;
}


/**
 * lw_dictionarycache_get_parsed:
 * @self: A #LwDictionaryCache
 * Returns: (transfer none): The parsed data for the dictionary
 */
LwParsed *
lw_dictionarycache_get_parsed (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->parsed;
}


static void
lw_dictionarycache_set_indexed (LwDictionaryCache * self,
                                LwIndexed         * indexed)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClass *klass = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYCACHE_GET_CLASS (self);
    klasspriv = klass->priv;
    if (indexed == priv->indexed) goto errored;

    if (indexed != NULL)
    {
      g_object_ref (indexed);
    }

    if (priv->indexed != NULL)
    {
      g_object_unref (priv->indexed);
      priv->indexed = NULL;
    }

    priv->indexed = indexed;
  
    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_INDEXED]);

errored:

    return;
}


/**
 * lw_dictionarycache_get_indexed:
 * @self: A #LwDictionaryCache
 * Returns: (transfer none): The indexed information for the dictionary
 */
LwIndexed *
lw_dictionarycache_get_indexed (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->indexed;
}


static gchar *
lw_dictionarycache_create_normalized_temporary_file (LwDictionaryCache  * self,
                                                     GError            ** error)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (self), NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Declarations
    gchar *path = NULL;
    gchar *tmpl = NULL;
    gchar const *TMPDIR = NULL;
    gint fd = -1;
    gchar const * NAME = NULL;
    gchar *filename = NULL;
    gboolean has_error = FALSE;
    gchar const * CONTENT = "uninitialized";

    //Initializations
    NAME = lw_dictionarycache_get_name (self);
    if (NAME == NULL) goto errored;
    tmpl = g_strdup_printf ("%s.normalized.XXXXXX", NAME);
    if (tmpl == NULL) goto errored;
    TMPDIR = g_get_tmp_dir ();
    if (TMPDIR == NULL) goto errored;
    fd = g_file_open_tmp (tmpl, &filename, error);
    if (error != NULL && *error != NULL)
    {
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
      );
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
      );
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


static gsize
_fwrite_chunk (gchar const  * original_chunk,
               gsize          original_chunk_length,
               gchar        * chunk,
               gsize          chunk_length,
               FILE         * stream,
               GError      ** error)
{
    return lw_io_fwrite_chunk (stream, chunk, chunk_length, error);
}

static gchar *
lw_dictionarycache_write_normalized_temporary_file (LwDictionaryCache * self,
                                                    gchar const       * CHECKSUM,
                                                    gchar const       * CONTENTS,
                                                    gsize               content_length,
                                                    LwProgress        * progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYCACHE (self));
    g_return_if_fail (CHECKSUM != NULL);
    g_return_if_fail (CONTENTS != NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwUtf8Flag flags = 0;
    FILE * stream = NULL;
    GError *error = NULL;
    gchar *path = NULL;
    gboolean has_error = FALSE;

    //Initializations
    priv = self->priv;
    flags = priv->flags;

    //Create the temporary file
    path = lw_dictionarycache_create_normalized_temporary_file (self, &error);
    LW_PROGRESS_TAKE_ERROR (progress, error);

    //Write to it
    stream = g_fopen (path, "w+");
    fwrite (CHECKSUM, sizeof(gchar), strlen(CHECKSUM) + 1, stream);
    lw_utf8_normalize_chunked (CONTENTS, content_length, flags, (LwUtf8ChunkHandler) _fwrite_chunk, stream, progress);
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);

errored:

    if (stream != NULL) fclose(stream);

    return path;
}
