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
//!  @file dictionary.c
//!
//!  @brief LwDictionary objects represent a loaded dictionary that the program
//!         can use to carry out searches.  You can uninstall dictionaries
//!         by using the object, but you cannot install them. LwDictInst
//!         objects exist for that purpose.
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/io.h>
#include <libwaei/dictionary.h>

#include <libwaei/dictionary-private.h>


G_DEFINE_DYNAMIC_TYPE_EXTENDED (LwDictionary, lw_dictionary, G_TYPE_OBJECT, G_TYPE_FLAG_ABSTRACT, {})


LwDictionary *
lw_dictionary_new (GType         type,
                   gchar const * FILENAME)
{
    return LW_DICTIONARY (g_object_new (type, "filename", FILENAME, NULL));
}


static void 
lw_dictionary_init (LwDictionary *self)
{
    self->priv = LW_DICTIONARY_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwDictionaryPrivate));
}


static void 
lw_dictionary_finalize (GObject *object)
{
    //Declarations
    LwDictionary *self = NULL;
    LwDictionaryPrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARY (object);
    priv = self->priv;

    g_free (priv->filename); priv->filename = NULL;
    g_free (priv->name); priv->name = NULL;

    lw_dictionary_set_cachetree (self, NULL);

    memset(self->priv, 0, sizeof(LwDictionaryPrivate));

    G_OBJECT_CLASS (lw_dictionary_parent_class)->finalize (object);
}


static void 
lw_dictionary_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwDictionary *self;
    LwDictionaryPrivate *priv;

    //Initializations
    self = LW_DICTIONARY (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_FILENAME:
        lw_dictionary_set_filename (self, g_value_get_string (value));
        break;
      case PROP_CACHETREE:
        lw_dictionary_set_cachetree (self, g_value_get_pointer (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionary_get_property (GObject      *object,
                            guint         property_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwDictionary *self = NULL;
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARY (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_FILENAME:
        g_value_set_string (value, lw_dictionary_get_filename (self));
        break;
      case PROP_NAME:
        g_value_set_string (value, lw_dictionary_get_name (self));
        break;
      case PROP_PATH:
        g_value_set_string (value, lw_dictionary_get_path (self));
        break;
      case PROP_ID:
        g_value_set_string (value, lw_dictionary_get_id (self));
        break;
      case PROP_CACHETREE:
        g_value_set_pointer (value, lw_dictionary_get_cachetree (self));
        break;
      case PROP_CHECKSUM:
        g_value_set_static_string (value, lw_dictionary_get_checksum (self));
        break;
      case PROP_CONTENTS:
        g_value_set_static_string (value, lw_dictionary_get_contents (self));
        break;
      case PROP_CONTENT_LENGTH:
        g_value_set_long (value, lw_dictionary_length (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_dictionary_class_finalize (LwDictionaryClass *klass)
{
    memset(klass->priv, 0, sizeof(LwDictionaryClassPrivate));
    g_free (klass->priv);
    klass->priv = NULL;
}


static void
lw_dictionary_class_init (LwDictionaryClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwDictionaryClassPrivate, 1);
    object_class->set_property = lw_dictionary_set_property;
    object_class->get_property = lw_dictionary_get_property;
    object_class->finalize = lw_dictionary_finalize;

    g_type_class_add_private (object_class, sizeof (LwDictionaryPrivate));

    klass->priv->pspec[PROP_FILENAME] = g_param_spec_string (
      "filename",
      gettext("Filename"),
      "Set the self's data.filename",
      "",
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_FILENAME, klass->priv->pspec[PROP_FILENAME]);

    klass->priv->pspec[PROP_PATH] = g_param_spec_string (
      "path",
      gettext("Path"),
      "Set the self's data.filename",
      "",
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_PATH, klass->priv->pspec[PROP_PATH]);

    klass->priv->pspec[PROP_ID] = g_param_spec_string (
      "id",
      gettext("Id"),
      "Set the self's data.filename",
      "",
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_ID, klass->priv->pspec[PROP_ID]);

    klass->priv->pspec[PROP_CACHETREE] = g_param_spec_pointer (
      "cache-tree",
      gettext("Buffer"),
      "Set the self's data.filename",
      G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_CACHETREE, klass->priv->pspec[PROP_CACHETREE]);

    klass->priv->pspec[PROP_CHECKSUM] = g_param_spec_string (
      "checksum",
      gettext("Checksum"),
      "Set the self's data.filename",
      "",
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CHECKSUM, klass->priv->pspec[PROP_CHECKSUM]);

    klass->priv->pspec[PROP_CONTENTS] = g_param_spec_string (
      "contents",
      gettext("Contents"),
      "Set the self's data.filename",
      "",
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS, klass->priv->pspec[PROP_CONTENTS]);

    klass->priv->pspec[PROP_CONTENT_LENGTH] = g_param_spec_ulong (
      "content-length",
      gettext("Content Length"),
      "Set the self's data.filename",
      0,
      G_MAXULONG,
      0,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENT_LENGTH, klass->priv->pspec[PROP_CONTENT_LENGTH]);

    klass->priv->pspec[PROP_CACHEFILE] = g_param_spec_object (
      "cachefile",
      gettext("Content Mapped File"),
      "Set the self's data.filename",
      LW_TYPE_MAPPEDFILE,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CACHEFILE, klass->priv->pspec[PROP_CACHEFILE]);
}



GType lw_dictionarycolumnhandling_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      GEnumValue values[] = {
        { LW_DICTIONARYCOLUMNHANDLING_UNUSED, LW_DICTIONARYCOLUMNHANDLINGNAME_UNUSED, LW_DICTIONARYCOLUMNHANDLINGNICK_UNUSED },
        { LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, LW_DICTIONARYCOLUMNHANDLINGNAME_INDEX_AND_SEARCH, LW_DICTIONARYCOLUMNHANDLINGNICK_INDEX_AND_SEARCH },
        { LW_DICTIONARYCOLUMNHANDLING_FILTER_ONLY, LW_DICTIONARYCOLUMNHANDLINGNAME_FILTER_ONLY, LW_DICTIONARYCOLUMNHANDLINGNICK_FILTER_ONLY },
        { 0, NULL, NULL },
      };

      type = g_enum_register_static ("LwDictionaryColumnHandling", values);
    }

    return type;
}


gint
lw_dictionary_get_total_columns (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), 0);

    //Declarations
    LwDictionaryClass *klass = NULL;
    gint total_columns = 0;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    total_columns = klass->priv->get_total_columns (self);
  
    return total_columns;
}
                                 

gchar const *
lw_dictionary_get_column_language (LwDictionary *self,
                                   gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);
    g_return_val_if_fail (column_num > -1, NULL);

    //Declarations
    LwDictionaryClass *klass = NULL;
    gint total_columns = 0;
    gchar const *language = NULL;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    total_columns = lw_dictionary_get_total_columns (self);
    if (total_columns < 1 || column_num >= total_columns) goto errored;
    language = klass->priv->get_column_language (self, column_num);

errored:

    return language;
}


LwDictionaryColumnHandling
lw_dictionary_get_column_handling (LwDictionary *self,
                                   gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), LW_DICTIONARYCOLUMNHANDLING_UNUSED);
    g_return_val_if_fail (column_num > -1, LW_DICTIONARYCOLUMNHANDLING_UNUSED);

    //Declarations
    LwDictionaryClass *klass = NULL;
    gint total_columns = 0;
    gint handling = LW_DICTIONARYCOLUMNHANDLING_UNUSED;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    total_columns = lw_dictionary_get_total_columns (self);
    if (total_columns < 1 || column_num >= total_columns) goto errored;
    handling = klass->priv->get_column_handling (self, column_num);

errored:

    return handling;
}


//!
//! @brief Deletes a LwDictionary from the harddrive.  LwDictInst objects are used
//!        for installing dictionaries that do not exist yet.  You still need to free
//!        the object after.
//! @param self An LwDictionary object to get the paths for the self file.
//! @param cb A LwIoProgresSCallback to show self uninstall progress or NULL.
//! @param error A pointer to a GError object to pass errors to or NULL.
//!
gboolean 
lw_dictionary_uninstall (LwDictionary *self)
{
  printf("BREAK1 lw_dictionary_uninstall \n");
    //Sanity check
    g_return_val_if_fail (self != NULL, FALSE);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwProgress *progress = NULL;
    gchar const *PATH = NULL;
    gchar const *MESSAGE = NULL;
    gchar const *name = lw_dictionary_get_name (self);

    //Initializations
    priv = self->priv;
    progress = priv->progress;
    PATH = lw_dictionary_get_path (self);
    if (PATH == NULL) goto errored;
  printf("BREAK2 lw_dictionary_uninstall \n");

    MESSAGE = gettext("Uninstalling %s Dictionary...");
    lw_progress_set_primary_message_printf (progress, MESSAGE, name);

    MESSAGE = gettext("Removing %s...");
    lw_progress_set_secondary_message_printf (progress, MESSAGE, PATH);
  printf("BREAK3 lw_dictionary_uninstall \n");

    lw_io_remove (PATH, progress);
  printf("BREAK4 lw_dictionary_uninstall \n");

errored:
  printf("BREAK5 lw_dictionary_uninstall \n");

    return (!lw_progress_errored (progress));
}
 

FILE *
lw_dictionary_open (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    FILE *file = NULL;
    gchar const *PATH = NULL;

    //Initializations
    PATH = lw_dictionary_get_path (self);
    if (PATH == NULL) goto errored;
    
    file = g_fopen (PATH, "r");

errored:

    return file;
}

gchar const *
lw_dictionary_get_install_directory ()
{
    //Declarations
    static gchar * path = NULL;

    if (path == NULL)
    {
      gchar const * DATA_DIR = NULL;

      DATA_DIR = g_get_user_data_dir ();
      if (DATA_DIR == NULL) goto errored;
      path = g_build_filename (DATA_DIR, "libwaei", "dictionary");
      if (path == NULL) goto errored;
    }
    if (path == NULL) goto errored;

     g_mkdir_with_parents (path, 0700);

errored:

    return path;
}


gchar const * 
lw_dictionary_get_path (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->path;
}

gchar *
lw_dictionary_build_path_by_type_and_name (GType         type,
                                           gchar const * FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (g_type_is_a (type, LW_TYPE_DICTIONARY), NULL);
    g_return_val_if_fail (FILENAME != NULL, NULL);

    //Declarations
    gchar const * INSTALL_DIRECTORY = NULL;
    gchar const * TYPENAME = NULL;
    gchar *path = NULL;

    //Initializations
    INSTALL_DIRECTORY = lw_dictionary_get_install_directory ();
    if (INSTALL_DIRECTORY == NULL) goto errored;
    TYPENAME = g_type_name (type);
    if (TYPENAME == NULL) goto errored;
    path = g_build_filename (INSTALL_DIRECTORY, TYPENAME, FILENAME, NULL);
    if (path == NULL) goto errored;

errored:

    return path;
}


static gchar *
lw_dictionary_build_path (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    gchar const * INSTALL_DIRECTORY = NULL;
    GType type = G_TYPE_INVALID;
    gchar const *FILENAME = NULL;
    gchar *path = NULL;

    //Initializations
    INSTALL_DIRECTORY = lw_dictionary_get_install_directory ();
    if (INSTALL_DIRECTORY == NULL) goto errored;
    type = G_OBJECT_TYPE (self);
    if (type == G_TYPE_INVALID) goto errored;
    FILENAME = lw_dictionary_get_filename (self);
    if (FILENAME == NULL) goto errored;
    path = lw_dictionary_build_path_by_type_and_name (type, FILENAME);
    if (path == NULL) goto errored;

errored:

    return path;
}


static void
lw_dictionary_sync_path (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    gchar *path = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_CLASS (self);
    path = lw_dictionary_build_path (self);
    if (g_strcmp0 (path, priv->path) == 0) goto errored;

    g_free(priv->path);
    priv->path = path;
    path = NULL;

    lw_dictionary_set_cachetree (self, NULL);
    lw_dictionary_sync_cachefile (self);

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_PATH]);

errored:

    g_free (path); path = NULL;
}


gchar const *
lw_dictionary_get_name (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv;

    //Initializations
    priv = self->priv;

    return priv->name;
}


gchar const *
lw_dictionary_get_filename (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->filename;
}


void
lw_dictionary_set_filename (LwDictionary * self,
                            gchar const  * FILENAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    
    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_CLASS (self);
    if (g_strcmp0 (FILENAME, priv->filename) == 0) goto errored;

    g_free (priv->filename);
    priv->filename = g_strdup (FILENAME);

    g_free (priv->name);
    priv->name = g_strdup (gettext(priv->filename));

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_FILENAME]);

    lw_dictionary_sync_id (self);
    lw_dictionary_sync_path (self);

errored:

    return;
}


void
lw_dictionary_set_cache (LwDictionary      * self,
                         LwDictionaryCache * cache)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    gpointer key = NULL;
    LwUtf8Flag flags = 0;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_CLASS (self);
    flags = lw_dictionarycache_get_flags (cache);
    key = GINT_TO_POINTER (flags);
    if (priv->caches == NULL) goto errored;
    if (g_tree_lookup (priv->caches, key) == cache) goto errored;

    if (cache != NULL)
    {
      g_tree_insert (priv->caches, key, cache);
    }
    else
    {
      g_tree_remove (priv->caches, key);
    }

errored:

    return;
}


static gint
_flag_compare_function (gconstpointer a,
                        gconstpointer b)
{
  gint af = GPOINTER_TO_INT (a);
  gint ab = GPOINTER_TO_INT (b);

  return ab - af;
}


struct _DictionaryCacheData {
  LwDictionary *dictionary;
  LwProgress *progress;
};


LwParsed * _dictionarycache_parse (LwCacheFile                 * cache_file,
                                   struct _DictionaryCacheData * data)
{
    g_return_val_if_fail (LW_IS_CACHEFILE (cache_file), NULL);
    g_return_val_if_fail (data != NULL, NULL);

    LwDictionary *dictionary = NULL;
    LwProgress *progress = NULL;

    dictionary = data->dictionary;
    g_return_val_if_fail (LW_IS_DICTIONARY (dictionary), NULL);
    progress = data->progress;
    g_return_val_if_fail (LW_IS_PROGRESS (progress), NULL);

    return lw_dictionary_parse (dictionary, cache_file, progress);
}


void
lw_dictionary_ensure_cache (LwDictionary      *self,
                            LwDictionaryCache *cache,
                            LwProgress        *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));
    g_return_if_fail (cache != NULL);

    //Declarations
    LwCacheFile *cache_file = NULL;
    struct _DictionaryCacheData data = {
      .dictionary = self,
      .progress = progress
    };

    //Initializations
    cache_file = lw_dictionary_get_cachefile (self);
    if (cache_file == NULL) goto errored;
    
    lw_dictionarycache_set_contents (cache, cache_file, (LwDictionaryCacheParseFunc) _dictionarycache_parse, &data, progress);

errored:

    return;
}


LwDictionaryCache*
lw_dictionary_get_cache (LwDictionary *self,
                         LwProgress   *progress,
                         LwUtf8Flag    flags)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    GTree *caches = NULL;
    LwDictionaryCache *cache = NULL;
    gchar const *FILENAME = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_CLASS (G_OBJECT_GET_CLASS (self));
    FILENAME = lw_dictionary_get_filename (self);

    caches = lw_dictionary_get_cachetree (self);
    if (caches == NULL)
    {
      caches = g_tree_new (_flag_compare_function);
      lw_dictionary_set_cachetree (self, caches);
    }
    if (caches == NULL) goto errored;

    cache = g_tree_lookup (caches, GINT_TO_POINTER (flags));
    if (cache == NULL)
    {
      cache = lw_dictionarycache_new (FILENAME, flags);
      lw_dictionary_set_cache (self, cache);
      lw_dictionary_ensure_cache (self, cache, progress);
    }

errored:

    return cache;
}


gboolean
lw_dictionary_equals (LwDictionary * dictionary1, 
                      LwDictionary * dictionary2)
{
    //Sanity checks
    g_return_val_if_fail (dictionary1 != NULL && dictionary2 != NULL, FALSE);

    //Declarations
    gchar const *FILENAME1;
    gchar const *FILENAME2;
    GType type1;
    GType type2;
    gboolean filenames_are_equal;
    gboolean types_are_equal;

    //Initializations
    FILENAME1 = lw_dictionary_get_filename (dictionary1);
    FILENAME2 = lw_dictionary_get_filename (dictionary2);
    type1 = G_OBJECT_TYPE (dictionary1);
    type2 = G_OBJECT_TYPE (dictionary2);

    filenames_are_equal = (strcmp(FILENAME1, FILENAME2) == 0);
    types_are_equal = g_type_is_a (type1, type2);

    return (filenames_are_equal && types_are_equal);
}


gchar const*
lw_dictionary_get_id (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->id;
}


static void
lw_dictionary_set_id (LwDictionary *self,
                      gchar const  *ID)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate * priv = NULL;
    LwDictionaryClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_GET_CLASS (self);

    if (g_strcmp0 (ID, priv->id) == 0) goto errored;

    g_free (priv->id);
    priv->id = g_strdup (ID);

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_ID]);

errored:

    return;
}


static void
lw_dictionary_sync_id (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    gchar const * DIRECTORYNAME = NULL;
    gchar const *FILENAME = NULL;
    gchar * id = NULL;

    //Initializations
    DIRECTORYNAME = G_OBJECT_TYPE_NAME (self);
    if (DIRECTORYNAME == NULL) goto errored;
    FILENAME = lw_dictionary_get_filename (self);
    if (FILENAME == NULL) goto errored;
    id = g_strdup_printf ("%s/%s", DIRECTORYNAME, FILENAME);
    if (id == NULL) goto errored;

    lw_dictionary_set_id (self, id);

errored:

    g_free (id);
    id = NULL;
}


void
lw_dictionary_regex_search (LwDictionary  *self,
                            LwSearch      *search)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);
    g_return_val_if_fail (LW_IS_SEARCH (self), NULL);

/*TODO
    //Declarations
    LwDictionaryCache *buffer = NULL;
    GList *matchlist = NULL;
    GRegex *regex = NULL;
    gint chunk = 0;
    LwProgress *progress = NULL;
    gchar **lines = NULL;
    gint num_lines = -1;

    //Initializations
    buffer = lw_dictionary_get_buffer (self);
    if (buffer == NULL) goto errored;
    regex = lw_search_get_regex (search);
    if (regex == NULL) goto errored;

    lw_progress_set_primary_message_printf (progress, "Searching %d self...", lw_dictionary_get_name (self));
    lw_progress_set_total (progress, num_lines);

    {
      gint i = 0;
      for (i = 0; i < num_lines && !lw_progress_should_abort (progress); i++)
      {
        /*TODO
        gchar const *BUFFER = lw_dictionarycache_get_line (buffer, i);
        if (BUFFER == NULL) break;

        if (g_regex_match (regex, BUFFER, 0, NULL) == TRUE)
        {
          matchlist = g_list_prepend (matchlist, GINT_TO_POINTER (i));
        }

        lw_progress_set_current (progress, i);

        if (chunk > LW_SEARCH_MAX_CHUNK) 
        {
          chunk = 0;
          //TODO
        }
        chunk++;
      }
    }

  errored:
*/
  return;
}


static gint
_count_max_columns (LwDictionary *self, gchar **lines, gint num_lines)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), -1);
    g_return_val_if_fail (lines != NULL, -1);

    //Declarations
    gint i = 0;
    gint count = 0;
    gint c = 0;

    //Initializations
    if (lines[i] != NULL)
    {
      while (lines[i + 1] != NULL)
      {
        c = lines[i + 1] - lines[i];
        if (c > count) count = c;
        i++;
      }
      c = strlen(lines[i++]);
      if (c > count) count = c;
    }

    return count;
}


LwParsedLine*
lw_dictionary_build_lines (LwDictionary      *self,
                           LwDictionaryCache *cache)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);
    g_return_val_if_fail (cache != NULL, NULL);

    //Declarations
    LwDictionaryClass *klass = NULL;
    gchar *str = NULL;
    gchar **lines = NULL;
    gint num_lines = -1;
    gint max_column_count = -1;
    gchar **column_buffer = NULL;
    LwParsedLine *dictionary_lines = NULL;
/*TODO
    //Initializations
    klass = LW_DICTIONARY_CLASS (self);
    if (klass->priv->columnize == NULL) goto errored;
    if (klass->priv->load_columns == NULL) goto errored;
    str = lw_dictionarycache_str (buffer);
    if (str == NULL) goto errored;
    lines = lw_utf8_split_lines (str, &num_lines);
    if (lines == NULL) goto errored;
    max_column_count = _count_max_columns (self, lines, num_lines);
    if (max_column_count < 1) goto errored;
    column_buffer = g_new (gchar*, max_column_count);
    if (column_buffer == NULL) goto errored;
    dictionary_lines = g_new (LwParsedLine, num_lines);
    if (dictionary_lines == NULL) goto errored;

    {
      gint i = 0;
      gint num_columns = 0;
      for (i = 0; i < num_lines; i++)
      {
        klass->priv->columnize (self, lines[i], column_buffer, &num_columns);
        klass->priv->load_columns (self, dictionary_lines + i, column_buffer, num_columns);
      }
    }

errored:

    g_free (column_buffer); column_buffer = NULL;
*/
    return dictionary_lines;
}


LwParsedLine**
lw_dictionary_get_lines (LwDictionary *self,
                         LwUtf8Flag    flags,
                         gint         *num_lines)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryCache *cache = NULL;
    LwParsedLine **lines = NULL;
/*
TODO
    if (priv->lines != NULL) BLAH

    //Initializations
    buffer = lw_dictionary_get_buffer (self, flags);
    if (buffer == NULL) goto errored;
    lines = lw_dictionarycache_get_lines (buffer, &num_lines);

    if (lines == NULL)
    {
      lines = lw_dictionary_build_lines (self, &num_lines);
      lw_dictionarycache_set_lines (self, lines, num_lines);
    }

errored:

    TODO
*/
    return lines;
}


// Properties


static void
lw_dictionary_set_cachefile (LwDictionary * self,
                             LwCacheFile  * cachefile)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));
    g_return_if_fail (LW_IS_CACHEFILE (cachefile));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_GET_CLASS (self);

    if (cachefile != NULL)
    {
      g_object_ref (cachefile);
    }

    if (priv->cachefile != NULL)
    {
      g_object_unref (priv->cachefile);
    }

    priv->cachefile = cachefile;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_CACHEFILE]);

errored:

    return;
}


static LwCacheFile*
lw_dictionary_get_cachefile (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->cachefile;
}


static void
lw_dictionary_sync_cachefile (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwCacheFile *cachefile = NULL;

    //Initializations
    priv = self->priv;

    if (priv->path != NULL)
    {
      cachefile = lw_cachefile_new (priv->path);
    }

    lw_dictionary_set_cachefile (self, cachefile);
}


gsize
lw_dictionary_length (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, -1);

    //Declarations
    LwCacheFile *cachefile = NULL;
    gsize length = 0;

    //Initializations
    cachefile = lw_dictionary_get_cachefile (self);
    if (cachefile == NULL) goto errored;
    length = lw_cachefile_length (cachefile);
    if (length == 0) goto errored;

errored:

    return length;
}


gchar const *
lw_dictionary_get_contents (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwCacheFile *cachefile = NULL;
    gchar const * CONTENTS = NULL;

    //Initializations
    cachefile = lw_dictionary_get_cachefile (self);
    if (cachefile == NULL) goto errored;
    CONTENTS = lw_cachefile_get_contents (cachefile);
    if (CONTENTS == NULL) goto errored;

errored:

    return CONTENTS;
}


gchar const *
lw_dictionary_get_checksum (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwCacheFile * cachefile = NULL;
    gchar const * CHECKSUM = NULL;

    //Initializations
    cachefile = lw_dictionary_get_cachefile (self);
    if (cachefile == NULL) goto errored;
    CHECKSUM = lw_cachefile_get_checksum (cachefile);
    if (CHECKSUM == NULL) goto errored;

errored:

    return CHECKSUM;
}


void
lw_dictionary_set_cachetree (LwDictionary * self,
                             GTree        * tree)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;

    //Initializations
    priv = self->priv;
    if (priv->caches == tree) goto errored;
    klass = LW_DICTIONARY_CLASS (self);

    if (tree != NULL)
    {
      g_tree_ref (tree);
    }

    if (priv->caches == NULL)
    {
      g_tree_unref (priv->caches);
    }

    priv->caches = tree;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_CACHETREE]);

errored:

    return;
}


GTree*
lw_dictionary_get_cachetree (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->caches;
}


static gchar**
lw_dictionary_columnize (LwDictionary *self,
                         gchar         *buffer,
                         gchar        **tokens,
                         gsize         *num_tokens)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryClass *klass = NULL;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    g_return_val_if_fail (klass->priv->columnize != NULL, NULL);

    return klass->priv->columnize(self, buffer, tokens, num_tokens);
}



static void
lw_dictionary_load_columns (LwDictionary  *self,
                            gchar         *buffer,
                            gchar        **tokens,
                            gint           num_tokens,
                            LwParsedLine  *line) 
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryClass *klass = NULL;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    g_return_if_fail (klass->priv->load_columns != NULL);

    klass->priv->load_columns(self, buffer, tokens, num_tokens, line);
}


LwParsed*
lw_dictionary_parse (LwDictionary *self,
                     LwCacheFile *cache_file,
                     LwProgress   *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);
    g_return_val_if_fail (LW_IS_CACHEFILE (cache_file), NULL);
    g_return_val_if_fail (LW_IS_PROGRESS (progress), NULL);

    //Declarations
    gchar *contents = NULL;
    gsize content_length = 0;
    gint num_lines = 0;
    LwParsed *parsed = NULL; 
    LwParsedLine* lines = NULL;
    gchar **tokens = NULL;
    gsize max_line_length = 0;
    gsize num_tokens = 0;

    //Initializations
    contents = lw_cachefile_get_contents (cache_file);
    if (contents == NULL) goto errored;
    content_length = lw_cachefile_length (cache_file);
    num_lines = lw_utf8_replace_linebreaks_with_nullcharacter (contents, content_length, &max_line_length, progress);
    if (num_lines == 0) goto errored;
    if (max_line_length < 1) goto errored;
    parsed = lw_parsed_new (cache_file);
    if (parsed == NULL) goto errored;
    lines = g_new0 (LwParsedLine, num_lines);
    tokens = g_new0 (gchar*, max_line_length + 1);
    if (tokens == NULL) goto errored;

    if (progress != NULL)
    {
      lw_progress_set_secondary_message (progress, "Parsing...");
      lw_progress_set_completed (progress, FALSE);
      lw_progress_set_total (progress, content_length);
      lw_progress_set_current (progress, 0);
    }

    {
      gchar *c = contents;
      gchar *e = contents + content_length;
      gint i = 0;
      LwParsedLine *line = NULL;
      while (c < e)
      {
        while (c < e && *c == '\0') c = g_utf8_next_char (c);
        if (c >= e) break;

        line = lines + i;
        lw_parsedline_init (line);
        lw_dictionary_columnize (self, c, tokens, &num_tokens);
        lw_dictionary_load_columns (self, contents, tokens, num_tokens, line);
        if (progress != NULL)
        {
          lw_progress_set_current (progress, c - contents);
        }
        i++;
        while (c < e && *c != '\0') c = g_utf8_next_char (c);
      }
    }

    if (progress != NULL)
    {
      lw_progress_set_current (progress, content_length);
      lw_progress_set_completed (progress, TRUE);
    }

errored:

    g_free (tokens); tokens = NULL;
    if (parsed != NULL) g_object_unref (parsed); parsed = NULL;

    return parsed;
}
