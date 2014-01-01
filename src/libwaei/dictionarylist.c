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
//! @file dictionarylist.c
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

#include <libwaei/dictionarylist-private.h>


G_DEFINE_TYPE (LwDictionaryList, lw_dictionarylist, G_TYPE_OBJECT)


//!
//! @brief Creates a new LwDictionaryList object
//! @param MAX The maximum items you want in the dictionary before old ones are deleted
//! @return An allocated LwDictionaryList that will be needed to be freed by lw_dictionary_free.
//!
LwDictionaryList* 
lw_dictionarylist_new ()
{
    LwDictionaryList *dictionary = NULL;

    //Initializations
    dictionary = LW_DICTIONARYLIST (g_object_new (LW_TYPE_DICTIONARYLIST, NULL));

    return dictionary;
}


static void 
lw_dictionarylist_init (LwDictionaryList *dictionary_list)
{
    dictionary_list->priv = LW_DICTIONARYLIST_GET_PRIVATE (dictionary_list);
    memset(dictionary_list->priv, 0, sizeof(LwDictionaryListPrivate));
    g_mutex_init (&dictionary_list->priv->mutex);

    lw_dictionarylist_connect_signals (dictionary_list);
}


static void 
lw_dictionarylist_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
    //Declarations
    LwDictionaryList *dictionary_list;
    LwDictionaryListPrivate *priv;

    //Initializations
    dictionary_list = LW_DICTIONARYLIST (object);
    priv = dictionary_list->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        lw_dictionarylist_set_preferences (dictionary_list, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarylist_get_property (GObject      *object,
                                guint         property_id,
                                GValue       *value,
                                GParamSpec   *pspec)
{
    //Declarations
    LwDictionaryList *dictionary_list;
    LwDictionaryListPrivate *priv;

    //Initializations
    dictionary_list = LW_DICTIONARYLIST (object);
    priv = dictionary_list->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        g_value_set_object (value, lw_dictionarylist_get_preferences (dictionary_list));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarylist_finalize (GObject *object)
{
    //Declarations
    LwDictionaryList *dictionary_list = NULL;
    LwDictionaryListPrivate *priv = NULL;

    //Initalizations
    dictionary_list = LW_DICTIONARYLIST (object);
    priv = dictionary_list->priv;

    g_mutex_clear (&priv->mutex);

    G_OBJECT_CLASS (lw_dictionarylist_parent_class)->finalize (object);
}


static void
lw_dictionarylist_dispose (GObject *object)
{
    //Declarations
    LwDictionaryList *dictionary_list = NULL;

    //Initializations
    dictionary_list = LW_DICTIONARYLIST (object);

    lw_dictionarylist_clear (dictionary_list);
    lw_dictionarylist_disconnect_signals (dictionary_list);

    G_OBJECT_CLASS (lw_dictionarylist_parent_class)->dispose (object);
}



static void
lw_dictionarylist_class_init (LwDictionaryListClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LwDictionaryListClassPrivate *klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwDictionaryListClassPrivate, 1);
    klasspriv = klass->priv;
    object_class->set_property = lw_dictionarylist_set_property;
    object_class->get_property = lw_dictionarylist_get_property;
    object_class->dispose = lw_dictionarylist_dispose;
    object_class->finalize = lw_dictionarylist_finalize;

    g_type_class_add_private (object_class, sizeof (LwDictionaryListPrivate));

    klasspriv->signalid[CLASS_SIGNALID_CHANGED] = g_signal_new (
        "internal-row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryListClass, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_INSERTED] = g_signal_new (
        "internal-row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryListClass, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_DELETED] = g_signal_new (
        "internal-row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryListClass, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_REORDERED] = g_signal_new (
        "internal-rows-reordered",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryListClass, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1,
        G_TYPE_POINTER
    );

    klasspriv->pspec[PROP_PREFERENCES] = g_param_spec_object (
        "preferences",
        "Preferences construct prop",
        "Set the preferences object",
        LW_TYPE_PREFERENCES,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERENCES, klasspriv->pspec[PROP_PREFERENCES]);
}


void
lw_dictionarylist_set_preferences (LwDictionaryList *dictionary_list,
                                   LwPreferences    *preferences)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (dictionary_list));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionaryListClass *klass = NULL;
    LwDictionaryListClassPrivate *klasspriv = NULL;

    //Initializations
    priv = dictionary_list->priv;
    klass = LW_DICTIONARYLIST_GET_CLASS (dictionary_list);
    klasspriv = klass->priv;

    if (priv->preferences != NULL) {
      g_object_remove_weak_pointer (G_OBJECT (priv->preferences), (gpointer*) &(priv->preferences));
      g_object_unref (priv->preferences);
      priv->preferences = NULL;
    }

    priv->preferences = preferences;

    if (priv->preferences != NULL) {
      g_object_add_weak_pointer (G_OBJECT (priv->preferences), (gpointer*) &(priv->preferences));
    }

    g_object_notify_by_pspec (G_OBJECT (dictionary_list), klasspriv->pspec[PROP_PREFERENCES]);
}


LwPreferences*
lw_dictionarylist_get_preferences (LwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (dictionary_list));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;

    //Initializations
    priv = dictionary_list->priv;

    if (priv->preferences == NULL)
    {
      lw_dictionarylist_set_preferences (dictionary_list, lw_preferences_get_default ());
    }

    return priv->preferences;
}


void 
lw_dictionarylist_clear (LwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (dictionary_list));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionaryListClass *klass = NULL;
    LwDictionaryListClassPrivate *klasspriv = NULL;

    priv = dictionary_list->priv;
    klass = LW_DICTIONARYLIST_GET_CLASS (dictionary_list);
    klasspriv = klass->priv;

    while (priv->list != NULL)
    {
      GList *link = priv->list;
      LwDictionary *dictionary = LW_DICTIONARY (link->data);
      if (dictionary != NULL)
      {
        g_object_unref (dictionary); link->data = NULL;
      }
      priv->list = g_list_delete_link (priv->list, link);
      g_signal_emit (G_OBJECT (dictionary_list), klasspriv->signalid[CLASS_SIGNALID_DELETED], 0, 0);
    }
    priv->list = NULL;
}


void 
lw_dictionarylist_load_installed (LwDictionaryList   *dictionary_list, 
                                  LwMorphologyEngine *morphologyengine)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (dictionary_list));

    //Declarations
    LwDictionaryListClass *klass = NULL;
    LwDictionaryListClassPrivate *klasspriv = NULL;
    gchar** idlist = NULL;
    gchar **iditer = NULL;
    gchar** pair = NULL;
    gchar *typename = NULL;
    GType type = 0;
    LwDictionary *dictionary = NULL;
    const gchar *FILENAME = NULL;

    //Initializations
    lw_dictionarylist_clear (dictionary_list);
    klass = LW_DICTIONARYLIST_GET_CLASS (dictionary_list);
    klasspriv = klass->priv;

    idlist = lw_dictionary_get_installed_idlist (G_TYPE_NONE);
    if (idlist != NULL)
    {
      for (iditer = idlist; *iditer != NULL; iditer++)
      {
        pair = g_strsplit_set (*iditer, "/", 2);
        if (pair != NULL && pair[0] != NULL && pair[1] != NULL) 
        {
          typename = lw_dictionary_directoryname_to_typename (pair[0]);
          type = g_type_from_name (typename);
          FILENAME = pair[1];
          dictionary = LW_DICTIONARY (g_object_new (type, "filename", FILENAME, "morphologyengine", morphologyengine, NULL));
          if (dictionary != NULL && LW_IS_DICTIONARY (dictionary))
            lw_dictionarylist_append (dictionary_list, dictionary);
          if (typename != NULL) g_free (typename); typename = NULL;
        }
        g_strfreev (pair); pair = NULL;
      }
      g_strfreev (idlist); idlist = NULL;
    }
}



//!
//! @brief Gets the dictionary by load position in the GUI
//! @param request The GUI load position of the desired dictionary
//! @return The position in the LwDictionaryList of the LwDictionary
//!
LwDictionary* 
lw_dictionarylist_get_dictionary_by_position (LwDictionaryList* dictionary_list,
                                              gint              position)
{
    //Declarations
    LwDictionary *dictionary;

    //Initializiations
    dictionary = g_list_nth_data (dictionary_list->priv->list, position);

    return dictionary;
}


gint
lw_dictionarylist_get_position (LwDictionaryList *dictionary_list,
                                LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary_list != NULL, -1);
    g_return_val_if_fail (dictionary != NULL, -1);

    return g_list_index (dictionary_list->priv->list, dictionary);
}


LwDictionary*
lw_dictionarylist_remove_by_position (LwDictionaryList *dictionary_list,
                                      gint              position)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (dictionary_list), NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionaryListClass *klass = NULL;
    LwDictionaryListClassPrivate *klasspriv = NULL;
    GList *link = NULL;
    LwDictionary *removed_dictionary = NULL;

    //Initializations
    priv = dictionary_list->priv;
    klass = LW_DICTIONARYLIST_GET_CLASS (dictionary_list);
    klasspriv = klass->priv;
    removed_dictionary = NULL;
    link = g_list_nth (priv->list, position);

    if (link != NULL)
    {
      removed_dictionary = link->data;
      priv->list = g_list_delete_link (priv->list, link);
      g_signal_emit (G_OBJECT (dictionary_list), klasspriv->signalid[CLASS_SIGNALID_DELETED], 0, position);
    }

    return removed_dictionary;
}


LwDictionary*
lw_dictionarylist_remove (LwDictionaryList *dictionary_list,
                          LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (dictionary_list), NULL);
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionaryListClass *klass = NULL;
    LwDictionaryListClassPrivate *klasspriv = NULL;
    GList *link = NULL;
    LwDictionary *removed_dictionary = NULL;
    gint position = 0;

    //Initializations
    priv = dictionary_list->priv;
    klass = LW_DICTIONARYLIST_GET_CLASS (dictionary_list);
    klasspriv = klass->priv;
    removed_dictionary = NULL;
    link = g_list_find (priv->list, dictionary);
    position = g_list_index (priv->list, dictionary);

    if (link != NULL)
    {
      removed_dictionary = link->data;
      priv->list = g_list_delete_link (priv->list, link);
      g_signal_emit (G_OBJECT (dictionary_list), klasspriv->signalid[CLASS_SIGNALID_DELETED], 0, position);
    }
        
    return removed_dictionary;
}


//!
//! @brief Adds a dictionary to the LwDictionaryList with sanity checks
//! @param TYPE Engine of the dictionary to add
//! @param FILENAME Name of the dictionary to add
//!
void 
lw_dictionarylist_append (LwDictionaryList *dictionary_list,
                          LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (dictionary_list));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));
    if (lw_dictionarylist_dictionary_exists (dictionary_list, dictionary)) return;

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionaryListClass *klass = NULL;
    LwDictionaryListClassPrivate *klasspriv = NULL;

    //Initializations
    priv = dictionary_list->priv;
    klass = LW_DICTIONARYLIST_GET_CLASS (dictionary_list);
    klasspriv = klass->priv;

    //Append to the dictionary  if was loadable
    priv->list = g_list_append (priv->list, dictionary);

    {
      gint index = g_list_index (priv->list, dictionary);
      g_signal_emit (G_OBJECT (dictionary_list), klasspriv->signalid[CLASS_SIGNALID_INSERTED], 0, index);
    }
}


//!
//! @brief Searchs for a specific dictionary by name
//! The function will go through each dictionary until it matches the requested
//! name.
//!
//! @param DICTTYPE The parsing engine of the dictionary wanted.  There can be
//!               dictionaries with the same name, but different engines.
//! @param NAME A constant string to search for in the dictionary names.
//! @returns The requested LwDictionary object if found or null.
//!
LwDictionary* 
lw_dictionarylist_get_dictionary (LwDictionaryList* dictionary_list,
                                  const GType       TYPE,
                                  const gchar*      FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (dictionary_list != NULL && FILENAME != NULL, NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    GList *iter = NULL;
    LwDictionary *dictionary = NULL;
    GType type = 0;
    const gchar *FILENAME2 = NULL;

    //Initializations
    priv = dictionary_list->priv;
    dictionary = NULL;

    for (iter = priv->list; iter != NULL; iter = iter->next)
    {
      dictionary = LW_DICTIONARY (iter->data);
      type = G_OBJECT_TYPE (dictionary);
      FILENAME2 = lw_dictionary_get_filename (dictionary);
      if (dictionary != NULL && g_type_is_a (type, TYPE) && strcmp (FILENAME2, FILENAME) == 0)
      {
        break;
      }
      dictionary = NULL;
    }

    return dictionary;
}

//!
//!  @brief  Gets a LwDictionary object by a fuzzy string description.
//!          It can be either of the form "parser/dictionary" or 
//!          just be the dictionary name.  Case is ignored.
//!  @param FUZZY_DESCRIPTION A fuzzy description of the wanted dictionary.
//!  @returns A matching LwDictionary object or NULL
//!
LwDictionary* 
lw_dictionarylist_get_dictionary_fuzzy (LwDictionaryList *dictionary_list,
                                        const gchar      *FUZZY_DESCRIPTION)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (dictionary_list), NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionary *dictionary = NULL;

    //Initializations
    priv = dictionary_list->priv;
    dictionary = NULL;

    //Try getting the first dictionary if none is specified
    if (FUZZY_DESCRIPTION == NULL)
    {
      if (priv->list != NULL)
        dictionary = LW_DICTIONARY (priv->list->data);
      else
        dictionary = NULL;
    }

    //Otherwise try getting a dictionary using a few different string parsers
    else
    {
      if (dictionary == NULL)
        dictionary = lw_dictionarylist_get_dictionary_by_id (dictionary_list, FUZZY_DESCRIPTION);
      if (dictionary == NULL)
        dictionary = lw_dictionarylist_get_dictionary_by_filename (dictionary_list, FUZZY_DESCRIPTION);
    }

    return dictionary;
}

//!
//! @brief Grabs the first dictionary with a matching dictionary 
//!        filename.  If you have dictionaries with different 
//!        parsers but the same name, the others will not 
//!        be accessible with this function.
//! @param NAME A constant string to search for in the dictionary names.  
//!             This is a fuzzy search, ignoring DICTTYPE and case
//! @returns The requested LwDictionary object if found or null.
//!
LwDictionary* 
lw_dictionarylist_get_dictionary_by_filename (LwDictionaryList *dictionary_list,
                                              const gchar      *FILENAME)
{
    //Sanity checks
    g_assert (FILENAME != NULL);

    //Declarations
    LwDictionaryListPrivate *priv;
    GList *iter;
    LwDictionary *dictionary;
    const gchar *FILENAME2;

    //Initializations
    priv = dictionary_list->priv;
    dictionary = NULL;

    for (iter = priv->list; iter != NULL; iter = iter->next)
    {
      dictionary = LW_DICTIONARY (iter->data);
      FILENAME2 = lw_dictionary_get_filename (dictionary);
      if (FILENAME2 != NULL && g_ascii_strcasecmp (FILENAME2, FILENAME) == 0)
        break;
      dictionary = NULL;
    }

    return dictionary;
}


//!
//! @brief Finds a dictionary by using an id string of the form of 
//!        "engine/dictionary". Case is ignored.
//! @param ENGINE_AND_FILENAME A string in the form "engine/dictionary"
//!                            used to search for a dictionary
//! @returns The requested LwDictionary object if found or NULL.
//!
LwDictionary* 
lw_dictionarylist_get_dictionary_by_id (LwDictionaryList *dictionary_list,
                                        const gchar      *ENGINE_AND_FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (dictionary_list != NULL && ENGINE_AND_FILENAME != NULL, NULL);

    //Declarations
    LwDictionaryListPrivate *priv;
    GList *link;
    LwDictionary *dictionary;
    gchar **pair;
    const gchar *FILENAME1;
    const gchar *FILENAME2;
    GType type1, type2;

    //Initializations
    priv = dictionary_list->priv;
    link = NULL;
    dictionary = NULL;
    pair = g_strsplit (ENGINE_AND_FILENAME, "/", 2);

    if (pair != NULL)
    {
      if (g_strv_length (pair) == 2)
      {
        type1 = g_type_from_name (pair[0]);
        FILENAME1 = pair[1];

        for (link = priv->list; link != NULL; link = link->next)
        {
          dictionary = LW_DICTIONARY (link->data);
          FILENAME2 = lw_dictionary_get_filename (dictionary);
          type2 = G_OBJECT_TYPE (dictionary);
          if (g_type_is_a (type1, type2) && g_ascii_strcasecmp (FILENAME1, FILENAME2) == 0)
            break;
          dictionary = NULL;
        }
      }

      g_strfreev (pair); pair = NULL;
    }

    return dictionary;
}



//!
//! @brief Searchs for a specific dictionary by name to see if it is installed
//! @param NAME request a const string to search for in the dictionary names
//! @return returns true if the dictionary is installed
//!
gboolean 
lw_dictionarylist_dictionary_exists (LwDictionaryList *dictionary_list,
                                     LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary != NULL, FALSE);

    //Declarations
    LwDictionaryListPrivate *priv;
    gboolean exists;
    GList *link;

    //Initializations
    priv = dictionary_list->priv;
    exists = FALSE;
    link = priv->list;

    //Return true if the dictionary exists
    while (link != NULL && exists == FALSE)
    {
      exists = lw_dictionary_equals (dictionary, LW_DICTIONARY (link->data));
      link = link->next;
    }

    return exists;
}


//!
//! @brief Returns the number of dictionaries in the dictionary 
//!
//! This is not the number of dictionaries that are active.  It shows
//! how many dictionary names are recorded in the dictionary .
//! By default, the default dictionaries appended to the  with
//! an UNLW_DICT_STATUS_INSTALLED status if they are unavailable. If the LW_DICT_MIX dictionary
//! is installed, Kanji and Radicals disappear from the GUI, but are still
//! in this .
//!
//! @return Integer representing the number of installed dictionaries
//!
gint 
lw_dictionarylist_get_total (LwDictionaryList *dictionary_list)
{
    //Sanity check
    g_return_val_if_fail (dictionary_list != NULL, 0);

    LwDictionaryListPrivate *priv;
    priv = dictionary_list->priv;

    return g_list_length (priv->list);
}


//
//! @brief Saves the current load order to the preferences
//
void 
lw_dictionarylist_save_order (LwDictionaryList *dictionary_list)
{
    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionary *dictionary = NULL;
    LwPreferences *preferences = NULL;
    gchar *order = NULL;
    gchar **atoms = NULL;

    //Initializations
    priv = dictionary_list->priv;
    preferences = lw_dictionarylist_get_preferences (dictionary_list);

    atoms = g_new0 (gchar*, lw_dictionarylist_get_total (dictionary_list) + 1);
    if (atoms == NULL) goto errored;

    //Construct the preference string
    {
      gchar **ptr = atoms;
      GList *link = priv->list;
      while (link != NULL)
      {
        dictionary = LW_DICTIONARY (link->data);
        *ptr = lw_dictionary_build_id (dictionary);
        if (*ptr == NULL) break;
        ptr++;
        link = link->next;
      }
      *ptr = NULL;
    }

    order = g_strjoinv (";", atoms);
    if (order == NULL) goto errored;

    lw_preferences_set_string_by_schema (preferences, LW_SCHEMA_DICTIONARY, LW_KEY_LOAD_ORDER, order);

errored:

    g_free (order); order = NULL;
    g_strfreev (atoms); atoms = NULL;
}


static GHashTable*
lw_dictionarylist_build_order_map (LwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (dictionary_list), NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gchar *order = NULL;
    gchar **atoms = NULL;
    gchar i = 0;
    LwPreferences *preferences = NULL;
    GHashTable *hashtable = NULL;

    //Initializations
    priv = dictionary_list->priv;
    hashtable = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    if (hashtable == NULL) goto errored;

    preferences = lw_dictionarylist_get_preferences (dictionary_list);;
    if (preferences == NULL) goto errored;

    order = lw_preferences_get_string_by_schema (preferences, LW_SCHEMA_DICTIONARY, LW_KEY_LOAD_ORDER);
    if (order == NULL) goto errored;

    atoms = g_strsplit_set (order, ";", -1);
    if (atoms == NULL) goto errored;

    //Generate the hashtable
    for (i = 0; atoms[i] != NULL; i++)
    {
      g_hash_table_insert (hashtable, atoms[i], GINT_TO_POINTER (i));
    }

errored:
    g_free (order); order = NULL;

    if (atoms != NULL) {
      if (hashtable != NULL) g_free (atoms);
      else g_strfreev (atoms); 
      atoms = NULL;
    }

    return hashtable;
}


static gint*
lw_dictionarylist_convert_order_map_to_array (LwDictionaryList *dictionary_list, 
                                              GHashTable       *hashtable)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (dictionary_list), NULL);
    g_return_val_if_fail (hashtable != NULL, NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    GList *link = NULL;
    gint *new_order = NULL;
    gint missing_index = 0;

    //Initializations
    priv = dictionary_list->priv;
    if (priv == NULL) goto errored;
    link = priv->list;
    if (link == NULL) goto errored;
    missing_index = g_list_length (priv->list);
    if (missing_index == 0) goto errored;
    new_order = g_new0 (gint, missing_index);
    if (new_order == NULL) goto errored;

    {
      gint i = 0;
      while (link != NULL)
      {
        LwDictionary *dictionary = LW_DICTIONARY (link->data);
        if (dictionary != NULL)
        {
          gchar *key = lw_dictionary_build_id (dictionary);
          if (key != NULL)
          {
            gpointer position_ptr = NULL;
            if (g_hash_table_lookup_extended (hashtable, key, NULL, &position_ptr))
            {
              new_order[i] = GPOINTER_TO_INT (position_ptr);
            }
            else
            {
              new_order[i] = missing_index;
              missing_index++;
            }

            g_free (key); key = NULL;
          }
        }

        i++;
        link = link->next;
      }
    }

errored:

    return new_order;
}


//!
//! @brief All dictionaries with a specific status get switched to the requested one
//! @param a Pointer to LwDictionary object a
//! @param b Pointer to LwDictionary object b
//! @returns Whether the position of a is less than (-1), equal (0) or greater than b (1)
//!
static gint 
lw_dictionarylist_sort_compare_function (gconstpointer a,
                                         gconstpointer b,
                                         gpointer      data)
{
    //Sanity checks
    g_return_val_if_fail (data != NULL, 0);
  
    //Declarations
    GHashTable *hashtable = NULL;
    gchar *description_a = NULL, *description_b = NULL;
    LwDictionary *dictionary_a = NULL, *dictionary_b = NULL;
    gpointer position_a_ptr = NULL, position_b_ptr = NULL;
    gint position_a, position_b;
    gboolean found_a, found_b;
    gint order;

    //Initializations
    hashtable = (GHashTable*) data;
    found_a = found_b = FALSE;

    dictionary_a = LW_DICTIONARY (a);
    description_a = lw_dictionary_build_id (dictionary_a);
    if (description_a != NULL)
    {
      found_a = g_hash_table_lookup_extended (hashtable, description_a, NULL, &position_a_ptr);
      position_a = GPOINTER_TO_INT (position_a_ptr);
      g_free (description_a); description_a = NULL;
    }
    
    dictionary_b = LW_DICTIONARY (b);
    description_b = lw_dictionary_build_id (dictionary_b);
    if (description_b != NULL)
    {
      found_b = g_hash_table_lookup_extended (hashtable, description_b, NULL, &position_b_ptr);
      position_b = GPOINTER_TO_INT (position_b_ptr);
      g_free (description_b); description_b = NULL;
    }

    //Calculate the proper order
    if (found_a && found_b)
    {
      if (position_a > position_b) order = 1;
      else if (position_a < position_b) order = -1;
      else order = 0;
    }
    else if (found_a)
    {
      order = -1;
    }
    else if (found_b)
    {
      order = 1;
    }
    else
    {
      order = 0;
    }

    return order;
}


//
//! @brief Loads the load order from the preferences
//
void 
lw_dictionarylist_load_order (LwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (dictionary_list));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionaryListClass *klass = NULL;
    LwDictionaryListClassPrivate *klasspriv = NULL;
    gint *new_order = NULL;
    GHashTable *order_map = NULL;

    //Initializations
    priv = dictionary_list->priv;
    klass = LW_DICTIONARYLIST_GET_CLASS (dictionary_list);
    klasspriv = klass->priv;

    if (priv->list == NULL) goto errored;

    order_map = lw_dictionarylist_build_order_map (dictionary_list);
    if (order_map == NULL) goto errored;

    new_order = lw_dictionarylist_convert_order_map_to_array (dictionary_list, order_map);
    if (new_order == NULL) goto errored;

    priv->list = g_list_sort_with_data (priv->list, lw_dictionarylist_sort_compare_function, order_map);
    if (priv->list == NULL) goto errored;

    printf("BREAk lw_dictionarylist_load_order CLASS_SIGNALID_REORDERED\n");
    g_signal_emit (G_OBJECT (dictionary_list), klasspriv->signalid[CLASS_SIGNALID_REORDERED], 0, new_order);

errored:

    g_free (new_order); new_order = NULL;
    if (order_map != NULL) g_hash_table_destroy (order_map); order_map = NULL;
}


//!
//! @brief Sets up the built-in installabale dictionaries
//!
void
lw_dictionarylist_load_installable (LwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (dictionary_list));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionaryListClass *klass = NULL;
    LwDictionaryListClassPrivate *klasspriv = NULL;
    LwDictionary* dictionary = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = dictionary_list->priv;
    klass = LW_DICTIONARYLIST_GET_CLASS (dictionary_list);
    klasspriv = klass->priv;
    preferences = lw_dictionarylist_get_preferences (dictionary_list);
    if (preferences == NULL) goto errored;

    {
      dictionary = lw_edictionary_new ("English", NULL);
      lw_dictionary_set_builtin_installer_full (
        dictionary, 
        "English",
        preferences,
        LW_KEY_ENGLISH_SOURCE,
        gettext("The venerable edict by Jim Breen."),
        LW_ENCODING_EUC_JP,
        FALSE
      );
      priv->list = g_list_append (priv->list, dictionary);
      g_signal_emit (dictionary_list, klasspriv->signalid[CLASS_SIGNALID_INSERTED], 0, g_list_index (priv->list, dictionary));
    }

    {
      dictionary = lw_kanjidictionary_new ("Kanji", NULL);
      lw_dictionary_set_builtin_installer_full (
        dictionary, 
        "Kanji",
        preferences,
        LW_KEY_KANJI_SOURCE,
        gettext("A Kanji dictionary based off of kanjidic with radical information combined."),
        LW_ENCODING_EUC_JP,
        TRUE
      );
      priv->list = g_list_append (priv->list, dictionary);
      g_signal_emit (dictionary_list, klasspriv->signalid[CLASS_SIGNALID_INSERTED], 0, g_list_index (priv->list, dictionary));
    }

    {
      dictionary = lw_edictionary_new ("Names and Places", NULL);
      lw_dictionary_set_builtin_installer_full (
        dictionary, 
        "Names;Places",
        preferences,
        LW_KEY_NAMES_PLACES_SOURCE,
        gettext("Based off of Enamdic, but with the names split from the places for 2 separate dictionaries."),
        LW_ENCODING_EUC_JP,
        TRUE
      );
      priv->list = g_list_append (priv->list, dictionary);
      g_signal_emit (dictionary_list, klasspriv->signalid[CLASS_SIGNALID_INSERTED], 0, g_list_index (priv->list, dictionary));
    }

    {
      dictionary = lw_exampledictionary_new ("Examples", NULL);
      lw_dictionary_set_builtin_installer_full (
        dictionary, 
        "Examples",
        preferences,
        LW_KEY_EXAMPLES_SOURCE,
        gettext("A collection of Japanese/English sentences initially compiled "
                "by Professor Yasuhito Tanaka at Hyogo University and his students."),
        LW_ENCODING_EUC_JP,
        TRUE
      );
      priv->list = g_list_append (priv->list, dictionary);
      g_signal_emit (dictionary_list, klasspriv->signalid[CLASS_SIGNALID_INSERTED], 0, g_list_index (priv->list, dictionary));
    }

/*
    {
      dictionary = lw_dictionary_new (
        "",
        gettext("Other"),
        gettext("Other Dictionary"),
        gettext("Install a custom dictionary."),
        "",
        LW_TYPE_UNKNOWNDICTIONARY,
        LW_COMPRESSION_NONE,
        LW_ENCODING_UTF8,
        FALSE,
        FALSE
      );
      priv->list = g_list_append (priv->list, dictionary);
      g_signal_emit (dictionary_list, klasspriv->signalid[CLASS_SIGNALID_INSERTED], 0, g_list_index (priv->list, dictionary));
    }
*/

errored:
    
    return;
}


//!
//! @brief Checks to see if the current InstallDictionaryList is installation ready
//!
gboolean 
lw_dictionarylist_installer_is_valid (LwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_val_if_fail (dictionary_list != NULL, FALSE);

    //Declarations
    LwDictionaryListPrivate *priv;
    GList *link;
    LwDictionary* dictionary;
    gint number_selected;
    gboolean selected;
    gboolean valid;

    //Initializations
    priv = dictionary_list->priv;
    link = priv->list;
    number_selected = 0;

    while (link != NULL)
    {
      dictionary = LW_DICTIONARY (link->data);
      valid = lw_dictionary_installer_is_valid (dictionary);
      selected = lw_dictionary_is_selected (dictionary);

      if (!valid && selected) return FALSE;
      if (selected) number_selected++;

      link = link->next;
    }

    return (number_selected > 0);
}


GList*
lw_dictionarylist_get_list (LwDictionaryList *dictionary_list)
{
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (dictionary_list), NULL);

    return g_list_copy (dictionary_list->priv->list);
}


void
lw_dictionarylist_sort_with_data (LwDictionaryList *dictionary_list, 
                                  GCompareDataFunc  compare_func, 
                                  gpointer          user_data)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (dictionary_list));

    //Declarations
    LwDictionaryListPrivate *priv;

    //Initializations
    priv = dictionary_list->priv;

    priv->list = g_list_sort_with_data (priv->list, compare_func, user_data);
}


void
lw_dictionarylist_menumodel_insert (LwDictionaryList *dictionary_list,
                                    LwDictionary     *dictionary,
                                    gint              index_)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (dictionary_list));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));

    //Declarations
    GMenuModel *menu_model = NULL;
    GMenu *menu = NULL;
    GMenuItem *menuitem = NULL;
    gchar *detailed_action = NULL;
    const gchar *SHORTNAME = NULL;
    gchar *longname = NULL;

    //Initializations
    menu_model = lw_dictionarylist_get_menumodel (dictionary_list);
    menu = G_MENU (menu_model);
    longname = NULL;
    detailed_action = NULL;
    menuitem = NULL;
    SHORTNAME = lw_dictionary_get_name (dictionary);

    longname = g_strdup_printf (gettext("%s Dictionary"), SHORTNAME);
    if (longname == NULL) goto errored;
    detailed_action = g_strdup_printf ("win.set-dictionary::%d", index_ + 1);
    if (detailed_action == NULL) goto errored;

    menuitem = g_menu_item_new (longname, detailed_action);
    if (menuitem == NULL) goto errored;
    g_menu_insert_item (menu, index_, menuitem);

errored:

    if (longname != NULL) g_free (longname); longname = NULL;
    if (detailed_action != NULL) g_free (detailed_action); detailed_action = NULL;
    if (menuitem != NULL) g_object_unref (menuitem); menuitem = NULL;
}


void
lw_dictionarylist_menumodel_append (LwDictionaryList *dictionary_list, 
                                    LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (dictionary_list));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));

    //Declarations
    GMenuModel *menu_model = NULL;
    gint index = 0;

    //Initializations
    menu_model = lw_dictionarylist_get_menumodel (dictionary_list);
    if (menu_model == NULL) goto errored;
    index = g_menu_model_get_n_items (menu_model);

    lw_dictionarylist_menumodel_insert (dictionary_list, dictionary, index);

errored:

    return;
}


void
lw_dictionarylist_sync_menumodel (LwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (dictionary_list));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    GMenuModel *menu_model = NULL;
    GMenu *menu = NULL;

    //Initializations
    priv = dictionary_list->priv;
    menu_model = lw_dictionarylist_get_menumodel (dictionary_list);
    menu = G_MENU (menu_model);

    //Clear the old menu items
    while (g_menu_model_get_n_items (menu_model) > 0)
    {
      g_menu_remove (menu, 0);
    }

    //Repopulate with the new menu items
    {
      GList *link = priv->list;
      while (link != NULL)
      {
        LwDictionary *dictionary = LW_DICTIONARY (link->data);
        if (dictionary != NULL)
        {
          lw_dictionarylist_menumodel_append (dictionary_list, dictionary);
        }
        link = link->next;
      }
    }
}


GMenuModel*
lw_dictionarylist_get_menumodel (LwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (dictionary_list), NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    GMenuModel *menu_model = NULL;

    //Initializations
    priv = dictionary_list->priv;
    if (priv == NULL) goto errored;
    if (priv->menu_model == NULL)
    {
      priv->menu_model = G_MENU_MODEL (g_menu_new ());
    }
    menu_model = priv->menu_model;

errored:

    return menu_model;
}


