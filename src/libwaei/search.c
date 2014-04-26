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
//! @file searchitem.c
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

#include <libwaei/search-private.h>

static LwSearchClass *_klass = NULL;
static LwSearchClassPrivate *_klasspriv = NULL;

G_DEFINE_TYPE (LwSearch, lw_search, G_TYPE_OBJECT)

//!
//! @brief Creates a new LwSearch object. 
//! @param query The text to be self for
//! @param dictionary The LwDictionary object to use
//! @param TARGET The widget to output the results to
//! @param error A GError to place errors into or NULL
//! @return Returns an allocated LwSearch object that should be freed with lw_search_free or NULL on error
//!
LwSearch* 
lw_search_new (const gchar  *QUERY,
               LwDictionary *dictionary,
               LwSearchFlag  flags)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (dictionary), NULL);
    g_return_val_if_fail (QUERY != NULL, NULL);

    //Declarations
    LwSearch *self = NULL;
    
    //Initializations
    self  = LW_SEARCH (g_object_new (LW_TYPE_SEARCH, "query", QUERY, "dictionary", dictionary, "flags", flags, NULL));

    return self;
}


LwSearch* 
lw_search_new_by_preferences (const gchar   *QUERY,
                              LwDictionary  *dictionary,
                              LwPreferences *preferences)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (dictionary), NULL);

    //Declarations
    LwSearch *self = NULL;
    LwSearchFlag flags = 0;

    //Initializations
    flags = lw_search_get_flags_from_preferences (preferences);
    self = lw_search_new (QUERY, dictionary, flags);

    return self;
}


static void
lw_search_init (LwSearch *self)
{
    self->priv = LW_SEARCH_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwSearchPrivate));

    LwSearchPrivate *priv = NULL;

    priv = self->priv;

    g_mutex_init (&priv->data.mutex);
    priv->data.status = LW_SEARCHSTATUS_IDLE;
    priv->config.max_results = -1;
}


static void 
lw_search_set_property (GObject      *object,
                        guint         property_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
    //Declarations
    LwSearch *self = NULL;
    LwSearchPrivate *priv = NULL;

    //Initializations
    self = LW_SEARCH (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_DICTIONARY:
        lw_search_set_dictionary (self, g_value_get_object (value));
        break;
      case PROP_QUERY:
        lw_search_set_query (self, g_value_get_string (value));
        break;
      case PROP_FLAGS:
        lw_search_set_flags (self, g_value_get_flags (value));
        break;
      case PROP_MAX_RESULTS:
        lw_search_set_max_results (self, g_value_get_int (value));
        break;
      case PROP_PROGRESS:
        lw_search_set_progress (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_search_get_property (GObject    *object,
                        guint       property_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
    //Declarations
    LwSearch *self = NULL;
    LwSearchPrivate *priv = NULL;

    //Initializations
    self = LW_SEARCH (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_DICTIONARY:
        g_value_set_object (value, lw_search_get_dictionary (self));
        break;
      case PROP_QUERY:
        g_value_set_string (value, lw_search_get_query (self));
        break;
      case PROP_FLAGS:
        g_value_set_int (value, lw_search_get_flags (self));
        break;
      case PROP_STATUS:
        g_value_set_int (value, lw_search_get_status (self));
        break;
      case PROP_MAX_RESULTS:
        g_value_set_int (value, lw_search_get_max_results (self));
        break;
      case PROP_PROGRESS:
        g_value_set_object (value, lw_search_get_progress (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_search_finalize (GObject *object)
{
    //Declarations
    LwSearch *self = NULL;
    LwSearchPrivate *priv = NULL;

    //Initalizations
    self = LW_SEARCH (object);
    priv = self->priv;


    G_OBJECT_CLASS (lw_search_parent_class)->finalize (object);
}


static void
lw_search_dispose (GObject *object)
{
    //Declarations
    LwSearch *self = NULL;

    //Initializations
    self = LW_SEARCH (object);

    G_OBJECT_CLASS (lw_search_parent_class)->dispose (object);
}


static void
lw_search_class_init (LwSearchClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwSearchClassPrivate, 1);
    object_class->set_property = lw_search_set_property;
    object_class->get_property = lw_search_get_property;
    object_class->dispose = lw_search_dispose;
    object_class->finalize = lw_search_finalize;

    g_type_class_add_private (object_class, sizeof (LwSearchPrivate));

    _klass = klass;
    _klasspriv = klass->priv;

    _klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED] = g_signal_new (
        "internal-row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwSearchClass, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED] = g_signal_new (
        "internal-row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwSearchClass, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED] = g_signal_new (
        "internal-row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwSearchClass, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED] = g_signal_new (
        "internal-rows-reordered",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwSearchClass, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1,
        G_TYPE_POINTER
    );

    _klasspriv->pspec[PROP_DICTIONARY] = g_param_spec_object (
        "dictionary",
        "FIlename construct prop",
        "Set the filename",
        LW_TYPE_DICTIONARY,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DICTIONARY, _klasspriv->pspec[PROP_DICTIONARY]);

    _klasspriv->pspec[PROP_QUERY] = g_param_spec_string (
        "query",
        "changed construct prop",
        "Set the changed",
        NULL,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_QUERY, _klasspriv->pspec[PROP_QUERY]);

    _klasspriv->pspec[PROP_FLAGS] = g_param_spec_int (
        "flags",
        "loaded construct prop",
        "Set the loaded",
        G_MININT,
        G_MAXINT,
        0,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_FLAGS, _klasspriv->pspec[PROP_FLAGS]);

    _klasspriv->pspec[PROP_STATUS] = g_param_spec_int (
        "status",
        "loaded construct prop",
        "Set the loaded",
        G_MININT,
        G_MAXINT,
        LW_SEARCHSTATUS_IDLE,
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_FLAGS, _klasspriv->pspec[PROP_FLAGS]);

    _klasspriv->pspec[PROP_MAX_RESULTS] = g_param_spec_int (
        "max-results",
        "loaded construct prop",
        "Set the loaded",
        0,
        G_MAXINT,
        0,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_MAX_RESULTS, _klasspriv->pspec[PROP_MAX_RESULTS]);

    _klasspriv->pspec[PROP_PROGRESS] = g_param_spec_object (
        "progress",
        "loaded construct prop",
        "Set the loaded",
        LW_TYPE_PROGRESS,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PROGRESS, _klasspriv->pspec[PROP_PROGRESS]);
}


void
lw_search_set_query (LwSearch    *self,
                     const gchar *QUERY)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;

    if (priv->data.query != NULL) lw_morphologystring_free (priv->data.query);
    priv->data.query = lw_morphologyengine_parse (priv->data.morphologyengine, QUERY, LW_NORMALIZATION_INSENSITIVE);

errored:

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_QUERY]);
}


const gchar*
lw_search_get_query (LwSearch *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.query->raw;
}


LwDictionary*
lw_search_get_dictionary (LwSearch *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.dictionary;
}


void
lw_search_set_dictionary (LwSearch     *self,
                          LwDictionary *dictionary)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (dictionary != NULL)
    {
      g_object_ref (dictionary);
    }

    if (priv->data.dictionary != NULL)
    {
      g_object_remove_weak_pointer (G_OBJECT (priv->data.dictionary), (gpointer*) &(priv->data.dictionary));
      g_object_unref (priv->data.dictionary);
    }

    priv->data.dictionary = dictionary;

    if (dictionary != NULL)
    {
      g_object_add_weak_pointer (G_OBJECT (priv->data.dictionary), (gpointer*) &(priv->data.dictionary));
    }
}

LwProgress*
lw_search_get_progress (LwSearch *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.progress;
}


void
lw_search_set_progress (LwSearch   *self,
                        LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (progress != NULL)
    {
      g_object_ref (progress);
    }

    if (priv->data.progress != NULL)
    {
      g_object_remove_weak_pointer (G_OBJECT (priv->data.progress), (gpointer*) &(priv->data.progress));
      g_object_unref (priv->data.progress);
    }

    priv->data.progress = progress;

    if (progress != NULL)
    {
      g_object_add_weak_pointer (G_OBJECT (priv->data.progress), (gpointer*) &(priv->data.progress));
    }
}


void
lw_search_set_max_results (LwSearch *self,
                           gint      max_results)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (priv->config.max_results != max_results);

    priv->config.max_results = max_results;

errored:

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_MAX_RESULTS]);
}


gint
lw_search_get_max_results (LwSearch *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.max_results;
}


void
lw_search_set_status (LwSearch       *self, 
                      LwSearchStatus  status)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));
    if (status < 0) return;

    //Declarations
    LwSearchPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed =  (priv->data.status != status);

    priv->data.status = status;

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_STATUS]);
}


LwSearchStatus
lw_search_get_status (LwSearch *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;
    LwSearchStatus status = 0;

    //Initializations
    priv = self->priv;
    status = priv->data.status;

    return status;
}


LwSearchFlag
lw_search_get_flags (LwSearch *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SEARCH (self), 0);

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.flags;
}


void
lw_search_set_flags (LwSearch *self,
                     LwSearchFlag flags)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SEARCH (self), 0);

    //Declarations
    LwSearchPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (priv->config.flags != flags);
    if (!changed) goto errored;

errored:

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_FLAGS]);
}


LwSearchFlag
lw_search_get_flags_from_preferences (LwPreferences *preferences)
{
    //Sanity checks
    g_return_val_if_fail (preferences != NULL, 0);

    //Declarations
    gboolean furigana_insensitive;
    gboolean case_insensitive;
    gboolean stem_insensitive;
    gint romaji_to_furigana;
    gboolean want_romaji_to_furigana_conv;
    gboolean index_results;
    gint32 flags;

    //Initializations
    furigana_insensitive = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_FURIGANA_INSENSITIVE);
    case_insensitive = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_CASE_INSENSITIVE);
    stem_insensitive = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_STEM_INSENSITIVE);
    romaji_to_furigana = lw_preferences_get_int_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_ROMAJI_TO_FURIGANA);
    want_romaji_to_furigana_conv = (romaji_to_furigana == 0 || (romaji_to_furigana == 2 && !lw_util_is_japanese_locale()));
    index_results = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_INDEX_RESULTS);
    flags = LW_SEARCHFLAG_RAW;

    if (furigana_insensitive) flags |= LW_SEARCHFLAG_FURIGANA_INSENSITIVE;
    if (case_insensitive) flags |= LW_SEARCHFLAG_CASE_INSENSITIVE;
    if (stem_insensitive) flags |= LW_SEARCHFLAG_STEM_INSENSITIVE;
    if (want_romaji_to_furigana_conv) flags |= LW_SEARCHFLAG_ROMAJI_TO_FURIGANA;
    if (index_results) flags |= LW_SEARCHFLAG_USE_INDEX;

    return flags;
}

/*
static LwIndexFlag WHAT IS THIS?
_lw_search_get_index_flags (LwSearch *self)
{
    g_return_val_if_fail (self != NULL, 0);

    LwIndexFlag flags  = 0;
    if (priv->config.flags & LW_SEARCHFLAG_RAW) flags |= LW_INDEX_FLAG_RAW;
    if (priv->config.flags & LW_SEARCHFLAG_CASE_INSENSITIVE) flags |= LW_INDEX_FLAG_CASE_INSENSITIVE;
    if (priv->config.flags & LW_SEARCHFLAG_FURIGANA_INSENSITIVE) flags |= LW_INDEX_FLAG_FURIGANA_INSENSITIVE;
    if (priv->config.flags & LW_SEARCHFLAG_STEM_INSENSITIVE) flags |= LW_INDEX_FLAG_STEM_INSENSITIVE;

    return flags;
}
*/
















static gpointer 
lw_search_stream_results_thread (LwSearch *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SEARCH (self), NULL);

printf("BREAK lw_search_stream_results_thread\n");
    //Declarations
    LwSearchPrivate *priv = NULL;
    LwProgress *progress = NULL;
    LwDictionary *dictionary = NULL;
    LwSearchFlag flags = 0;

    //Initializations
    priv = self->priv;
    g_return_val_if_fail (self != NULL, NULL);
    progress = priv->data.progress;
    dictionary = priv->data.dictionary;
    flags = priv->config.flags;

    //Index the dictionary if it isn't already
    if (flags & LW_SEARCHFLAG_USE_INDEX)
    {
printf("BREAK lw_search_stream_results_thread trying to load index\n");
      if (!lw_dictionary_index_exists (dictionary)) lw_dictionary_index_create (dictionary, progress);
      if (lw_progress_should_abort (progress)) goto errored;
      lw_dictionary_index_load (dictionary, progress);
      if (lw_progress_should_abort (progress)) goto errored;
    }

    g_mutex_lock (&priv->data.mutex);

/*TODO
    //Indexed self
    if (lw_dictionary_index_is_loaded (dictionary) && flags & LW_SEARCHFLAG_USE_INDEX && !lw_util_is_regex_pattern (priv->data.query, NULL))
    {
printf("BREAK lw_search_stream_results_thread index is loaded\n");
      LwMorphologyList *morphologylist = lw_search_get_query_as_morphologylist (self);

      if (self->resulttable != NULL) g_hash_table_unref (self->resulttable); self->resulttable = NULL;
      self->resulttable = lw_dictionary_index_search (dictionary, morphologylist, flags, progress);

      lw_morphologylist_free (morphologylist); morphologylist = NULL;
    }

    //Regex self
    else
    {
      if (self->resulttable != NULL) g_hash_table_unref (self->resulttable);
      self->resulttable = lw_dictionary_regex_search (dictionary, priv->data.query, flags, progress);
    }
*/

errored:

    priv->data.status = LW_SEARCHSTATUS_FINISHING;
    priv->data.thread = NULL;

    g_mutex_unlock (&priv->data.mutex);

    return NULL;
}


//!
//! @brief Start a dictionary self
//! @param self a LwSearch argument to calculate results
//! @param create_thread Whether the self should run in a new thread.
//! @param exact Whether to show only exact matches for this self
//!
void 
lw_search_start (LwSearch *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    lw_search_stream_results_thread ((gpointer) self);
}


void
lw_search_start_async (LwSearch *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;
    GError *error = NULL;

    //Initializations
    priv = self->priv;
    priv->data.thread = NULL;
    priv->data.status = LW_SEARCHSTATUS_SEARCHING;

    priv->data.thread = g_thread_try_new (
      "libwaei-search-thread",
      (GThreadFunc) lw_search_stream_results_thread, 
      (gpointer) self, 
      &error
    );

    if (error != NULL)
    {
      g_warning ("Thread Creation Error: %s\n", error->message);
      g_clear_error (&error);
    }
}


//!
//! @brief Uses a searchitem to cancel a window
//!
//! @param self A LwSearch to gleam information from
//!
void 
lw_search_cancel (LwSearch *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self)) ;

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.progress != NULL) lw_progress_cancel (priv->data.progress);
    lw_search_set_status (self, LW_SEARCHSTATUS_CANCELING);

    if (priv->data.thread != NULL)
    {
      g_thread_join (priv->data.thread);
      priv->data.thread = NULL;
    }

    lw_search_set_status (self, LW_SEARCHSTATUS_IDLE);
}





//!
//! @brief comparison function for determining if two LwSearchs are equal
//! @param item1 The first self
//! @param item2 The second self
//! @returns Returns true when both items are either the same self or have similar innards
//!
gboolean 
lw_search_equal (LwSearch *item1,
                 LwSearch *item2)
{
  //Declarations
  gboolean queries_are_equal = FALSE;
  gboolean dictionaries_are_equal = FALSE;
  LwSearchPrivate *priv1 = NULL;
  LwSearchPrivate *priv2 = NULL;
  gchar *query1 = NULL;
  gchar *query2 = NULL;

  //Sanity checks
  if (item1 == NULL)
  {
    return FALSE;
  }
  if (item2 == NULL)
  {
    return FALSE;
  }

  if (item1 == item2)
  {
    return TRUE;
  }

  priv1 = item1->priv;
  priv2 = item2->priv;

  query1 = priv1->data.query->raw;
  query2 = priv2->data.query->raw;

  //Initializations
  queries_are_equal = (g_strcmp0 (query1, query2) == 0);
  dictionaries_are_equal = (priv1->data.dictionary == priv2->data.dictionary);

  return (queries_are_equal && dictionaries_are_equal);
}



