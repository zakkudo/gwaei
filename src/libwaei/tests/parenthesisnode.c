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
parse_english_string_with_no_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "test english search";

    //Initializations
    root = lw_parenthesisnode_new (TEXT);

    //Assert
    LwParenthesisNode expected_root = {
      .has_parenthesis = FALSE,
      .refs = 1,
      .OPEN = TEXT,
      .CLOSE = TEXT + strlen(TEXT) - 1,
      .explicit_children = NULL,
      .children = root->children
    }; 

    g_assert (memcmp(&expected_root, root, sizeof(LwParenthesisNode)) == 0);
    g_assert_cmpint (g_list_length (root->children), ==, 1);

    {
      LwParenthesisNode * node = NULL;
      node = root->children->data;
      
      LwParenthesisNode expected_node = {
        .has_parenthesis = FALSE,
        .refs = 1,
        .OPEN = TEXT,
        .CLOSE = TEXT + strlen(TEXT) - 1,
        .explicit_children = NULL,
        .children = NULL,
      };

      g_assert (memcmp(&expected_node, node, sizeof(LwParenthesisNode)) == 0);
    }

    lw_parenthesisnode_unref (root);
    root = NULL;
}


void
parse_english_string_with_single_parenthesis (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParenthesisNode * root = NULL;
    gchar const * TEXT = "test english(search)";

    //Initializations
    root = lw_parenthesisnode_new (TEXT);

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

    lw_parenthesisnode_unref (root);
    root = NULL;
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/libwaei/parenthesisnode/parse_english_string_with_no_parenthesis", Fixture, NULL, NULL, parse_english_string_with_no_parenthesis, NULL);
    g_test_add ("/libwaei/parenthesisnode/parse_english_string_with_single_parenthesis", Fixture, NULL, NULL, parse_english_string_with_single_parenthesis, NULL);

    return g_test_run();
}


