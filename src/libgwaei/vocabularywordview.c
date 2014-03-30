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

//!
//! @file dictionarylistbox.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularywordview-private.h>

static LgwVocabularyWordViewClass *_klass = NULL;
static LgwVocabularyWordViewClassPrivate *_klasspriv = NULL;

G_DEFINE_TYPE_WITH_CODE (LgwVocabularyWordView, lgw_vocabularywordview, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_ACTIONABLE, lgw_vocabularywordview_implement_actionable_interface));

//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_vocabularywordview_new ()
{
    //Declarations
    LgwVocabularyWordView *widget = NULL;

    //Initializations
    widget = LGW_VOCABULARYWORDVIEW (g_object_new (LGW_TYPE_VOCABULARYWORDVIEW, "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 0, NULL));

    return GTK_WIDGET (widget);
}


static void 
lgw_vocabularywordview_init (LgwVocabularyWordView *widget)
{
    widget->priv = LGW_VOCABULARYWORDVIEW_GET_PRIVATE (widget);
    memset(widget->priv, 0, sizeof(LgwVocabularyWordViewPrivate));

    LgwVocabularyWordViewPrivate *priv;
    priv = widget->priv;
}


static void 
lgw_vocabularywordview_finalize (GObject *object)
{
    LgwVocabularyWordView *widget;
    LgwVocabularyWordViewPrivate *priv;

    widget = LGW_VOCABULARYWORDVIEW (object);
    priv = widget->priv;

    G_OBJECT_CLASS (lgw_vocabularywordview_parent_class)->finalize (object);
}


static void
lgw_vocabularywordview_dispose (GObject *object)
{
    //Declarations
    LgwVocabularyWordView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (object);

    lgw_vocabularywordview_disconnect_signals (self);

    G_OBJECT_CLASS (lgw_vocabularywordview_parent_class)->dispose (object);
}


static void 
lgw_vocabularywordview_set_property (GObject      *object,
                                    guint         property_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
    //Declarations
    LgwVocabularyWordView *self = NULL;
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwActionable *actionable = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (object);
    priv = self->priv;
    actionable = LGW_ACTIONABLE (object);

    switch (property_id)
    {
      case PROP_VOCABULARYWORDSTORES:
        lgw_vocabularywordview_set_wordstores (self, g_value_get_pointer (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lgw_vocabularywordview_get_property (GObject      *object,
                                    guint         property_id,
                                    GValue       *value,
                                    GParamSpec   *pspec)
{
    //Declarations
    LgwVocabularyWordView *self = NULL;
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwActionable *actionable = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (object);
    priv = self->priv;
    actionable = LGW_ACTIONABLE (object);

    switch (property_id)
    {
      case PROP_ACTIONS:
        g_value_set_pointer (value, lgw_actionable_get_actions (actionable));
        break;
      case PROP_VOCABULARYWORDSTORES:
        g_value_set_pointer (value, lgw_vocabularywordview_get_wordstores (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}



static void 
lgw_vocabularywordview_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    LgwVocabularyWordView *self = NULL;
    LgwVocabularyWordViewPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_vocabularywordview_parent_class)->constructed (object);
    }

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (object);
    priv = self->priv;
    priv->ui.box = GTK_BOX (self);

    {
      GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
      priv->ui.scrolled_window = GTK_SCROLLED_WINDOW (scrolled_window);
      gtk_scrolled_window_set_shadow_type (priv->ui.scrolled_window, GTK_SHADOW_IN);
      gtk_scrolled_window_set_policy (priv->ui.scrolled_window, GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
      gtk_box_pack_start (priv->ui.box, scrolled_window, TRUE, TRUE, 0);
      gtk_widget_show (scrolled_window);

      {
        GtkStyleContext *context = gtk_widget_get_style_context (scrolled_window);
        gtk_style_context_set_junction_sides (context, GTK_JUNCTION_BOTTOM);
        gtk_widget_reset_style (scrolled_window);
      }

      {
        GtkWidget *tree_view = gtk_tree_view_new ();
        priv->ui.tree_view = GTK_TREE_VIEW (tree_view);
        gtk_tree_view_set_headers_clickable (priv->ui.tree_view, TRUE);
        gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
        gtk_widget_show (tree_view);

        {
            priv->data.tree_selection = gtk_tree_view_get_selection (priv->ui.tree_view);
            gtk_tree_selection_set_mode (priv->data.tree_selection, GTK_SELECTION_MULTIPLE);
            g_object_add_weak_pointer (G_OBJECT (priv->data.tree_selection), (gpointer*) &(priv->data.tree_selection));
        }

        {
          GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
          GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
              gettext("#"),
              renderer,
              "text", LGW_VOCABULARYWORDSTORE_COLUMN_POSITION,
              NULL
          );
          gtk_tree_view_append_column (priv->ui.tree_view, column);
          priv->ui.cell_renderer[CELLRENDERER_POSITION] = renderer;
          priv->ui.tree_view_column[TREEVIEWCOLUMN_POSITION] = column;
        }

        {
          GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
          GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
              gettext("#!"),
              renderer,
              "text", LGW_VOCABULARYWORDSTORE_COLUMN_SAVED_POSITION,
              NULL
          );
          gtk_tree_view_append_column (priv->ui.tree_view, column);
          priv->ui.cell_renderer[CELLRENDERER_SAVED_POSITION] = renderer;
          priv->ui.tree_view_column[TREEVIEWCOLUMN_SAVED_POSITION] = column;
        }

        {
          GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
          GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
              gettext("Word"),
              renderer,
              "text", LGW_VOCABULARYWORDSTORE_COLUMN_KANJI,
              NULL
          );
          gtk_tree_view_column_set_sort_column_id (column, LGW_VOCABULARYWORDSTORE_COLUMN_KANJI);
          gtk_tree_view_append_column (priv->ui.tree_view, column);
          priv->ui.cell_renderer[CELLRENDERER_KANJI] = renderer;
          priv->ui.tree_view_column[TREEVIEWCOLUMN_KANJI] = column;
        }

        {
          GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
          GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
              gettext("Reading"),
              renderer,
              "text", LGW_VOCABULARYWORDSTORE_COLUMN_READING,
              NULL
          );
          gtk_tree_view_column_set_sort_column_id (column, LGW_VOCABULARYWORDSTORE_COLUMN_READING);
          gtk_tree_view_append_column (priv->ui.tree_view, column);
          priv->ui.cell_renderer[CELLRENDERER_READING] = renderer;
          priv->ui.tree_view_column[TREEVIEWCOLUMN_READING] = column;
        }

        {
          GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
          GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
              gettext("Definition"),
              renderer,
              "text", LGW_VOCABULARYWORDSTORE_COLUMN_DEFINITION,
              NULL
          );
          gtk_tree_view_column_set_sort_column_id (column, LGW_VOCABULARYWORDSTORE_COLUMN_DEFINITION);
          gtk_tree_view_append_column (priv->ui.tree_view, column);
          priv->ui.cell_renderer[CELLRENDERER_DEFINITION] = renderer;
          priv->ui.tree_view_column[TREEVIEWCOLUMN_DEFINITION] = column;
        }
      }
    }

    {
      GtkWidget *toolbar = gtk_toolbar_new ();
      priv->ui.toolbar = GTK_TOOLBAR (toolbar);
      gtk_toolbar_set_icon_size (priv->ui.toolbar, GTK_ICON_SIZE_MENU);
      gtk_toolbar_set_show_arrow (priv->ui.toolbar, FALSE);
      gtk_widget_show (toolbar);
      gtk_box_pack_start (priv->ui.box, toolbar, FALSE, FALSE, 0);

      {
        GtkStyleContext *context = gtk_widget_get_style_context (toolbar);
        gtk_style_context_add_class (context, "inline-toolbar");
        gtk_style_context_set_junction_sides (context, GTK_JUNCTION_TOP);
        gtk_widget_reset_style (toolbar);

        {
          GtkIconTheme *theme = gtk_icon_theme_get_default ();

          {
            GtkToolItem *item = gtk_tool_button_new (NULL, NULL);
            if (gtk_icon_theme_has_icon (theme, "list-add-symbolic"))
            {
              gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON (item), "list-add-symbolic");
            }
            else {
              gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON (item), "list-add");
            }
            gtk_toolbar_insert (priv->ui.toolbar, item, -1);
            gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (item), "win.add-new-word");
            gtk_widget_show (GTK_WIDGET (item));
          }
          
          {
            GtkToolItem* item = gtk_tool_button_new (NULL, NULL);
            if (gtk_icon_theme_has_icon (theme, "list-remove-symbolic"))
            {
              gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON (item), "list-remove-symbolic");
            }
            else {
              gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON (item), "list-remove");
            }
            gtk_toolbar_insert (priv->ui.toolbar, item, -1);
            gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (item), "win.delete-selected-words");
            gtk_widget_show (GTK_WIDGET (item));
          }
        }
      }
    }

    lgw_vocabularywordview_connect_signals (self);
}


static void
lgw_vocabularywordview_class_init (LgwVocabularyWordViewClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    _klass = klass;
    _klasspriv = klass->priv = g_new0 (LgwVocabularyWordViewClassPrivate, 1);

    object_class->set_property = lgw_vocabularywordview_set_property;
    object_class->get_property = lgw_vocabularywordview_get_property;
    object_class->constructed = lgw_vocabularywordview_constructed;
    object_class->dispose = lgw_vocabularywordview_dispose;
    object_class->finalize = lgw_vocabularywordview_finalize;

    g_type_class_add_private (object_class, sizeof (LgwVocabularyWordViewPrivate));

    _klasspriv->pspec[PROP_VOCABULARYWORDSTORES] = g_param_spec_pointer (
      "vocabulary-word-stores",
      "used for the datamodel",
      "for the datamodel",
      G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_VOCABULARYWORDSTORES, _klasspriv->pspec[PROP_VOCABULARYWORDSTORES]);

    g_object_class_override_property (object_class, PROP_ACTIONS, "actions");
}


static gboolean
lgw_vocabularywordview_contains_all_wordstores (LgwVocabularyWordView *self,
                                                GList                 *wordstores)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    gboolean changed = FALSE;
    GHashTable *table = NULL;
    gboolean contains_all = TRUE;

    //Initializations
    priv = self->priv;
    table = g_hash_table_new (g_direct_hash, g_direct_equal);
    contains_all = (g_list_length (wordstores) == g_list_length (priv->data.vocabulary_word_stores));
    if (contains_all == FALSE) goto errored;

    {
      GList *link = NULL;
      for (link = priv->data.vocabulary_word_stores; link != NULL; link = link->next)
      {
        LgwVocabularyWordStore *s = LGW_VOCABULARYWORDSTORE (link->data);
        if (s != NULL)
        {
          g_hash_table_add (table, s);
        }
      }
    }

    {
      GList *link = NULL;
      for (link = wordstores; link != NULL && contains_all; link = link->next)
      {
        LgwVocabularyWordStore *s = LGW_VOCABULARYWORDSTORE (link->data);
        if (s != NULL && !g_hash_table_contains (table, s))
        {
          contains_all = FALSE;
        }
      }
    }

errored:

      if (table != NULL) g_hash_table_unref (table); table = NULL;

      return contains_all;
}


void
lgw_vocabularywordview_set_wordstores (LgwVocabularyWordView  *self,
                                       GList                  *wordstores)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwActionable *actionable = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    actionable = LGW_ACTIONABLE (self);
    changed = !lgw_vocabularywordview_contains_all_wordstores (self, wordstores);
    if (!changed) goto errored;
 
    if (wordstores != NULL)
    {
      g_list_foreach (wordstores, (GFunc) g_object_ref, NULL);
      g_list_foreach (wordstores, (GFunc) lw_vocabulary_load, NULL);
    }

    if (priv->data.vocabulary_word_stores != NULL)
    {
      g_list_free_full (priv->data.vocabulary_word_stores, (GDestroyNotify) g_object_unref);
      priv->data.vocabulary_word_stores = NULL;
      priv->data.vocabulary_word_store = NULL;
    }

    if (wordstores != NULL)
    {
      priv->data.vocabulary_word_stores = g_list_copy (wordstores);
      priv->data.vocabulary_word_store = LGW_VOCABULARYWORDSTORE (wordstores->data);
      gtk_tree_view_set_model (priv->ui.tree_view, GTK_TREE_MODEL (wordstores->data));
    }
    else
    {
      gtk_tree_view_set_model (priv->ui.tree_view, NULL);
    }

    lgw_actionable_sync_actions (actionable);
    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_VOCABULARYWORDSTORES]);

errored:

      return;
}


LgwVocabularyWordStore*
lgw_vocabularywordview_get_wordstore (LgwVocabularyWordView  *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.vocabulary_word_store;
}


GList*
lgw_vocabularywordview_get_wordstores (LgwVocabularyWordView  *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    GList *copy = NULL;

    //Initializations
    priv = self->priv;
    copy = g_list_copy (priv->data.vocabulary_word_stores);

    return priv->data.vocabulary_word_stores;
}


void
lgw_vocabularywordview_delete_selected (LgwVocabularyWordView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    GtkTreeModel *tree_model = NULL;
    GList *pathlist = NULL;
    GList *wordlist = NULL;

    //Initializations
    priv = self->priv;
    vocabulary_word_store = lgw_vocabularywordview_get_wordstore (self);
    if (vocabulary_word_store == NULL) goto errored;
    tree_model = GTK_TREE_MODEL (vocabulary_word_store);
    pathlist = gtk_tree_selection_get_selected_rows (priv->data.tree_selection, &tree_model);
    if (pathlist == NULL) goto errored;

    wordlist = lgw_vocabularywordstore_remove (vocabulary_word_store, pathlist);

errored:

    if (pathlist != NULL) g_list_free_full (pathlist, (GDestroyNotify) gtk_tree_path_free); pathlist = NULL;
    if (wordlist != NULL) g_list_free_full (wordlist, (GDestroyNotify) lw_word_free); wordlist = NULL;
}


void
lgw_vocabularywordview_add_new (LgwVocabularyWordView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LwVocabulary *vocabulary = NULL;
    GtkWidget *toplevel = NULL;
    GtkWindow *window = NULL;
    GtkDialog *dialog = NULL;
    GtkWidget *add_vocabulary_dialog = NULL;

    //Initializations
    priv = self->priv;
    vocabulary_word_store = lgw_vocabularywordview_get_wordstore (self);
    if (vocabulary_word_store == NULL) goto errored;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    toplevel = gtk_widget_get_toplevel (GTK_WIDGET (self));
    if (gtk_widget_is_toplevel (toplevel))
    {
      window = GTK_WINDOW (toplevel);
    }
    add_vocabulary_dialog = lgw_addvocabularydialog_new (window);
    dialog = GTK_DIALOG (add_vocabulary_dialog);

    lgw_addvocabularydialog_set_wordstore (LGW_ADDVOCABULARYDIALOG (add_vocabulary_dialog), vocabulary_word_store);
    gint response = gtk_dialog_run (GTK_DIALOG (add_vocabulary_dialog));

    //Select the added item
    if (response == LGW_ADDVOCABULARYDIALOG_RESPONSE_ADD)
    {
      gint length = lw_vocabulary_length (vocabulary);
      GtkTreePath *tree_path = gtk_tree_path_new_from_indices (length - 1, -1);
      if (tree_path != NULL)
      {
        gtk_tree_view_set_cursor (priv->ui.tree_view, tree_path, NULL, FALSE);
        gtk_tree_path_free (tree_path); tree_path = NULL;
      }
    }

errored:

    if (add_vocabulary_dialog != NULL) gtk_widget_destroy (add_vocabulary_dialog);
}


void
lgw_vocabularywordview_save (LgwVocabularyWordView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LwVocabulary *vocabulary = NULL;

    //Initializations
    vocabulary_word_store = lgw_vocabularywordview_get_wordstore (self);
    if (vocabulary_word_store == NULL) goto errored;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);

    lw_vocabulary_save (vocabulary, NULL);

errored:

   return;
}


void
lgw_vocabularywordview_revert (LgwVocabularyWordView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LwVocabulary *vocabulary = NULL;

    //Initializations
    vocabulary_word_store = lgw_vocabularywordview_get_wordstore (self);
    if (vocabulary_word_store == NULL) goto errored;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);

    lw_vocabulary_load (vocabulary, NULL);

errored:

   return;
}


GList*
lgw_treeview_get_selected_words (GtkTreeView *self)
{
    //Sanity checks
    g_return_if_fail (GTK_IS_TREE_VIEW (self));

    //Declarations
    GtkTreeModel *tree_model = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LwVocabulary *vocabulary = NULL;
    GtkTreeSelection *tree_selection = NULL;
    GList *selected_rows = NULL;
    GList *selected_wordstores = NULL;

    //Initializations
    tree_model = GTK_TREE_MODEL (gtk_tree_view_get_model (self));
    if (!LGW_IS_VOCABULARYWORDSTORE (tree_model)) goto errored;
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (tree_model);
    vocabulary = LW_VOCABULARY (tree_model);
    tree_selection = gtk_tree_view_get_selection (self);
    selected_rows = gtk_tree_selection_get_selected_rows (tree_selection, &tree_model);
    if (selected_rows == NULL) goto errored;

    {
      GList *link = NULL;
      for (link = selected_rows; link != NULL; link = link->next)
      {
        GtkTreePath *tree_path = link->data;
        LwWord *word = lgw_vocabularywordstore_get_word (vocabulary_word_store, tree_path);
        selected_wordstores = g_list_prepend (selected_wordstores, word);
      }
    }

errored:

    if (selected_rows != NULL)
    {
      g_list_free_full (selected_rows, (GDestroyNotify) gtk_tree_path_free);
      selected_rows = NULL;
    }

    selected_wordstores = g_list_reverse (selected_wordstores);

    return selected_wordstores;
}


GList*
lgw_vocabularywordview_get_selected_words (LgwVocabularyWordView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    GList *selected_words = NULL;

    //Initializations
    priv = self->priv;
    if (priv->ui.tree_view == NULL) goto errored;
    selected_words = lgw_treeview_get_selected_words (priv->ui.tree_view);

errored:

    return selected_words;
}


void
lgw_vocabularywordview_select (LgwVocabularyWordView *self,
                               GList                 *tree_paths)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;

    //Initializations
    priv = self->priv;
    vocabulary_word_store = priv->data.vocabulary_word_store;
    if (vocabulary_word_store == NULL) goto errored;
    if (priv->data.tree_selection == NULL) goto errored;

    gtk_tree_selection_unselect_all (priv->data.tree_selection);

    {
      GList *link = NULL;
      for (link = tree_paths; link != NULL; link = link->next)
      {
        GtkTreePath *tree_path = link->data;
        if (tree_path != NULL)
        {
          gtk_tree_selection_select_path (priv->data.tree_selection, tree_path);
        }
      }
    }

errored:

    return;
}


void
lgw_vocabularywordview_select_words (LgwVocabularyWordView *self,
                                     GList                 *words)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    GList *tree_paths = NULL;

    //Initializations
    priv = self->priv;
    if (priv->data.tree_selection == NULL) goto errored;
    vocabulary_word_store = priv->data.vocabulary_word_store;
    if (vocabulary_word_store == NULL) goto errored;
    tree_paths = lgw_vocabularywordstore_get_tree_paths (vocabulary_word_store, words);

    gtk_tree_selection_unselect_all (priv->data.tree_selection);

    {
      GList *link = NULL;
      for (link = tree_paths; link != NULL; link = link->next)
      {
        GtkTreePath *tree_path = link->data;
        if (tree_path != NULL)
        {
          gtk_tree_selection_select_path (priv->data.tree_selection, tree_path);
        }
      }
    }

errored:

    if (tree_paths != NULL) g_list_free_full (tree_paths, (GDestroyNotify) gtk_tree_path_free); tree_paths = NULL;

    return;
}


void
lgw_vocabularywordview_paste (LgwVocabularyWordView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    GtkClipboard *clipboard = NULL;
    gchar *text = NULL;
    LwVocabulary *reader = NULL;
    GList* words = NULL;
    gint selection_count = -1;
    GtkTreePath *tree_path = NULL;
    GList *inserted_tree_paths = NULL;

    //Initializations
    priv = self->priv;
    vocabulary_word_store = priv->data.vocabulary_word_store;
    if (vocabulary_word_store == NULL) goto errored;
    clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
    if (clipboard == NULL) goto errored;
    reader = lw_vocabulary_new (NULL);
    if (reader == NULL) goto errored;
    text = gtk_clipboard_wait_for_text (clipboard);
    if (text == NULL) goto errored;

    lw_vocabulary_load_from_string (reader, text, NULL);

    words = lw_vocabulary_remove (reader, NULL);
    if (words == NULL) goto errored;
    selection_count = gtk_tree_selection_count_selected_rows (priv->data.tree_selection);
    gtk_tree_view_get_cursor (priv->ui.tree_view, &tree_path, NULL);
    if (tree_path != NULL && selection_count > 0) gtk_tree_path_next (tree_path);

    inserted_tree_paths = lgw_vocabularywordstore_insert (vocabulary_word_store, tree_path, words);
    lgw_vocabularywordview_select (self, inserted_tree_paths);

errored:

    g_free (text); text = NULL;
    if (reader != NULL) g_object_unref (reader); reader = NULL;
    if (words != NULL) g_list_free (words);
    if (tree_path != NULL) gtk_tree_path_free (tree_path); tree_path = NULL;
    if (inserted_tree_paths != NULL) g_list_free_full (inserted_tree_paths, (GDestroyNotify) gtk_tree_path_free); inserted_tree_paths = NULL;
}


void
lgw_vocabularywordview_cut_selected (LgwVocabularyWordView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    lgw_vocabularywordview_copy_selected (self);
    lgw_vocabularywordview_delete_selected (self);
}


void
lgw_vocabularywordview_copy_selected (LgwVocabularyWordView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    GtkClipboard *clipboard = NULL;
    LwVocabulary *vocabulary = NULL;
    LwVocabulary *writer = NULL;
    GList* words = NULL;
    GList *copy = NULL;
    gchar *text = NULL;

    //Initializations
    clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
    if (clipboard == NULL) goto errored;
    writer = lw_vocabulary_new (NULL);
    if (writer == NULL) goto errored;
    words = lgw_vocabularywordview_get_selected_words (self);
    if (words == NULL) goto errored;
    copy = g_list_copy_deep (words, (GCopyFunc) lw_word_copy, NULL);
    if (copy == NULL) goto errored;

    lw_vocabulary_insert (writer, -1, copy);

    text = lw_vocabulary_to_string (writer, NULL);
    if (text == NULL) goto errored;

    gtk_clipboard_set_text (clipboard, text, -1);

errored:

    if (words != NULL) g_list_free (words); words = NULL;
    if (copy != NULL) g_list_free (copy); copy = NULL;
    if (writer != NULL) g_object_unref (writer); writer = NULL;
    g_free (text); text = NULL;

    return;
}


void
lgw_vocabularywordview_sync_editable (LgwVocabularyWordView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    gint count = -1;
    gboolean editable = FALSE;
    gint index[] = {
      CELLRENDERER_KANJI,
      CELLRENDERER_READING,
      CELLRENDERER_DEFINITION,
      TOTAL_CELLRENDERERS
    };

    //Initializations
    priv = self->priv;
    count = gtk_tree_selection_count_selected_rows (priv->data.tree_selection);
    editable = (count == 1);

    {
      gint i = 0;
      for (i = 0; index[i] != TOTAL_CELLRENDERERS; i++) {
        GObject *o = G_OBJECT (priv->ui.cell_renderer[index[i]]);
        g_object_set (o, "editable", editable, NULL);
      }
    }
}


GtkTreePath*
lgw_vocabularywordview_get_tree_path (LgwVocabularyWordView *self,
                                      GdkDragContext        *drag_context,
                                      gint                   x,
                                      gint                   y)
{
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LwVocabulary *vocabulary = NULL;
    GtkTreePath *tree_path = NULL;
    GtkTreeViewDropPosition drop_position = 0;
    gint length = 0;

    //Initializations
    priv = self->priv;
    vocabulary_word_store = lgw_vocabularywordview_get_wordstore (self);
    if (vocabulary_word_store == NULL) goto errored;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    if (vocabulary == NULL) goto errored;
    gtk_tree_view_get_dest_row_at_pos (priv->ui.tree_view, x, y, &tree_path, &drop_position);
    if (tree_path == NULL) goto errored;
    length = lw_vocabulary_length (vocabulary);

    if (drop_position == GTK_TREE_VIEW_DROP_INTO_OR_AFTER || drop_position == GTK_TREE_VIEW_DROP_AFTER)
    {
      gtk_tree_path_next (tree_path);
    }

errored:

    return tree_path;
}


