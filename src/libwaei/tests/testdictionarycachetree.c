#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>
#include <libwaei/dictionary.h>
#include <libwaei/dictionarycachetree.h>

#include "testdictionary.h"

struct _Fixture {
   LwDictionaryCacheTree * cachetree;
   LwDictionaryCache * cache_with_one_reference;
   LwDictionaryCache * cache_with_two_references;
};
typedef struct _Fixture Fixture;


void setup (Fixture *fixture, gconstpointer data)
{
    fixture->cachetree = lw_dictionarycachetree_new ();

    fixture->cache_with_one_reference = lw_dictionarycache_new ("test1", LW_TYPE_TESTDICTIONARY, 0);
    fixture->cache_with_two_references = lw_dictionarycache_new ("test2", LW_TYPE_TESTDICTIONARY, 0);
    g_object_ref (fixture->cache_with_two_references);
}


void teardown (Fixture *fixture, gconstpointer data)
{
    lw_dictionarycachetree_unref (fixture->cachetree);

    if (fixture->cache_with_one_reference)
    {
      g_object_unref (fixture->cache_with_one_reference);
    }

    if (fixture->cache_with_two_references != NULL)
    {
      g_object_unref (fixture->cache_with_two_references);
      g_object_unref (fixture->cache_with_two_references);
    }
}


void
new_empty_cachetree (Fixture *fixture, gconstpointer data)
{
}


void
insert_cache_that_has_only_one_ref (Fixture *fixture, gconstpointer data)
{
    LwDictionaryCache * cache = fixture->cache_with_one_reference;
    lw_dictionarycachetree_insert (fixture->cachetree, cache);

    g_assert_cmpint (G_OBJECT (cache)->ref_count, ==, 1);
}


void
insert_cache_that_has_two_refs (Fixture *fixture, gconstpointer data)
{
    LwDictionaryCache * cache = fixture->cache_with_two_references;
    lw_dictionarycachetree_insert (fixture->cachetree, cache);

    g_assert_cmpint (G_OBJECT (cache)->ref_count, ==, 2);
}


void
insert_cache_that_is_freed_early (Fixture *fixture, gconstpointer data)
{
    LwDictionaryCache * cache = fixture->cache_with_one_reference;
    lw_dictionarycachetree_insert (fixture->cachetree, cache);

    g_assert_cmpint (G_OBJECT (cache)->ref_count, ==, 1);

    g_object_unref (cache);
    fixture->cache_with_one_reference = NULL;
}


/*
LwDictionaryCache * lw_dictionarycachetree_lookup_by_utf8flags (LwDictionaryCacheTree * self, LwUtf8Flag flags);
void lw_dictionarycachetree_remove (LwDictionaryCacheTree * self, LwDictionaryCache * cache);
void lw_dictionarycachetree_remove_by_utf8flags (LwDictionaryCacheTree * self, LwUtf8Flag flags);
*/

void
remove_existing_cache (Fixture * fixture, gconstpointer data)
{
    LwDictionaryCache * cache = fixture->cache_with_one_reference;
    lw_dictionarycachetree_insert (fixture->cachetree, cache);

    g_assert_cmpint (G_OBJECT (cache)->ref_count, ==, 1);

    lw_dictionarycachetree_remove (fixture->cachetree, cache);

    g_assert_cmpint (G_OBJECT (cache)->ref_count, ==, 1);
}

void
remove_cache_twice (Fixture * fixture, gconstpointer data)
{
    LwDictionaryCache * cache = fixture->cache_with_one_reference;
    lw_dictionarycachetree_insert (fixture->cachetree, cache);

    g_assert_cmpint (G_OBJECT (cache)->ref_count, ==, 1);

    lw_dictionarycachetree_remove (fixture->cachetree, cache);
    lw_dictionarycachetree_remove (fixture->cachetree, cache);

    g_assert_cmpint (G_OBJECT (cache)->ref_count, ==, 1);
}


void
remove_by_utf8flags (Fixture * fixture, gconstpointer data)
{
    LwDictionaryCache * cache = fixture->cache_with_one_reference;
    lw_dictionarycachetree_insert (fixture->cachetree, cache);
    LwUtf8Flag flags = lw_dictionarycache_get_flags (cache);

    g_assert_cmpint (G_OBJECT (cache)->ref_count, ==, 1);

    lw_dictionarycachetree_remove_by_utf8flags (fixture->cachetree, flags);

    g_assert_cmpint (G_OBJECT (cache)->ref_count, ==, 1);
}

void
remove_by_utf8flags_twice (Fixture * fixture, gconstpointer data)
{
    LwDictionaryCache * cache = fixture->cache_with_one_reference;
    lw_dictionarycachetree_insert (fixture->cachetree, cache);
    LwUtf8Flag flags = lw_dictionarycache_get_flags (cache);

    g_assert_cmpint (G_OBJECT (cache)->ref_count, ==, 1);

    lw_dictionarycachetree_remove_by_utf8flags (fixture->cachetree, flags);
    lw_dictionarycachetree_remove_by_utf8flags (fixture->cachetree, flags);

    g_assert_cmpint (G_OBJECT (cache)->ref_count, ==, 1);
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/new/empty_cachetree", Fixture, NULL, setup, new_empty_cachetree, teardown);

    g_test_add ("/insert/cache_that_only_has_one_ref", Fixture, NULL, setup, insert_cache_that_has_only_one_ref, teardown);
    g_test_add ("/insert/cache_that_only_two_refs", Fixture, NULL, setup, insert_cache_that_has_two_refs, teardown);
    g_test_add ("/insert/cache_that_is_freed_early", Fixture, NULL, setup, insert_cache_that_is_freed_early, teardown);

    g_test_add ("/remove/existing_cache", Fixture, NULL, setup, remove_existing_cache, teardown);
    g_test_add ("/remove/cache_twice", Fixture, NULL, setup, remove_cache_twice, teardown);

    g_test_add ("/remove/by_utf8flags", Fixture, NULL, setup, remove_by_utf8flags_twice, teardown);
    g_test_add ("/remove/by_utf8flags_twice", Fixture, NULL, setup, remove_by_utf8flags_twice, teardown);

    return g_test_run();
}



