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
//! @file searchwidget.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <glib-object.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>
#include <libgwaei/searchwidget-private.h>


G_DEFINE_TYPE_WITH_CODE (LgwSearchWidget, lgw_searchwidget, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_MENUABLE, lgw_searchwidget_init_menuable_interface)
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_ACTIONABLE, lgw_searchwidget_impliment_actionable_interface));


//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_searchwidget_new ()
{
    //Declarations
    LgwSearchWidget *widget = NULL;

    //Initializations
    widget = LGW_SEARCHWIDGET (g_object_new (LGW_TYPE_SEARCHWIDGET, "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 0, NULL));

    return GTK_WIDGET (widget);
}


static void 
lgw_searchwidget_init (LgwSearchWidget *widget)
{
    widget->priv = LGW_SEARCHWIDGET_GET_PRIVATE (widget);
    memset(widget->priv, 0, sizeof(LgwSearchWidgetPrivate));

    LgwSearchWidgetPrivate *priv;
    priv = widget->priv;
}


static void 
lgw_searchwidget_finalize (GObject *object)
{
    LgwSearchWidget *widget;
    LgwSearchWidgetPrivate *priv;

    widget = LGW_SEARCHWIDGET (object);
    priv = widget->priv;

    G_OBJECT_CLASS (lgw_searchwidget_parent_class)->finalize (object);
}


static void
lgw_searchwidget_set_property (GObject      *object,
                               guint         property_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
    //Declarations
    LgwSearchWidget *search_widget = NULL;
    LgwActionable *actionable = NULL;
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    search_widget = LGW_SEARCHWIDGET (object);
    actionable = LGW_ACTIONABLE (object);
    priv = search_widget->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        lgw_actionable_set_actiongroup (actionable, g_value_get_pointer (value));
        break;
      case PROP_PREFERENCES:
        lgw_searchwidget_set_preferences (search_widget, g_value_get_object (value));
        break;
      case PROP_DICTIONARYLISTSTORE:
        lgw_searchwidget_set_dictionaryliststore (search_widget, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_searchwidget_get_property (GObject      *object,
                               guint         property_id,
                               GValue       *value,
                               GParamSpec   *pspec)
{
    //Declarations
    LgwSearchWidget *search_widget = NULL;
    LgwActionable *actionable = NULL;
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    search_widget = LGW_SEARCHWIDGET (object);
    actionable = LGW_ACTIONABLE (object);
    priv = search_widget->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        g_value_set_pointer (value, lgw_actionable_get_actions (actionable));
        break;
      case PROP_PREFERENCES:
        g_value_set_object (value, lgw_searchwidget_get_preferences (search_widget));
        break;
      case PROP_DICTIONARYLISTSTORE:
        g_value_set_object (value, lgw_searchwidget_get_dictionaryliststore (search_widget));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lgw_searchwidget_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    LgwSearchWidget *search_widget = NULL;
    LgwSearchWidgetPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_searchwidget_parent_class)->constructed (object);
    }

    //Initializations
    search_widget = LGW_SEARCHWIDGET (object);
    priv = search_widget->priv;
    priv->ui.box = GTK_BOX (search_widget);

    {
      GtkWidget *search_bar = gtk_search_bar_new ();
      priv->ui.search_bar = GTK_SEARCH_BAR (search_bar);
      gtk_box_pack_start (priv->ui.box, search_bar, FALSE, FALSE, 0);
      gtk_widget_show (search_bar);

      {
        GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_container_add (GTK_CONTAINER (priv->ui.search_bar), box);
        gtk_widget_show (box);

        {
          GtkWidget *entry = lgw_searchentry_new ();
          priv->ui.search_entry = LGW_SEARCHENTRY (entry);
          gtk_search_bar_connect_entry (priv->ui.search_bar, lgw_searchentry_get_entry (priv->ui.search_entry));
          gtk_box_pack_start (GTK_BOX (box), entry, FALSE, FALSE, 0);
          gtk_widget_show (entry);
        }

        {
          GtkWidget *button = gtk_menu_button_new ();
          gtk_button_set_label (GTK_BUTTON (button), "漢");
          gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 0);
          gtk_widget_show (button);

          {
            GtkWidget *menu = gtk_menu_new ();
            gtk_menu_button_set_popup (GTK_MENU_BUTTON (button), menu);
            gtk_widget_show (menu);

            {
              GtkWidget *menu_item = gtk_separator_menu_item_new ();
              gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
              gtk_widget_show (menu_item);

              {
                GtkWidget *label = gtk_label_new ("test");
                gtk_container_add (GTK_CONTAINER (menu_item), label);
                gtk_widget_show (label);
              }
            }
          }
        }
      }
    }

    {
      GtkWidget *paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);    
      priv->ui.paned = GTK_PANED (paned);
      gtk_box_pack_start (priv->ui.box, paned, TRUE, TRUE, 0);
      gtk_container_set_border_width (GTK_CONTAINER (paned), 6);
      gtk_widget_show (paned);

      {
        GtkWidget *dictionary_list_box = lgw_dictionarylistbox_new ();
        gtk_container_set_border_width (GTK_CONTAINER (dictionary_list_box), 6);
        priv->ui.dictionary_list_box = LGW_DICTIONARYLISTBOX (dictionary_list_box);
        gtk_paned_pack1 (priv->ui.paned, dictionary_list_box, FALSE, FALSE);
        gtk_widget_show (dictionary_list_box);
      }

      {
        GtkWidget *results_view = lgw_resultstextview_new ();
        gtk_container_set_border_width (GTK_CONTAINER (results_view), 6);
        priv->ui.results_view = LGW_RESULTSVIEW (results_view);
        gtk_paned_pack2 (priv->ui.paned, results_view, TRUE, TRUE);
        gtk_widget_show (results_view);
      }
    }

    {
      priv->data.button_menu_model = G_MENU_MODEL (lgw_load_menu_model ("searchwidget-menumodel-button.ui"));
      priv->data.window_menu_model = G_MENU_MODEL (lgw_load_menu_model ("searchwidget-menumodel-window.ui"));
    }

    lgw_searchwidget_connect_signals (search_widget);
}


static void
lgw_searchwidget_dispose (GObject *object)
{
    //Declarations
    LgwSearchWidget *entry = NULL;
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    entry = LGW_SEARCHWIDGET (object);
    priv = entry->priv;

    lgw_searchwidget_disconnect_signals (entry);

    G_OBJECT_CLASS (lgw_searchwidget_parent_class)->dispose (object);
}


static void
lgw_searchwidget_class_init (LgwSearchWidgetClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LgwSearchWidgetClassPrivate *klasspriv = NULL;

    object_class = G_OBJECT_CLASS (klass);

    klass->priv = g_new0(LgwSearchWidgetClassPrivate, 1);
    klasspriv = klass->priv;

    object_class->constructed = lgw_searchwidget_constructed;
    object_class->set_property = lgw_searchwidget_set_property;
    object_class->get_property = lgw_searchwidget_get_property;
    object_class->dispose = lgw_searchwidget_dispose;
    object_class->finalize = lgw_searchwidget_finalize;

    g_type_class_add_private (object_class, sizeof (LgwSearchWidgetPrivate));

    g_object_class_override_property (object_class, PROP_ACTIONS, "actions");

    klasspriv->pspec[PROP_PREFERENCES] = g_param_spec_object (
        "preferences",
        "Preferences construct prop",
        "Set the preferences object",
        LW_TYPE_PREFERENCES,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERENCES, klasspriv->pspec[PROP_PREFERENCES]);

    klasspriv->pspec[PROP_DICTIONARYLISTSTORE] = g_param_spec_object (
        "dictionaryliststore",
        "dictionary list",
        "Set the related dictionary list",
        LGW_TYPE_DICTIONARYLISTSTORE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DICTIONARYLISTSTORE, klasspriv->pspec[PROP_DICTIONARYLISTSTORE]);
}


static void
lgw_searchwidget_init_menuable_interface (LgwMenuableInterface *iface) {
    iface->get_window_menu_model = lgw_searchwidget_get_window_menu_model;
    iface->get_button_menu_model = lgw_searchwidget_get_button_menu_model;
}


gboolean
lgw_searchwidget_get_search_mode (LgwSearchWidget *widget)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_SEARCHWIDGET (widget), FALSE);
    g_return_val_if_fail (widget->priv != NULL, FALSE);
    g_return_val_if_fail (widget->priv->ui.search_bar != NULL, FALSE);

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;
    gboolean search_mode = FALSE;

    //Initializations
    priv = widget->priv;
    search_mode = gtk_search_bar_get_search_mode (priv->ui.search_bar);

    return search_mode;
}

void
lgw_searchwidget_set_search_mode (LgwSearchWidget *widget,
                                  gboolean         search_mode)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (widget));
    g_return_val_if_fail (widget->priv != NULL, FALSE);
    g_return_val_if_fail (widget->priv->ui.search_bar != NULL, FALSE);

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    priv = widget->priv;

    gtk_search_bar_set_search_mode (priv->ui.search_bar, search_mode);
}


static GMenuModel*
lgw_searchwidget_get_button_menu_model (LgwMenuable *menuable)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (menuable));

    //Declarations
    LgwSearchWidget *search_widget = LGW_SEARCHWIDGET (menuable);
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    priv = search_widget->priv;

    return priv->data.button_menu_model;
}


static GMenuModel*
lgw_searchwidget_get_window_menu_model (LgwMenuable *menuable)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (menuable));

    //Declarations
    LgwSearchWidget *search_widget = LGW_SEARCHWIDGET (menuable);
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    priv = search_widget->priv;

    return priv->data.window_menu_model;
}


void
lgw_searchwidget_set_preferences (LgwSearchWidget *search_widget,
                                  LwPreferences    *preferences)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (search_widget));

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;
    LgwSearchWidgetClass *klass = NULL;
    LgwSearchWidgetClassPrivate *klasspriv = NULL;

    //Initializations
    priv = search_widget->priv;
    klass = LGW_SEARCHWIDGET_GET_CLASS (search_widget);
    klasspriv = klass->priv;

    if (preferences != NULL)
    {
      g_object_ref (preferences);
    }

    if (priv->config.preferences != NULL) {
      g_object_remove_weak_pointer (G_OBJECT (priv->config.preferences), (gpointer*) &(priv->config.preferences));
      g_object_unref (priv->config.preferences);
      priv->config.preferences = NULL;
    }

    priv->config.preferences = preferences;

    if (priv->config.preferences != NULL) {
      g_object_add_weak_pointer (G_OBJECT (priv->config.preferences), (gpointer*) &(priv->config.preferences));
    }

    g_object_notify_by_pspec (G_OBJECT (search_widget), klasspriv->pspec[PROP_PREFERENCES]);
}


LwPreferences*
lgw_searchwidget_get_preferences (LgwSearchWidget *search_widget)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (search_widget));

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    priv = search_widget->priv;

    if (priv->config.preferences == NULL)
    {
      lgw_searchwidget_set_preferences (search_widget, lw_preferences_get_default ());
    }

    return priv->config.preferences;
}


void
lgw_searchwidget_set_dictionaryliststore (LgwSearchWidget   *search_widget,
                                          LgwDictionaryListStore *dictionary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (search_widget));

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;
    LgwSearchWidgetClass *klass = NULL;
    LgwSearchWidgetClassPrivate *klasspriv = NULL;

    //Initialziations
    priv = search_widget->priv;
    klass = LGW_SEARCHWIDGET_GET_CLASS (search_widget);
    if (priv->ui.dictionary_list_box == NULL) goto errored;
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

    lgw_dictionarylistbox_set_dictionaryliststore (priv->ui.dictionary_list_box, dictionary_list_store);

    g_object_notify_by_pspec (G_OBJECT (search_widget), klasspriv->pspec[PROP_DICTIONARYLISTSTORE]);

errored:

    return;
}

LgwDictionaryListStore*
lgw_searchwidget_get_dictionaryliststore (LgwSearchWidget *search_widget)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (search_widget));

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;

    //Initialziations
    priv = search_widget->priv;

    return priv->data.dictionary_list_store;
}
