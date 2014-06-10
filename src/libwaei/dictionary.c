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
#include <libwaei/libwaei.h>

#include <libwaei/dictionary-private.h>


static LwDictionaryClass *_klass = NULL;
static LwDictionaryClassPrivate *_klasspriv = NULL;

G_DEFINE_ABSTRACT_TYPE (LwDictionary, lw_dictionary, G_TYPE_OBJECT);


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

    g_free (priv->config.filename); priv->config.filename = NULL;
    g_free (priv->data.name); priv->data.name = NULL;

    lw_index_free (priv->data.index); priv->data.index = NULL;
    lw_dictionarydata_free (priv->data.data); priv->data.data = NULL;

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
      case PROP_MORPHOLOGYENGINE:
        lw_dictionary_set_morphologyengine (self, g_value_get_object (value));
        break;
      case PROP_PROGRESS:
        lw_dictionary_set_progress (self, g_value_get_object (value));
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
      case PROP_MORPHOLOGYENGINE:
        g_value_set_object (value, lw_dictionary_get_morphologyengine (self));
        break;
      case PROP_PROGRESS:
        g_value_set_object (value, lw_dictionary_get_progress (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


size_t
lw_dictionary_length (LwDictionary *self)
{
    g_return_val_if_fail (self != NULL, -1);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryData *data = NULL;
    size_t length = 0;

    //Initializations
    priv = self->priv;
    data = priv->data.data;
    if (data == NULL) goto errored;
    length = lw_dictionarydata_length (data);

errored:

    return length;
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

    _klass = klass;
    _klasspriv = klass->priv;

    _klasspriv->pspec[PROP_FILENAME] = g_param_spec_string (
      "filename",
      "Filename of the self",
      "Set the self's config.filename",
      "",
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_FILENAME, _klasspriv->pspec[PROP_FILENAME]);

    _klasspriv->pspec[PROP_MORPHOLOGYENGINE] = g_param_spec_object (
      "morphology-engine",
      "Morphology Engine referenced by the self",
      "Set the self's Morphology Engine used for indexing",
      LW_TYPE_MORPHOLOGYENGINE,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_MORPHOLOGYENGINE, _klasspriv->pspec[PROP_MORPHOLOGYENGINE]);
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
    //Sanity check
    g_return_val_if_fail (self != NULL, FALSE);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwProgress *progress = NULL;
    gchar *uri = NULL;
    const gchar *MESSAGE = NULL;
    const gchar *name = lw_dictionary_get_name (self);

    //Initializations
    priv = self->priv;
    progress = priv->data.progress;
    uri = lw_dictionary_get_path (self);
    if (uri == NULL) goto errored;

    MESSAGE = gettext("Uninstalling %s Dictionary...");
    lw_progress_set_primary_message_printf (progress, MESSAGE, name);

    MESSAGE = gettext("Removing %s...");
    lw_progress_set_secondary_message_printf (progress, MESSAGE, uri);

    lw_io_remove (uri, progress);

errored:

    g_free (uri); uri = NULL;

    return (!lw_progress_errored (progress));
}
 

FILE*
lw_dictionary_open (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    FILE *file = NULL;
    gchar *path = NULL;

    //Initializations
    path = lw_dictionary_get_path (self); if (path == NULL) goto errored;
    
    file = g_fopen (path, "r");

errored:

    if (path != NULL) g_free (path); path = NULL;

    return file;
}


gchar*
lw_dictionary_get_directoryname (GType dictionary_type)
{
    //Declarations
    const gchar* TYPENAME = NULL;
    const gchar *start = NULL;
    const gchar *end = NULL;
    gchar *name = NULL;
    gchar *lowercase = NULL;
    
    //Initializations
    TYPENAME = g_type_name (dictionary_type);
    start = TYPENAME + strlen("Lw");
    end = TYPENAME + strlen(TYPENAME) - strlen("Dictionary"); if (strcmp(end, "Dictionary") != 0 || start > end) goto errored;
    name = g_strndup (start, end - start); if (name == NULL) goto errored;
    lowercase = g_ascii_strdown (name, -1);

errored:

    if (name != NULL) g_free (name); name = NULL;

    return lowercase;
}


gchar*
lw_dictionary_get_directory (GType dictionary_type)
{
    //Declarations
    gchar *path;
    gchar *directoryname;

    //Initializations
    path = NULL;
    directoryname = lw_dictionary_get_directoryname (dictionary_type);

    if (directoryname != NULL) 
    {
      path = lw_util_build_filename (LW_PATH_DICTIONARY, directoryname);
      g_free (directoryname); directoryname = NULL;
    }

    g_mkdir_with_parents (path, 0755);
  
    return path;
}


gchar* 
lw_dictionary_get_path (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    gchar *directory = NULL;
    const gchar *filename = NULL;
    gchar *path = NULL;

    //Initializations
    directory = lw_dictionary_get_directory (G_OBJECT_TYPE (self));
    filename = lw_dictionary_get_filename (self);

    if (directory != NULL)
    {
      path = g_build_filename (directory, filename, NULL);
      g_free (directory); directory = NULL;
    }

    return path;
}


LwResult*
lw_dictionary_parse (LwDictionary *self, 
                     LwResult     *result, 
                     const gchar  *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (result != NULL, FALSE);
    g_return_val_if_fail (TEXT != NULL, FALSE);

    //Initializations
    _klass = LW_DICTIONARY_CLASS (G_OBJECT_GET_CLASS (self));
    g_return_val_if_fail (_klass->priv->parse != NULL, FALSE);

    return _klass->priv->parse (self, TEXT);
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

    return priv->config.filename;
}


void
lw_dictionary_set_filename (LwDictionary *self,
                            const gchar *FILENAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    gboolean changed = FALSE;
    
    //Initializations
    priv = self->priv;
    changed = (g_strcmp0 (FILENAME, priv->config.filename) != 0);
    if (!changed) goto errored;

    g_free (priv->config.filename);
    priv->config.filename = g_strdup (FILENAME);

    g_free (priv->data.name);
    priv->data.name = g_strdup (gettext(priv->config.filename));

    lw_index_free (priv->data.index); priv->data.index = NULL;
    lw_dictionarydata_free (priv->data.data); priv->data.data = NULL;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_FILENAME]);

errored:

    return;
}


LwMorphologyEngine*
lw_dictionary_get_morphologyengine (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.morphology_engine;
}


void
lw_dictionary_set_morphologyengine (LwDictionary *self,
                                    LwMorphologyEngine *morphology_engine)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));

    //Declarations
    LwDictionaryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (morphology_engine != NULL)
    {
      g_object_ref_sink (morphology_engine);
    }

    if (priv->data.morphology_engine != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.morphology_engine),
        (gpointer*) &(priv->data.morphology_engine)
      );
      g_object_unref (priv->data.morphology_engine);
    }

    priv->data.morphology_engine = morphology_engine;

    if (priv->data.morphology_engine != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->data.morphology_engine),
        (gpointer*) &(priv->data.morphology_engine)
      );
    }
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

    //Initializations
    priv = self->priv;

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

    //Declarations
    gchar *id = NULL;
    gchar *directoryname = NULL;

    //Initializations
    directoryname = lw_dictionary_get_directoryname (type); if (directoryname == NULL) goto errored;
    id = g_strdup_printf ("%s/%s", directoryname, FILENAME); if (id == NULL) goto errored;

errored:

    if (directoryname != NULL) g_free (directoryname); directoryname = NULL;

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


gchar*
lw_dictionary_build_id (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    gchar *id;
    GType type;
    const gchar *FILENAME;

    //Initializations
    type = G_OBJECT_TYPE (self);
    FILENAME = lw_dictionary_get_filename (self);
    id = lw_dictionary_build_id_from_type (type, FILENAME);

    return id;
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
    GType* childlist;
    GType *childiter;

    const gchar* filename;
    gchar *directorypath;
    GDir *directory;

    gchar **idlist;
    gchar **iditer;

    gint length;

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
      directorypath = lw_dictionary_get_directory (*childiter);
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
      directorypath = lw_dictionary_get_directory (*childiter);
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

    return idlist;
}


const gchar*
lw_dictionary_get_buffer (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryData *data = NULL;
    const gchar *BUFFER = NULL;

    //Initializations
    priv = self->priv;
    data = priv->data.data;
    if (data == NULL) goto errored;
    BUFFER = lw_dictionarydata_get_buffer (data);

errored:

    return BUFFER;
}


const gchar*
lw_dictionary_get_string (LwDictionary *self, 
                          LwOffset      offset)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryPrivate *priv;

    //Initializations
    priv = self->priv;

    return lw_dictionarydata_get_string (priv->data.data, offset);
}

