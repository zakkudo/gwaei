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
 * SECTION:dictionarycachetree
 * @short_description: A queryable cache of #LwDictionaryCaches indexed by their #LwUtf8Flag normalization flags
 * @title: LwDictionaryCacheTree
 * 
 * Stores normalized cache data indexed by the #LwUtf8Flag of the #LwDictionaryCache.  The tree holds a weak
 * reference to the cache, automatically removing the cache from itself if the #LwDictionaryCache is destroyed.
 * When you want to guarantee the #LwDictionaryCache stays valid for as long as you need it, you need make sure to
 * g_object_ref() the cache returned by lw_dictionarycachetree_lookup().
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include <stdlib.h>

#include <glib.h>

#include <libwaei/gettext.h>

#include <libwaei/dictionarycache.h>
#include <libwaei/dictionarycachetree.h>


struct _LwDictionaryCacheTreeCleanupData {
  LwDictionaryCacheTree * tree;
  LwUtf8Flag key;
};
typedef struct _LwDictionaryCacheTreeCleanupData LwDictionaryCacheTreeCleanupData;

static void _cleanup_cache (LwDictionaryCacheTreeCleanupData * data, GObject * where_the_object_was);

static gint
_direct_compare_function (gconstpointer a,
                          gconstpointer b)
{
  return b - a;
}


static void
lw_dictionarycachetree_remove_internal (LwDictionaryCacheTree * self,
                                        LwDictionaryCache     * cache,
                                        gboolean                weak_unref)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (LW_IS_DICTIONARYCACHE (cache));

    //Declarations
    LwDictionaryCacheTreeCleanupData * data = NULL;

    //Initializations
    data = g_tree_lookup (self->cleanup_data, cache);
    if (data == NULL) goto errored;

    if (weak_unref)
    {
      g_object_weak_unref (G_OBJECT (cache), (GWeakNotify) _cleanup_cache, data);
    }

    g_tree_remove (self->data, GINT_TO_POINTER (data->key));
    g_tree_remove (self->cleanup_data, (gpointer) cache);

errored:

    return;
}


static void
lw_dictionarycachetree_remove_by_utf8flags_internal (LwDictionaryCacheTree * self,
                                                     LwUtf8Flag              flags,
                                                     gboolean                weak_unref)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LwDictionaryCache * cache = NULL;

    //Initializations
    cache = lw_dictionarycachetree_lookup_by_utf8flags (self, flags);
    if (cache == NULL) goto errored;
  
    lw_dictionarycachetree_remove_internal (self, cache, weak_unref);

errored:

    return;
}


static void
_cleanup_cache (LwDictionaryCacheTreeCleanupData * data, 
                GObject                          * where_the_object_was)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    lw_dictionarycachetree_remove_by_utf8flags_internal (data->tree, data->key, FALSE);
}


/**
 * lw_dictionarycachetree_new:
 * Returns: (transfer full): A new #LwDictionarCacheTree that should be freed with lw_dictionarycachetree_unref()
 */
LwDictionaryCacheTree *
lw_dictionarycachetree_new ()
{
    //Declarations
    LwDictionaryCacheTree * self = NULL;
    GTree * data = NULL;
    GTree * cleanup_data = NULL;

    //Initializations
    data = g_tree_new_full ((GCompareDataFunc) _direct_compare_function, NULL, NULL, NULL);
    if (data == NULL) goto errored;
    cleanup_data = g_tree_new_full ((GCompareDataFunc) _direct_compare_function, NULL, NULL, (GDestroyNotify) g_free);
    if (cleanup_data == NULL) goto errored;
    self = g_new0 (LwDictionaryCacheTree, 1);
    if (self == NULL) goto errored;

    self->data = data;
    self->cleanup_data = cleanup_data;
    self->refs = 1;

    data = NULL;
    cleanup_data = NULL;

errored:

    if (data != NULL) g_tree_unref (data); data = NULL;
    if (cleanup_data != NULL) g_tree_unref (cleanup_data); cleanup_data = NULL;

    return self;
}


/**
 * lw_dictionarycachetree_get_type:
 * Returns: the boxed type of the #LwDictionaryCacheTree
 */
GType
lw_dictionarycachetree_get_type ()
{
    static GType type = 0;

    if (type == 0)
    {
      type = g_boxed_type_register_static (
        "LwDictionaryCacheTree",
        (GBoxedCopyFunc) lw_dictionarycachetree_ref,
        (GBoxedFreeFunc) lw_dictionarycachetree_unref
      );
    }

    return type;
}

gboolean
_orphan (LwDictionaryCache                * cache,
         LwDictionaryCacheTreeCleanupData * data,
         LwDictionaryCacheTree            * tree)
{
    g_object_weak_unref (G_OBJECT (cache), (GWeakNotify)_cleanup_cache, data);
    return FALSE;
}

static void
lw_dictionarycachetree_free (LwDictionaryCacheTree * self)
{
    //Sanity checks
    if (self == NULL) return;

    g_tree_foreach (self->cleanup_data, (GTraverseFunc) _orphan, self);

    if (self->data != NULL) g_tree_unref (self->data);
    if (self->cleanup_data != NULL) g_tree_unref (self->cleanup_data);

    memset(self, 0, sizeof(LwDictionaryCacheTree));

    g_free (self);
}


/**
 * lw_dictionarycachetree_ref:
 * 
 * Increases the reference count on an object.
 *
 * Returns: The #LwDictionaryCacheTree with its ref count incremented
 */
LwDictionaryCacheTree *
lw_dictionarycachetree_ref (LwDictionaryCacheTree * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}


/**
 * lw_dictionarycachetree_unref:
 * @self: A #LwDictionaryCacheTree
 *
 * Reduces the ref count on the object and frees it once it reaches 0
 */
void
lw_dictionarycachetree_unref (LwDictionaryCacheTree * self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_dictionarycachetree_free (self);
    }
}


/**
 * lw_dictionarycachetree_insert:
 * @self: A #LwDictionaryCacheTree
 * @cache: (transfer none): A #LwDictionaryCache to store in the tree
 */
void
lw_dictionarycachetree_insert (LwDictionaryCacheTree * self,
                               LwDictionaryCache     * cache)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (LW_IS_DICTIONARYCACHE (cache));

    //Declarations
    LwDictionaryCacheTreeCleanupData * data;

    //Initializations
    data = g_new0 (LwDictionaryCacheTreeCleanupData, 1);
    data->tree = self;
    data->key = lw_dictionarycache_get_flags (cache);

    //Make sure any existing cache with the same key is removed
    lw_dictionarycachetree_remove_by_utf8flags (self, data->key);

    //Insert the new cache
    g_tree_insert (self->data, GINT_TO_POINTER (data->key), cache);
    g_tree_insert (self->cleanup_data, cache, data);
    g_object_weak_ref (G_OBJECT (cache), (GWeakNotify)_cleanup_cache, data);
}


/**
 * lw_dictionarycachetree_remove:
 * @self: A #LwDictionaryCacheTree
 * @cache: (transfer none): The #LwDictionaryCache to remove if it exists.  Otherwise, this method is a no-op.
 */
void
lw_dictionarycachetree_remove (LwDictionaryCacheTree * self,
                               LwDictionaryCache     * cache)
{
    lw_dictionarycachetree_remove_internal (self, cache, TRUE);
}


/**
 * lw_dictionarycachetree_remove_by_utf8flags:
 * @self: A LwDictionaryCacheTree
 * @flags: The #LwUtf8Flags to lookup the #LwDictionaryCache by
 *
 * A convenience method for lw_dictionarycachetree_remove(), performing the
 * lw_dictionarycachetree_lookup_by_utf8flags() lookup for you.
 */
void
lw_dictionarycachetree_remove_by_utf8flags (LwDictionaryCacheTree * self,
                                            LwUtf8Flag              flags)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LwDictionaryCache * cache = NULL;

    //Initializations
    cache = lw_dictionarycachetree_lookup_by_utf8flags (self, flags);
    if (cache == NULL) goto errored;
  
    lw_dictionarycachetree_remove_internal (self, cache, TRUE);

errored:

    return;
}


/**
 * lw_dictionarycachetree_lookup_by_utf8flags:
 * @self: A #LwDictionaryCacheTree
 * @flags: The #LwUtf8Flags flags to lookup the #LwDictionaryCache by
 * Returns: A #LwDictionaryCache or %NULL if nothing was found
 */
LwDictionaryCache *
lw_dictionarycachetree_lookup_by_utf8flags (LwDictionaryCacheTree * self,
                                            LwUtf8Flag              flags)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    LwDictionaryCache * cache = NULL;


    cache = g_tree_lookup (self->data, GINT_TO_POINTER (flags));

    return cache;
}
