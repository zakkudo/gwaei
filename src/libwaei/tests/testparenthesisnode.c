#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>


struct _Fixture { GHashTable * children; };
typedef struct _Fixture Fixture;

void
_set_children (struct _Fixture    * fixture,
               GList             ** children,
               LwParenthesisNode  * nodes,
               gint                 num_nodes)
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
parse_string_with_no_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "test english search";
    GError *error = NULL;

    //Initializations
    root = lw_parenthesisnode_new_tree_from_string (TEXT, &error);

    //Assert
    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = NULL,
      .children = NULL
    }; 

    lw_parenthesisnode_assert_equals (root, &expected_root);
    g_assert_no_error (error);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_string_with_only_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "(test english search)";
    GError *error = NULL;

    //Initializations
    root = lw_parenthesisnode_new_tree_from_string (TEXT, &error);

    //Assert
    LwParenthesisNode expected_root = {
      .has_parenthesis = TRUE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = NULL,
      .children = NULL
    }; 

    lw_parenthesisnode_assert_equals (root, &expected_root);
    g_assert_no_error (error);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_string_ends_with_parenthesis (Fixture *fixture, gconstpointer data)
{
    // Arrange
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "test english(search)";
    GError *error = NULL;

    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = NULL,
      .children = NULL 
    }; 

    LwParenthesisNode explicit_children[] = {{
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT + strlen("test english"),
      .CLOSE = TEXT + strlen("test english(search)") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(expected_root.explicit_children), explicit_children, G_N_ELEMENTS(explicit_children));

    LwParenthesisNode children[] = {{
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen("test english") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }, {
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT + strlen("test english"),
      .CLOSE = TEXT + strlen("test english(search)") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    //Act
    root = lw_parenthesisnode_new_tree_from_string (TEXT, &error);

    //Assert
    lw_parenthesisnode_assert_equals (root, &expected_root);
    g_assert_no_error (error);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_string_starts_with_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Arrange
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "(search)test english";
    GError *error = NULL;

    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = NULL,
      .children = NULL
    }; 

    LwParenthesisNode explicit_children[] = {{
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen("(search)") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(expected_root.explicit_children), explicit_children, G_N_ELEMENTS(explicit_children));

    LwParenthesisNode children[] = {{
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen("(search)") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }, {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT + strlen("(search)"),
      .CLOSE = TEXT + strlen("(search)test english") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    //Act
    root = lw_parenthesisnode_new_tree_from_string (TEXT, &error);

    //Assert
    lw_parenthesisnode_assert_equals (root, &expected_root);
    g_assert_no_error (error);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_string_with_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Arrange
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "test(search)english";
    GError *error = NULL;

    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = NULL,
      .children = NULL
    }; 

    LwParenthesisNode explicit_children[] = {{
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT + strlen("test"),
      .CLOSE = TEXT + strlen("test(search)") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(expected_root.explicit_children), explicit_children, G_N_ELEMENTS(explicit_children));

    LwParenthesisNode children[] = {{
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen("test") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }, {
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT + strlen("test"),
      .CLOSE = TEXT + strlen("test(search)") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }, {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT + strlen("test(search)"),
      .CLOSE = TEXT + strlen("test(search)english") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    //Act
    root = lw_parenthesisnode_new_tree_from_string (TEXT, &error);

    //Assert
    lw_parenthesisnode_assert_equals (root, &expected_root);
    g_assert_no_error (error);

    lw_parenthesisnode_unref (root);
    root = NULL;
}



void
parse_string_with_empty_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "1()";
    GError *error = NULL;

    //Initializations
    root = lw_parenthesisnode_new_tree_from_string (TEXT, &error);

    //Assert
    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = NULL,
      .children = NULL 
    }; 

    LwParenthesisNode explicit_children[] = {{
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT + strlen("1"),
      .CLOSE = TEXT + strlen("1()") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(expected_root.explicit_children), explicit_children, G_N_ELEMENTS(explicit_children));

    LwParenthesisNode children[] = {{
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen("1") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }, {
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT + strlen("1"),
      .CLOSE = TEXT + strlen("1()") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS(children));

    //Assert
    lw_parenthesisnode_assert_equals (root, &expected_root);
    g_assert_no_error (error);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_string_with_embedded_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Arrange
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "1(2(3))";
    GError *error = NULL;

    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = NULL,
      .children = NULL
    }; 

    LwParenthesisNode children[] = {{
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT,
      .explicit_children = NULL,
      .children = NULL,
    }, {
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT + strlen("1"),
      .CLOSE = TEXT + strlen("1(2(3))") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(expected_root.children), children, G_N_ELEMENTS (children));

    LwParenthesisNode explicit_children[] = {{
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT + strlen("1"),
      .CLOSE = TEXT + strlen("1(2(3))") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(expected_root.explicit_children), explicit_children, G_N_ELEMENTS(explicit_children));

    LwParenthesisNode embedded_children[] = {{
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT + strlen("1("),
      .CLOSE = TEXT + strlen("1(2") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }, {
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT + strlen("1(2"),
      .CLOSE = TEXT + strlen("1(2(3)") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(children[1].children), embedded_children, G_N_ELEMENTS (embedded_children));
    _set_children (fixture, &(explicit_children[0].children), embedded_children, G_N_ELEMENTS (embedded_children));

    LwParenthesisNode embedded_explicit_children[] = {{
      .has_parenthesis = TRUE,
      .refs = 2,
      .OPEN = TEXT + strlen("1(2"),
      .CLOSE = TEXT + strlen("1(2(3)") - 1,
      .explicit_children = NULL,
      .children = NULL,
    }};
    _set_children (fixture, &(children[1].explicit_children), embedded_explicit_children, G_N_ELEMENTS (embedded_explicit_children));
    _set_children (fixture, &(explicit_children[0].explicit_children), embedded_explicit_children, G_N_ELEMENTS (embedded_explicit_children));

    //Act
    root = lw_parenthesisnode_new_tree_from_string (TEXT, &error);

    //Assert
    lw_parenthesisnode_assert_equals (root, &expected_root);
    g_assert_no_error (error);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_string_with_only_start_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "(test english search";
    GError *error = NULL;

    //Initializations
    root = lw_parenthesisnode_new_tree_from_string (TEXT, &error);

    //Assert
    g_assert_null (root);
    g_assert_error (error, LW_PARENTHESISNODE_ERROR, LW_PARENTHESISNODE_UNMATCHED_START_PARENTHESIS_ERROR);

    g_clear_error (&error);
}


void
parse_string_with_only_end_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "test english search)";
    GError *error = NULL;

    //Initializations
    root = lw_parenthesisnode_new_tree_from_string (TEXT, &error);

    //Assert
    g_assert_null (root);
    g_assert_error (error, LW_PARENTHESISNODE_ERROR, LW_PARENTHESISNODE_UNMATCHED_END_PARENTHESIS_ERROR);

    g_clear_error (&error);
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
    g_test_add ("/new_tree_from_string/parse_string_with_empty_parenthesis", Fixture, NULL, setup, parse_string_with_empty_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_embedded_parenthesis", Fixture, NULL, setup, parse_string_with_embedded_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_only_start_parenthesis", Fixture, NULL, setup, parse_string_with_only_start_parenthesis, teardown);
    g_test_add ("/new_tree_from_string/parse_string_with_only_end_parenthesis", Fixture, NULL, setup, parse_string_with_only_end_parenthesis, teardown);

    return g_test_run();
}


