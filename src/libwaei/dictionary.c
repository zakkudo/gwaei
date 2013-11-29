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


G_DEFINE_ABSTRACT_TYPE (LwDictionary, lw_dictionary, G_TYPE_OBJECT)

typedef enum
{
  PROP_0,
  PROP_FILENAME,
  PROP_MORPHOLOGYENGINE
} LwDictionaryProps;


LwDictionaryInstall*
lw_dictionary_steal_installer (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    LwDictionaryInstall *install;

    install = dictionary->priv->install;
    dictionary->priv = NULL;  
    
    return install;
}


void
lw_dictionary_set_installer (LwDictionary        *dictionary,
                             LwDictionaryInstall *install)
{
    g_return_if_fail (dictionary != NULL);
    g_return_if_fail (install != NULL);

    dictionary->priv->install = install;
}


void
lw_dictionary_set_installer_full (LwDictionary *dictionary,
                                  const gchar  *FILES,
                                  const gchar  *DOWNLOADS,
                                  const gchar  *DESCRIPTION,
                                  LwEncoding    encoding,
                                  gboolean      postprocess)
{
    g_return_if_fail (dictionary != NULL);
    g_return_if_fail (FILES != NULL && *FILES != '\0');

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations
    priv = dictionary->priv;
    if (priv->install != NULL) lw_dictionaryinstall_free (priv->install); 
    priv->install = lw_dictionaryinstall_new ();
    if (priv->install == NULL) return;
    install = priv->install;

    if (FILES != NULL) install->files = g_strdup (FILES);
    if (DOWNLOADS != NULL) install->downloads = g_strdup (DOWNLOADS);
    if (DESCRIPTION != NULL) install->description = g_strdup (DESCRIPTION);
    install->encoding = encoding;
    install->postprocess = postprocess;
}


void
lw_dictionary_set_builtin_installer_full (LwDictionary  *dictionary,
                                          const gchar   *FILES,
                                          LwPreferences *preferences,
                                          const gchar   *KEY,
                                          const gchar   *DESCRIPTION,
                                          LwEncoding     encoding,
                                          gboolean       postprocess)
{
    //Sanity checks
    g_return_if_fail (dictionary != NULL);
    g_return_if_fail (FILES != NULL);
    g_return_if_fail (preferences != NULL);
    g_return_if_fail (KEY != NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    lw_dictionary_set_installer_full (dictionary, FILES, NULL, DESCRIPTION, encoding, postprocess);

    priv = dictionary->priv;
    install = priv->install;
    if (install == NULL) return;

    install->listenerid = lw_preferences_add_change_listener_by_schema (
      preferences, 
      LW_SCHEMA_DICTIONARY, 
      KEY, 
      lw_dictionary_sync_downloadlist_cb, 
      dictionary
    );

    install->preferences = preferences;
    install->key = KEY;
    install->builtin = TRUE;
}


static void 
lw_dictionary_init (LwDictionary *dictionary)
{
    dictionary->priv = LW_DICTIONARY_GET_PRIVATE (dictionary);
    memset(dictionary->priv, 0, sizeof(LwDictionaryPrivate));
}


static void 
lw_dictionary_finalize (GObject *object)
{
    //Declarations
    LwDictionary *dictionary;
    LwDictionaryPrivate *priv;

    //Initalizations
    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;

    if (priv->filename != NULL) g_free (priv->filename); 
    if (priv->name != NULL) g_free (priv->name);

    if (priv->install != NULL) lw_dictionaryinstall_free (priv->install); 

    if (priv->index != NULL) lw_index_free (priv->index); 
    if (priv->data != NULL) lw_dictionarydata_free (priv->data);

    memset(dictionary->priv, 0, sizeof(LwDictionaryPrivate));

    G_OBJECT_CLASS (lw_dictionary_parent_class)->finalize (object);
}


static void 
lw_dictionary_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwDictionary *dictionary;
    LwDictionaryPrivate *priv;

    //Initializations
    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;

    switch (property_id)
    {
      case PROP_FILENAME:
        if (priv->filename != NULL) g_free (priv->filename); priv->filename = g_value_dup_string (value);
        if (priv->name != NULL) g_free (priv->name); priv->name = g_strdup (priv->filename);
        if (priv->index != NULL) lw_index_free (priv->index); priv->index = NULL;
        if (priv->data != NULL) lw_dictionarydata_free (priv->data); priv->data = NULL;
        break;
      case PROP_MORPHOLOGYENGINE:
        if (priv->morphologyengine != NULL) g_object_unref (priv->morphologyengine); 
        priv->morphologyengine = g_value_get_object (value);
        if (priv->morphologyengine != NULL) g_object_ref_sink (priv->morphologyengine);
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
    LwDictionary *dictionary;
    LwDictionaryPrivate *priv;

    //Initializations
    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;

    switch (property_id)
    {
      case PROP_FILENAME:
        g_value_set_string (value, priv->filename);
        break;
      case PROP_MORPHOLOGYENGINE:
        g_value_set_object (value, priv->morphologyengine);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


size_t
lw_dictionary_get_length (LwDictionary *dictionary)
{
    g_return_val_if_fail (dictionary != NULL, -1);

    //Declarations
    LwDictionaryPrivate *priv;
    gchar *uri;

    //Initializations
    priv = dictionary->priv;

    if (priv->length == 0)
    {
      uri = lw_dictionary_get_path (dictionary);
      if (uri != NULL)
      {
        priv->length = lw_io_get_size_for_uri (uri);
        g_free (uri); uri = NULL;
      }
    }

    return priv->length;
}


static void
lw_dictionary_class_init (LwDictionaryClass *klass)
{
    //Declarations
    GParamSpec *pspec = NULL;
    GObjectClass *object_class = NULL;
    LwDictionaryClassPrivate *klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lw_dictionary_set_property;
    object_class->get_property = lw_dictionary_get_property;
    object_class->finalize = lw_dictionary_finalize;

    klass->priv = g_new0 (LwDictionaryClassPrivate, 1);
    klasspriv = klass->priv;
    klasspriv->parse_result = NULL;

    klasspriv->signalid[CLASS_SIGNALID_PROGRESS_CHANGED] = g_signal_new (
        "progress-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST | G_SIGNAL_DETAILED,
        G_STRUCT_OFFSET (LwDictionaryClassPrivate, progress_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    g_type_class_add_private (object_class, sizeof (LwDictionaryPrivate));

    pspec = g_param_spec_string ("filename",
                                 "Filename of the dictionary",
                                 "Set the dictionary's filename",
                                 "",
                                 G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_FILENAME, pspec);

    pspec = g_param_spec_object ("morphologyengine",
                                 "Morphology Engine referenced by the dictionary",
                                 "Set the dictionary's Morphology Engine used for indexing",
                                 LW_TYPE_MORPHOLOGYENGINE,
                                 G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_MORPHOLOGYENGINE, pspec);
}


//!
//! @brief Deletes a LwDictionary from the harddrive.  LwDictInst objects are used
//!        for installing dictionaries that do not exist yet.  You still need to free
//!        the object after.
//! @param dictionary An LwDictionary object to get the paths for the dictionary file.
//! @param cb A LwIoProgresSCallback to show dictionary uninstall progress or NULL.
//! @param error A pointer to a GError object to pass errors to or NULL.
//!
gboolean 
lw_dictionary_uninstall (LwDictionary *dictionary, 
                         LwProgress   *progress)
{
    //Sanity check
    g_return_val_if_fail (dictionary != NULL, FALSE);
    if (lw_progress_errored (progress)) return FALSE;

    lw_progress_set_object (progress, dictionary);

    //Declarations
    gchar *uri = NULL;
    const gchar *MESSAGE = NULL;
    const gchar *name = lw_dictionary_get_name (dictionary);

    //Initializations
    uri = lw_dictionary_get_path (dictionary);

    MESSAGE = gettext("Uninstalling %s Dictionary...");
    lw_progress_set_primary_message (progress, MESSAGE, name);

    MESSAGE = gettext("Removing %s...");
    lw_progress_set_secondary_message (progress, MESSAGE, uri);

    lw_progress_run_callback (progress);

    if (uri != NULL)
    {
      lw_io_remove (uri, progress);
      g_free (uri); uri = NULL;
    }

    return (!lw_progress_errored (progress));
}
 

FILE*
lw_dictionary_open (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    FILE *file = NULL;
    gchar *path = NULL;

    //Initializations
    path = lw_dictionary_get_path (dictionary); if (path == NULL) goto errored;
    
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
lw_dictionary_get_path (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    gchar *directory;
    const gchar *filename;
    gchar *path;

    //Initializations
    directory = lw_dictionary_get_directory (G_OBJECT_TYPE (dictionary));
    filename = lw_dictionary_get_filename (dictionary);
    path = NULL;

    if (directory != NULL)
    {
      path = g_build_filename (directory, filename, NULL);
      g_free (directory); directory = NULL;
    }

    return path;
}


gboolean 
lw_dictionary_parse_result (LwDictionary *dictionary, 
                            LwResult     *result, 
                            const gchar  *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (result != NULL, FALSE);
    g_return_val_if_fail (TEXT != NULL, FALSE);

    //Declarations
    LwDictionaryClass *klass = NULL;

    //Initializations
    klass = LW_DICTIONARY_CLASS (G_OBJECT_GET_CLASS (dictionary));
    g_return_val_if_fail (klass->priv->parse_result != NULL, FALSE);
    result->dictionary = dictionary;

    return klass->priv->parse_result (dictionary, result, TEXT);
}


const gchar*
lw_dictionary_get_name (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;

    //Initializations
    priv = dictionary->priv;

    return priv->name;
}


const gchar*
lw_dictionary_get_filename (LwDictionary *dictionary)
{
    LwDictionaryPrivate *priv;

    priv = dictionary->priv;

    return priv->filename;
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
lw_dictionary_build_id (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    gchar *id;
    GType type;
    const gchar *FILENAME;

    //Initializations
    type = G_OBJECT_TYPE (dictionary);
    FILENAME = lw_dictionary_get_filename (dictionary);
    id = lw_dictionary_build_id_from_type (type, FILENAME);

    return id;
}


//!
//! @brief Installs a LwDictionary object using the provided gui update callback
//!        This function should normally only be used in the lw_installdictionary_install function.
//! @param dictionary The LwDictionary object to use for installing the dictionary with.
//! @param cb A LwIoProgressCallback used to giver user feedback on how far the installation is.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_install (LwDictionary *dictionary, 
                       LwProgress   *progress)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv != NULL, FALSE);
    g_assert (dictionary->priv->install != NULL);
    if (lw_progress_errored (progress)) return FALSE;

    lw_progress_set_object (progress, dictionary);
    const gchar *name = lw_dictionary_get_name (dictionary);
    const gchar *MESSAGE = gettext("Installing %s Dictionary...");
    lw_progress_set_primary_message (progress, MESSAGE, name);
    lw_progress_run_callback (progress);

    lw_dictionary_installer_download (dictionary, progress);
    lw_dictionary_installer_decompress (dictionary, progress);
    lw_dictionary_installer_convert_encoding (dictionary, progress);
    lw_dictionary_installer_postprocess (dictionary, progress);
    lw_dictionary_installer_install (dictionary, progress);
    lw_dictionary_installer_clean (dictionary, progress);

    return (!lw_progress_errored (progress));
}


gboolean
lw_dictionary_is_selected (LwDictionary *dictionary)
{
    //Sanity check
    g_return_val_if_fail (dictionary != NULL, FALSE);

    //Declarations
    LwDictionaryPrivate *priv;

    //Initializations
    priv = dictionary->priv;

    return priv->selected;
}


void
lw_dictionary_set_selected (LwDictionary *dictionary, 
                            gboolean      selected)
{
    //Sanity check
    g_return_if_fail (dictionary != NULL);

    //Declarations
    LwDictionaryPrivate *priv;

    //Initializations
    priv = dictionary->priv;

    priv->selected = selected;
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
lw_dictionary_get_buffer (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    gchar *path = NULL;

    //Initializations
    priv = dictionary->priv;
    path = lw_dictionary_get_path (dictionary); if (path == NULL) goto errored;
    if (priv->data == NULL) priv->data = lw_dictionarydata_new (); if (priv->data == NULL) goto errored;
    if (priv->data->buffer == NULL) lw_dictionarydata_create (priv->data, path); if (priv->data->buffer == NULL) goto errored;

    if (path != NULL) g_free (path);

    return priv->data->buffer;

errored:

    if (path != NULL) g_free (path);

    return NULL;
}


const gchar*
lw_dictionary_get_string (LwDictionary *dictionary, 
                          LwOffset      offset)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;

    //Initializations
    priv = dictionary->priv;

    return lw_dictionarydata_get_string (priv->data, offset);
}

