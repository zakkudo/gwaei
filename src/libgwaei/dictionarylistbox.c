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

#include <libgwaei/dictionarylistbox-private.h>

static void lgw_dictionarylistbox_init (LgwDictionaryListBox *widget);

G_DEFINE_TYPE (LgwDictionaryListBox, lgw_dictionarylistbox, GTK_TYPE_BOX)

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
    LgwDictionaryListBox *self = NULL;
    LgwDictionaryListBoxPrivate *priv = NULL;

    self = LGW_DICTIONARYLISTBOX (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_EDITABLE:
        lgw_dictionarylistbox_set_editable (self, g_value_get_boolean (value));
        break;
      case PROP_DICTIONARYLISTSTORE:
        lgw_dictionarylistbox_set_dictionaryliststore (self, g_value_get_object (value));
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
    LgwDictionaryListBox *self = NULL;
    LgwDictionaryListBoxPrivate *priv = NULL;

    self = LGW_DICTIONARYLISTBOX (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_EDITABLE:
        g_value_set_boolean (value, lgw_dictionarylistbox_is_editable (self));
        break;
      case PROP_DICTIONARYLISTSTORE:
        g_value_set_object (value, lgw_dictionarylistbox_get_dictionaryliststore (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
_update_header (GtkListBoxRow        *row,
                GtkListBoxRow        *before,
                LgwDictionaryListBox *list_box)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DICTIONARYLISTBOX (list_box));
    if (before != NULL) return;

/* CURRENTLY UNUSED
    //Declarations
    GtkWidget *current_header = NULL;

    //Initializations
    current_header = gtk_list_box_row_get_header (row);

    if (current_header == NULL)
    {
      GtkWidget *label = gtk_label_new (gettext("Dictionary"));
      if (label != NULL)
      {
        gtk_list_box_row_set_header (row, label);
      }
    }
*/
}


static void 
lgw_dictionarylistbox_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    LgwDictionaryListBox *self = NULL;
    LgwDictionaryListBoxPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_dictionarylistbox_parent_class)->constructed (object);
    }

    //Initializations
    self = LGW_DICTIONARYLISTBOX (object);
    priv = self->priv;
    priv->ui.box = GTK_BOX (self);

    {
      GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
      priv->ui.scrolled_window = GTK_SCROLLED_WINDOW (scrolled_window);
      //gtk_scrolled_window_set_shadow_type (priv->ui.scrolled_window, GTK_SHADOW_IN);
      gtk_scrolled_window_set_policy (priv->ui.scrolled_window, GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
      gtk_box_pack_start (priv->ui.box, scrolled_window, TRUE, TRUE, 0);
      gtk_widget_show (scrolled_window);

      {
        GtkStyleContext *context = gtk_widget_get_style_context (scrolled_window);
        gtk_style_context_set_junction_sides (context, GTK_JUNCTION_BOTTOM);
        gtk_widget_reset_style (scrolled_window);
      }

      {
        GtkWidget *list_box = gtk_list_box_new ();
        priv->ui.list_box = GTK_LIST_BOX (list_box);
        gtk_container_add (GTK_CONTAINER (scrolled_window), list_box);
        gtk_widget_set_size_request (list_box, 150, -1);
        gtk_widget_show (list_box);

        {
          GtkWidget *label = gtk_label_new (NULL);
          gtk_label_set_markup (GTK_LABEL (label), "Click + to add a dictionary");
          gtk_list_box_set_placeholder (priv->ui.list_box, label);
          gtk_widget_show (label);
        }
      }
    }

    {
      GtkWidget *toolbar = gtk_toolbar_new ();
      priv->ui.toolbar = GTK_TOOLBAR (toolbar);
      gtk_toolbar_set_icon_size (priv->ui.toolbar, GTK_ICON_SIZE_MENU);
      gtk_toolbar_set_show_arrow (priv->ui.toolbar, FALSE);
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

    gtk_list_box_set_header_func (priv->ui.list_box, (GtkListBoxUpdateHeaderFunc) _update_header, self, (GDestroyNotify) gtk_widget_destroy);
}


static GtkWidget*
lgw_dictionarylist_create_row (LwDictionary *dictionary)
{
    //Sanity checks;
    g_return_val_if_fail (LW_IS_DICTIONARY (dictionary), NULL);

    GtkWidget *row = gtk_list_box_row_new ();
    {
      GtkWidget *columns = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
      {
        GtkWidget *label = gtk_label_new (lw_dictionary_get_name (dictionary));
        gtk_box_pack_start (GTK_BOX (columns), label, FALSE, FALSE, 0);
        gtk_widget_show (label);
      }
      gtk_widget_show (columns);
      gtk_container_add (GTK_CONTAINER (row), columns);
    }

    return row;
}


static void
lgw_dictionarylistbox_sync_list (LgwDictionaryListBox *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DICTIONARYLISTBOX (self));

    //Declarations
    LgwDictionaryListBoxPrivate *priv = NULL;
    LgwDictionaryListStore *dictionary_list_store = NULL;

    //Initializations
    priv = self->priv;
    dictionary_list_store = lgw_dictionarylistbox_get_dictionaryliststore (self);

    if (priv->ui.list_box == NULL) goto errored;

    //Clear the previous list
    {
      GList *list = gtk_container_get_children (GTK_CONTAINER (priv->ui.list_box));
      GList *link = list;
      while (link != NULL)
      {
        GtkWidget *widget = GTK_WIDGET (link->data);
        if (widget != NULL)
        {
          gtk_widget_destroy (widget);
        }
        link->data = NULL;
        link = link->next;
      }
      if (list != NULL) g_list_free (list); list = NULL;
    }

    //Set the new list
    if (dictionary_list_store != NULL)
    {
      GList *list = lw_dictionarylist_get_list (LW_DICTIONARYLIST (dictionary_list_store));
      GList *link = list;;

      while (link != NULL)
      {
        LwDictionary *dictionary = LW_DICTIONARY (link->data);
        GtkWidget *row = lgw_dictionarylist_create_row (dictionary);

        gtk_container_add (GTK_CONTAINER (priv->ui.list_box), row);
        gtk_widget_show (row);

        link = link->next;
      }

      if (list != NULL) g_list_free (list); list = NULL;
    }

errored:

    return;
}


static void
lgw_dictionarylistbox_class_init (LgwDictionaryListBoxClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LgwDictionaryListBoxClassPrivate *klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klasspriv = klass->priv = g_new0(LgwDictionaryListBoxClassPrivate, 1);

    object_class->set_property = lgw_dictionarylistbox_set_property;
    object_class->get_property = lgw_dictionarylistbox_get_property;
    object_class->constructed = lgw_dictionarylistbox_constructed;
    object_class->finalize = lgw_dictionarylistbox_finalize;

    g_type_class_add_private (object_class, sizeof (LgwDictionaryListBoxPrivate));

    klasspriv->pspec[PROP_DICTIONARYLISTSTORE] = g_param_spec_object (
      "dictionaryliststore",
      "Dictionary list used for the datamodel",
      "Dictionarylist used for the datamodel",
      LGW_TYPE_DICTIONARYLISTSTORE,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DICTIONARYLISTSTORE, klasspriv->pspec[PROP_DICTIONARYLISTSTORE]);

    klasspriv->pspec[PROP_EDITABLE] = g_param_spec_boolean (
      "editable",
      "TODO",
      "TODO",
      FALSE,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_EDITABLE, klasspriv->pspec[PROP_EDITABLE]);
}


void
lgw_dictionarylistbox_set_dictionaryliststore (LgwDictionaryListBox *self,
                                               LgwDictionaryListStore    *dictionary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DICTIONARYLISTBOX (self));

    //Declarations
    LgwDictionaryListBoxPrivate *priv = NULL;
    LgwDictionaryListBoxClass *klass = NULL;
    LgwDictionaryListBoxClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LGW_DICTIONARYLISTBOX_GET_CLASS (self);
    klasspriv = klass->priv;

    if (priv->data.dictionary_list_store != NULL)
    {
      g_object_remove_weak_pointer (G_OBJECT (priv->data.dictionary_list_store), (gpointer*) &(priv->data.dictionary_list_store));
      g_object_unref (priv->data.dictionary_list_store);
      priv->data.dictionary_list_store = NULL;
    }

    priv->data.dictionary_list_store = dictionary_list_store;

    if (priv->data.dictionary_list_store != NULL)
    {
      g_object_ref (priv->data.dictionary_list_store);
      g_object_add_weak_pointer (G_OBJECT (priv->data.dictionary_list_store), (gpointer*) &(priv->data.dictionary_list_store));
    }

    lgw_dictionarylistbox_sync_list (self);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_DICTIONARYLISTSTORE]);
}


LgwDictionaryListStore*
lgw_dictionarylistbox_get_dictionaryliststore (LgwDictionaryListBox *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DICTIONARYLISTBOX (self));

    //Declarations
    LgwDictionaryListBoxPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.dictionary_list_store;
}


void
lgw_dictionarylistbox_set_editable (LgwDictionaryListBox *self,
                                    gboolean              editable)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DICTIONARYLISTBOX (self));

    //Declarations
    LgwDictionaryListBoxPrivate *priv = NULL;
    LgwDictionaryListBoxClass *klass = NULL;
    LgwDictionaryListBoxClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = LGW_DICTIONARYLISTBOX_GET_CLASS (self);
    klasspriv = klass->priv;
    changed = priv->config.editable != editable;

    if (changed)
    {
      priv->config.editable = editable;
      g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_EDITABLE]);
    }
}


gboolean
lgw_dictionarylistbox_is_editable (LgwDictionaryListBox *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DICTIONARYLISTBOX (self));

    //Declarations
    LgwDictionaryListBoxPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.editable;
}



