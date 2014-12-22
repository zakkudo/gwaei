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

    g_free (priv->data.filename); priv->data.filename = NULL;
    g_free (priv->data.name); priv->data.name = NULL;

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
      case PROP_PROGRESS:
        lw_dictionary_set_progress (self, g_value_get_object (value));
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
      case PROP_PROGRESS:
        g_value_set_object (value, lw_dictionary_get_progress (self));
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
      case PROP_LENGTH:
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
    LwDictionaryClassPrivate *klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwDictionaryClassPrivate, 1);
    object_class->set_property = lw_dictionary_set_property;
    object_class->get_property = lw_dictionary_get_property;
    object_class->finalize = lw_dictionary_finalize;

    g_type_class_add_private (object_class, sizeof (LwDictionaryPrivate));

    klasspriv = klass->priv;

    klasspriv->pspec[PROP_FILENAME] = g_param_spec_string (
      "filename",
      gettext("Filename"),
      "Set the self's data.filename",
      "",
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_FILENAME, klasspriv->pspec[PROP_FILENAME]);

    klasspriv->pspec[PROP_PATH] = g_param_spec_string (
      "path",
      gettext("Path"),
      "Set the self's data.filename",
      "",
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_PATH, klasspriv->pspec[PROP_PATH]);

    klasspriv->pspec[PROP_PROGRESS] = g_param_spec_object (
      "progress",
      gettext("Progress"),
      "Set the self's data.filename",
      LW_TYPE_PROGRESS,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PROGRESS, klasspriv->pspec[PROP_PROGRESS]);

    klasspriv->pspec[PROP_ID] = g_param_spec_string (
      "id",
      gettext("Id"),
      "Set the self's data.filename",
      "",
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_ID, klasspriv->pspec[PROP_ID]);

    klasspriv->pspec[PROP_CACHETREE] = g_param_spec_pointer (
      "cache-tree",
      gettext("Buffer"),
      "Set the self's data.filename",
      G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_CACHETREE, klasspriv->pspec[PROP_CACHETREE]);

    klasspriv->pspec[PROP_LENGTH] = g_param_spec_ulong (
      "length",
      gettext("Length"),
      "Set the self's data.filename",
      0,
      G_MAXULONG,
      0,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_LENGTH, klasspriv->pspec[PROP_LENGTH]);

    klasspriv->pspec[PROP_CHECKSUM] = g_param_spec_string (
      "checksum",
      gettext("Checksum"),
      "Set the self's data.filename",
      "",
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CHECKSUM, klasspriv->pspec[PROP_CHECKSUM]);

    klasspriv->pspec[PROP_CONTENTS] = g_param_spec_string (
      "contents",
      gettext("Contents"),
      "Set the self's data.filename",
      "",
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS, klasspriv->pspec[PROP_CONTENTS]);
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
    LwDictionaryClassPrivate *klasspriv = NULL;
    gint total_columns = 0;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    klasspriv = klass->priv;
    total_columns = klasspriv->get_total_columns (self);
  
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
    LwDictionaryClassPrivate *klasspriv = NULL;
    gint total_columns = 0;
    gchar const *language = NULL;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    klasspriv = klass->priv;
    total_columns = lw_dictionary_get_total_columns (self);
    if (total_columns < 1 || column_num >= total_columns) goto errored;
    language = klasspriv->get_column_language (self, column_num);

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
    LwDictionaryClassPrivate *klasspriv = NULL;
    gint total_columns = 0;
    gint handling = LW_DICTIONARYCOLUMNHANDLING_UNUSED;

    //Initializations
    klass = LW_DICTIONARY_GET_CLASS (self);
    klasspriv = klass->priv;
    total_columns = lw_dictionary_get_total_columns (self);
    if (total_columns < 1 || column_num >= total_columns) goto errored;
    handling = klasspriv->get_column_handling (self, column_num);

errored:

    return handling;
}


gsize
lw_dictionary_length (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, -1);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    gsize length = 0;

    //Initializations
    priv = self->priv;
    if (priv->data.mapped_file == NULL) goto errored;
    length = g_mapped_file_get_length (priv->data.mapped_file);

errored:

    return length;
}


gint
lw_dictionary_num_lines (LwDictionary *self)
{
  //TODO
  return 0;
}


void
lw_dictionary_take_contents (LwDictionary *self,
                             gchar        *contents)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_CLASS (self);
    klasspriv = klass->priv;
    changed = (contents != priv->data.contents);

    if (changed == FALSE) goto errored;

    priv->data.contents = contents;
    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CONTENTS]);

errored:
    
    return;
}


void
lw_dictionary_sync_contents (LwDictionary *self,
                             LwProgress   *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    gchar *contents = NULL;
    gint length = -1;

    //Initializations
    priv = self->priv;
    if (priv->data.mapped_file == NULL) goto errored;

    contents = g_mapped_file_get_contents (priv->data.mapped_file);
    length = g_mapped_file_get_length (priv->data.mapped_file);

    if (!lw_utf8_validate (contents, length, progress))
    {
      contents = NULL;
    }

errored:

    lw_dictionary_take_contents (self, contents);
    contents = NULL;
}


const gchar*
lw_dictionary_get_contents (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.contents == NULL)
    {
      lw_dictionary_sync_contents (self, NULL);
    }

errored:

    return priv->data.contents;
}


void
lw_dictionary_take_checksum (LwDictionary *self,
                             gchar        *checksum)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_CLASS (self);
    klasspriv = klass->priv;
    changed = (g_strcmp0 (checksum, priv->data.checksum) != 0);

    g_free (priv->data.checksum);
    priv->data.checksum = checksum;

    if (changed == FALSE) goto errored;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CHECKSUM]);

errored:

    return;
}


void
lw_dictionary_sync_checksum (LwDictionary *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    const gchar *CONTENTS = NULL;
    gint length = -1;
    gchar *checksum = NULL;

    //Initializations
    CONTENTS = lw_dictionary_get_contents (self);
    if (CONTENTS == NULL) goto errored;
    length = lw_dictionary_length (self);
    if (length < 1) goto errored;
    checksum = g_compute_checksum_for_string (LW_DICTIONARY_CHECKSUM, CONTENTS, length);

errored:

    lw_dictionary_take_checksum (self, checksum);
    checksum = NULL;
}


const gchar*
lw_dictionary_get_checksum (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.checksum == NULL)
    {
      lw_dictionary_sync_checksum (self);
    }

    return priv->data.checksum;
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
    const gchar *PATH = NULL;
    const gchar *MESSAGE = NULL;
    const gchar *name = lw_dictionary_get_name (self);

    //Initializations
    priv = self->priv;
    progress = priv->data.progress;
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
 

FILE*
lw_dictionary_open (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    FILE *file = NULL;
    const gchar *PATH = NULL;

    //Initializations
    PATH = lw_dictionary_get_path (self);
    if (PATH == NULL) goto errored;
    
    file = g_fopen (PATH, "r");

errored:

    return file;
}


gchar*
lw_dictionary_build_directoryname (GType type)
{
    //Sanity checks
    g_return_val_if_fail (g_type_is_a (type, LW_TYPE_DICTIONARY), NULL);

    //Declarations
    const gchar* TYPENAME = NULL;
    const gchar *start = NULL;
    const gchar *end = NULL;
    gchar *name = NULL;
    gchar *lowercase = NULL;

    //Initializations
    TYPENAME = g_type_name (type);
    if (strncmp(TYPENAME, "Lw", 2) != 0) goto errored;
    start = TYPENAME + strlen("Lw");
    if (!(g_str_has_suffix (start, "Dictionary"))) goto errored;
    end = TYPENAME + strlen(TYPENAME) - strlen("Dictionary");
    if (start >= end) goto errored;
    name = g_strndup (start, end - start);
    if (name == NULL) goto errored;
    lowercase = g_ascii_strdown (name, -1);

errored:

    g_free (name); name = NULL;

    return lowercase;
}


gchar*
lw_dictionary_build_directory (GType type)
{
    //Sanity checks
    g_return_val_if_fail (g_type_is_a (type, LW_TYPE_DICTIONARY), NULL);

    //Declarations
    gchar *path = NULL;
    gchar *directoryname = NULL;
/*TODO
    //Initializations
    path = NULL;
    directoryname = lw_dictionary_build_directoryname (type);
    if (directoryname == NULL) goto errored;
    path = lw_util_build_filename (LW_PATH_DICTIONARY, directoryname);
    if (path == NULL) goto errored;

    g_mkdir_with_parents (path, 0755);

errored:

    g_free (directoryname);
 */ 
    return path;
}


const gchar* 
lw_dictionary_get_path (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.path;
}


gchar*
lw_dictionary_build_path (GType        type,
                          const gchar *FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (g_type_is_a (type, LW_TYPE_DICTIONARY), NULL);
    g_return_val_if_fail (FILENAME != NULL, NULL);

    //Declarations
    gchar *directory = NULL;
    gchar *path = NULL;

    //Initializations
    directory = lw_dictionary_build_directory (type);
    if (directory == NULL) goto errored;
    path = g_build_filename (directory, FILENAME, NULL);
    if (path == NULL) goto errored;

errored:

    g_free (directory); directory = NULL;

    return path;
}


void
lw_dictionary_sync_path (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;
    GType type = G_TYPE_INVALID;
    const gchar *FILENAME = NULL;
    gchar *path = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_CLASS (self);
    klasspriv = klass->priv;
    type = G_OBJECT_TYPE (self);
    FILENAME = lw_dictionary_get_filename (self);
    path = lw_dictionary_build_path (type, FILENAME);
    if (g_strcmp0 (path, priv->data.path) == 0) goto errored;

    g_free(priv->data.path);
    priv->data.path = path;
    path = NULL;

    lw_dictionary_set_cachetree (self, NULL);
    
    if (priv->data.mapped_file != NULL)
    {
      g_mapped_file_unref (priv->data.mapped_file);
    }
    priv->data.mapped_file = g_mapped_file_new (priv->data.path, FALSE, NULL);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PATH]);

errored:

    g_free (path); path = NULL;
}


const gchar*
lw_dictionary_get_name (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv;

    //Initializations
    priv = self->priv;

    return priv->data.name;
}


const gchar*
lw_dictionary_get_filename (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.filename;
}


void
lw_dictionary_set_filename (LwDictionary *self,
                            const gchar *FILENAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;
    
    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_CLASS (self);
    klasspriv = klass->priv;
    if (g_strcmp0 (FILENAME, priv->data.filename) == 0) goto errored;

    g_free (priv->data.filename);
    priv->data.filename = g_strdup (FILENAME);

    g_free (priv->data.name);
    priv->data.name = g_strdup (gettext(priv->data.filename));

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_FILENAME]);

    lw_dictionary_sync_id (self);
    lw_dictionary_sync_path (self);

errored:

    return;
}


LwProgress*
lw_dictionary_get_progress (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.progress == NULL)
    {
      LwProgress *progress = lw_progress_new ();
      lw_dictionary_set_progress (self, progress);
      progress = NULL;
    }

    return priv->data.progress;
}


void
lw_dictionary_set_progress (LwDictionary *self,
                            LwProgress   *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_CLASS (self);
    klasspriv = klass->priv;
    if (priv->data.progress == progress) goto errored;

    if (progress != NULL)
    {
      g_object_ref_sink (progress);
    }

    if (priv->data.progress != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.progress),
        (gpointer*) &(priv->data.progress)
      );
      g_object_unref (priv->data.progress);
    }

    priv->data.progress = progress;

    if (priv->data.progress != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->data.progress),
        (gpointer*) &(priv->data.progress)
      );
    }

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PROGRESS]);

errored:

    return;
}


void
lw_dictionary_set_cachetree (LwDictionary *self,
                              GTree        *tree)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    if (priv->data.caches == tree) goto errored;
    klass = LW_DICTIONARY_CLASS (self);
    klasspriv = klass->priv;

    if (tree != NULL)
    {
      g_tree_ref (tree);
    }

    if (priv->data.caches == NULL)
    {
      g_tree_unref (priv->data.caches);
    }

    priv->data.caches = tree;

    if (priv->data.caches != NULL)
    {
      g_tree_ref (priv->data.caches);
    }

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CACHETREE]);

errored:

    return;
}


GTree*
lw_dictionary_get_cachetree (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.caches;
}


void
lw_dictionary_set_cache (LwDictionary        *self,
                          LwDictionaryCache  *cache)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;
    gpointer key = NULL;
    LwUtf8Flag flags = 0;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_CLASS (self);
    klasspriv = klass->priv;
    flags = lw_dictionarycache_get_flags (cache);
    key = GINT_TO_POINTER (flags);
    if (priv->data.caches == NULL) goto errored;
    if (g_tree_lookup (priv->data.caches, key) == cache) goto errored;

    if (cache != NULL)
    {
      g_tree_insert (priv->data.caches, key, cache);
    }
    else
    {
      g_tree_remove (priv->data.caches, key);
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


static LwParsed*
_dictionarycache_parse (gchar                       *contents,
                        gsize                        content_length,
                        struct _DictionaryCacheData *data)
{
    //Declarations
    LwDictionary *dictionary = NULL;
    LwProgress *progress = NULL;

    //Initializations
    dictionary = data->dictionary;
    progress = data->progress;

    g_return_if_fail (LW_IS_DICTIONARY (dictionary));
    g_return_if_fail (contents != NULL);
    g_return_if_fail (content_length > 0);

    return lw_dictionary_parse (dictionary, contents, content_length, progress);
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
    LwDictionaryClass *klass = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;
    const gchar *CHECKSUM = NULL;
    const gchar *CONTENTS = NULL;
    gsize content_length = 0;
    LwDictionaryCacheParseFunc func = NULL;
    struct _DictionaryCacheData data = {
      .dictionary = self,
      .progress = progress
    };
    
    //Initializations
    klass = LW_DICTIONARY_CLASS (self);
    klasspriv = klass->priv;
    CHECKSUM = lw_dictionary_get_checksum (self);
    CONTENTS = lw_dictionary_get_contents (self);
    content_length = lw_dictionary_length (self);
    func = (LwDictionaryCacheParseFunc) klasspriv->parse;

    lw_dictionarycache_set_contents (cache, CHECKSUM, CONTENTS, content_length, (LwDictionaryCacheParseFunc) _dictionarycache_parse, &data, progress);

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
    const gchar *FILENAME = NULL;

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


LwParsed*
lw_dictionary_parse (LwDictionary *self,
                     gchar        *contents,
                     gsize         content_length,
                     LwProgress   *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);
    g_return_val_if_fail (contents != NULL, NULL);
    g_return_val_if_fail (content_length > 0, NULL);

    //Declarations
    LwDictionaryClass *klass = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;

    //Initializations
    klass = LW_DICTIONARY_CLASS (self);
    klasspriv = klass->priv;

    return klasspriv->parse (self, contents, content_length, progress);
}


gboolean
lw_dictionary_equals (LwDictionary *dictionary1, 
                      LwDictionary *dictionary2)
{
    //Sanity checks
    g_return_val_if_fail (dictionary1 != NULL && dictionary2 != NULL, FALSE);

    //Declarations
    const gchar *FILENAME1;
    const gchar *FILENAME2;
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


gchar*
lw_dictionary_build_id_from_type (GType        type, 
                                  const gchar *FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (FILENAME != NULL, NULL);
    g_return_val_if_fail (g_type_is_a (type, LW_TYPE_DICTIONARY), NULL);

    //Declarations
    gchar *id = NULL;
    gchar *directoryname = NULL;

    //Initializations
    directoryname = lw_dictionary_build_directoryname (type);
    if (directoryname == NULL) goto errored;
    id = g_strdup_printf ("%s/%s", directoryname, FILENAME);
    if (id == NULL) goto errored;

errored:

    g_free (directoryname); directoryname = NULL;

    return id;
}


gchar*
lw_dictionary_directoryname_to_typename (const gchar *DIRECTORYNAME)
{
    //Sanity checks
    g_return_val_if_fail (DIRECTORYNAME != NULL, NULL);

    //Declarations
    gchar *directoryname = NULL;
    gchar *typename = NULL;

    directoryname = g_strdup (DIRECTORYNAME); if (directoryname == NULL || *directoryname == '\0') goto errored;
    directoryname[0] = g_ascii_toupper (directoryname[0]);
    typename = g_strdup_printf ("Lw%sDictionary", directoryname); if (typename == NULL) goto errored;

errored:

    if (directoryname != NULL) g_free (directoryname); directoryname = NULL;

    return typename;
}


const gchar*
lw_dictionary_get_id (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.id;
}


void
lw_dictionary_sync_id (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryClass *klass = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;
    gchar *id = NULL;
    GType type = G_TYPE_INVALID;
    const gchar *FILENAME = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARY_CLASS (self);
    klasspriv = klass->priv;
    type = G_OBJECT_TYPE (self);
    FILENAME = lw_dictionary_get_filename (self);
    if (FILENAME == NULL) goto errored;
    id = lw_dictionary_build_id_from_type (type, FILENAME);

    if (g_strcmp0 (id, priv->data.id) == 0) goto errored;

    g_free (priv->data.id);
    priv->data.id = id;
    id = NULL;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_ID]);

errored:

    g_free (id); id = NULL;

    return;
}


gboolean
lw_dictionary_is_selected (LwDictionary *self)
{
    //Sanity check
    g_return_val_if_fail (LW_IS_DICTIONARY (self), FALSE);

    //Declarations
    LwDictionaryPrivate *priv;

    //Initializations
    priv = self->priv;

    return priv->config.selected;
}


void
lw_dictionary_set_selected (LwDictionary *self, 
                            gboolean      selected)
{
    //Sanity check
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv;

    //Initializations
    priv = self->priv;

    priv->config.selected = selected;
}


gchar**
lw_dictionary_get_installed_idlist (GType type_filter)
{
    //Declarations
    GType* childlist = NULL;
    GType *childiter = NULL;

    gchar const* FILENAME = NULL;
    gchar *directorypath = NULL;
    GDir *directory = NULL;

    gchar **idlist = NULL;
    gchar **iditer = NULL;

    gint length = 0;

/*TODO
    childiter = childlist = g_new (GType, 5);
    childlist[0] = lw_edictionary_get_type ();
    childlist[1] = lw_kanjidictionary_get_type ();
    childlist[2] = lw_exampledictionary_get_type ();
    childlist[3] = lw_unknowndictionary_get_type ();
    childlist[4] = 0;

    if (childiter == NULL) return NULL;

    length = 0;

    //Find out how long the array has to be
    while (*childiter != 0)
    {
      directorypath = lw_dictionary_build_directory (*childiter);
      directory = g_dir_open (directorypath, 0, NULL);
      if (directory != NULL)
      {
        while ((filename = g_dir_read_name (directory)) != NULL)
        {
          length++;
        }
        g_dir_close (directory); directory = NULL;
      }
      g_free (directorypath); directorypath = NULL;
      childiter++;
    }

    iditer = idlist = g_new0 (gchar*, length + 1);
    childiter = childlist;

    //Find out how long the array has to be
    while (*childiter != 0)
    {
      directorypath = lw_dictionary_build_directory (*childiter);
      directory = g_dir_open (directorypath, 0, NULL);
      if (directory != NULL)
      {
        while ((filename = g_dir_read_name (directory)) != NULL && length > 0)
        {
          *iditer = lw_dictionary_build_id_from_type (*childiter, filename);
          fprintf(stderr, "id: %s\n", *iditer);
          
          iditer++;
          length--;
        }
        g_dir_close (directory); directory = NULL;
      }
      g_free (directorypath); directorypath = NULL;
      childiter++;
    }

    if (childlist != NULL) g_free (childlist); childlist = NULL;
*/
    return idlist;
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
        const gchar *BUFFER = lw_dictionarycache_get_line (buffer, i);
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




gint
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
lw_dictionary_build_lines (LwDictionary *self, LwDictionaryCache *cache)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);
    g_return_val_if_fail (cache != NULL, NULL);

    //Declarations
    LwDictionaryClass *klass = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;
    gchar *str = NULL;
    gchar **lines = NULL;
    gint num_lines = -1;
    gint max_column_count = -1;
    gchar **column_buffer = NULL;
    LwParsedLine *dictionary_lines = NULL;

  /*TODO

    //Initializations
    klass = LW_DICTIONARY_CLASS (self);
    klasspriv = klass->priv;
    if (klasspriv->columnize == NULL) goto errored;
    if (klasspriv->load_columns == NULL) goto errored;
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
        klasspriv->columnize (self, lines[i], column_buffer, &num_columns);
        klasspriv->load_columns (self, dictionary_lines + i, column_buffer, num_columns);
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
    if (priv->data.lines != NULL) BLAH

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



