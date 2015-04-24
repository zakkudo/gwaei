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
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "test english search";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "test english search",
      .children = NULL,
      .refs = 1,
    }; 

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_with_only_parenthesis (Fixture       * fixture,
                                    gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "(test english search)";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "(test english search)",
      .children = NULL,
      .refs = 1,
    }; 

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_ends_with_parenthesis (Fixture       * fixture,
                                    gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "test english(search)";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "test english",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "(search)",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_starts_with_parenthesis (Fixture       * fixture,
                                      gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "(test)english search";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "(test)",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "english search",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_with_parenthesis (Fixture       * fixture,
                               gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "test(english)search";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "test",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "(english)",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "search",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_two_tokens_split_by_and (Fixture       * fixture,
                                      gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1&&2";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "1",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_AND,
      .key = NULL,
      .data = "2",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}

void
parse_string_two_tokens_split_by_or (Fixture       * fixture,
                                     gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1||2";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "1",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_OR,
      .key = NULL,
      .data = "2",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_three_tokens_split_by_and_and_or (Fixture       * fixture,
                                               gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1&&2||3";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "1",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_AND,
      .key = NULL,
      .data = "2",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_OR,
      .key = NULL,
      .data = "3",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_and_embedded_in_or (Fixture       * fixture,
                                 gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1||(2&&3)";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "1",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_OR,
      .key = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    LwQueryNode embedded_children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "(2)",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_AND,
      .key = NULL,
      .data = "(3)",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(children[1].children), embedded_children, G_N_ELEMENTS(embedded_children));

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_and_embedded_in_or_with_capture (Fixture       * fixture,
                                              gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1||(?=2&&3)";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "1",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_OR,
      .key = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    LwQueryNode embedded_children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "(?=2)",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_AND,
      .key = NULL,
      .data = "(?=3)",
      .children = NULL,
      .refs = 1,
    }};
    _set_children (fixture, &(children[1].children), embedded_children, G_N_ELEMENTS(embedded_children));

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}

void
parse_string_hanging_and (Fixture       * fixture,
                          gconstpointer   data)
{
    //Arange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1&&";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "1",
      .children = NULL,
      .refs = 1,
    }; 

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_cmpint (operation, ==, LW_QUERYNODE_OPERATION_AND);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
parse_string_hanging_and_in_parenthesis (Fixture       * fixture,
                                         gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "(1&&)";
    GError * error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    g_assert_null (root);
    g_assert_true (g_error_matches (error, LW_QUERYNODE_ERROR, LW_QUERYNODE_HANGING_END_LOGICAL_CONNECTOR));

    g_clear_error (&error);
}


void
parse_string_starts_with_hanging_and (Fixture       * fixture,
                                      gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "&&1";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    g_assert_null (root);
    g_assert_true (g_error_matches (error, LW_QUERYNODE_ERROR, LW_QUERYNODE_HANGING_START_LOGICAL_CONNECTOR));

    g_clear_error (&error);
}


void
parse_string_starts_with_hanging_and_in_parenthesis (Fixture       * fixture,
                                                     gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "(&&1)";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    g_assert_null (root);
    g_assert_true (g_error_matches (error, LW_QUERYNODE_ERROR, LW_QUERYNODE_HANGING_START_LOGICAL_CONNECTOR));

    g_clear_error (&error);
}


void
parse_string_embedded_hanging_and (Fixture       * fixture,
                                   gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1(2&&)";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    g_assert_null (root);
    g_assert_true (g_error_matches (error, LW_QUERYNODE_ERROR, LW_QUERYNODE_HANGING_END_LOGICAL_CONNECTOR));

    g_clear_error (&error);
}


void
parse_string_only_start_parenthesis (Fixture       * fixture,
                                     gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "(2";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    g_assert_null (root);
    g_assert_true (g_error_matches (error, LW_PARENTHESISNODE_ERROR, LW_PARENTHESISNODE_UNMATCHED_START_PARENTHESIS_ERROR));

    g_clear_error (&error);
}

void
parse_string_only_end_parenthesis (Fixture       * fixture,
                                   gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "2)";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    g_assert_null (root);
    g_assert_true (g_error_matches (error, LW_PARENTHESISNODE_ERROR, LW_PARENTHESISNODE_UNMATCHED_END_PARENTHESIS_ERROR));

    g_clear_error (&error);
}


void
parse_string_with_empty_parenthesis (Fixture       * fixture,
                                     gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1()";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "1",
      .children = NULL,
      .refs = 1,
    }; 

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);
    

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
    g_test_add ("/libwaei/querynode/parse_string_two_tokens_split_by_or", Fixture, NULL, setup, parse_string_two_tokens_split_by_or, teardown);
    g_test_add ("/libwaei/querynode/parse_string_three_tokens_split_by_and_and_or", Fixture, NULL, setup, parse_string_three_tokens_split_by_and_and_or, teardown);
    g_test_add ("/libwaei/querynode/parse_string_and_embedded_in_or", Fixture, NULL, setup, parse_string_and_embedded_in_or, teardown);
    g_test_add ("/libwaei/querynode/parse_string_and_embedded_in_or_with_capture", Fixture, NULL, setup, parse_string_and_embedded_in_or_with_capture, teardown);
    g_test_add ("/libwaei/querynode/parse_string_hanging_and", Fixture, NULL, setup, parse_string_hanging_and, teardown);
    g_test_add ("/libwaei/querynode/parse_string_hanging_and_in_parenthesis", Fixture, NULL, setup, parse_string_hanging_and_in_parenthesis, teardown);
    g_test_add ("/libwaei/querynode/parse_string_starts_with_hanging_and", Fixture, NULL, setup, parse_string_starts_with_hanging_and, teardown);
    g_test_add ("/libwaei/querynode/parse_string_starts_with_hanging_and_in_parenthesis", Fixture, NULL, setup, parse_string_starts_with_hanging_and_in_parenthesis, teardown);
    g_test_add ("/libwaei/querynode/parse_string_embedded_hanging_and", Fixture, NULL, setup, parse_string_embedded_hanging_and, teardown);
    g_test_add ("/libwaei/querynode/parse_string_only_start_parenthesis", Fixture, NULL, setup, parse_string_only_start_parenthesis, teardown);
    g_test_add ("/libwaei/querynode/parse_string_only_end_parenthesis", Fixture, NULL, setup, parse_string_only_end_parenthesis, teardown);
    g_test_add ("/libwaei/querynode/parse_string_with_empty_parenthesis", Fixture, NULL, setup, parse_string_with_empty_parenthesis, teardown);

    return g_test_run();
}


