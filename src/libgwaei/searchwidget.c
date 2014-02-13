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
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/searchwidget-private.h>


G_DEFINE_TYPE_WITH_CODE (LgwSearchWidget, lgw_searchwidget, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_MENUABLE, lgw_searchwidget_implement_menuable_interface)
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_ACTIONABLE, lgw_searchwidget_implement_actionable_interface));


//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_searchwidget_new ()
{
    //Declarations
    LgwSearchWidget *widget = NULL;

    //Initializations
    widget = LGW_SEARCHWIDGET (g_object_new (LGW_TYPE_SEARCHWIDGET, "orientation", GTK_ORIENTATION_HORIZONTAL, "spacing", 0, NULL));

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
    LgwSearchWidget *self = NULL;
    LgwActionable *actionable = NULL;
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    self = LGW_SEARCHWIDGET (object);
    actionable = LGW_ACTIONABLE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        lgw_actionable_set_actiongroup (actionable, g_value_get_pointer (value));
        break;
      case PROP_PREFERENCES:
        lgw_searchwidget_set_preferences (self, g_value_get_object (value));
        break;
      case PROP_DICTIONARYLISTSTORE:
        lgw_searchwidget_set_dictionaryliststore (self, g_value_get_object (value));
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
    LgwSearchWidget *self = NULL;
    LgwActionable *actionable = NULL;
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    self = LGW_SEARCHWIDGET (object);
    actionable = LGW_ACTIONABLE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        g_value_set_pointer (value, lgw_actionable_get_actions (actionable));
        break;
      case PROP_PREFERENCES:
        g_value_set_object (value, lgw_searchwidget_get_preferences (self));
        break;
      case PROP_DICTIONARYLISTSTORE:
        g_value_set_object (value, lgw_searchwidget_get_dictionaryliststore (self));
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
    LgwSearchWidget *self = NULL;
    LgwSearchWidgetPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_searchwidget_parent_class)->constructed (object);
    }

    //Initializations
    self = LGW_SEARCHWIDGET (object);
    priv = self->priv;
    priv->ui.box = GTK_BOX (self);

    {
      /*
      GtkWidget *paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);    
      priv->ui.paned = GTK_PANED (paned);
      gtk_box_pack_start (priv->ui.box, paned, TRUE, TRUE, 0);
      gtk_container_set_border_width (GTK_CONTAINER (paned), 6);
      gtk_widget_show (paned);
      */

      {
        GtkWidget *dictionary_list_box = lgw_dictionarylistbox_new ();
        gtk_container_set_border_width (GTK_CONTAINER (dictionary_list_box), 0);
        priv->ui.dictionary_list_box = LGW_DICTIONARYLISTBOX (dictionary_list_box);
        //gtk_paned_pack1 (priv->ui.paned, dictionary_list_box, FALSE, FALSE);
        gtk_box_pack_start (priv->ui.box, dictionary_list_box, FALSE, FALSE, 0);
        gtk_widget_show (dictionary_list_box);
      }

      {
        GtkWidget *results_view = lgw_resultstextview_new ();
        gtk_container_set_border_width (GTK_CONTAINER (results_view), 0);
        priv->ui.results_view = LGW_RESULTSVIEW (results_view);
        //gtk_paned_pack2 (priv->ui.paned, results_view, TRUE, TRUE);
        gtk_box_pack_start (priv->ui.box, results_view, TRUE, TRUE, 0);
        gtk_widget_show (results_view);
      }
    }

    {
      priv->data.button_menu_model = G_MENU_MODEL (lgw_load_menu_model ("searchwidget-menumodel-button.ui"));
      priv->data.window_menu_model = G_MENU_MODEL (lgw_load_menu_model ("searchwidget-menumodel-window.ui"));
    }

    lgw_searchwidget_connect_signals (self);
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


void
lgw_searchwidget_set_preferences (LgwSearchWidget *self,
                                  LwPreferences    *preferences)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (self));

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;
    LgwSearchWidgetClass *klass = NULL;
    LgwSearchWidgetClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LGW_SEARCHWIDGET_GET_CLASS (self);
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

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PREFERENCES]);
}


LwPreferences*
lgw_searchwidget_get_preferences (LgwSearchWidget *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (self));

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->config.preferences == NULL)
    {
      lgw_searchwidget_set_preferences (self, lw_preferences_get_default ());
    }

    return priv->config.preferences;
}


void
lgw_searchwidget_set_dictionaryliststore (LgwSearchWidget   *self,
                                          LgwDictionaryListStore *dictionary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (self));

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;
    LgwSearchWidgetClass *klass = NULL;
    LgwSearchWidgetClassPrivate *klasspriv = NULL;

    //Initialziations
    priv = self->priv;
    klass = LGW_SEARCHWIDGET_GET_CLASS (self);
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

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_DICTIONARYLISTSTORE]);

errored:

    return;
}

LgwDictionaryListStore*
lgw_searchwidget_get_dictionaryliststore (LgwSearchWidget *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (self));

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;

    //Initialziations
    priv = self->priv;

    return priv->data.dictionary_list_store;
}
