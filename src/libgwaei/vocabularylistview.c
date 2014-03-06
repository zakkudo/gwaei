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
//! @file vocabularylistview.c
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

#include <libgwaei/vocabularylistview-private.h>

static LgwVocabularyListViewClass *_klass = NULL;
static LgwVocabularyListViewClassPrivate *_klasspriv = NULL;

G_DEFINE_TYPE_WITH_CODE (LgwVocabularyListView, lgw_vocabularylistview, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_ACTIONABLE, lgw_vocabularylistview_implement_actionable_interface));

//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_vocabularylistview_new ()
{
    //Declarations
    LgwVocabularyListView *widget = NULL;

    //Initializations
    widget = LGW_VOCABULARYLISTVIEW (g_object_new (LGW_TYPE_VOCABULARYLISTVIEW, "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 0, NULL));

    return GTK_WIDGET (widget);
}


static void 
lgw_vocabularylistview_init (LgwVocabularyListView *widget)
{
    widget->priv = LGW_VOCABULARYLISTVIEW_GET_PRIVATE (widget);
    memset(widget->priv, 0, sizeof(LgwVocabularyListViewPrivate));

    LgwVocabularyListViewPrivate *priv;
    priv = widget->priv;
}


static void 
lgw_vocabularylistview_finalize (GObject *object)
{
    LgwVocabularyListView *widget = NULL;
    LgwVocabularyListViewPrivate *priv = NULL;

    widget = LGW_VOCABULARYLISTVIEW (object);
    priv = widget->priv;

    G_OBJECT_CLASS (lgw_vocabularylistview_parent_class)->finalize (object);
}


static void
lgw_vocabularylistview_dispose (GObject *object)
{
    //Declarations
    LgwVocabularyListView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTVIEW (object);

    lgw_vocabularylistview_disconnect_signals (self);

    G_OBJECT_CLASS (lgw_vocabularylistview_parent_class)->dispose (object);
}


static void 
lgw_vocabularylistview_set_property (GObject      *object,
                                    guint         property_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
    LgwVocabularyListView *self = NULL;
    LgwVocabularyListViewPrivate *priv = NULL;
    LgwActionable *actionable = NULL;

    self = LGW_VOCABULARYLISTVIEW (object);
    priv = self->priv;
    actionable = LGW_ACTIONABLE (object);

    switch (property_id)
    {
      case PROP_VOCABULARYLISTSTORE:
        lgw_vocabularylistview_set_liststore (self, g_value_get_object (value));
        break;
      case PROP_VOCABULARYWORDVIEW:
        lgw_vocabularylistview_set_wordview (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lgw_vocabularylistview_get_property (GObject      *object,
                                    guint         property_id,
                                    GValue       *value,
                                    GParamSpec   *pspec)
{
    LgwVocabularyListView *self = NULL;
    LgwVocabularyListViewPrivate *priv = NULL;
    LgwActionable *actionable = NULL;

    self = LGW_VOCABULARYLISTVIEW (object);
    priv = self->priv;
    actionable = LGW_ACTIONABLE (object);

    switch (property_id)
    {
      case PROP_ACTIONS:
        g_value_set_pointer (value, lgw_actionable_get_actions (actionable));
        break;
      case PROP_VOCABULARYLISTSTORE:
        g_value_set_object (value, lgw_vocabularylistview_get_liststore (self));
        break;
      case PROP_VOCABULARYWORDVIEW:
        g_value_set_object (value, lgw_vocabularylistview_get_wordview (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lgw_vocabularylistview_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    LgwVocabularyListView *widget = NULL;
    LgwVocabularyListViewPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_vocabularylistview_parent_class)->constructed (object);
    }

    //Initializations
    widget = LGW_VOCABULARYLISTVIEW (object);
    priv = widget->priv;
    priv->ui.box = GTK_BOX (widget);

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

        const GtkTargetEntry row_targets[] = {
          { "GTK_TREE_MODEL_ROW", GTK_TARGET_SAME_APP, 0 }
        };

        gtk_tree_view_enable_model_drag_source (
          priv->ui.tree_view,
          GDK_BUTTON1_MASK,
          row_targets,
          G_N_ELEMENTS (row_targets),
          GDK_ACTION_MOVE
        );

        gtk_tree_view_enable_model_drag_dest (
          priv->ui.tree_view,
          row_targets,
          G_N_ELEMENTS (row_targets),
          GDK_ACTION_MOVE | GDK_ACTION_COPY
        );

        gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
        gtk_widget_show (tree_view);

        {
            priv->data.tree_selection = gtk_tree_view_get_selection (priv->ui.tree_view);
            gtk_tree_selection_set_mode (priv->data.tree_selection, GTK_SELECTION_MULTIPLE);
            g_object_add_weak_pointer (G_OBJECT (priv->data.tree_selection), (gpointer*) &(priv->data.tree_selection));
        }

        {
          GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
          priv->ui.cell_renderer[CELLRENDERER_POSITION] = renderer;

          {
            GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
                gettext("#"),
                renderer,
                "text", LGW_VOCABULARYLISTSTORE_COLUMN_POSITION,
                "weight", LGW_VOCABULARYLISTSTORE_COLUMN_STYLE_WEIGHT,
                NULL
            );
            gtk_tree_view_append_column (priv->ui.tree_view, column);
            priv->ui.tree_view_column[TREEVIEWCOLUMN_POSITION] = column;
          }
        }

        {
          GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
          priv->ui.cell_renderer[CELLRENDERER_SAVED_POSITION] = renderer;

          {
            GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
                gettext("!#"),
                renderer,
                "text", LGW_VOCABULARYLISTSTORE_COLUMN_SAVED_POSITION,
                "weight", LGW_VOCABULARYLISTSTORE_COLUMN_STYLE_WEIGHT,
                NULL
            );
            gtk_tree_view_append_column (priv->ui.tree_view, column);
            priv->ui.tree_view_column[TREEVIEWCOLUMN_SAVED_POSITION] = column;
          }
        }

        {
          GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
          priv->ui.cell_renderer[CELLRENDERER_NAME] = renderer;
          g_object_set (G_OBJECT (renderer), "editable", TRUE, NULL);

          {
            GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
                gettext("Vocabulary List"),
                renderer,
                "text", LGW_VOCABULARYLISTSTORE_COLUMN_NAME,
                "weight", LGW_VOCABULARYLISTSTORE_COLUMN_STYLE_WEIGHT,
                NULL
            );
            gtk_tree_view_append_column (priv->ui.tree_view, column);
            priv->ui.tree_view_column[TREEVIEWCOLUMN_NAME] = column;
          }
        }

        {
          GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
          priv->ui.cell_renderer[CELLRENDERER_TOTAL_WORDS] = renderer;
          g_object_set (G_OBJECT (renderer), "editable", TRUE, NULL);

          {
            GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
                gettext("Total"),
                renderer,
                "text", LGW_VOCABULARYLISTSTORE_COLUMN_TOTAL_WORDS,
                NULL
            );
            gtk_tree_view_append_column (priv->ui.tree_view, column);
            priv->ui.tree_view_column[TREEVIEWCOLUMN_TOTAL_WORDS] = column;
          }
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
            gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (item), "win.add-new-vocabulary-list");
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
            gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (item), "win.remove-selected-vocabulary-lists");
            gtk_widget_show (GTK_WIDGET (item));
          }
        }
      }
    }

    lgw_vocabularylistview_connect_signals (widget);
}


static void
lgw_vocabularylistview_class_init (LgwVocabularyListViewClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);

    _klass = klass;
    _klasspriv = _klass->priv = g_new0(LgwVocabularyListViewClassPrivate, 1);

    object_class->set_property = lgw_vocabularylistview_set_property;
    object_class->get_property = lgw_vocabularylistview_get_property;
    object_class->constructed = lgw_vocabularylistview_constructed;
    object_class->finalize = lgw_vocabularylistview_finalize;
    object_class->dispose = lgw_vocabularylistview_dispose;

    g_type_class_add_private (object_class, sizeof (LgwVocabularyListViewPrivate));

    _klasspriv->pspec[PROP_VOCABULARYLISTSTORE] = g_param_spec_object (
      "vocabulary-list-store",
      "used for the datamodel",
      "for the datamodel",
      LGW_TYPE_VOCABULARYLISTSTORE,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_VOCABULARYLISTSTORE, _klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);

    _klasspriv->pspec[PROP_VOCABULARYWORDVIEW] = g_param_spec_object (
      "vocabulary-word-view",
      "used for the datamodel",
      "for the datamodel",
      LGW_TYPE_VOCABULARYWORDVIEW,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_VOCABULARYWORDVIEW, _klasspriv->pspec[PROP_VOCABULARYWORDVIEW]);

    g_object_class_override_property (object_class, PROP_ACTIONS, "actions");
}


void
lgw_vocabularylistview_set_liststore (LgwVocabularyListView  *self,
                                      LgwVocabularyListStore *vocabulary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;
    LgwActionable *actionable = NULL;

    //Initializations
    priv = self->priv;
    actionable = LGW_ACTIONABLE (self);
    
    if (vocabulary_list_store != priv->data.vocabulary_list_store)
    {
      if (vocabulary_list_store != NULL)
      {
        g_object_ref (vocabulary_list_store);
      }

      if (priv->data.vocabulary_list_store != NULL)
      {
        g_object_remove_weak_pointer (G_OBJECT (priv->data.vocabulary_list_store), (gpointer*) &(priv->data.vocabulary_list_store));
        g_object_unref (priv->data.vocabulary_list_store);
        priv->data.vocabulary_list_store = NULL;
      }

      priv->data.vocabulary_list_store = vocabulary_list_store;

      if (priv->data.vocabulary_list_store != NULL)
      {
        g_object_add_weak_pointer (G_OBJECT (priv->data.vocabulary_list_store), (gpointer*) &(priv->data.vocabulary_list_store));
      }

      gtk_tree_view_set_model (priv->ui.tree_view, GTK_TREE_MODEL (vocabulary_list_store));
      lgw_actionable_sync_actions (actionable);

      g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);
    }
}


LgwVocabularyListStore*
lgw_vocabularylistview_get_liststore (LgwVocabularyListView  *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.vocabulary_list_store;
}


void
lgw_vocabularylistview_set_wordview (LgwVocabularyListView *self,
                                     LgwVocabularyWordView *vocabulary_word_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;
    GList *selected_wordstores = NULL;

    //Initializations
    priv = self->priv;
    selected_wordstores = lgw_vocabularylistview_get_selected_wordstores (self);

    if (priv->config.vocabulary_word_view != NULL)
    {
      g_object_remove_weak_pointer (G_OBJECT (vocabulary_word_view), (gpointer*) &(priv->config.vocabulary_word_view));
      priv->config.vocabulary_word_view = NULL;
    }

    priv->config.vocabulary_word_view = vocabulary_word_view;

    if (priv->config.vocabulary_word_view != NULL)
    {
      g_object_add_weak_pointer (G_OBJECT (vocabulary_word_view), (gpointer*) &(priv->config.vocabulary_word_view));
      lgw_vocabularywordview_set_wordstores (priv->config.vocabulary_word_view, selected_wordstores);
    }
}


LgwVocabularyWordView*
lgw_vocabularylistview_get_wordview (LgwVocabularyListView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.vocabulary_word_view;
}


GList*
lgw_treeview_get_selected_wordstores (GtkTreeView *self)
{
    //Sanity checks
    g_return_if_fail (GTK_IS_TREE_VIEW (self));

    //Declarations
    GtkTreeModel *tree_model = NULL;
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    GtkTreeSelection *tree_selection = NULL;
    GList *selected_rows = NULL;
    GList *selected_wordstores = NULL;

    //Initializations
    tree_model = GTK_TREE_MODEL (gtk_tree_view_get_model (self));
    if (!LGW_IS_VOCABULARYLISTSTORE (tree_model)) goto errored;
    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (tree_model);
    tree_selection = gtk_tree_view_get_selection (self);
    if (tree_selection == NULL) goto errored;
    selected_rows = gtk_tree_selection_get_selected_rows (tree_selection, &tree_model);
    printf("BREAK gtk_tree_selection_get_selected_rows %d\n", g_list_length (selected_rows));
    if (selected_rows == NULL) goto errored;

    {
      GList *link = NULL;
      for (link = selected_rows; link != NULL; link = link->next)
      {
        GtkTreePath *tree_path = link->data;
        LgwVocabularyWordStore *vocabulary_word_store = lgw_vocabularyliststore_get_wordstore (vocabulary_list_store, tree_path);
        selected_wordstores = g_list_prepend (selected_wordstores, vocabulary_word_store);
        printf("BREAK lgw_vocabularyliststore_get_wordstore %d\n", g_list_length (selected_wordstores));
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
lgw_vocabularylistview_get_selected_wordstores (LgwVocabularyListView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;
    GList *selected_wordstores = NULL;

    //Initializations
    priv = self->priv;
    if (priv->ui.tree_view == NULL) goto errored;
    selected_wordstores = lgw_treeview_get_selected_wordstores (priv->ui.tree_view);

errored:

    return selected_wordstores;
}


void
lgw_vocabularylistview_add_new (LgwVocabularyListView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    gchar *filename = NULL;
    GList *wordstorelist = NULL;
    gint length = 0;

    GtkTreePath *path = NULL;

    //Initializations
    priv = self->priv;
    vocabulary_list_store = lgw_vocabularylistview_get_liststore (self);
    if (vocabulary_list_store == NULL) goto errored;
    filename = lw_vocabulary_generate_new_filename ();
    if (filename == NULL) goto errored;
    vocabulary_word_store = lgw_vocabularywordstore_new (filename);
    if (vocabulary_word_store == NULL) goto errored;
    wordstorelist = g_list_prepend (wordstorelist, vocabulary_word_store);
    if (wordstorelist == NULL) goto errored;

    length = lgw_vocabularyliststore_length (vocabulary_list_store);

    path = gtk_tree_path_new_from_indices (length, -1);
    if (path == NULL) goto errored;

    lgw_vocabularyliststore_insert_all (vocabulary_list_store, -1, wordstorelist);
    gtk_widget_grab_focus (GTK_WIDGET (priv->ui.tree_view));
    gtk_tree_view_set_cursor (priv->ui.tree_view, path, priv->ui.tree_view_column[TREEVIEWCOLUMN_NAME], TRUE);

errored:

    if (filename != NULL) g_free (filename); filename = NULL;
    if (wordstorelist != NULL) g_list_free (wordstorelist); wordstorelist = NULL;
    if (path != NULL) gtk_tree_path_free (path); path = NULL;
}


void
lgw_vocabularylistview_delete_selected (LgwVocabularyListView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    GList *pathlist = NULL;
    GtkTreeModel *tree_model = NULL;
    gint length = 0;
    gint *positions = NULL;
    GList *wordstorelist = NULL;

    //Initializations
    priv = self->priv;
    vocabulary_list_store = lgw_vocabularylistview_get_liststore (self);
    if (vocabulary_list_store == NULL) goto errored;
    tree_model = GTK_TREE_MODEL (vocabulary_list_store);
    if (tree_model == NULL) goto errored;
    pathlist = gtk_tree_selection_get_selected_rows (priv->data.tree_selection, &tree_model);
    if (pathlist == NULL) goto errored;
    length = g_list_length (pathlist);
    if (length < 1) goto errored;
    positions = g_new0 (gint, length + 1);
    if (positions == NULL) goto errored;

    {
      GList *link = NULL;
      gint i = 0;
      for (link = pathlist; link != NULL; link = link->next)
      {
        GtkTreePath *path = link->data;
        if (path != NULL)
        {
          gint* indices = gtk_tree_path_get_indices (path);
          if (indices != NULL)
          {
            positions[i] = indices[0];
            i++;
          }
        }
      }
      positions[i] = -1;
    }

    wordstorelist = lgw_vocabularyliststore_delete_all (vocabulary_list_store, positions);

errored:

    if (positions != NULL) g_free (positions); positions = NULL;
    if (pathlist != NULL) g_list_free_full (pathlist, (GDestroyNotify) gtk_tree_path_free); pathlist = NULL;
    if (wordstorelist != NULL) g_list_free_full (wordstorelist, (GDestroyNotify) g_object_unref);
}


void
lgw_vocabularylistview_select_wordstores (LgwVocabularyListView *self,
                                          GList                 *wordstores)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;
    LgwVocabularyListStore *vocabulary_list_store = NULL;

    //Initializations
    priv = self->priv;
    vocabulary_list_store = priv->data.vocabulary_list_store;
    if (vocabulary_list_store == NULL) goto errored;
    if (priv->data.tree_selection == NULL) goto errored;

    gtk_tree_selection_unselect_all (priv->data.tree_selection);

    {
      GList *link = NULL;
      for (link = wordstores; link != NULL; link = link->next)
      {
        LgwVocabularyWordStore *wordstore = LGW_VOCABULARYWORDSTORE (link->data);
        if (wordstore != NULL)
        {
          GtkTreePath *tree_path = lgw_vocabularyliststore_find_by_wordstore (vocabulary_list_store, wordstore);
          if (tree_path != NULL)
          {
            gtk_tree_selection_select_path (priv->data.tree_selection, tree_path);
            gtk_tree_path_free (tree_path); tree_path = NULL;
          }
        }
      }
    }

errored:

    return;
}


