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
 * SECTION: dictionary
 * @short_description: Parse, normalize, and uninstall Dictionaries
 * @title: LwDictionary
 * @include: libwaei/dictionary.h
 *
 * Handles dictionary parsing, installing, uninstall, and normalization caching.
 * #LwDictionary is an abstract object with a number of virtual methods that must
 * be implemented before it can be used. The *_class_init() will usually have a form similar to below:
 *
 * |[<!-- langauge="C" -->
 * static void
 * lw_edictionary_class_init (LwEDictionaryClass *klass)
 * {
 *     //Declarations
 *     GObjectClass *object_class;
 *     LwDictionaryClass *dictionary_class;
 *
 *     //Initializations
 *     object_class = G_OBJECT_CLASS (klass);
 *     object_class->finalize = lw_edictionary_finalize;
 *     object_class->constructed = lw_edictionary_constructed;
 *
 *     dictionary_class = LW_DICTIONARY_GET_CLASS (klass);
 *     dictionary_class->get_column_handling = lw_edictionary_get_column_handling;
 *     dictionary_class->get_total_columns = lw_edictionary_get_total_columns;
 *     dictionary_class->get_column_language = lw_edictionary_get_column_language;
 *     dictionary_class->columnize = lw_edictionary_columnize;
 *     dictionary_class->load_columns = lw_edictionary_load_columns;
 *     dictionary_class->calculate_applicable_columns_for_text = lw_edictionary_calculate_applicable_columns_for_text;
 *     dictionary_class->columnid_get_type = lw_edictionary_columnid_get_type;
 * }
 * ]|
 */

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


G_DEFINE_TYPE_EXTENDED (LwDictionary, lw_dictionary, G_TYPE_OBJECT, G_TYPE_FLAG_ABSTRACT, {})



/**
 * lw_dictionary_new:
 * @type: A #GType that is a subclass of #LwDictionary
 * @FILENAME: (transfer none): A filename to load using the program searchpaths
 * Returns: (transfer full): A new #LwDictionary that should be freed with g_object_unref()
 */
LwDictionary *
lw_dictionary_new (GType         type,
                   gchar const * FILENAME)
{
    return LW_DICTIONARY (g_object_new (type, "contents-filename", FILENAME, NULL));
}


static void 
lw_dictionary_init (LwDictionary * self)
{
    self->priv = LW_DICTIONARY_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwDictionaryPrivate));

    g_mutex_init (&self->priv->mutex);
}


static void 
lw_dictionary_finalize (GObject *object)
{
    //Declarations
    LwDictionary * self = NULL;
    LwDictionaryPrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARY (object);
    priv = self->priv;

    g_free (priv->id);
    g_free (priv->name);

    g_free (priv->contents_filename);
    g_free (priv->contents_path);

    g_mutex_clear (&self->priv->mutex);

    if (priv->contents_mappedfile) g_object_unref (priv->contents_mappedfile);
    lw_dictionary_set_parsed_cachetree (self, NULL);

    memset(self->priv, 0, sizeof(LwDictionaryPrivate));

    G_OBJECT_CLASS (lw_dictionary_parent_class)->finalize (object);
}


static void 
lw_dictionary_set_property (GObject      * object,
                            guint          property_id,
                            const GValue * value,
                            GParamSpec   * pspec)
{
    //Declarations
    LwDictionary * self;
    LwDictionaryPrivate * priv;

    //Initializations
    self = LW_DICTIONARY (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_CONTENTS_FILENAME:
        lw_dictionary_set_contents_filename (self, g_value_get_string (value));
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
    LwDictionary * self = NULL;
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARY (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_NAME:
        g_value_set_string (value, lw_dictionary_get_name (self));
        break;
      case PROP_ID:
        g_value_set_string (value, lw_dictionary_get_id (self));
        break;
      case PROP_CONTENTS_FILENAME:
        g_value_set_string (value, lw_dictionary_get_contents_filename (self));
        break;
      case PROP_CONTENTS_PATH:
        g_value_set_string (value, lw_dictionary_get_contents_path (self));
        break;
      case PROP_CONTENTS_CHECKSUM:
        g_value_set_static_string (value, lw_dictionary_get_contents_checksum (self));
        break;
      case PROP_CONTENTS:
        g_value_set_static_string (value, lw_dictionary_get_contents (self));
        break;
      case PROP_CONTENT_LENGTH:
        g_value_set_long (value, lw_dictionary_contents_length (self));
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
lw_dictionary_class_init (LwDictionaryClass * klass)
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

    klass->priv->pspec[PROP_ID] = g_param_spec_string (
      "id",
      gettext("Id"),
      gettext("Unique identifier for the dictionary"),
      NULL,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_ID, klass->priv->pspec[PROP_ID]);

    klass->priv->pspec[PROP_NAME] = g_param_spec_string (
      "name",
      gettext("Name"),
      gettext("Human readable (and localized) name for the dictionary"),
      NULL,
      G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_NAME, klass->priv->pspec[PROP_NAME]);

    klass->priv->pspec[PROP_CONTENTS] = g_param_spec_string (
      "contents",
      gettext("Contents"),
      gettext("Raw contents of the dictionary"),
      NULL,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS, klass->priv->pspec[PROP_CONTENTS]);

    klass->priv->pspec[PROP_CONTENT_LENGTH] = g_param_spec_ulong (
      "contents-length",
      gettext("Content Length"),
      gettext("Byte length of the raw contents of the dictionary"),
      0,
      G_MAXULONG,
      0,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENT_LENGTH, klass->priv->pspec[PROP_CONTENT_LENGTH]);

    klass->priv->pspec[PROP_CONTENTS_FILENAME] = g_param_spec_string (
      "contents-filename",
      gettext("Filename"),
      gettext("Filename of the dictionary"),
      NULL,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS_FILENAME, klass->priv->pspec[PROP_CONTENTS_FILENAME]);

    klass->priv->pspec[PROP_CONTENTS_PATH] = g_param_spec_string (
      "contents-path",
      gettext("Path"),
      gettext("Full path of the dictionary"),
      NULL,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS_PATH, klass->priv->pspec[PROP_CONTENTS_PATH]);


    klass->priv->pspec[PROP_CONTENTS_CHECKSUM] = g_param_spec_string (
      "contents-checksum",
      gettext("Contents Checksum"),
      gettext("Checksum of the raw dictionary file"),
      NULL,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS_CHECKSUM, klass->priv->pspec[PROP_CONTENTS_CHECKSUM]);
}



/**
 * lw_dictionarycolumnhandling_get_type:
 * Returns: The #GType of the enumeration for column handling
 */
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


/**
 * lw_dictionary_total_columns:
 * @self: A #LwDictionary
 * Returns: The total number of columns as a %gint
 */
gint
lw_dictionary_total_columns (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), 0);

    //Declarations
    LwDictionaryClass *klass = NULL;
    gint total_columns = 0;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    total_columns = klass->get_total_columns (self);
  
    return total_columns;
}
                                 

/**
 * lw_dictionary_get_column_language:
 * @self: A #LwDictionary
 * @column_num: The column to query it's langauge
 * Returns: (transfer none): The language of the column as a string.  This string is owned by the dictionary, so it should not be modified or freed.
 */
gchar const *
lw_dictionary_get_column_language (LwDictionary * self,
                                   gint           column_num)
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
    total_columns = lw_dictionary_total_columns (self);
    if (total_columns < 1 || column_num >= total_columns) goto errored;
    language = klass->get_column_language (self, column_num);

errored:

    return language;
}


/**
 * lw_dictionary_get_column_handling:
 * @self: A #LwDictionary
 * @column_num: The column to get the handling of
 * Returns: A %LwDictionaryColumnHandling denoting the type of processing that should be handled on this column
 */
LwDictionaryColumnHandling
lw_dictionary_get_column_handling (LwDictionary * self,
                                   gint           column_num)
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
    total_columns = lw_dictionary_total_columns (self);
    if (total_columns < 1 || column_num >= total_columns) goto errored;
    handling = klass->get_column_handling (self, column_num);

errored:

    return handling;
}


/**
 * lw_dictionary_uninstall:
 * @self: A #LwDictionary
 * Returns: %TRUE if deletion was sucessful.
 */
gboolean 
lw_dictionary_uninstall (LwDictionary * self)
{
  printf("BREAK1 lw_dictionary_uninstall \n");
    //Sanity check
    g_return_val_if_fail (self != NULL, FALSE);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwProgress * progress = NULL;
    gchar const *PATH = NULL;
    gchar const *MESSAGE = NULL;
    gchar const *name = lw_dictionary_get_name (self);

    //Initializations
    priv = self->priv;
    progress = priv->progress;
    PATH = lw_dictionary_get_contents_path (self);
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
 

/**
 * lw_dictionary_open:
 * @self: A #LwDictionary
 *
 * A convenience method for opening the raw dictionary contents file.
 * 
 * Returns: (transfer full): A C #FILE that can be used to read the file.  It should be closed with fclose()
 */
FILE *
lw_dictionary_open (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    FILE *file = NULL;
    gchar const *PATH = NULL;

    //Initializations
    PATH = lw_dictionary_get_contents_path (self);
    if (PATH == NULL) goto errored;
    
    file = g_fopen (PATH, "r");

errored:

    return file;
}


/**
 * lw_dictionary_get_install_directory:
 *
 * Returns the install path of where dictionaries are installed.  The directory base is determined by the
 * user's data directory.  This can be overwridden in the environmental variables with 
 * %XDG_CACHE_HOME on unix platforms or %CSIDL_INTERNET_CACHE on Windows. Please refer to
 * g_get_user_cache_dir() for more information.
 *
 * Returns: (transfer none): Returns the install path of dictionaries which is usually in the user's data directory.  This string is owned by the dictionary class and should not be edited or freed.
 */
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
      path = g_build_filename (DATA_DIR, "libwaei", "dictionary", NULL);
      if (path == NULL) goto errored;
    }
    if (path == NULL) goto errored;

     g_mkdir_with_parents (path, 0700);

errored:

    return path;
}


/**
 * lw_dictionary_get_contents_path:
 * @self: A #LwDictionary
 * Returns: (transfer none): A string denoting the install path of the raw dictionary data.  This string is owned by the dictionary and should not be modified or freed.
 */
gchar const * 
lw_dictionary_get_contents_path (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->contents_path;
}


/**
 * lw_dictionary_build_contents_path_by_type_and_name:
 * @type: A #GType that is a subtype of #LwDictionary
 * @FILENAME: (transfer none): The filename of the dictionary
 * Returns: (transfer full): The path of a theoretical dictioanry with this type and filename.  This string should be freed with g_free() when finished
 */
gchar *
lw_dictionary_build_contents_path_by_type_and_name (GType         type,
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
lw_dictionary_build_contents_path (LwDictionary * self,
                                   gchar const  * FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);
    if (FILENAME == NULL) return NULL;

    //Declarations
    gchar const * INSTALL_DIRECTORY = NULL;
    GType type = G_TYPE_INVALID;
    gchar *path = NULL;

    //Initializations
    INSTALL_DIRECTORY = lw_dictionary_get_install_directory ();
    if (INSTALL_DIRECTORY == NULL) goto errored;
    type = G_OBJECT_TYPE (self);
    if (type == G_TYPE_INVALID) goto errored;
    path = lw_dictionary_build_contents_path_by_type_and_name (type, FILENAME);
    if (path == NULL) goto errored;

errored:

    return path;
}


static void
lw_dictionary_set_contents_path (LwDictionary * self,
                                 gchar const  * PATH)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_GET_CLASS (self);
    if (g_strcmp0 (PATH, priv->contents_path) == 0) goto errored;

    g_free(priv->contents_path);
    priv->contents_path = g_strdup (PATH);

    lw_dictionary_sync_contents (self);

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_CONTENTS_PATH]);

errored:

    return;
}


/**
 * lw_dictionary_set_name:
 * @self: A #LwDictionary
 * @NAME: The new name of the dictionary. This name is independant of the filename and is made to be human readable.
 */
void
lw_dictionary_set_name (LwDictionary * self,
                        gchar const *  NAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate * priv = NULL;
    LwDictionaryClass * klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_GET_CLASS (self);
    if (g_strcmp0 (priv->name, NAME) == 0) goto errored;

    g_free (priv->name);
    priv->name = g_strdup (NAME);

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_NAME]);

errored:

    return;
}


/**
 * lw_dictionary_get_name:
 * @self: A #LwDictionary
 * Returns: (transfer none): The name of the dictionary or %NULL.  This string is owned by the dictionary and should not be freed or modified.
 */
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


/**
 * lw_dictionary_get_contents_filename:
 * @self: A #LwDictionary
 * Returns: (transfer none): The filename of the dictionary or %NULL.  This string is owned by the dictionary and should not be freed or modified.
 */
gchar const *
lw_dictionary_get_contents_filename (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->contents_filename;
}


static gboolean
_is_anchored_path (gchar const * PATH)
{
    if (PATH == NULL) return FALSE;

    gboolean is_relative_anchored = FALSE;
    gboolean is_absolute_anchored = FALSE;

    is_absolute_anchored = g_path_is_absolute (PATH);
    is_relative_anchored = (strncmp(PATH, "." G_DIR_SEPARATOR_S, strlen("." G_DIR_SEPARATOR_S)) == 0 ||
                            strncmp(PATH, ".." G_DIR_SEPARATOR_S, strlen(".." G_DIR_SEPARATOR_S)) == 0);

    return (is_absolute_anchored || is_relative_anchored);
}


/**
 * lw_dictionary_set_contents_filename:
 * @self: A #LwDictionary
 * @FILENAME: The new filename for the dictionary.  It can be absolute, or relative if you want to use the dictionary search paths
 *
 * This method should generally only be used internally.  It sets the file that
 * the dictionary is referencing for its base data.  Once set, it should generally
 * not be changed.
 */
void
lw_dictionary_set_contents_filename (LwDictionary * self,
                                     gchar const  * FILENAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate * priv = NULL;
    LwDictionaryClass * klass = NULL;
    gchar * path = NULL;
    gchar * filename = NULL;
    gboolean path_changed = FALSE;
    gboolean filename_changed = FALSE;
    
    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_GET_CLASS (self);
    if (g_strcmp0 (FILENAME, priv->contents_filename) == 0) goto errored;

    if (_is_anchored_path (FILENAME))
    {
      filename = g_path_get_basename (FILENAME);
      path = g_strdup (FILENAME);
    }
    else
    {
      filename = g_strdup (FILENAME);
      path = lw_dictionary_build_contents_path (self, filename);
    }

    filename_changed = (g_strcmp0 (filename, priv->contents_filename) != 0);
    path_changed = (g_strcmp0 (path, priv->contents_path) != 0);

    if (filename_changed)
    {
      g_free (priv->contents_filename);
      priv->contents_filename = filename;
      filename = NULL;

      lw_dictionary_sync_id (self);

      g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_CONTENTS_FILENAME]);
    }

    if (path_changed)
    {
      g_free (priv->contents_path);
      priv->contents_path = path;
      path = NULL;

      lw_dictionary_sync_contents (self);

      g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_CONTENTS_PATH]);
    }

errored:

    g_free (filename); filename = NULL;
    g_free (path); path = NULL;
}


/**
 * lw_dictionary_set_parsed_cache:
 * @self: A #LwDictionary
 * @cache: A #LwDictionaryCache to set to the parsed cache tree or %NULL to unset a cache
 *
 * This method is made for internal use by lw_dictionary_ensure_parsed_cache_by_utf8flags().
 * Most outside methods should have no use for this method.
 */
static void
lw_dictionary_set_parsed_cache (LwDictionary      * self,
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
    klass = LW_DICTIONARY_GET_CLASS (self);
    flags = lw_dictionarycache_get_flags (cache);
    key = GINT_TO_POINTER (flags);
    if (priv->parsed_cachetree == NULL) goto errored;
    if (g_tree_lookup (priv->parsed_cachetree, key) == cache) goto errored;

    if (cache != NULL)
    {
      g_tree_insert (priv->parsed_cachetree, key, cache);
    }
    else
    {
      g_tree_remove (priv->parsed_cachetree, key);
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
  LwProgress * progress;
};


static LwParsed *
_dictionarycache_parse (LwCacheFile                 * cache_file,
                        struct _DictionaryCacheData * data)
{
    g_return_val_if_fail (LW_IS_CACHEFILE (cache_file), NULL);
    g_return_val_if_fail (data != NULL, NULL);

    LwDictionary *dictionary = NULL;
    LwProgress * progress = NULL;

    dictionary = data->dictionary;
    g_return_val_if_fail (LW_IS_DICTIONARY (dictionary), NULL);
    progress = data->progress;
    g_return_val_if_fail (LW_IS_PROGRESS (progress), NULL);

    return lw_dictionary_parse (dictionary, cache_file, progress);
}


/**
 * lw_dictionary_ensure_parsed_cache_by_utf8flags:
 * @self: A #LwDictionary
 * @flags: The flags of the dictionary cache to search for, otherwise one will be built with these flags
 * @progress: A #LwProgress to track parsing progress or %NULL to ignore it
 * Returns: (transfer none): A #LwDictionaryCache that matches the #LwUtf8Flags. This method will never return %NULL
 */
LwDictionaryCache *
lw_dictionary_ensure_parsed_cache_by_utf8flags (LwDictionary * self,
                                                LwUtf8Flag     flags,
                                                LwProgress   * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);
    if (progress != NULL && lw_progress_should_abort (progress)) return NULL;

    //Declarations
    GTree * parsed_cachetree;
    gchar const * FILENAME = NULL;
    LwDictionaryCache * cache = NULL;
    LwMappedFile * contents_mapped_file = NULL;
    struct _DictionaryCacheData data = {
      .dictionary = self,
      .progress = progress
    };
    gchar const * CHECKSUM = NULL;
    gchar const * CONTENTS = NULL;
    gsize content_length = 0;


    //Initializations
    parsed_cachetree = lw_dictionary_get_parsed_cachetree (self);
    cache = LW_DICTIONARYCACHE (g_tree_lookup (parsed_cachetree, GINT_TO_POINTER (flags)));
    if (cache != NULL) goto errored;

    FILENAME = lw_dictionary_get_contents_filename (self);
    if (FILENAME == NULL) goto errored;

    contents_mapped_file = lw_dictionary_get_contents_mappedfile (self);
    if (contents_mapped_file == NULL) goto errored;

    CHECKSUM = lw_dictionary_get_contents_checksum (self);
    if (CHECKSUM == NULL) goto errored;

    CONTENTS = lw_dictionary_get_contents (self);
    if (CONTENTS == NULL) goto errored;

    content_length = lw_dictionary_contents_length (self);
    if (content_length == 0) goto errored;

    cache = lw_dictionarycache_new (FILENAME, flags);
    if (cache == NULL) goto errored;
    
    lw_dictionary_set_parsed_cache (self, cache);

    lw_dictionarycache_read (cache, CHECKSUM, progress);
    if (lw_progress_should_abort (progress))
    {
      lw_progress_set_error (progress, NULL);
      lw_dictionarycache_write (cache, CHECKSUM, CONTENTS, content_length, (LwDictionaryCacheParseFunc) _dictionarycache_parse, &data, progress);
      lw_dictionarycache_read (cache, CHECKSUM, progress);
    }


errored:

    return cache;
}


/**
 * lw_dictionary_lookup_parsed_cache_by_utf8flags:
 * @self: A #LwDictionary
 * @flags: Flags of the #LwDictionaryCache to lookup
 * Returns: (transfer none): A #LwDictionaryCache matching the flags or %NULL if none were found
 */
LwDictionaryCache *
lw_dictionary_lookup_parsed_cache_by_utf8flags (LwDictionary * self,
                                                LwUtf8Flag     flags)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    GTree * caches = NULL;
    LwDictionaryCache * cache = NULL;
    gchar const *FILENAME = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_GET_CLASS (self);
    FILENAME = lw_dictionary_get_contents_filename (self);

    caches = lw_dictionary_get_parsed_cachetree (self);
    if (caches == NULL)
    {
      caches = g_tree_new (_flag_compare_function);
      lw_dictionary_set_parsed_cachetree (self, caches);
    }
    if (caches == NULL) goto errored;

    cache = g_tree_lookup (caches, GINT_TO_POINTER (flags));
    if (cache == NULL) goto errored;

errored:

    return cache;
}


/**
 * lw_dictionary_get_id:
 * @self: A #LwDictionary
 * Returns: (transfer none): A human-readable string uniquely identifying this dictionary. This string is owned by the dictionary and thus should not be modified or freed.
 */
gchar const *
lw_dictionary_get_id (LwDictionary * self)
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
lw_dictionary_set_id (LwDictionary * self,
                      gchar const  * ID)
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
lw_dictionary_sync_id (LwDictionary * self)
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
    FILENAME = lw_dictionary_get_contents_filename (self);
    if (FILENAME == NULL) goto errored;
    id = g_strdup_printf ("%s/%s", DIRECTORYNAME, FILENAME);
    if (id == NULL) goto errored;

    lw_dictionary_set_id (self, id);

errored:

    g_free (id);
    id = NULL;
}


static gint
_count_max_columns (LwDictionary * self, gchar ** lines, gint num_lines)
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


static void
lw_dictionary_set_contents_mappedfile (LwDictionary * self,
                                      LwMappedFile  * contents_mappedfile)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_GET_CLASS (self);

    if (contents_mappedfile != NULL)
    {
      g_object_ref (contents_mappedfile);
    }

    if (priv->contents_mappedfile != NULL)
    {
      g_object_unref (priv->contents_mappedfile);
    }

    priv->contents_mappedfile = contents_mappedfile;

errored:

    return;
}


static LwMappedFile*
lw_dictionary_get_contents_mappedfile (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->contents_mappedfile;
}


static void
lw_dictionary_sync_contents (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwMappedFile * mapped_file = NULL;
    gchar * contents = NULL;
    gsize length = NULL;

    //Initializations
    priv = self->priv;

    if (priv->contents_path != NULL)
    {
      mapped_file = lw_mappedfile_new (priv->contents_path);
      contents = lw_mappedfile_get_contents (mapped_file);
      length = lw_mappedfile_length (mapped_file);
    }

    lw_dictionary_set_contents_mappedfile (self, mapped_file);

    if (priv->contents_path != NULL)
    {
      g_object_unref (mapped_file);
    }

    g_free (priv->contents_checksum); priv->contents_checksum = NULL;
    if (contents != NULL)
    {
      priv->contents_checksum = g_compute_checksum_for_data (LW_DICTIONARY_CHECKSUM, contents, length);
    }

    lw_dictionary_set_parsed_cachetree (self, NULL);
}


/**
 * lw_dictionary_contents_length:
 * @self: A #LwDictionary
 * Returns: The byte length of the raw dictionary contents as returned by lw_dictionary_get_contents()
 */
gsize
lw_dictionary_contents_length (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, -1);

    //Declarations
    LwMappedFile * mapped_file = NULL;
    gsize length = 0;

    //Initializations
    mapped_file = lw_dictionary_get_contents_mappedfile (self);
    if (mapped_file == NULL) goto errored;
    length = lw_mappedfile_length (mapped_file);
    if (length == 0) goto errored;

errored:

    return length;
}


/**
 * lw_dictionary_get_contents:
 * @self: A #LwDictionary
 * Returns: (transfer none): The raw dictionary contents as a byte array.  This byte array is owned by the dictionary and thus should not be freed or modified.
 */
gchar const *
lw_dictionary_get_contents (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwMappedFile * mapped_file = NULL;
    gchar const * CONTENTS = NULL;

    //Initializations
    mapped_file = lw_dictionary_get_contents_mappedfile (self);
    if (mapped_file == NULL) goto errored;
    CONTENTS = lw_mappedfile_get_contents (mapped_file);
    if (CONTENTS == NULL) goto errored;

errored:

    return CONTENTS;
}


/**
 * lw_dictionary_get_contents_checksum:
 * @self: A #LwDictionary
 * Returns: (transfer none): The checksum of the the string as returned by lw_dictionary_get_contents() method.  This string is owned by the dictionary and thus should not be modified or freed.
 */
gchar const *
lw_dictionary_get_contents_checksum (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate * priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->contents_checksum;
}


static void
lw_dictionary_set_parsed_cachetree (LwDictionary * self,
                                    GTree        * tree)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;

    //Initializations
    priv = self->priv;
    if (priv->parsed_cachetree == tree) goto errored;
    klass = LW_DICTIONARY_GET_CLASS (self);

    if (tree != NULL)
    {
      g_tree_ref (tree);
    }

    if (priv->parsed_cachetree == NULL)
    {
      g_tree_unref (priv->parsed_cachetree);
    }

    priv->parsed_cachetree = tree;

errored:

    return;
}


/**
 * lw_dictionary_get_parsed_cachetree:
 * @self: A #LwDictionary
 * Returns: The internal cachetree holding parsed dictionary information
 */
static GTree *
lw_dictionary_get_parsed_cachetree (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->parsed_cachetree;
}


static gchar **
lw_dictionary_columnize (LwDictionary * self,
                         gchar         * buffer,
                         gchar        ** tokens,
                         gsize         * num_tokens)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryClass *klass = NULL;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    g_return_val_if_fail (klass->columnize != NULL, NULL);

    return klass->columnize(self, buffer, tokens, num_tokens);
}



static void
lw_dictionary_load_columns (LwDictionary  * self,
                            gchar         * buffer,
                            gchar        ** tokens,
                            gint            num_tokens,
                            LwParsedLine  * line) 
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryClass *klass = NULL;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    g_return_if_fail (klass->load_columns != NULL);

    klass->load_columns(self, buffer, tokens, num_tokens, line);
}


/**
 * lw_dictionary_parse:
 * @self: A #LwDictionary
 * @cache_file: (transfer none): A #LwCacheFile containing the raw dictionary data
 * @progress: (transfer none): A #LwProgress to track progress or %NULL to ignore it
 *
 * Parses a #LwCacheFile containing raw dictionary data as returned from
 * lw_dictionary_get_contents_mappedfile().
 *
 * @Returns: (transfer full): A #LwParsed that should be freed with g_object_unref()
 */
static LwParsed*
lw_dictionary_parse (LwDictionary * self,
                     LwCacheFile  * cache_file,
                     LwProgress   * progress)
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
        lw_parsedline_init_full (line, (GDestroyNotify) g_free);
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


/**
 * lw_dictionary_calculate_applicable_columns_for_text:
 * @self: A #LwDictionary
 * @TEXT: (transfer none): Text to examine
 *
 * This method is primarily made to parse search queries to
 * aid in reduce comparisons againt unrelated columns.
 *
 * Returns: (transfer full): An array of column ids that shold be freed with g_free()
 */
gint *
lw_dictionary_calculate_applicable_columns_for_text (LwDictionary * self,
                                                     gchar const  * TEXT)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);
    if (TEXT == NULL || *TEXT == '\0') return NULL;

    //Declarations
    LwDictionaryClass *klass = NULL;
    gint * columns = NULL;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    columns = klass->calculate_applicable_columns_for_text (self, TEXT);

errored:

    return columns;
}


/**
 * lw_dictionary_columnid_get_type:
 * @self: A #LwDictionary
 *
 * Returns: The columnid type from the implementor of this class.
 */
GType
lw_dictionary_columnid_get_type (LwDictionary * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    GType type = G_TYPE_NONE;
    LwDictionaryClass *klass = NULL;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    type = klass->columnid_get_type ();

errored:

    return type;
  
}

