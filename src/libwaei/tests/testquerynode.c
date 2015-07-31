#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>

struct _Fixture {
  GHashTable * children;
  GList * regexes;
  GList * strvs;
  LwParsedLine parsed_line;
};
typedef struct _Fixture Fixture;

static gchar const * CONTENTS = "国語\0こくご\0Japanese\0national language";

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

    lw_parsedline_init_full (&fixture->parsed_line, (GDestroyNotify) g_free);

    {
      gchar ** buffer = g_new0(gchar*, 2);
      buffer[0] = (gchar*) CONTENTS;
      lw_parsedline_set_strv (&fixture->parsed_line, 1, buffer);
    }

    {
      gchar ** buffer = g_new0(gchar*, 2);
      buffer[0] = (gchar*) CONTENTS + strlen("語彙 ");
      lw_parsedline_set_strv (&fixture->parsed_line, 2, buffer);
    }
    {
      gchar ** buffer = g_new0(gchar*, 3);
      buffer[0] = (gchar*) CONTENTS + strlen("国語 こくご ");
      buffer[1] = (gchar*) CONTENTS + strlen("国語 こくご Japanese ");
      lw_parsedline_set_strv (&fixture->parsed_line, 3, buffer);
    }
}


void teardown (Fixture *fixture, gconstpointer data)
{
    g_hash_table_unref (fixture->children);
    fixture->children = NULL;
    g_list_free_full (fixture->regexes, (GDestroyNotify) g_regex_unref);
    fixture->regexes = NULL;

    lw_parsedline_clear (&fixture->parsed_line);
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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_error (error, LW_QUERYNODE_ERROR, LW_QUERYNODE_HANGING_END_LOGICAL_CONNECTOR);

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
    g_assert_error (error, LW_QUERYNODE_ERROR, LW_QUERYNODE_HANGING_START_LOGICAL_CONNECTOR);

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
    g_assert_error (error, LW_QUERYNODE_ERROR, LW_QUERYNODE_HANGING_START_LOGICAL_CONNECTOR);

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
    g_assert_error (error, LW_QUERYNODE_ERROR, LW_QUERYNODE_HANGING_END_LOGICAL_CONNECTOR);

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
    g_assert_error (error, LW_PARENTHESISNODE_ERROR, LW_PARENTHESISNODE_UNMATCHED_START_PARENTHESIS_ERROR);

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
    g_assert_error (error, LW_PARENTHESISNODE_ERROR, LW_PARENTHESISNODE_UNMATCHED_END_PARENTHESIS_ERROR);

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
    g_assert_no_error (error);
    

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
    g_assert_no_error (error);
    

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
    g_assert_no_error (error);
    

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
    g_assert_no_error (error);
    

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
    g_assert_no_error (error);
    

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
    g_assert_no_error (error);
    

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
    g_assert_no_error (error);
    

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
    g_assert_error (error, LW_QUERYNODE_ERROR, LW_QUERYNODE_ERROR_MISSING_KEY_AND_VALUE_FOR_KEYED_QUERYNODE);
    
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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
      .key = NULL,
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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

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
    g_assert_no_error (error);

    lw_querynode_unref (root);
    root = NULL;
}


void
match_parsedline_matches_first_column (Fixture       * fixture,
                                       gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwQueryNodeMatchInfo * match_info = NULL;
    gint columns[] = {1, -1};
    gboolean matches = FALSE;

    //Act
    match_info = lw_querynodematchinfo_new ();
    root = lw_querynode_new_tree_from_string ("国語", &operation, &error);
    root->columns = g_memdup (columns, sizeof(gint) * 2);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);
    matches = lw_querynode_match_parsedline (root, parsed_line, match_info);

    //Assert
    g_assert_true (matches);

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 1);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);

      g_assert_false (has_more);
      g_assert_true (is_match);
      g_assert_cmpstr (START, ==, "国語");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 2);
      g_assert_null (column_match_info);
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 3);
      g_assert_null (column_match_info);
    }

    lw_querynode_unref (root);
    root = NULL;
    lw_querynodematchinfo_unref (match_info);
    match_info = NULL;
}


void
match_parsedline_matches_nothing (Fixture       * fixture,
                                  gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwQueryNodeMatchInfo * match_info = NULL;
    gint columns[] = {1, -1};
    gboolean matches = FALSE;


    //Act
    match_info = lw_querynodematchinfo_new ();
    root = lw_querynode_new_tree_from_string ("Non Matching Query", &operation, &error);
    root->columns = g_memdup (columns, sizeof(gint) * 2);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);
    matches = lw_querynode_match_parsedline (root, parsed_line, match_info);

    //Assert
    g_assert_false (matches);

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 1);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "国語");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 2);
      g_assert_null (column_match_info);
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 3);
      g_assert_null (column_match_info);
    }

    lw_querynode_unref (root);
    root = NULL;
    lw_querynodematchinfo_unref (match_info);
    match_info = NULL;
}


void
match_parsedline_matches_last_in_third_column (Fixture       * fixture,
                                               gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwQueryNodeMatchInfo * match_info = NULL;
    gint columns[] = {3, -1};
    gboolean matches = FALSE;


    //Act
    match_info = lw_querynodematchinfo_new ();
    root = lw_querynode_new_tree_from_string ("national", &operation, &error);
    root->columns = g_memdup (columns, sizeof(gint) * 2);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);
    matches = lw_querynode_match_parsedline (root, parsed_line, match_info);

    //Assert
    g_assert_true (matches);

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 1);
      g_assert_null (column_match_info);
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 2);
      g_assert_null (column_match_info);
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 3);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_true (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "Japanese");

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_true (has_more);
      g_assert_true (is_match);
      g_assert_cmpstr (START, ==, "national language");

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, " language");
    }

    lw_querynode_unref (root);
    root = NULL;
    lw_querynodematchinfo_unref (match_info);
    match_info = NULL;
}


void
match_parsedline_matches_one_of_multiple_columns (Fixture       * fixture,
                                                  gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwQueryNodeMatchInfo * match_info = NULL;
    gint columns[] = {1, 2, 3, -1};
    gboolean matches = FALSE;


    //Act
    match_info = lw_querynodematchinfo_new ();
    root = lw_querynode_new_tree_from_string ("national", &operation, &error);
    root->columns = g_memdup (columns, sizeof(gint) * 4);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);
    matches = lw_querynode_match_parsedline (root, parsed_line, match_info);

    //Assert
    g_assert_true (matches);

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 1);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "国語");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 2);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "こくご");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 3);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_true (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "Japanese");

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_true (has_more);
      g_assert_true (is_match);
      g_assert_cmpstr (START, ==, "national language");

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, " language");
    }

    lw_querynode_unref (root);
    root = NULL;
    lw_querynodematchinfo_unref (match_info);
    match_info = NULL;
}


void
match_parsedline_matches_second_in_strv (Fixture       * fixture,
                                         gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwQueryNodeMatchInfo * match_info = NULL;
    gint columns[] = {3, -1};
    gboolean matches = FALSE;

    //Act
    match_info = lw_querynodematchinfo_new ();
    root = lw_querynode_new_tree_from_string ("Japanese", &operation, &error);
    root->columns = g_memdup (columns, sizeof(gint) * 2);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);
    matches = lw_querynode_match_parsedline (root, parsed_line, match_info);

    //Assert
    g_assert_true (matches);

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 1);
      g_assert_null (column_match_info);
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 2);
      g_assert_null (column_match_info);
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 3);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_true (has_more);
      g_assert_true (is_match);
      g_assert_cmpstr (START, ==, "Japanese");

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "national language");
    }

    lw_querynode_unref (root);
    root = NULL;
    lw_querynodematchinfo_unref (match_info);
    match_info = NULL;
}


void
match_parsedline_matches_or_where_one_matches (Fixture       * fixture,
                                               gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwQueryNodeMatchInfo * match_info = NULL;
    gint columns[] = {1, 2, 3, -1};
    gboolean matches = FALSE;

    //Act
    match_info = lw_querynodematchinfo_new ();
    root = lw_querynode_new_tree_from_string ("English||Japanese", &operation, &error);
    ((LwQueryNode*)root->children->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    ((LwQueryNode*)root->children->next->data)->columns = g_memdup(columns, sizeof(gint) * 4);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);
    matches = lw_querynode_match_parsedline (root, parsed_line, match_info);

    //Assert
    g_assert_true (matches);

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 1);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "国語");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 2);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "こくご");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 3);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_true (has_more);
      g_assert_true (is_match);
      g_assert_cmpstr (START, ==, "Japanese");

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "national language");
    }

    lw_querynode_unref (root);
    root = NULL;
    lw_querynodematchinfo_unref (match_info);
    match_info = NULL;
}


void
match_parsedline_or_where_neither_matches (Fixture       * fixture,
                                           gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwQueryNodeMatchInfo * match_info = NULL;
    gint columns[] = {1, 2, 3, -1};
    gboolean matches = FALSE;

    //Act
    match_info = lw_querynodematchinfo_new ();
    root = lw_querynode_new_tree_from_string ("English||Frensh", &operation, &error);
    ((LwQueryNode*)root->children->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    ((LwQueryNode*)root->children->next->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);
    matches = lw_querynode_match_parsedline (root, parsed_line, match_info);

    //Assert
    g_assert_false (matches);

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 1);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "国語");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 2);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "こくご");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 3);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_true (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "Japanese");

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "national language");
    }

    lw_querynode_unref (root);
    root = NULL;
    lw_querynodematchinfo_unref (match_info);
    match_info = NULL;
}


void
match_parsedline_and_where_both_match (Fixture       * fixture,
                                       gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwQueryNodeMatchInfo * match_info = NULL;
    gint columns[] = {1, 2, 3, -1};
    gboolean matches = FALSE;

    //Act
    match_info = lw_querynodematchinfo_new ();
    root = lw_querynode_new_tree_from_string ("Japanese&&国語", &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);
    ((LwQueryNode*)root->children->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    ((LwQueryNode*)root->children->next->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    matches = lw_querynode_match_parsedline (root, parsed_line, match_info);

    //Assert
    g_assert_true (matches);

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 1);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_true (is_match);
      g_assert_cmpstr (START, ==, "国語");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 2);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "こくご");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 3);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_true (has_more);
      g_assert_true (is_match);
      g_assert_cmpstr (START, ==, "Japanese");

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "national language");
    }

    lw_querynode_unref (root);
    root = NULL;
    lw_querynodematchinfo_unref (match_info);
    match_info = NULL;
}


void
match_parsedline_and_where_one_matches (Fixture       * fixture,
                                        gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwQueryNodeMatchInfo * match_info = NULL;
    gint columns[] = {1, 2, 3, -1};
    gboolean matches = FALSE;

    //Act
    match_info = lw_querynodematchinfo_new ();
    root = lw_querynode_new_tree_from_string ("Japanese&&English", &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);
    ((LwQueryNode*)root->children->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    ((LwQueryNode*)root->children->next->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    matches = lw_querynode_match_parsedline (root, parsed_line, match_info);

    //Assert
    g_assert_false (matches);

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 1);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "国語");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 2);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "こくご");
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 3);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_true (has_more);
      g_assert_true (is_match);
      g_assert_cmpstr (START, ==, "Japanese");

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "national language");
    }

    lw_querynode_unref (root);
    root = NULL;
    lw_querynodematchinfo_unref (match_info);
    match_info = NULL;
}


void
match_parsedline_and_or_where_last_matches (Fixture       * fixture,
                                            gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwQueryNodeMatchInfo * match_info = NULL;
    gint columns[] = {3, -1};
    gboolean matches = FALSE;

    //Act
    match_info = lw_querynodematchinfo_new ();
    root = lw_querynode_new_tree_from_string ("French&&English||Japanese", &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);
    ((LwQueryNode*)root->children->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    ((LwQueryNode*)root->children->next->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    ((LwQueryNode*)root->children->next->next->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    matches = lw_querynode_match_parsedline (root, parsed_line, match_info);

    //Assert
    g_assert_true (matches);

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 1);
      g_assert_null (column_match_info);
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 2);
      g_assert_null (column_match_info);
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 3);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_true (has_more);
      g_assert_true (is_match);
      g_assert_cmpstr (START, ==, "Japanese");

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "national language");
    }

    lw_querynode_unref (root);
    root = NULL;
    lw_querynodematchinfo_unref (match_info);
    match_info = NULL;
}


void
match_parsedline_and_or_where_last_doesnt_match (Fixture       * fixture,
                                                 gconstpointer   data)
{
    //Arrange
    LwQueryNode * root = NULL;
    GError *error = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwQueryNodeMatchInfo * match_info = NULL;
    gint columns[] = {3, -1};
    gboolean matches = FALSE;

    //Act
    match_info = lw_querynodematchinfo_new ();
    root = lw_querynode_new_tree_from_string ("Japanese&&English||French", &operation, &error);
    lw_querynode_compile (root, LW_UTF8FLAG_NONE, &error);
    ((LwQueryNode*)root->children->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    ((LwQueryNode*)root->children->next->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    ((LwQueryNode*)root->children->next->next->data)->columns = g_memdup (columns, sizeof(gint) * 4);
    matches = lw_querynode_match_parsedline (root, parsed_line, match_info);

    //Assert
    g_assert_false (matches);

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 1);
      g_assert_null (column_match_info);
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 2);
      g_assert_null (column_match_info);
    }

    {
      LwQueryNodeColumnMatchInfoIter iter = {0};
      gchar const * START = NULL;
      gchar const * END = NULL;
      gboolean is_match = FALSE;
      gboolean has_more = FALSE;
      gint i = 0;
      LwQueryNodeColumnMatchInfo * column_match_info = lw_querynodematchinfo_get_column (match_info, 3);

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_true (has_more);
      g_assert_true (is_match);
      g_assert_cmpstr (START, ==, "Japanese");

      has_more = lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &START, &END, &is_match);
      g_assert_false (has_more);
      g_assert_false (is_match);
      g_assert_cmpstr (START, ==, "national language");
    }

    lw_querynode_unref (root);
    root = NULL;
    lw_querynodematchinfo_unref (match_info);
    match_info = NULL;
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

    g_test_add ("/match_parsedline/matches_first_column", Fixture, NULL, setup, match_parsedline_matches_first_column, teardown);
    g_test_add ("/match_parsedline/matches_nothing", Fixture, NULL, setup, match_parsedline_matches_nothing, teardown);
    g_test_add ("/match_parsedline/matches_last_in_third_column", Fixture, NULL, setup, match_parsedline_matches_last_in_third_column, teardown);
    g_test_add ("/match_parsedline/matches_one_of_multiple_columns", Fixture, NULL, setup, match_parsedline_matches_one_of_multiple_columns, teardown);
    g_test_add ("/match_parsedline/matches_second_in_strv", Fixture, NULL, setup, match_parsedline_matches_second_in_strv, teardown);
    g_test_add ("/match_parsedline/matches_or_where_one_matches", Fixture, NULL, setup, match_parsedline_matches_or_where_one_matches, teardown);
    g_test_add ("/match_parsedline/or_where_neither_matches", Fixture, NULL, setup, match_parsedline_or_where_neither_matches, teardown);
    g_test_add ("/match_parsedline/and_where_both_match", Fixture, NULL, setup, match_parsedline_and_where_both_match, teardown);
    g_test_add ("/match_parsedline/and_where_one_matches", Fixture, NULL, setup, match_parsedline_and_where_one_matches, teardown);
    g_test_add ("/match_parsedline/and_or_where_last_matches", Fixture, NULL, setup, match_parsedline_and_or_where_last_matches, teardown);
    g_test_add ("/match_parsedline/and_or_where_last_doesnt_match", Fixture, NULL, setup, match_parsedline_and_or_where_last_doesnt_match, teardown);

    return g_test_run();
}


