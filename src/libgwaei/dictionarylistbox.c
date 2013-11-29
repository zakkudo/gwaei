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

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/dictionarylistbox-private.h>

static void lgw_dictionarylistbox_init (LgwDictionaryListBox *widget);

G_DEFINE_TYPE (LgwDictionaryListBox, lgw_dictionarylistbox, GTK_TYPE_BOX)

typedef enum
{
  PROP_0,
  PROP_DICTIONARYLIST
} LgwDictionaryListBoxProps;

//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_dictionarylistbox_new ()
{
    //Declarations
    LgwDictionaryListBox *widget = NULL;

    //Initializations
    widget = LGW_DICTIONARYLISTBOX (g_object_new (LGW_TYPE_DICTIONARYLISTBOX, "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 0, NULL));

    return GTK_WIDGET (widget);
}


static void 
lgw_dictionarylistbox_init (LgwDictionaryListBox *widget)
{
    widget->priv = LGW_DICTIONARYLISTBOX_GET_PRIVATE (widget);
    memset(widget->priv, 0, sizeof(LgwDictionaryListBoxPrivate));

    LgwDictionaryListBoxPrivate *priv;
    priv = widget->priv;
}


static void 
lgw_dictionarylistbox_finalize (GObject *object)
{
    LgwDictionaryListBox *widget;
    LgwDictionaryListBoxPrivate *priv;

    widget = LGW_DICTIONARYLISTBOX (object);
    priv = widget->priv;

    G_OBJECT_CLASS (lgw_dictionarylistbox_parent_class)->finalize (object);
}


static void 
lgw_dictionarylistbox_set_property (GObject      *object,
                                    guint         property_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
    LgwDictionaryListBox *dictionarylistbox = NULL;
    LgwDictionaryListBoxPrivate *priv = NULL;

    dictionarylistbox = LGW_DICTIONARYLISTBOX (object);
    priv = dictionarylistbox->priv;

    switch (property_id)
    {
      case PROP_DICTIONARYLIST:
        priv->data.dictionary_list = LW_DICTIONARYLIST (g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lgw_dictionarylistbox_get_property (GObject      *object,
                                    guint         property_id,
                                    GValue       *value,
                                    GParamSpec   *pspec)
{
    LgwDictionaryListBox *dictionarylistbox = NULL;
    LgwDictionaryListBoxPrivate *priv = NULL;

    dictionarylistbox = LGW_DICTIONARYLISTBOX (object);
    priv = dictionarylistbox->priv;

    switch (property_id)
    {
      case PROP_DICTIONARYLIST:
        g_value_set_object (value, priv->data.dictionary_list);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}



static void 
lgw_dictionarylistbox_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    LgwDictionaryListBox *widget = NULL;
    LgwDictionaryListBoxPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_dictionarylistbox_parent_class)->constructed (object);
    }

    //Initializations
    widget = LGW_DICTIONARYLISTBOX (object);
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
        GtkWidget *list_box = gtk_list_box_new ();
        priv->ui.list_box = GTK_LIST_BOX (list_box);
        gtk_container_add (GTK_CONTAINER (scrolled_window), list_box);
        gtk_widget_show (list_box);
      }
    }

    {
      GtkWidget *toolbar = gtk_toolbar_new ();
      priv->ui.toolbar = GTK_TOOLBAR (toolbar);
      gtk_toolbar_set_icon_size (priv->ui.toolbar, GTK_ICON_SIZE_MENU);
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
            gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (item), "win.add-dictionary");
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
            gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (item), "win.remove-dictionary");
            gtk_widget_show (GTK_WIDGET (item));
          }
        }
      }
    }

/*
    {
        GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
        gtk_cell_renderer_set_padding (renderer, 6, 4);
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (gettext("Dictionary"), renderer, "text", LGW_DICTIONARYLIST_COLUMN_LONG_NAME, NULL);
        gtk_tree_view_column_set_min_width (column, 100);
        gtk_tree_view_append_column (priv->ui.tree_view, column);
    }
    */


/*TODO
    priv = window->priv;
    application = gw_window_get_application (GW_WINDOW (window));
    dictionarylist = gw_application_get_installed_dictionarylist (application);
    liststore = gw_dictionarylist_get_liststore (dictionarylist);
    treemodel = GTK_TREE_MODEL (liststore);
    view = priv->manage_dictionaries_treeview;
    selection = gtk_tree_view_get_selection (view);

    gtk_tree_view_set_model (view, treemodel);

    //Create the columns and renderer for each column
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_cell_renderer_set_padding (renderer, 6, 4);
    column = gtk_tree_view_column_new ();
    gtk_tree_view_column_set_title (column, " ");
    gtk_tree_view_column_pack_start (column, renderer, TRUE);
    gtk_tree_view_column_set_attributes (column, renderer, "icon-name", GW_DICTIONARYLIST_COLUMN_IMAGE, NULL);
    gtk_tree_view_append_column (view, column);

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_padding (renderer, 6, 4);
    column = gtk_tree_view_column_new_with_attributes ("#", renderer, "text", GW_DICTIONARYLIST_COLUMN_POSITION, NULL);
    gtk_tree_view_append_column (view, column);

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_padding (renderer, 6, 4);
    column = gtk_tree_view_column_new_with_attributes (gettext("Name"), renderer, "text", GW_DICTIONARYLIST_COLUMN_LONG_NAME, NULL);
    gtk_tree_view_column_set_min_width (column, 100);
    gtk_tree_view_append_column (view, column);

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_padding (renderer, 6, 4);
    column = gtk_tree_view_column_new_with_attributes (gettext("Engine"), renderer, "text", GW_DICTIONARYLIST_COLUMN_ENGINE, NULL);
    gtk_tree_view_append_column (view, column);

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_padding (renderer, 6, 4);
    column = gtk_tree_view_column_new_with_attributes (gettext("Shortcut"), renderer, "text", GW_DICTIONARYLIST_COLUMN_SHORTCUT, NULL);
    gtk_tree_view_append_column (view, column);

    gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
*/


}


static void
lgw_dictionarylistbox_class_init (LgwDictionaryListBoxClass *klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = lgw_dictionarylistbox_constructed;
    object_class->finalize = lgw_dictionarylistbox_finalize;

    g_type_class_add_private (object_class, sizeof (LgwDictionaryListBoxPrivate));
/* TODO
    klass->signalid[GW_ADDVOCABULARYWINDOW_CLASS_SIGNALID_WORD_ADDED] = g_signal_new (
        "word-added",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (GwAddVocabularyWindowClass, word_added),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );
*/
}


void
lw_dictionarylistbox_set_dictionarylist (LgwDictionaryListBox *box,
                                         LwDictionaryList* dictionarylist)
{
}

