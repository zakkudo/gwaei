#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>

struct _Fixture { GHashTable * children; };
typedef struct _Fixture Fixture;

void
_set_children (struct _Fixture  * fixture,
               GList           ** children,
               LwQueryNode      * nodes,
               gint               num_nodes)
{
    while (num_nodes-- > 0)
    {
      *children = g_list_prepend (*children, &nodes[num_nodes]);
    }

    if (!g_hash_table_contains (fixture->children, children))
    {
      g_hash_table_add (fixture->children, *children);
    }
}


void setup (Fixture *fixture, gconstpointer data)
{
    fixture->children = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, (GDestroyNotify) g_list_free);
}


void teardown (Fixture *fixture, gconstpointer data)
{
    g_hash_table_unref (fixture->children);
    fixture->children = NULL;
}


void
parse_string_with_no_parenthesis (Fixture       * fixture,
                                  gconstpointer   data)
{
    //Declarations
    LwQueryNode * root = NULL;
    gchar const * TEXT = "test english search";

    //Initializations
    root = lw_querynode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = "test english search",
      .children = NULL,
      .refs = 1,
    }; 

    lw_querynode_assert_equals (root, &expected_root);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_with_only_parenthesis (Fixture       * fixture,
                                    gconstpointer   data)
{
    //Declarations
    LwQueryNode * root = NULL;
    gchar const * TEXT = "(test english search)";

    //Initializations
    root = lw_querynode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = "(test english search)",
      .children = NULL,
      .refs = 1,
    }; 

    lw_querynode_assert_equals (root, &expected_root);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_ends_with_parenthesis (Fixture       * fixture,
                                    gconstpointer   data)
{
    //Declarations
    LwQueryNode * root = NULL;
    gchar const * TEXT = "test english(search)";

    //Initializations
    root = lw_querynode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = "test english",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = "(search)",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    lw_querynode_assert_equals (root, &expected_root);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_starts_with_parenthesis (Fixture       * fixture,
                                      gconstpointer   data)
{
    //Declarations
    LwQueryNode * root = NULL;
    gchar const * TEXT = "(test)english search";

    //Initializations
    root = lw_querynode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = "(test)",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = "english search",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    lw_querynode_assert_equals (root, &expected_root);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_with_parenthesis (Fixture       * fixture,
                               gconstpointer   data)
{
    //Declarations
    LwQueryNode * root = NULL;
    gchar const * TEXT = "test(english)search";

    //Initializations
    root = lw_querynode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = "test",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = "(english)",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = "search",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    lw_querynode_assert_equals (root, &expected_root);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_two_tokens_split_by_and (Fixture       * fixture,
                                      gconstpointer   data)
{
    //Declarations
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1&&2";

    //Initializations
    root = lw_querynode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .language = NULL,
      .data = "1",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_AND,
      .language = NULL,
      .data = "2",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    lw_querynode_assert_equals (root, &expected_root);

    lw_querynode_unref (root);
    root = NULL;
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/libwaei/querynode/parse_string_with_no_parenthesis", Fixture, NULL, setup, parse_string_with_no_parenthesis, teardown);
    g_test_add ("/libwaei/querynode/parse_string_with_only_parenthesis", Fixture, NULL, setup, parse_string_with_only_parenthesis, teardown);
    g_test_add ("/libwaei/querynode/parse_string_ends_with_parenthesis", Fixture, NULL, setup, parse_string_ends_with_parenthesis, teardown);
    g_test_add ("/libwaei/querynode/parse_string_starts_with_parenthesis", Fixture, NULL, setup, parse_string_starts_with_parenthesis, teardown);
    g_test_add ("/libwaei/querynode/parse_string_with_parenthesis", Fixture, NULL, setup, parse_string_with_parenthesis, teardown);
    g_test_add ("/libwaei/querynode/parse_string_two_tokens_split_by_and", Fixture, NULL, setup, parse_string_two_tokens_split_by_and, teardown);

    return g_test_run();
}


