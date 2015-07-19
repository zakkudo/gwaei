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
 * SECTION:search
 * @short_description: Search and search result management
 * @title: LwSearch
 * @include: libwaei/search.h
 *
 * Methods for easily executing searches.  They pair a #LwDictionary
 * against a query string using the prefered normalization configured by
 * #LwUtf8Flags. Searches can easily be started in a new thread or the same
 * thread.  The results are stored in a #GSequence which should be easy
 * to convert to things like #GtkTreeModels or whatever your prefered
 * toolkit is for lists.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/search.h>

#include <libwaei/search-private.h>


G_DEFINE_TYPE (LwSearch, lw_search, LW_TYPE_RESULTS)


/**
 * lw_search_new:
 * @QUERY: The query string as taken from seach field
 * @dictionary: An #LwDictionary to search against
 * @flags: Options for normaliation of the query and dictionary search such as to make the search case-independant
 *
 * Returns: A new #LwSearch that should be freed with g_object_unref()
 */
LwSearch* 
lw_search_new (const gchar  * QUERY,
               LwDictionary * dictionary,
               LwSearchFlag   flags)
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


/**
 * lw_search_new_by_preferences:
 * @QUERY: The query string as taken from seach field
 * @dictionary: An #LwDictionary to search against
 * @preferences: An #LwPreferences object to calculate permissions from
 *
 * A version of lw_search_new() that automatically calculates the #LwUtf8Flags using the user's preferences
 *
 * Returns: A new #LwSearch that should be freed with g_object_unref()
 */
LwSearch* 
lw_search_new_by_preferences (const gchar   *  QUERY,
                              LwDictionary  *  dictionary,
                              LwPreferences *  preferences)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (dictionary), NULL);

    //Declarations
    LwSearch *self = NULL;
    LwSearchFlag flags = 0;

    //Initializations
    flags = lw_search_build_flags_from_preferences (preferences);
    self = lw_search_new (QUERY, dictionary, flags);

    return self;
}


static void
lw_search_init (LwSearch * self)
{
    self->priv = LW_SEARCH_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwSearchPrivate));

    LwSearchPrivate *priv = NULL;

    priv = self->priv;

    g_mutex_init (&priv->mutex);
    priv->status = LW_SEARCHSTATUS_UNSTARTED;
    priv->max_results = -1;
}


static void 
lw_search_set_property (GObject      * object,
                        guint          property_id,
                        const GValue * value,
                        GParamSpec   * pspec)
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
      case PROP_QUERY_TREE:
        lw_search_set_query_tree (self, g_value_get_boxed (value));
        break;
      case PROP_STATUS:
        lw_search_set_status (self, g_value_get_enum (value));
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
lw_search_get_property (GObject    * object,
                        guint        property_id,
                        GValue     * value,
                        GParamSpec * pspec)
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
      case PROP_QUERY_TREE:
        g_value_set_boxed (value, lw_search_get_query_tree (self));
        break;
      case PROP_FLAGS:
        g_value_set_int (value, lw_search_get_flags (self));
        break;
      case PROP_STATUS:
        g_value_set_flags (value, lw_search_get_status (self));
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
lw_search_finalize (GObject * object)
{
    //Declarations
    LwSearch *self = NULL;
    LwSearchPrivate *priv = NULL;

    //Initalizations
    self = LW_SEARCH (object);
    priv = self->priv;

    lw_search_set_dictionary (self, NULL);
    lw_search_set_query (self, NULL);
    lw_search_set_query_tree (self, NULL);

    g_list_free (priv->results_buffer);
    if (priv->dictionary_cache) g_object_unref (priv->dictionary_cache);

    G_OBJECT_CLASS (lw_search_parent_class)->finalize (object);
}


static void
lw_search_dispose (GObject *object)
{
    //Declarations
    LwSearch *self = NULL;
    LwSearchPrivate * priv = NULL;

    //Initializations
    self = LW_SEARCH (object);
    priv = self->priv;

    lw_search_cancel (self);

    G_OBJECT_CLASS (lw_search_parent_class)->dispose (object);
}


static void 
lw_search_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_search_parent_class)->constructed (object);
    }

    //Declarations
    LwSearch *self = NULL;
    LwSearchPrivate *priv = NULL;
    LwProgress * progress = NULL;
    gchar const * QUERY = NULL;
    LwQueryNode * root = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwUtf8Flag flags = LW_UTF8FLAG_NONE;
    GError * error = NULL;
    gboolean has_error = FALSE;

    //Initializations
    self = LW_SEARCH (object);
    priv = self->priv;
    progress = lw_progress_new ();
    if (progress == NULL) goto errored;
    QUERY = lw_search_get_query (self);
    if (QUERY == NULL) goto errored;
    flags = lw_search_build_utf8flags (self);

    lw_search_set_progress (self, progress);

    root = lw_querynode_new_tree_from_string (QUERY, &operation, &error);
    LW_PROGRESS_TAKE_ERROR (progress, error);

    lw_querynode_compile (root, flags, &error);
    LW_PROGRESS_TAKE_ERROR (progress, error);

    if (lw_querynode_walk (root, (LwQueryNodeWalkFunc) _apply_search_columns_to_query, self)) goto errored;
    
errored:

    if (root != NULL) lw_querynode_unref (root);
    root = NULL;

    return;
}


static void
lw_search_class_init (LwSearchClass * klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwSearchClassPrivate, 1);
    object_class->constructed = lw_search_constructed;
    object_class->set_property = lw_search_set_property;
    object_class->get_property = lw_search_get_property;
    object_class->dispose = lw_search_dispose;
    object_class->finalize = lw_search_finalize;

    g_type_class_add_private (object_class, sizeof (LwSearchPrivate));

    LwSearchClassPrivate *klasspriv = klass->priv;

    klasspriv->pspec[PROP_DICTIONARY] = g_param_spec_object (
        "dictionary",
        gettext("FIlename construct prop"),
        gettext("Set the filename"),
        LW_TYPE_DICTIONARY,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    /**
     * LwSearch:dictionary:
     *
     * The dictionary this search is using for fetching results.
     */
    g_object_class_install_property (object_class, PROP_DICTIONARY, klasspriv->pspec[PROP_DICTIONARY]);

    klasspriv->pspec[PROP_QUERY] = g_param_spec_string (
        "query",
        gettext("changed construct prop"),
        gettext("Set the changed"),
        NULL,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    /**
     * LwSearch:query:
     *
     * The search query text that the search was initialized with.
     */
    g_object_class_install_property (object_class, PROP_QUERY, klasspriv->pspec[PROP_QUERY]);

    klasspriv->pspec[PROP_QUERY_TREE] = g_param_spec_boxed (
        "query-tree",
        gettext("Query Tree"),
        gettext("The parsed query tree generated from query"),
        LW_TYPE_QUERYNODE,
        G_PARAM_READABLE
    );
    /**
     * LwSearch:query-tree:
     *
     * The parsed query tree generated from the query
     */
    g_object_class_install_property (object_class, PROP_QUERY_TREE, klasspriv->pspec[PROP_QUERY_TREE]);

    klasspriv->pspec[PROP_FLAGS] = g_param_spec_flags (
        "flags",
        gettext("Flags"),
        gettext("Flags to configure the search"),
        LW_TYPE_SEARCHFLAG,
        0,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    /**
     * LwSearch:flags:
     *
     * Search flags to modify the search
     */
    g_object_class_install_property (object_class, PROP_FLAGS, klasspriv->pspec[PROP_FLAGS]);

    klasspriv->pspec[PROP_STATUS] = g_param_spec_enum (
        "status",
        gettext("Status"),
        gettext("The current search status"),
        LW_TYPE_SEARCHSTATUS,
        LW_SEARCHSTATUS_UNSTARTED,
        G_PARAM_READABLE
    );
    /**
     * LwSearch:status:
     *
     * The current search status.
     */
    g_object_class_install_property (object_class, PROP_STATUS, klasspriv->pspec[PROP_STATUS]);

    klasspriv->pspec[PROP_MAX_RESULTS] = g_param_spec_int (
        "max-results",
        gettext("loaded construct prop"),
        gettext("Set the loaded"),
        0,
        G_MAXINT,
        0,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    /**
     * LwSearch:max-results:
     *
     * The max number of results to return
     */
    g_object_class_install_property (object_class, PROP_MAX_RESULTS, klasspriv->pspec[PROP_MAX_RESULTS]);

    klasspriv->pspec[PROP_PROGRESS] = g_param_spec_object (
        "progress",
        gettext("loaded construct prop"),
        gettext("Set the loaded"),
        LW_TYPE_PROGRESS,
        G_PARAM_READABLE
    );

    /**
     * LwSearch:progress
     *
     * Internal #LwProgress for tracking query progress
     */
    g_object_class_install_property (object_class, PROP_PROGRESS, klasspriv->pspec[PROP_PROGRESS]);
}


static void
lw_search_set_query (LwSearch    * self,
                     const gchar * QUERY)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;
    LwSearchClass *klass = NULL;
    gchar * query = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_SEARCH_GET_CLASS (self);
    if (g_strcmp0 (QUERY, priv->query) == 0) goto errored;
    query = g_strdup (QUERY);
    if (query == NULL && QUERY != NULL) goto errored;

    g_free (priv->query);
    priv->query = query;
    query = NULL;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_QUERY]);

errored:

    g_free (query);
    query = NULL;

    return;
}


/**
 * lw_search_get_query:
 * @self: A #LwSearch
 * Returns: The query string that the #LwSearch was inititalized with
 */
const gchar *
lw_search_get_query (LwSearch * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->query;
}


static void
lw_search_set_query_tree (LwSearch    * self,
                          LwQueryNode * query_tree)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate * priv = NULL;    
    LwSearchClass * klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_SEARCH_GET_CLASS (self);
    if (priv->query_tree == query_tree) goto errored;

    lw_querynode_ref (query_tree);
    lw_querynode_unref (priv->query_tree);
    priv->query_tree = query_tree;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_QUERY_TREE]);

errored:

    return;
}


/**
 * lw_search_get_query_tree:
 * @self: A #LwSearch
 * Returns: (transfer none): The query tree generated on initialization
 */
LwQueryNode *
lw_search_get_query_tree (LwSearch * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate * priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->query_tree;
}


/**
 * lw_search_get_dictionary:
 * @self: A #LwSearch
 * Returns: (transfer none): The dictionary set on initialization
 */
LwDictionary*
lw_search_get_dictionary (LwSearch *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->dictionary;
}


static void
lw_search_set_dictionary (LwSearch     * self,
                          LwDictionary * dictionary)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));
    g_return_if_fail (dictionary != NULL && LW_IS_DICTIONARY (dictionary));

    //Declarations
    LwSearchPrivate * priv = NULL;
    LwSearchClass * klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_SEARCH_GET_CLASS (self);
    if (priv->dictionary == dictionary) goto errored;

    if (dictionary != NULL) g_object_ref (dictionary);
    if (priv->dictionary != NULL) g_object_unref (priv->dictionary);

    priv->dictionary = dictionary;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_DICTIONARY]);

errored:

    return;
}


/**
 * lw_search_get_progress:
 * @self: A #LwSearch
 * Returns: (no transfer): The internal #LwProgress
 */
LwProgress*
lw_search_get_progress (LwSearch * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->progress;
}


static void
lw_search_set_progress (LwSearch   * self,
                        LwProgress * progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));
    g_return_if_fail (progress != NULL && LW_IS_PROGRESS (progress));

    //Declarations
    LwSearchPrivate *priv = NULL;
    LwSearchClass * klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_SEARCH_GET_CLASS (self);
    if (priv->progress == progress) goto errored;

    if (progress != NULL) g_object_ref (progress);
    if (priv->progress != NULL) g_object_unref (priv->progress);

    priv->progress = progress;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_DICTIONARY]);

errored:

    return;
}


void
lw_search_set_max_results (LwSearch * self,
                           gint       max_results)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;
    LwSearchClass *klass = NULL;
    LwSearchClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_SEARCH_GET_CLASS (self);
    klasspriv = klass->priv;
    changed = (priv->max_results != max_results);

    priv->max_results = max_results;

errored:

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_MAX_RESULTS]);
}


gint
lw_search_get_max_results (LwSearch * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->max_results;
}


static void
lw_search_set_status (LwSearch       * self, 
                      LwSearchStatus   status)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));
    if (status < 0) return;

    //Declarations
    LwSearchPrivate *priv = NULL;
    LwSearchClass *klass = NULL;
    LwSearchClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed =  (priv->status != status);

    priv->status = status;

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_STATUS]);
}


/**
 * lw_searchstatus_get_type:
 * Returns: The #GType of the flags
 */
GType
lw_searchstatus_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      static GEnumValue values[] = {
        { LW_SEARCHSTATUS_UNSTARTED, LW_SEARCHSTATUSNAME_UNSTARTED, LW_SEARCHSTATUSNICK_UNSTARTED },
        { LW_SEARCHSTATUS_SEARCHING, LW_SEARCHSTATUSNAME_SEARCHING, LW_SEARCHSTATUSNICK_SEARCHING },
        { LW_SEARCHSTATUS_CANCELING, LW_SEARCHSTATUSNAME_CANCELING, LW_SEARCHSTATUSNICK_CANCELING },
        { LW_SEARCHSTATUS_CANCELLED, LW_SEARCHSTATUSNAME_CANCELLED, LW_SEARCHSTATUSNICK_CANCELLED },
        { LW_SEARCHSTATUS_FINISHED,  LW_SEARCHSTATUSNAME_FINISHED,  LW_SEARCHSTATUSNICK_FINISHED },
        { 0, NULL, NULL },
      };

      type = g_enum_register_static ("LwSearchStatus", values);
    }

    return type;
}


/**
 * lw_search_get_status:
 * @self: A #LwSearch
 * Returns: The status of the search
 */
LwSearchStatus
lw_search_get_status (LwSearch * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;
    LwSearchStatus status = 0;

    //Initializations
    priv = self->priv;
    status = priv->status;

    return status;
}


GType
lw_searchflag_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      static GFlagsValue values[] = {
        { LW_SEARCHFLAG_RAW, LW_SEARCHFLAGNAME_RAW, LW_SEARCHFLAGNICK_RAW },
        { LW_SEARCHFLAG_FURIGANA_INSENSITIVE, LW_SEARCHFLAGNAME_FURIGANA_INSENSITIVE, LW_SEARCHFLAGNICK_FURIGANA_INSENSITIVE },
        { LW_SEARCHFLAG_CASE_INSENSITIVE, LW_SEARCHFLAGNAME_CASE_INSENSITIVE, LW_SEARCHFLAGNICK_CASE_INSENSITIVE },
        { LW_SEARCHFLAG_STEM_INSENSITIVE, LW_SEARCHFLAGNAME_STEM_INSENSITIVE, LW_SEARCHFLAGNICK_STEM_INSENSITIVE },
        { LW_SEARCHFLAG_ROMAJI_TO_FURIGANA, LW_SEARCHFLAGNAME_ROMAJI_TO_FURIGANA, LW_SEARCHFLAGNICK_ROMAJI_TO_FURIGANA },
        { LW_SEARCHFLAG_USE_INDEX, LW_SEARCHFLAGNAME_USE_INDEX, LW_SEARCHFLAGNICK_USE_INDEX },
        { LW_SEARCHFLAG_INSENSITIVE, LW_SEARCHFLAGNAME_INSENSITIVE, LW_SEARCHFLAGNICK_INSENSITIVE },
        { 0, NULL, NULL },
      };

      type = g_flags_register_static ("LwSearchFlag", values);
    }

    return type;
}


/**
 * lw_search_get_flags:
 * @self: A #LwSearch
 * Returns: The flags set on initialization
 */
LwSearchFlag
lw_search_get_flags (LwSearch * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SEARCH (self), 0);

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->flags;
}


static void
lw_search_set_flags (LwSearch     * self,
                     LwSearchFlag   flags)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SEARCH (self), 0);

    //Declarations
    LwSearchPrivate *priv = NULL;
    LwSearchClass *klass = NULL;
    LwSearchClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_SEARCH_GET_CLASS (self);
    klasspriv = klass->priv;
    if (priv->flags == flags) goto errored;

    priv->flags = flags;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_FLAGS]);

errored:

    return;
}


/**
 * lw_search_build_flags_from_preferences:
 * @self: A #LwPreferences
 * Returns: Builds the search flags based on the currently set preferences
 */
LwSearchFlag
lw_search_build_flags_from_preferences (LwPreferences * preferences)
{
    //Sanity checks
    g_return_val_if_fail (preferences != NULL, 0);

    //Declarations
    gboolean furigana_insensitive = FALSE;
    gboolean case_insensitive = FALSE;
    gboolean stem_insensitive = FALSE;
    gint romaji_to_furigana = 0;
    gboolean want_romaji_to_furigana_conv = FALSE;
    gboolean index_results = FALSE;
    gint32 flags = 0;

    //Initializations
    furigana_insensitive = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_FURIGANA_INSENSITIVE);
    case_insensitive = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_CASE_INSENSITIVE);
    stem_insensitive = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_STEM_INSENSITIVE);
    romaji_to_furigana = lw_preferences_get_int_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_ROMAJI_TO_FURIGANA);
    //want_romaji_to_furigana_conv = (romaji_to_furigana == 0 || (romaji_to_furigana == 2 && !lw_util_is_japanese_locale()));
    want_romaji_to_furigana_conv = (romaji_to_furigana == 0);
    index_results = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_INDEX_RESULTS);
    flags = LW_SEARCHFLAG_RAW;

    if (furigana_insensitive) flags |= LW_SEARCHFLAG_FURIGANA_INSENSITIVE;
    if (case_insensitive) flags |= LW_SEARCHFLAG_CASE_INSENSITIVE;
    if (stem_insensitive) flags |= LW_SEARCHFLAG_STEM_INSENSITIVE;
    if (want_romaji_to_furigana_conv) flags |= LW_SEARCHFLAG_ROMAJI_TO_FURIGANA;
    if (index_results) flags |= LW_SEARCHFLAG_USE_INDEX;

    return flags;
}


/**
 * lw_search_watch_timeout:
 * @self: A #LwSearch
 *
 * An idle method made to cleanup the search thread in a thread-safe way
 * when it completes.
 */
static gboolean
lw_search_watch_timeout (LwSearch * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SEARCH (self), FALSE);

    //Declarations
    LwSearchPrivate * priv = NULL;
    LwProgress * progress = NULL;
    gboolean is_completed = FALSE;

    //Initializations
    priv = self->priv;
    progress = priv->progress;
    is_completed = lw_progress_completed (progress);

    if (is_completed)
    {
      if (priv->status = LW_SEARCHSTATUS_FINISHED)
      {
        GList * link = priv->results_buffer;
        LwResults * results = LW_RESULTS (self);
        while (link != NULL)
        {
          LwParsedLine * line = LW_PARSEDLINE (link->data);
          if (line != NULL) lw_results_append_line (results, line);
          link = link->next;
        }
      }
      g_list_free (priv->results_buffer);
      priv->results_buffer = NULL;

      priv->watch_id = 0;
    }

    return !is_completed;
}


static gpointer 
lw_search_stream_results_thread (LwSearch * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SEARCH (self), NULL);

    //Declarations
    LwSearchPrivate *priv = NULL;
    LwProgress *progress = NULL;
    LwDictionary *dictionary = NULL;
    LwSearchFlag flags = 0;
    LwParsed * parsed = NULL;
    LwDictionaryCache * cache = NULL;

    //Initializations
    priv = self->priv;
    progress = priv->progress;
    dictionary = priv->dictionary;
    flags = priv->flags;

    cache = priv->dictionary_cache = lw_search_ensure_dictionarycache (self);
    if (cache == NULL) goto errored;
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);
    parsed = lw_dictionarycache_get_parsed (cache);
    if (parsed == NULL) goto errored;

    lw_search_query_parsed (self, parsed);

errored:

    if (lw_progress_errored (progress))
    {
      g_list_free (priv->results_buffer);
      g_mutex_lock (&priv->mutex);
      priv->status = LW_SEARCHSTATUS_ERRORED;
      g_mutex_unlock (&priv->mutex);
    }
    else if (LW_SEARCHSTATUS_CANCELING)
    {
      g_list_free (priv->results_buffer);
      g_mutex_lock (&priv->mutex);
      priv->status = LW_SEARCHSTATUS_CANCELLED;
      g_mutex_unlock (&priv->mutex);
    }
    else
    {
      g_mutex_lock (&priv->mutex);
      priv->status = LW_SEARCHSTATUS_FINISHED;
      g_mutex_unlock (&priv->mutex);
    }

    priv->thread = NULL;

    lw_progress_set_completed (progress, TRUE);

    return NULL;
}


static gint *
_calculate_columns_by_name (LwQueryNode  * self,
                            LwDictionary * dictionary)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (LW_IS_DICTIONARY (dictionary), NULL);

    //Declarations
    GFlagsValue * value = NULL;
    GType type = G_TYPE_NONE;
    GFlagsClass * klass = NULL;
    gint column = -1;
    gint * columns = NULL;

    //Initializations
    type = lw_dictionary_columnid_get_type (dictionary);
    klass = G_FLAGS_CLASS (G_OBJECT_GET_CLASS (type));
    value = g_flags_get_value_by_name (klass, self->key);
    if (value == NULL) goto errored;
    column = value->value;
    columns = g_new0 (gint, 2);
    if (columns == NULL) goto errored;
    columns[0] = column;
    columns[1] = -1;

errored:

    return columns;
}

static gboolean
_apply_search_columns_to_query (LwQueryNode  * self,
                                LwSearch     * search)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, TRUE);
    g_return_val_if_fail (LW_IS_SEARCH (search), TRUE);

    //Declarations
    gboolean should_stop = TRUE;
    gint * columns = NULL;
    gchar const * QUERY = NULL;
    LwDictionary * dictionary = NULL;

    //Initializations
    QUERY = lw_search_get_query (search);
    if (QUERY == NULL) goto errored;
    dictionary = lw_search_get_dictionary (search);
    if (dictionary == NULL) goto errored;
    columns = _calculate_columns_by_name (self, dictionary);
    if (columns == NULL)
    {
      columns = lw_dictionary_calculate_applicable_columns_for_text (dictionary, QUERY);
    }
    if (columns == NULL) goto errored;

    g_free (self->columns);
    self->columns = columns;
    columns = NULL;

    should_stop = FALSE;

errored:

    g_free (columns);
    columns = NULL;

    return should_stop;
}


/**
 * lw_search_query_results:
 * @self: A #LwSearch
 * Returns: (transfer full): A #LwResults object that will be filled in with results.  You should unref it with g_object_unref() when finished.
 *
 * Starts a search synchonously, meaning it will lock the UI, making this most appropriate for
 * console programs.
 */
LwResults *
lw_search_query_results (LwSearch * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->mutex);
    if (priv->status != LW_SEARCHSTATUS_UNSTARTED)
    {
      g_mutex_unlock (&priv->mutex);
      goto errored;
    }
    priv->status = LW_SEARCHSTATUS_SEARCHING;
    g_mutex_unlock (&priv->mutex);

    priv->watch_id = g_timeout_add (100, (GSourceFunc) lw_search_watch_timeout, self);

    lw_search_stream_results_thread (self);

errored:

    return LW_RESULTS (self);
}


/**
 * lw_search_query_results_async:
 * @self: A #LwSearch
 * @error: A #GError or %NULL
 * Returns: (transfer full): A #LwResults object that will be filled in with results.  You should unref it with g_object_unref() when finished.
 *
 * Starts an asynchronous search.  You an monitor it's progress or cancel it by
 * fetching the #LwProgress object using lw_search_get_progress(). If a search 
 * is running or has already been completed, calling this method is a no-op. This
 * method should only be called from the main thread of the application.
 */
LwResults *
lw_search_query_results_async (LwSearch *  self,
                               GError   ** error)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->mutex);
    if (priv->status != LW_SEARCHSTATUS_UNSTARTED)
    {
      g_mutex_unlock (&priv->mutex);
      goto errored;
    }
    priv->status = LW_SEARCHSTATUS_SEARCHING;
    g_mutex_unlock (&priv->mutex);

    priv->watch_id = g_timeout_add (100, (GSourceFunc) lw_search_watch_timeout, self);

    priv->thread = g_thread_try_new (
      "libwaei-search-thread",
      (GThreadFunc) lw_search_stream_results_thread, 
      (gpointer) self, 
      error
    );
    if (error != NULL && *error != NULL) goto errored;

errored:

    return LW_RESULTS (self);
}


/**
 * lw_search_cancel:
 * @self: A #LwSearch
 *
 * Use to cancel a search in a thread-safe way.  This method will for
 * the program to wait for the thread to join before allowing continuation
 * of executation.  Once the thread is stopped, the #LwSearch will have its
 * status set back to %LW_SEARCHSTATUS_UNSTARTED.  This method should only
 * be called from the main thread of the application.
 */
void 
lw_search_cancel (LwSearch * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self)) ;

    //Declarations
    GThread * thread = NULL;
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    thread = priv->thread;

    if (priv->watch_id != 0) g_source_remove (priv->watch_id);

    g_mutex_lock (&priv->mutex);
    priv->status = LW_SEARCHSTATUS_CANCELING;
    g_mutex_unlock (&priv->mutex);

    if (priv->progress != NULL) lw_progress_cancel (priv->progress);

    if (thread != NULL)
    {
      priv->thread = NULL;
      g_thread_join (thread);
    }
}


/**
 * lw_search_build_utf8flags:
 * @self: A #LwSearch
 * Returns: The utf8 flags the search was constructed with
 */
LwUtf8Flag
lw_search_build_utf8flags (LwSearch * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SEARCH (self), 0);

    //Declarations
    LwSearchPrivate *priv = NULL;
    LwUtf8Flag flags = 0;

    //Initializations
    priv = self->priv;
    flags = LW_UTF8FLAG_PRINTABLE | LW_UTF8FLAG_COMPARABLE;

    if (priv->flags & LW_SEARCHFLAG_FURIGANA_INSENSITIVE)
      flags |= LW_UTF8FLAG_FURIGANAFOLD;
    if (priv->flags & LW_SEARCHFLAG_CASE_INSENSITIVE)
      flags |= LW_UTF8FLAG_FURIGANAFOLD;

    return flags;
}


static gboolean
_query_parsed_line (LwParsed     * parsed,
                     LwParsedLine * line,
                     LwSearch     * search)
{
    //Declarations
    LwSearchPrivate * priv = NULL;
    LwQueryNode * query_tree = NULL;
    LwProgress * progress = NULL;
    gdouble current = 0.0;
    
    //Initializations
    priv = search->priv;

    query_tree = priv->query_tree;
    if (query_tree == NULL) goto errored;

    progress = priv->progress;
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);

    if (lw_querynode_match_parsedline (query_tree, line, NULL)) {
      g_mutex_lock (&priv->mutex);
      priv->results_buffer = g_list_prepend (priv->results_buffer, line);
      priv->total_results++;
      g_mutex_unlock (&priv->mutex);
    }

    if (progress != NULL)
    {
      current = lw_progress_get_current (progress);
      lw_progress_set_current (progress, current + 1);
    }

errored:

    return FALSE;
}


static LwDictionaryCache *
lw_search_ensure_dictionarycache (LwSearch * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SEARCH (self), NULL);

    //Declarations
    LwUtf8Flag flags = 0;
    LwDictionary * dictionary = NULL;
    LwDictionaryCache *cache = NULL;
    LwProgress * progress = NULL;

    //Initializations
    flags = lw_search_build_utf8flags (self);
    progress = lw_search_get_progress (self);
    if (progress == NULL) goto errored;
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);
    dictionary = lw_search_get_dictionary (self);
    if (dictionary == NULL) goto errored;
    cache = lw_dictionary_ensure_cache_by_utf8flags (dictionary, flags, progress);
    if (cache == NULL) goto errored;
    LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT (progress);

errored:

    return cache;
}


static void
lw_search_query_parsed (LwSearch * self,
                        LwParsed * parsed)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));
    g_return_if_fail (parsed != NULL);

    //Declarations
    LwProgress * progress = NULL;
    LwDictionary * dictionary = NULL;
    gchar const * DICTIONARY_NAME = NULL;
    gdouble num_lines = 0.0;

    //Initializations
    progress = lw_search_get_progress (self);
    if (progress == NULL) goto errored;
    dictionary = lw_search_get_dictionary (self);
    if (dictionary == NULL) goto errored;
    DICTIONARY_NAME = lw_dictionary_get_name (dictionary);
    if (DICTIONARY_NAME == NULL) goto errored;
    num_lines = lw_parsed_num_lines (parsed);

    lw_progress_set_primary_message_printf (progress, "Searching %d dictionary...", DICTIONARY_NAME);
    lw_progress_set_total (progress, num_lines);
    lw_progress_set_current (progress, 0.0);

    lw_parsed_foreach (parsed, (LwParsedForeachFunc) _query_parsed_line, self);

errored:

    return;
}
