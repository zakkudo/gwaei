#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>

struct _Fixture { GHashTable * children; GList * regexes;};
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

GRegex*
_new_regex (struct _Fixture * fixture,
            char const      * PATTERN)
{
    GRegex *regex = NULL;

    regex = g_regex_new (PATTERN, 0, 0, NULL);

    if (regex != NULL)
    {
      fixture->regexes = g_list_prepend (fixture->regexes, regex);
    }

    return regex;
}


void setup (Fixture *fixture, gconstpointer data)
{
    fixture->children = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, (GDestroyNotify) g_list_free);
}


void teardown (Fixture *fixture, gconstpointer data)
{
    g_hash_table_unref (fixture->children);
    fixture->children = NULL;
    g_list_free_full (fixture->regexes, (GDestroyNotify) g_regex_unref);
    fixture->regexes = NULL;
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


void
parse_string_with_keyed_value (Fixture       * fixture,
                               gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "strokes:1";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = "strokes",
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


void
parse_string_with_ending_keyed_value (Fixture       * fixture,
                                      gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1 strokes:2";
    GError *error = NULL;
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
      .key = "strokes",
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
parse_string_with_starting_keyed_value (Fixture       * fixture,
                                        gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "strokes:1 2";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = "strokes",
      .data = "1 2",
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
parse_string_with_multiple_keyed_values (Fixture       * fixture,
                                         gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "strokes:1 grade:7";
    GError *error = NULL;
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
      .key = "strokes",
      .data = "1",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_AND,
      .key = "grade",
      .data = "7",
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
parse_string_with_dangling_key (Fixture       * fixture,
                                gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "strokes:";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = "strokes",
      .data = NULL,
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
parse_string_with_missing_key (Fixture       * fixture,
                               gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = ":1";
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


void
parse_string_with_missing_key_and_value (Fixture       * fixture,
                                         gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = ":";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);

    //Assert
    g_assert_null (root);
    g_assert_true (g_error_matches (error, LW_QUERYNODE_ERROR, LW_QUERYNODE_ERROR_MISSING_KEY_AND_VALUE_FOR_KEYED_QUERYNODE));
    
    g_clear_error (&error);
    error = NULL;
}


void
parse_string_with_key_and_embedded_value (Fixture       * fixture,
                                          gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1:(2&&3)4";
    GError *error = NULL;
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
      .key = "1",
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "4",
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
parse_string_with_keyed_embedded (Fixture       * fixture,
                                  gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1(2:3)4";
    GError *error = NULL;
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
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = "2",
      .data = "3",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "4",
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
compile_string_with_parenthesis (Fixture       * fixture,
                                 gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1(2)3";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "1(2)3",
      .children = NULL,
      .refs = 1,
    }; 
    expected_root.regex = _new_regex (fixture, "1(2)3");

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
compile_string_with_only_parenthesis (Fixture       * fixture,
                                      gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "(1)";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "(1)",
      .children = NULL,
      .refs = 1,
    }; 
    expected_root.regex = _new_regex (fixture, "(1)");

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
compile_string_with_two_sets_of_parenthesis (Fixture       * fixture,
                                             gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "(1)(2)3";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "(1)(2)3",
      .children = NULL,
      .refs = 1,
    }; 
    expected_root.regex = _new_regex (fixture, "(1)(2)3");

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
compile_string_with_no_parenthesis (Fixture       * fixture,
                                    gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "1",
      .children = NULL,
      .refs = 1,
    }; 
    expected_root.regex = _new_regex (fixture, "1");

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
compile_string_with_keyed_embedded (Fixture       * fixture,
                                    gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1:(2)";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = "1",
      .data = "(2)",
      .children = NULL,
      .refs = 1,
    }; 
    expected_root.regex = _new_regex (fixture, "(2)");

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
compile_string_with_keyed_embedded_and_on_value (Fixture       * fixture,
                                                 gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1:(2&&3)";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = "1",
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }; 

    LwQueryNode children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = "1",
      .data = "(2)",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_AND,
      .key = "1",
      .data = "(3)",
      .children = NULL,
      .refs = 1,
    }};
    children[0].regex = _new_regex (fixture, "(2)");
    children[1].regex = _new_regex (fixture, "(3)");
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
compile_string_with_keyed_embedded_and_on_key (Fixture       * fixture,
                                               gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "(1&&2):3";
    GError *error = NULL;
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
      .data = NULL,
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_AND,
      .key = NULL,
      .data = "3",
      .children = NULL,
      .refs = 1,
    }};
    children[1].regex = _new_regex (fixture, "3");
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    LwQueryNode embedded_children[] = {{
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "(1)",
      .children = NULL,
      .refs = 1,
    }, {
      .operation = LW_QUERYNODE_OPERATION_AND,
      .key = NULL,
      .data = "(2)",
      .children = NULL,
      .refs = 1,
    }};
    embedded_children[0].regex = _new_regex (fixture, "(1)");
    embedded_children[1].regex = _new_regex (fixture, "(2)");
    _set_children (fixture, &(children[0].children), embedded_children, G_N_ELEMENTS(embedded_children));

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
compile_string_with_empty_parenthesis_directly_after_keyed (Fixture       * fixture,
                                                            gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1:()2";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = "1",
      .data = "2",
      .children = NULL,
      .refs = 1,
    }; 
    expected_root.regex = _new_regex (fixture, "2");

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
compile_string_with_empty_parenthesis_directly_before_keyed (Fixture       * fixture,
                                                             gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "1():2";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "12",
      .children = NULL,
      .refs = 1,
    }; 
    expected_root.regex = _new_regex (fixture, "12");

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}



void
compile_string_with_furiganafold_on (Fixture       * fixture,
                                     gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "abcABCあいうえおアイウエオ";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "abcABCあいうえおアイウエオ",
      .children = NULL,
      .refs = 1,
    }; 
    expected_root.regex = _new_regex (fixture, "abcABCあいうえおあいうえお");

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_FURIGANAFOLD, &error);

    //Assert
    lw_querynode_assert_equals (root, &expected_root);
    g_assert_null (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
compile_string_with_casefold_on (Fixture       * fixture,
                                 gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    gchar const * TEXT = "abcABCあいうえおアイウエオ";
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    LwQueryNode expected_root = {
      .operation = LW_QUERYNODE_OPERATION_NONE,
      .key = NULL,
      .data = "abcABCあいうえおアイウエオ",
      .children = NULL,
      .refs = 1,
    }; 
    expected_root.regex = _new_regex (fixture, "abcabcあいうえおアイウエオ");

    //Act
    root = lw_querynode_new_tree_from_string (TEXT, &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_CASEFOLD, &error);

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

    g_test_add ("/new_tree_from_string/parse_string_with_no_parenthesis", Fixture, NULL, setup, parse_string_with_no_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_only_parenthesis", Fixture, NULL, setup, parse_string_with_only_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_ends_with_parenthesis", Fixture, NULL, setup, parse_string_ends_with_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_starts_with_parenthesis", Fixture, NULL, setup, parse_string_starts_with_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_parenthesis", Fixture, NULL, setup, parse_string_with_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_two_tokens_split_by_and", Fixture, NULL, setup, parse_string_two_tokens_split_by_and, teardown);
    g_test_add ("/new_tree_from_string/parse_string_two_tokens_split_by_or", Fixture, NULL, setup, parse_string_two_tokens_split_by_or, teardown);
    g_test_add ("/new_tree_from_string/parse_string_three_tokens_split_by_and_and_or", Fixture, NULL, setup, parse_string_three_tokens_split_by_and_and_or, teardown);
    g_test_add ("/new_tree_from_string/parse_string_and_embedded_in_or", Fixture, NULL, setup, parse_string_and_embedded_in_or, teardown);
    g_test_add ("/new_tree_from_string/parse_string_and_embedded_in_or_with_capture", Fixture, NULL, setup, parse_string_and_embedded_in_or_with_capture, teardown);
    g_test_add ("/new_tree_from_string/parse_string_hanging_and", Fixture, NULL, setup, parse_string_hanging_and, teardown);
    g_test_add ("/new_tree_from_string/parse_string_hanging_and_in_parenthesis", Fixture, NULL, setup, parse_string_hanging_and_in_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_starts_with_hanging_and", Fixture, NULL, setup, parse_string_starts_with_hanging_and, teardown);
    g_test_add ("/new_tree_from_string/parse_string_starts_with_hanging_and_in_parenthesis", Fixture, NULL, setup, parse_string_starts_with_hanging_and_in_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_embedded_hanging_and", Fixture, NULL, setup, parse_string_embedded_hanging_and, teardown);
    g_test_add ("/new_tree_from_string/parse_string_only_start_parenthesis", Fixture, NULL, setup, parse_string_only_start_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_only_end_parenthesis", Fixture, NULL, setup, parse_string_only_end_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_empty_parenthesis", Fixture, NULL, setup, parse_string_with_empty_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_keyed_value", Fixture, NULL, setup, parse_string_with_keyed_value, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_ending_keyed_value", Fixture, NULL, setup, parse_string_with_ending_keyed_value, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_starting_keyed_value", Fixture, NULL, setup, parse_string_with_starting_keyed_value, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_multiple_keyed_values", Fixture, NULL, setup, parse_string_with_multiple_keyed_values, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_dangling_key", Fixture, NULL, setup, parse_string_with_dangling_key, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_missing_key", Fixture, NULL, setup, parse_string_with_missing_key, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_missing_key_and_value", Fixture, NULL, setup, parse_string_with_missing_key_and_value, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_key_and_embedded_value", Fixture, NULL, setup, parse_string_with_key_and_embedded_value, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_keyed_embedded", Fixture, NULL, setup, parse_string_with_keyed_embedded, teardown);

    g_test_add ("/compile/string_with_parenthesis", Fixture, NULL, setup, compile_string_with_parenthesis, teardown);
    g_test_add ("/compile/string_with_only_parenthesis", Fixture, NULL, setup, compile_string_with_only_parenthesis, teardown);
    g_test_add ("/compile/string_with_no_parenthesis", Fixture, NULL, setup, compile_string_with_no_parenthesis, teardown);
    g_test_add ("/compile/string_with_keyed_embedded", Fixture, NULL, setup, compile_string_with_keyed_embedded, teardown);
    g_test_add ("/compile/string_with_two_sets_of_parenthesis", Fixture, NULL, setup, compile_string_with_two_sets_of_parenthesis, teardown);
    g_test_add ("/compile/string_with_keyed_embedded_and_on_value", Fixture, NULL, setup, compile_string_with_keyed_embedded_and_on_value, teardown);
    g_test_add ("/compile/string_with_keyed_embedded_and_on_key", Fixture, NULL, setup, compile_string_with_keyed_embedded_and_on_key, teardown);
    g_test_add ("/compile/string_with_empty_parenthesis_directly_after_keyed", Fixture, NULL, setup, compile_string_with_empty_parenthesis_directly_after_keyed, teardown);
    g_test_add ("/compile/string_with_empty_parenthesis_directly_before_keyed", Fixture, NULL, setup, compile_string_with_empty_parenthesis_directly_before_keyed, teardown);
    g_test_add ("/compile/string_with_furiganafold_on", Fixture, NULL, setup, compile_string_with_furiganafold_on, teardown);
    g_test_add ("/compile/string_with_casefold_on", Fixture, NULL, setup, compile_string_with_casefold_on, teardown);

    return g_test_run();
}


