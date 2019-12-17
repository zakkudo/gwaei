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
 * SECTION: dictionary_cache
 * @short_description: Searchable normalized parsed dictionary information
 * @title: LwDictionaryCache
 * @include: libwaei/dictionary_cache.h
 *
 * Used for storing and loading parsed dictionary information
 * from cache_files.  This is done by serializing/deserializing
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
#include <unistd.h>

#include <libwaei/dictionary.h>
#include <libwaei/dictionary-cache.h>
#include <libwaei/serializable.h>
#include <libwaei/io.h>
#include <libwaei/gettext.h>

#define lw_dictionary_cache_class_get_private() G_TYPE_CLASS_GET_PRIVATE(g_type_class_peek(LW_TYPE_DICTIONARY_CACHE), LW_TYPE_DICTIONARY_CACHE, LwDictionaryCacheClassPrivate)

typedef enum {
    PROP_0,
    PROP_FLAGS,
    PROP_PARSED,
    PROP_INDEXED,
    PROP_DICTIONARY_NAME,
    PROP_DICTIONARY_TYPE,
    PROP_PROGRESS,
    TOTAL_PROPS
} Props;


struct _LwDictionaryCache {
    GObject parent;
};

typedef struct {
  GType dictionary_type;
  gchar *dictionary_name;
  LwUtf8Flag flags;
  LwParsed *parsed;
  LwIndexed *indexed;
  LwProgress * progress;
  GMutex mutex;
} LwDictionaryCachePrivate;

typedef struct {
  GParamSpec *pspec[TOTAL_PROPS];
} LwDictionaryCacheClassPrivate;

//Methods

static void lw_dictionary_cache_set_flags (LwDictionaryCache *self, LwUtf8Flag flags);

static gchar* lw_dictionary_cache_write_normalized_temporary_file (LwDictionaryCache *self, gchar const *CHECKSUM, gchar const *CONTENTS, gsize content_length, LwProgress  *progress);

static void lw_dictionary_cache_clear (LwDictionaryCache *self);

static void lw_dictionary_cache_set_dictionary_name (LwDictionaryCache *self, gchar const *DICTIONARY_NAME);

static void lw_dictionary_cache_set_parsed (LwDictionaryCache *self, LwParsed *parsed);
static void lw_dictionary_cache_set_indexed (LwDictionaryCache *self, LwIndexed *indexed);

static gchar* lw_dictionary_cache_build_filename (LwDictionaryCache *self, gchar const *TYPE);
static gchar* lw_dictionary_cache_build_path (LwDictionaryCache *self, gchar const *TYPE);

static void lw_dictionary_cache_set_progress (LwDictionaryCache * self, LwProgress * progress);
static void lw_dictionary_cache_set_dictionary_type (LwDictionaryCache * self, GType dictionary_type);

G_DEFINE_TYPE_WITH_CODE (LwDictionaryCache, lw_dictionary_cache, G_TYPE_OBJECT, G_ADD_PRIVATE(LwDictionaryCache) g_type_add_class_private(LW_TYPE_DICTIONARY_CACHE, sizeof(LwDictionaryCacheClassPrivate)) )


GQuark
lw_dictionary_cache_error_quark ()
{
    return g_quark_from_static_string ("lw-dictionary_cache-error");
}


/**
 * lw_dictionary_cache_new:
 * @NAME: (transfer none): Base name of the cache on the filesystem
 * @flags: Normalization flags for the dictionary cache that will be applied to written data
 * Returns: (transfer full): A new #LwDictionaryCache that must be freed with g_object_unref()
 */
LwDictionaryCache *
lw_dictionary_cache_new (gchar const * DICTIONARY_NAME,
                        GType         dictionary_type,
                        LwUtf8Flag    flags)
{
    //Declarations
    LwDictionaryCache *self = NULL;

    //Initializations
    self = LW_DICTIONARY_CACHE (g_object_new (LW_TYPE_DICTIONARY_CACHE,
      "dictionary-name", DICTIONARY_NAME,
      "dictionary-type", dictionary_type,
      "normalization-flags", flags,
      NULL
    ));

    return self;
}


static void 
lw_dictionary_cache_init (LwDictionaryCache * self)
{
}


static void 
lw_dictionary_cache_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_dictionary_cache_parent_class)->constructed (object);
    }

    //Declarations
    LwDictionaryCache *self = NULL;
    LwProgress * progress = NULL;

    //Initializations
    self = LW_DICTIONARY_CACHE (object);

    progress = lw_progress_new ();
    if (progress == NULL) goto errored;
    lw_dictionary_cache_set_progress (self, progress);
    g_object_unref (progress);

errored:

    return;
}


static void 
lw_dictionary_cache_set_property (GObject      * object,
                                 guint          property_id,
                                 const GValue * value,
                                 GParamSpec   * pspec)
{
    //Declarations
    LwDictionaryCache *self = NULL;
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARY_CACHE (object);
    priv = lw_dictionary_cache_get_instance_private (self);

    switch (property_id)
    {
      case PROP_DICTIONARY_NAME:
        lw_dictionary_cache_set_dictionary_name (self, g_value_get_string (value));
        break;
      case PROP_FLAGS:
        lw_dictionary_cache_set_flags (self, g_value_get_flags (value));
        break;
      case PROP_DICTIONARY_TYPE:
        lw_dictionary_cache_set_dictionary_type (self, g_value_get_gtype (value));
        break;
      case PROP_PROGRESS:
        lw_dictionary_cache_set_progress (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionary_cache_get_property (GObject     * object,
                                  guint        property_id,
                                  GValue     * value,
                                  GParamSpec * pspec)
{
    //Declarations
    LwDictionaryCache *self = NULL;
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARY_CACHE (object);
    priv = lw_dictionary_cache_get_instance_private (self);

    switch (property_id)
    {
      case PROP_DICTIONARY_NAME:
        g_value_set_string (value, lw_dictionary_cache_get_dictionary_name (self));
        break;
      case PROP_FLAGS:
        g_value_set_flags (value, lw_dictionary_cache_get_flags (self));
        break;
      case PROP_PARSED:
        g_value_set_object (value, lw_dictionary_cache_get_parsed (self));
        break;
      case PROP_INDEXED:
        g_value_set_object (value, lw_dictionary_cache_get_indexed (self));
        break;
      case PROP_DICTIONARY_TYPE:
        g_value_set_gtype (value, lw_dictionary_cache_get_dictionary_type (self));
        break;
      case PROP_PROGRESS:
        g_value_set_object (value, lw_dictionary_cache_get_progress (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionary_cache_finalize (GObject * object)
{
    //Declarations
    LwDictionaryCache *self = NULL;
    LwDictionaryCachePrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARY_CACHE (object);
    priv = lw_dictionary_cache_get_instance_private (self);

    g_free (priv->dictionary_name);

    if (priv->parsed) g_object_unref (priv->parsed);
    if (priv->indexed) g_object_unref (priv->indexed);

    lw_dictionary_cache_set_progress (self, NULL);

    memset(priv, 0, sizeof(LwDictionaryCachePrivate));

    G_OBJECT_CLASS (lw_dictionary_cache_parent_class)->finalize (object);
}


static void
lw_dictionary_cache_dispose (GObject * object)
{
    //Declarations
    LwDictionaryCache *self = NULL;

    //Initializations
    self = LW_DICTIONARY_CACHE (object);

    lw_dictionary_cache_clear (self);

    G_OBJECT_CLASS (lw_dictionary_cache_parent_class)->dispose (object);
}


static void
lw_dictionary_cache_class_init (LwDictionaryCacheClass * klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klasspriv = lw_dictionary_cache_class_get_private ();
    object_class->constructed = lw_dictionary_cache_constructed;
    object_class->set_property = lw_dictionary_cache_set_property;
    object_class->get_property = lw_dictionary_cache_get_property;
    object_class->dispose = lw_dictionary_cache_dispose;
    object_class->finalize = lw_dictionary_cache_finalize;

    klasspriv->pspec[PROP_DICTIONARY_NAME] = g_param_spec_string (
        "dictionary-name",
        gettext("Dictionary Name"),
        gettext("The base name of the data cache.  This is used to build the filename."),
        NULL,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DICTIONARY_NAME, klasspriv->pspec[PROP_DICTIONARY_NAME]);

    klasspriv->pspec[PROP_DICTIONARY_TYPE] = g_param_spec_gtype (
        "dictionary-type",
        gettext("Dictionary Type"),
        gettext("The GType of the dictionary that his cache is for"),
        LW_TYPE_DICTIONARY, 
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY
    );
    g_object_class_install_property (object_class, PROP_DICTIONARY_TYPE, klasspriv->pspec[PROP_DICTIONARY_TYPE]);

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

    klasspriv->pspec[PROP_PROGRESS] = g_param_spec_object (
        "progress",
        gettext("Progress"),
        gettext("Tracked progress for read/write operations"),
        LW_TYPE_PROGRESS, 
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_PROGRESS, klasspriv->pspec[PROP_PROGRESS]);
}


static void
lw_dictionary_cache_clear (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_CACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);

    lw_dictionary_cache_set_parsed (self, NULL);
    lw_dictionary_cache_set_indexed (self, NULL);
}


static LwCacheFile*
_write_mapped (LwDictionaryCache * self,
               gchar const       * NAME,
               gchar const       * CHECKSUM,
               LwCacheFile       * mapped,
               LwProgress        * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);
    g_return_val_if_fail (CHECKSUM != NULL, NULL);
    g_return_val_if_fail (LW_IS_CACHE_FILE (mapped), NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);

    //Declarations
    gchar *path = NULL;
    LwCacheFile *cache_file = NULL;

    //Initializations
    path = lw_dictionary_cache_build_path (self, "normalized");
    if (path == NULL) goto errored;
    cache_file = lw_cache_file_new (path);
    if (cache_file == NULL) goto errored;

    lw_cache_file_write_cache_file (cache_file, CHECKSUM, mapped, progress);

errored:

    g_free (path); path = NULL;

    return cache_file;
}


LwCacheFile*
_write_serializable (LwDictionaryCache * self,
                     gchar const       * NAME,
                     gchar const       * CHECKSUM,
                     LwSerializable    * serializable,
                     LwProgress        * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);
    g_return_val_if_fail (CHECKSUM != NULL, NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);

    //Declarations
    gchar *path = NULL;
    LwCacheFile *cache_file = NULL;
    gsize bytes_written = 0;

    //Initializations
    path = lw_dictionary_cache_build_path (self, NAME);
    if (path == NULL) goto errored;
    cache_file = lw_cache_file_new (path);
    bytes_written = lw_serializable_serialize_to_cache_file (serializable, CHECKSUM, cache_file, progress);

errored:

    g_free (path);
    path = NULL;

    return cache_file;
}


static LwCacheFile *
_map_contents (LwDictionaryCache * self,
               gchar const       * CHECKSUM,
               gchar const       * CONTENTS,
               gsize               content_length,
               LwProgress        * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);
    g_return_val_if_fail (CHECKSUM != NULL, NULL);
    g_return_val_if_fail (CONTENTS != NULL, NULL);
    g_return_val_if_fail (content_length > 0, NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);

    //Declarations
    gchar * path = NULL;
    LwCacheFile * cache_file = NULL;

    //Copy and normalize the dictionary contents
    path = lw_dictionary_cache_write_normalized_temporary_file (self, CHECKSUM, CONTENTS, content_length, progress);
    if (path == NULL) goto errored;

    cache_file = lw_cache_file_new (path);
    if (cache_file == NULL) goto errored;

    lw_mapped_file_set_delete_on_free (LW_MAPPED_FILE (cache_file), TRUE);
    lw_cache_file_read (cache_file, CHECKSUM, progress);

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
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);
    g_return_val_if_fail (LW_IS_MAPPED_FILE (mapped), NULL);
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
 * lw_dictionary_cache_write:
 * @self: A #LwDictionaryCache
 * @CHECKSUM: (transfer none): A checksum to write to disk
 * @CONTENTS: (transfer none): The contents to write to disk
 * @content_length: The byte length of @CONTENTS
 * @parse: A function pointer for parsing the data before it is written
 * @data: Data to pass to @parse when it is called
 * @progress: A #LwProgress to track parsing progress or %NULL to ignore it
 */
void
lw_dictionary_cache_write (LwDictionaryCache          * self,
                          gchar const                * CHECKSUM,
                          gchar const                * CONTENTS,
                          gsize                        content_length,
                          LwDictionaryCacheParseFunc   parse,
                          gpointer                     data,
                          LwProgress                 * progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_CACHE (self));
    g_return_if_fail (CHECKSUM != NULL);
    g_return_if_fail (CONTENTS != NULL);
    LW_PROGRESS_RETURN_IF_SHOULD_ABORT (progress);

    //Declarations
    LwDictionaryCachePrivate * priv = NULL;
    LwCacheFile *mapped_contents = NULL;
    LwParsed *parsed = NULL;
    LwIndexed *indexed = NULL;
    LwCacheFile *normalized_cache_file = NULL;
    LwCacheFile *parsed_cache_file = NULL;
    LwCacheFile *indexed_cache_file = NULL;

    priv = lw_dictionary_cache_get_instance_private (self);

    //Map the dictionary contents to a normalized file
    mapped_contents = _map_contents (self, CHECKSUM, CONTENTS, content_length, progress);
    if (mapped_contents == NULL) goto errored;

    //Parse the dictionary, tokenizing the contents inline in the mapped file
    parsed = _parse (self, mapped_contents, parse, data);
    if (parsed == NULL) goto errored;

    //Create an index from the parsed data
    indexed = _index (self, parsed, progress);

    //Write the files perminently if we made it this far
    normalized_cache_file = _write_mapped (self, "normalized", CHECKSUM, mapped_contents, progress);
    if (normalized_cache_file == NULL) goto errored;

    if (parsed != NULL)
    {
      parsed_cache_file = _write_serializable (self, "parsed", CHECKSUM, LW_SERIALIZABLE (parsed), progress);
      if (parsed_cache_file == NULL) goto errored;
    }

    if (indexed != NULL)
    {
      indexed_cache_file = _write_serializable (self, "indexed", CHECKSUM, LW_SERIALIZABLE (indexed), progress);
      if (indexed_cache_file == NULL) goto errored;
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

    //Unreference the cache_files
    if (normalized_cache_file != NULL)
    {
      g_object_unref (normalized_cache_file);
      normalized_cache_file = NULL;
    }

    if (parsed != NULL)
    {
      g_object_unref (parsed);
      parsed = NULL;
    }

    if (parsed_cache_file != NULL)
    {
      g_object_unref (parsed_cache_file);
      parsed_cache_file = NULL;
    }

    if (indexed != NULL)
    {
      g_object_unref (indexed);
      indexed = NULL;
    }

    if (indexed_cache_file != NULL)
    {
      g_object_unref (indexed_cache_file);
      indexed_cache_file = NULL;
    }
}


static LwCacheFile*
_read_cache_file (LwDictionaryCache * self,
                 gchar const       * NAME,
                 gchar const       * EXPECTED_CHECKSUM,
                 LwProgress        * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);

    //Declarations
    gchar *path = NULL;
    LwCacheFile *cache_file = NULL;
    gsize bytes_written = 0;

    //Initializations
    path = lw_dictionary_cache_build_path (self, NAME);
    if (path == NULL) goto errored;
    cache_file = lw_cache_file_new (path);
    lw_cache_file_read (cache_file, EXPECTED_CHECKSUM, progress);

errored:

    if (lw_cache_file_get_contents (cache_file) == NULL)
    {
      g_object_unref (cache_file);
      cache_file = NULL;
    }

    g_free (path);
    path = NULL;

    return cache_file;
}


/**
 * lw_dictionary_cache_read:
 * @self: A #LwDictionaryCache
 * @EXPECTED_CHECKSUM: (transfer none): The expected checksum of the data that will be read
 * @progress: A #LwProgress to track read and validation progress or %NULL to ignore it
 * Returns: %TRUE if the read was sucessful
 */
gboolean
lw_dictionary_cache_read (LwDictionaryCache * self,
                         gchar const       * EXPECTED_CHECKSUM,
                         LwProgress        * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), FALSE);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, FALSE);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, FALSE);

    //Declarations
    LwDictionaryCachePrivate * priv = NULL;
    LwCacheFile *normalized_cache_file = NULL;
    LwCacheFile *parsed_cache_file = NULL;
    LwCacheFile *indexed_cache_file = NULL;
    LwParsed *parsed = NULL;
    LwIndexed *indexed = NULL;
    gboolean read_successful = FALSE;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);
    lw_dictionary_cache_clear (self);

    //Load the dictionary file
    normalized_cache_file = _read_cache_file (self, "normalized", EXPECTED_CHECKSUM, progress);
    if (normalized_cache_file == NULL) goto errored;
    
    //Load the parsed information
    parsed_cache_file = _read_cache_file (self, "parsed", EXPECTED_CHECKSUM, progress);
    if (parsed_cache_file == NULL) goto errored;
    parsed = lw_parsed_new (normalized_cache_file); 
    if (parsed == NULL) goto errored;
    lw_serializable_deserialize_from_cache_file_into (LW_SERIALIZABLE (parsed), EXPECTED_CHECKSUM, parsed_cache_file, progress);
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);

    //Load the indexed information
/*
    indexed_cache_file = _read_cache_file (self, "indexed", EXPECTED_CHECKSUM, progress);
    if (indexed_cache_file == NULL) goto errored;
    indexed = lw_indexed_new (parsed); 
    if (indexed == NULL) goto errored;
    lw_serializable_deserialize_from_cache_file_into (LW_SERIALIZABLE (indexed), EXPECTED_CHECKSUM, indexed_cache_file, progress);
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);
*/

    //Finalize
    lw_dictionary_cache_set_parsed (self, parsed);
    lw_dictionary_cache_set_indexed (self, indexed);

    read_successful = TRUE;

errored:

    if (parsed != NULL) g_object_unref (parsed);
    if (indexed != NULL) g_object_unref (indexed);

    if (normalized_cache_file != NULL) g_object_unref (normalized_cache_file);
    if (parsed_cache_file != NULL) g_object_unref (parsed_cache_file);
    if (indexed_cache_file != NULL) g_object_unref (indexed_cache_file);

    return read_successful;
}


static void
lw_dictionary_cache_set_progress (LwDictionaryCache * self,
                                 LwProgress        * progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_CACHE (self));

    //Declarations
    LwDictionaryCachePrivate * priv = NULL;
    LwDictionaryCacheClassPrivate * klasspriv = NULL;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);
    klasspriv = lw_dictionary_cache_class_get_private ();
    if (priv->progress == progress) goto errored;

    if (progress != NULL) g_object_ref (progress);
    if (priv->progress != NULL) g_object_unref (priv->progress);
    priv->progress = progress;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PROGRESS]);

errored:

    return;
}


LwProgress *
lw_dictionary_cache_get_progress (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PROGRESS (self), NULL);

    //Declarations
    LwDictionaryCachePrivate * priv = NULL;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);

    return priv->progress;
}


GType
lw_dictionary_cache_get_dictionary_type (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), G_TYPE_INVALID);

    //Declarations
    LwDictionaryCachePrivate * priv = NULL;

    //Initializatons
    priv = lw_dictionary_cache_get_instance_private (self);

    return priv->dictionary_type;
}


static void
lw_dictionary_cache_set_dictionary_type (LwDictionaryCache * self,
                                        GType               dictionary_type)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_CACHE (self));
    g_return_if_fail (dictionary_type != G_TYPE_NONE);
    g_return_if_fail (dictionary_type != G_TYPE_INVALID);

    //Declarations
    LwDictionaryCachePrivate * priv = NULL;
    LwDictionaryCacheClassPrivate * klasspriv = NULL;

    //Initializatons
    priv = lw_dictionary_cache_get_instance_private (self);
    klasspriv = lw_dictionary_cache_class_get_private ();
    if (dictionary_type == priv->dictionary_type) goto errored;

    priv->dictionary_type = dictionary_type;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_DICTIONARY_TYPE]);

errored:

    return;
}


static void
lw_dictionary_cache_set_flags (LwDictionaryCache * self,
                              LwUtf8Flag          flags)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_CACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);
    klasspriv = lw_dictionary_cache_class_get_private ();
    flags = lw_utf8flag_clean (flags);
    if (flags == priv->flags) goto errored;
    priv->flags = flags;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_FLAGS]);

errored:

    return;
}


/**
 * lw_dictionary_cache_get_flags:
 * @self: A #LwDictionaryCache
 * Returns: The #LwUtf8Flags that were set on construction
 */
LwUtf8Flag
lw_dictionary_cache_get_flags (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), 0);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);

    priv->flags;
}


static void
lw_dictionary_cache_set_dictionary_name (LwDictionaryCache * self,
                                       gchar const        * DICTIONARY_NAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_CACHE (self));

    //Declarations    
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);
    klasspriv = lw_dictionary_cache_class_get_private ();
    if (g_strcmp0 (DICTIONARY_NAME, priv->dictionary_name) == 0) goto errored;

    g_free (priv->dictionary_name);
    priv->dictionary_name = g_strdup (DICTIONARY_NAME);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_DICTIONARY_NAME]);

errored:

    return;
}


/**
 * lw_dictionary_cache_get_dictionary_name:
 * @self: A #LwDictionaryCache
 * Returns: (transfer none): The name of the dictionary cache set on consruction. This string is owned by the dictionary cache and so it should not be modified or freed.
 */
gchar const *
lw_dictionary_cache_get_dictionary_name (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);

    //Declarations    
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);

    return priv->dictionary_name;
}


gchar *
lw_dictionary_cache_build_filename (LwDictionaryCache * self,
                                   gchar const       * CACHETYPE) 
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), FALSE);

    //Declarations    
    LwDictionaryCachePrivate *priv = NULL;
    GFlagsClass *flags_class = NULL;
    gchar const *DICTIONARY_NAME = NULL;
    gchar const * * flag_names = NULL;
    gchar *filename = NULL;
    gint i = 0;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);
    flags_class = g_type_class_ref (LW_TYPE_UTF8FLAG);
    if (flags_class == NULL) goto errored;
    DICTIONARY_NAME = priv->dictionary_name;
    if (DICTIONARY_NAME == NULL) goto errored;
    flag_names = g_new0 (gchar const*, flags_class->n_values + 2);
    if (flag_names == NULL) goto errored;

    flag_names[i++] = DICTIONARY_NAME;
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
lw_dictionary_cache_build_directory (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);
    
    //Declarations
    gchar const * CACHE_DIR = NULL;
    gchar * path = NULL;
    GType dictionary_type = G_TYPE_NONE;

    //Initializations
    dictionary_type = lw_dictionary_cache_get_dictionary_type (self);
    if (dictionary_type == G_TYPE_NONE) goto errored;
    if (dictionary_type == G_TYPE_INVALID) goto errored;
    CACHE_DIR = g_getenv("DICTIONARY_CACHEDIR");
    if (CACHE_DIR == NULL) CACHE_DIR = g_get_user_cache_dir ();
    if (CACHE_DIR == NULL) goto errored;
    path = g_build_filename (CACHE_DIR, "libwaei", "dictionary", g_type_name (dictionary_type), NULL);
    if (path == NULL) goto errored;

    g_mkdir_with_parents (path, 0700);

errored:

    return path;
}


gchar *
lw_dictionary_cache_build_path (LwDictionaryCache * self,
                               gchar const       * CACHETYPE)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);
    
    //Declarations
    gchar * filename = NULL;
    gchar * path = NULL;
    gchar * directory = NULL;

    //Initializations
    directory = lw_dictionary_cache_build_directory (self);
    if (directory == NULL) goto errored;
    filename = lw_dictionary_cache_build_filename (self, CACHETYPE);
    if (filename == NULL) goto errored;
    path = g_build_filename (directory, filename, NULL);
    if (path == NULL) goto errored;

errored:

    g_free (directory); directory = NULL;
    g_free (filename); filename = NULL;

    return path;
}


static void
lw_dictionary_cache_set_parsed (LwDictionaryCache * self,
                               LwParsed          * parsed)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_CACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);
    klasspriv = lw_dictionary_cache_class_get_private ();
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
 * lw_dictionary_cache_get_parsed:
 * @self: A #LwDictionaryCache
 * Returns: (transfer none): The parsed data for the dictionary
 */
LwParsed *
lw_dictionary_cache_get_parsed (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);

    return priv->parsed;
}


static void
lw_dictionary_cache_set_indexed (LwDictionaryCache * self,
                                LwIndexed         * indexed)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_CACHE (self));

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwDictionaryCacheClassPrivate *klasspriv = NULL;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);
    klasspriv = lw_dictionary_cache_class_get_private ();
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
 * lw_dictionary_cache_get_indexed:
 * @self: A #LwDictionaryCache
 * Returns: (transfer none): The indexed information for the dictionary
 */
LwIndexed *
lw_dictionary_cache_get_indexed (LwDictionaryCache * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);

    return priv->indexed;
}


static gchar *
lw_dictionary_cache_create_normalized_temporary_file (LwDictionaryCache  * self,
                                                     GError            ** error)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Declarations
    gchar *path = NULL;
    gchar *tmpl = NULL;
    gchar const *TMPDIR = NULL;
    gint fd = -1;
    gchar const * DICTIONARY_NAME = NULL;
    gchar *filename = NULL;
    gboolean has_error = FALSE;
    gchar const * CONTENT = "uninitialized";

    //Initializations
    DICTIONARY_NAME = lw_dictionary_cache_get_dictionary_name (self);
    if (DICTIONARY_NAME == NULL) goto errored;
    tmpl = g_strdup_printf ("%s.normalized.XXXXXX", DICTIONARY_NAME);
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
lw_dictionary_cache_write_normalized_temporary_file (LwDictionaryCache * self,
                                                    gchar const       * CHECKSUM,
                                                    gchar const       * CONTENTS,
                                                    gsize               content_length,
                                                    LwProgress        * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (self), NULL);
    g_return_val_if_fail (CHECKSUM != NULL, NULL);
    g_return_val_if_fail (CONTENTS != NULL, NULL);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, NULL);

    //Declarations
    LwDictionaryCachePrivate *priv = NULL;
    LwUtf8Flag flags = 0;
    FILE * stream = NULL;
    GError *error = NULL;
    gchar *path = NULL;
    gboolean has_error = FALSE;

    //Initializations
    priv = lw_dictionary_cache_get_instance_private (self);
    flags = priv->flags;

    //Create the temporary file
    path = lw_dictionary_cache_create_normalized_temporary_file (self, &error);
    LW_PROGRESS_TAKE_ERROR (progress, error);

    //Write to it
    stream = g_fopen (path, "w+");
    fwrite (CHECKSUM, sizeof(gchar), strlen(CHECKSUM) + 1, stream);
    lw_utf8_normalize_chunked (CONTENTS, content_length, flags, (LwUtf8ChunkHandler) _fwrite_chunk, stream, progress);
    fwrite ("", sizeof(gchar), 1, stream);
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);

errored:

    if (stream != NULL) fclose(stream);

    return path;
}


gboolean
lw_dictionary_cache_trylock (LwDictionaryCache * self)
{
    LwDictionaryCachePrivate * priv = NULL;

    priv = lw_dictionary_cache_get_instance_private (self);

    return g_mutex_trylock (&priv->mutex);
}

void
lw_dictionary_cache_lock (LwDictionaryCache * self)
{
    LwDictionaryCachePrivate * priv = NULL;

    priv = lw_dictionary_cache_get_instance_private (self);

    g_mutex_lock (&priv->mutex);
}


void
lw_dictionary_cache_unlock (LwDictionaryCache * self)
{
    LwDictionaryCachePrivate * priv = NULL;

    priv = lw_dictionary_cache_get_instance_private (self);

    g_mutex_unlock (&priv->mutex);
}
