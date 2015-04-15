#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>

/*
  Methods tested

  lw_index_append_data_offset
  lw_index_get_data_offsets
  lw_index_get_data_offsets_length
  lw_index_search
  lw_index_data_is_valid
*/

struct _Fixture { gint unused; };
typedef struct _Fixture Fixture;
void index_test_setup (Fixture *fixture, gconstpointer data) {}
void index_test_teardown (Fixture *fixture, gconstpointer data) {}


void
parse_string_with_no_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "test english search";

    //Initializations
    root = lw_parenthesisnode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = NULL,
      .children = NULL
    }; 

    g_assert (memcmp(&expected_root, root, sizeof(LwParenthesisNode)) == 0);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_string_with_only_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "(test english search)";

    //Initializations
    root = lw_parenthesisnode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwParenthesisNode expected_root = {
      .has_parenthesis = TRUE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = NULL,
      .children = NULL
    }; 

    g_assert (memcmp(&expected_root, root, sizeof(LwParenthesisNode)) == 0);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_string_ends_with_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "test english(search)";

    //Initializations
    root = lw_parenthesisnode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = root->explicit_children,
      .children = root->children
    }; 

    g_assert (memcmp(&expected_root, root, sizeof(LwParenthesisNode)) == 0);
    g_assert_cmpint (g_list_length (root->children), ==, 2);
    g_assert_cmpint (g_list_length (root->explicit_children), ==, 1);

    {
      LwParenthesisNode * node = NULL;
      node = root->children->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = FALSE,
        .refs = 1,
        .OPEN = TEXT,
        .CLOSE = TEXT + strlen("test english") - 1,
        .explicit_children = NULL,
        .children = NULL,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    {
      LwParenthesisNode * node = NULL;
      node = root->children->next->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = TRUE,
        .refs = 2,
        .OPEN = TEXT + strlen("test english"),
        .CLOSE = TEXT + strlen("test english(search)") - 1,
        .explicit_children = NULL,
        .children = NULL,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    g_assert (root->children->next->data == root->explicit_children->data);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_string_starts_with_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "(search)test english";

    //Initializations
    root = lw_parenthesisnode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = root->explicit_children,
      .children = root->children
    }; 

    g_assert (memcmp(&expected_root, root, sizeof(LwParenthesisNode)) == 0);
    g_assert_cmpint (g_list_length (root->explicit_children), ==, 1);
    g_assert_cmpint (g_list_length (root->children), ==, 2);

    {
      LwParenthesisNode * node = NULL;
      node = root->children->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = TRUE,
        .refs = 2,
        .OPEN = TEXT,
        .CLOSE = TEXT + strlen("(search)") - 1,
        .explicit_children = NULL,
        .children = NULL,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    {
      LwParenthesisNode * node = NULL;
      node = root->children->next->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = FALSE,
        .refs = 1,
        .OPEN = TEXT + strlen("(search)"),
        .CLOSE = TEXT + strlen("(search)test english") - 1,
        .explicit_children = NULL,
        .children = NULL,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    g_assert (root->children->data == root->explicit_children->data);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_string_with_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "test(search)english";

    //Initializations
    root = lw_parenthesisnode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = root->explicit_children,
      .children = root->children
    }; 

    g_assert (memcmp(&expected_root, root, sizeof(LwParenthesisNode)) == 0);
    g_assert_cmpint (g_list_length (root->explicit_children), ==, 1);
    g_assert_cmpint (g_list_length (root->children), ==, 3);

    {
      LwParenthesisNode * node = NULL;
      node = root->children->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = FALSE,
        .refs = 1,
        .OPEN = TEXT,
        .CLOSE = TEXT + strlen("test") - 1,
        .explicit_children = NULL,
        .children = NULL,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    {
      LwParenthesisNode * node = NULL;
      node = root->children->next->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = TRUE,
        .refs = 2,
        .OPEN = TEXT + strlen("test"),
        .CLOSE = TEXT + strlen("test(search)") - 1,
        .explicit_children = NULL,
        .children = NULL,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    {
      LwParenthesisNode * node = NULL;
      node = root->children->next->next->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = FALSE,
        .refs = 1,
        .OPEN = TEXT + strlen("test(search)"),
        .CLOSE = TEXT + strlen("test(search)english") - 1,
        .explicit_children = NULL,
        .children = NULL,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    g_assert (root->children->next->data == root->explicit_children->data);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_string_with_embedded_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "1(2(3))";

    //Initializations
    root = lw_parenthesisnode_new_tree_from_string (TEXT, NULL);

    //Assert
    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = root->explicit_children,
      .children = root->children
    }; 

    g_assert (memcmp(&expected_root, root, sizeof(LwParenthesisNode)) == 0);
    g_assert_cmpint (g_list_length (root->explicit_children), ==, 1);
    g_assert_cmpint (g_list_length (root->children), ==, 2);

    {
      LwParenthesisNode * node = NULL;
      node = root->children->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = FALSE,
        .refs = 1,
        .OPEN = TEXT,
        .CLOSE = TEXT,
        .explicit_children = NULL,
        .children = NULL,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    {
      LwParenthesisNode * node = NULL;
      node = root->children->next->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = TRUE,
        .refs = 2,
        .OPEN = TEXT + strlen("1"),
        .CLOSE = TEXT + strlen("1(2(3))") - 1,
        .explicit_children = node->explicit_children,
        .children = node->children,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    {
      LwParenthesisNode * node = NULL;
      node = LW_PARENTHESISNODE (root->children->next->data)->children->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = FALSE,
        .refs = 1,
        .OPEN = TEXT + strlen("1("),
        .CLOSE = TEXT + strlen("1(2") - 1,
        .explicit_children = NULL,
        .children = NULL,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    {
      LwParenthesisNode * node = NULL;
      node = LW_PARENTHESISNODE (root->children->next->data)->children->next->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = TRUE,
        .refs = 2,
        .OPEN = TEXT + strlen("1(2"),
        .CLOSE = TEXT + strlen("1(2(3)") - 1,
        .explicit_children = NULL,
        .children = NULL,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    g_assert (root->children->next->data == root->explicit_children->data);
    g_assert (LW_PARENTHESISNODE (root->children->next->data)->children->next->data == LW_PARENTHESISNODE (root->explicit_children->data)->explicit_children->data);

    lw_parenthesisnode_unref (root);
    root = NULL;
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/libwaei/parenthesisnode/parse_string_with_no_parenthesis", Fixture, NULL, NULL, parse_string_with_no_parenthesis, NULL);
    g_test_add ("/libwaei/parenthesisnode/parse_string_with_only_parenthesis", Fixture, NULL, NULL, parse_string_with_only_parenthesis, NULL);
    g_test_add ("/libwaei/parenthesisnode/parse_string_ends_with_parenthesis", Fixture, NULL, NULL, parse_string_ends_with_parenthesis, NULL);
    g_test_add ("/libwaei/parenthesisnode/parse_string_starts_with_parenthesis", Fixture, NULL, NULL, parse_string_starts_with_parenthesis, NULL);
    g_test_add ("/libwaei/parenthesisnode/parse_string_with_parenthesis", Fixture, NULL, NULL, parse_string_with_parenthesis, NULL);
    g_test_add ("/libwaei/parenthesisnode/parse_string_with_embedded_parenthesis", Fixture, NULL, NULL, parse_string_with_embedded_parenthesis, NULL);

    return g_test_run();
}


