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

#include "gettext.h"
#include "search.h"

#include "search-private.h"

typedef enum {
    PROP_0,
    PROP_QUERY,
    PROP_FLAGS,
    TOTAL_PROPS
} Props;

MAKE RESULTS PRIVATE, MAKE SEARCH ITERABLE

struct _LwSearchPrivate {
  gchar * query;
  LwQueryNode * query_tree;
  LwList * list;
  LwList * normalized_list;
  LwEditableList * results;

  LwSearchFlag flags;
};

struct _LwSearchClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};


G_DEFINE_TYPE_WITH_PRIVATE (LwSearch, lw_search, LW_TYPE_LIST)


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
               LwSearchFlag   flags,
               LwList       * haystack)
{
    //Sanity checks
    g_return_val_if_fail (QUERY != NULL, NULL);

    //Declarations
    LwSearch *self = NULL;
    
    //Initializations
    self = LW_SEARCH (g_object_new (LW_TYPE_SEARCH, "query", QUERY, "flags", flags, "haystack", haystack, NULL));

    return self;
}


static void
lw_search_init (LwSearch * self)
{
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
    priv = lw_search_get_instance_private (self);

    switch (property_id)
    {
      case PROP_QUERY:
        lw_search_set_query (self, g_value_get_string (value));
        break;
      case PROP_FLAGS:
        lw_search_set_flags (self, g_value_get_flags (value));
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
    priv = lw_search_get_instance_private (self);

    switch (property_id)
    {
      case PROP_QUERY:
        g_value_set_string (value, lw_search_get_query (self));
        break;
      case PROP_FLAGS:
        g_value_set_int (value, lw_search_get_flags (self));
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
    priv = lw_search_get_instance_private (self);

    lw_search_set_query (self, NULL);

    G_OBJECT_CLASS (lw_search_parent_class)->finalize (object);
}


static void
lw_search_class_init (LwSearchClass * klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwSearchClassPrivate, 1);
    object_class->set_property = lw_search_set_property;
    object_class->get_property = lw_search_get_property;
    object_class->finalize = lw_search_finalize;

    LwSearchClassPrivate *klasspriv = klass->priv;

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
    priv = lw_search_get_instance_private (self);
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
    g_return_val_if_fail (LW_IS_SEARCH (self), NULL);

    //Declarations
    LwSearchPrivate *priv = NULL;

    //Initializations
    priv = lw_search_get_instance_private (self);

    return priv->query;
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
    priv = lw_search_get_instance_private (self);

    return priv->flags;
}


static void
lw_search_set_flags (LwSearch     * self,
                     LwSearchFlag   flags)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));

    //Declarations
    LwSearchPrivate *priv = NULL;
    LwSearchClass *klass = NULL;
    LwSearchClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = lw_search_get_instance_private (self);
    klass = LW_SEARCH_GET_CLASS (self);
    klasspriv = klass->priv;
    if (priv->flags == flags) goto errored;

    priv->flags = flags;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_FLAGS]);

errored:

    return;
}


/**
 * lw_search_query_results:
 * @self: A #LwSearch
 * Returns: (transfer full): A #LwResults object that will be filled in with results.  You should unref it with g_object_unref() when finished.
 *
 * Starts a search synchonously, meaning it will lock the UI, making this most appropriate for
 * console programs.
 */
void
lw_search_execute (LwSearch * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCH (self));
    if (error != NULL && *error != NULL) return;

    //Declarations
    LwSearchPrivate * priv = NULL;
    LwSearchFlag flags = 0;
    LwList * normalized = NULL;
    LwIter iter = {0};
    LwResults * results = NULL;
    gboolean match = FALSE;
    LwQuery query = NULL;

    //Initializations
    priv = lw_search_get_instance_private (self);
    progress = priv->progress;
    dictionary = priv->dictionary;
    flags = priv->flags;

    if (priv->results != NULL) return;

    query = lw_query_new_from_string (priv->query, error);
    normalized_haystack = lw_list_normalize (haystack, flags); 
    results = lw_results_new (haystack, normalized_haystack);

    lw_list_get_begin_iter (normalized_haystack, &iter);

    while (!lw_iter_is_end (&iter))
    {
        if (lw_query_match_iter (query_node, &iter, NULL))
        {
            lw_results_append (results, lw_iter_get_position (&iter));
        }

        lw_iter_next(&iter);
    }

    priv->results = results;
    results = NULL;

errored:

    if (results != NULL)
    {
        g_object_unref (results);
        results = NULL;
    }

    if (query != NULL)
    {
        lw_query_unref (query);
        query = NULL;
    }

    if (normalized_haystack != NULL)
    {
        g_object_unref (normalized_haystack);
        normalized_haystack = NULL;
    }
}


/**
 * lw_search_build_utf8flags:
 * @self: A #LwSearch
 * Returns: The utf8 flags the search was constructed with
 */
static LwUtf8Flag
lw_search_build_utf8flags (LwSearch * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SEARCH (self), 0);

    //Declarations
    LwSearchPrivate *priv = NULL;
    LwUtf8Flag flags = 0;

    //Initializations
    priv = lw_search_get_instance_private (self);
    flags = LW_UTF8FLAG_PRINTABLE | LW_UTF8FLAG_COMPARABLE;

    if (priv->flags & LW_SEARCHFLAG_FURIGANA_INSENSITIVE)
      flags |= LW_UTF8FLAG_FURIGANAFOLD;
    if (priv->flags & LW_SEARCHFLAG_CASE_INSENSITIVE)
      flags |= LW_UTF8FLAG_FURIGANAFOLD;

    return flags;
}
