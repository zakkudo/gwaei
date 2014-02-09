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
//! @file vocabularywidget.c
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

#include <libgwaei/vocabularywidget-private.h>


G_DEFINE_TYPE_WITH_CODE (LgwVocabularyWidget, lgw_vocabularywidget, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_MENUABLE, lgw_vocabularywidget_implement_menuable_interface)
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_ACTIONABLE, lgw_vocabularywidget_implement_actionable_interface));


//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_vocabularywidget_new ()
{
    //Declarations
    LgwVocabularyWidget *widget = NULL;

    //Initializations
    widget = LGW_VOCABULARYWIDGET (g_object_new (LGW_TYPE_VOCABULARYWIDGET, "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 0, NULL));

    return GTK_WIDGET (widget);
}


static void 
lgw_vocabularywidget_init (LgwVocabularyWidget *widget)
{
    widget->priv = LGW_VOCABULARYWIDGET_GET_PRIVATE (widget);
    memset(widget->priv, 0, sizeof(LgwVocabularyWidgetPrivate));

    LgwVocabularyWidgetPrivate *priv;
    priv = widget->priv;
}


static void 
lgw_vocabularywidget_finalize (GObject *object)
{
    LgwVocabularyWidget *widget;
    LgwVocabularyWidgetPrivate *priv;

    widget = LGW_VOCABULARYWIDGET (object);
    priv = widget->priv;

    G_OBJECT_CLASS (lgw_vocabularywidget_parent_class)->finalize (object);
}


static void
lgw_vocabularywidget_set_property (GObject      *object,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
    //Declarations
    LgwVocabularyWidget *vocabulary_widget = NULL;
    LgwActionable *actionable = NULL;
    LgwVocabularyWidgetPrivate *priv = NULL;

    //Initializations
    vocabulary_widget = LGW_VOCABULARYWIDGET (object);
    actionable = LGW_ACTIONABLE (object);
    priv = vocabulary_widget->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        lgw_actionable_set_actiongroup (actionable, g_value_get_pointer (value));
        break;
      case PROP_PREFERENCES:
        lgw_vocabularywidget_set_preferences (vocabulary_widget, g_value_get_object (value));
        break;
      case PROP_VOCABULARYLISTSTORE:
        lgw_vocabularywidget_set_vocabularyliststore (vocabulary_widget, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_vocabularywidget_get_property (GObject      *object,
                                   guint         property_id,
                                   GValue       *value,
                                   GParamSpec   *pspec)
{
    //Declarations
    LgwVocabularyWidget *vocabulary_widget = NULL;
    LgwActionable *actionable = NULL;
    LgwVocabularyWidgetPrivate *priv = NULL;

    //Initializations
    vocabulary_widget = LGW_VOCABULARYWIDGET (object);
    actionable = LGW_ACTIONABLE (object);
    priv = vocabulary_widget->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        g_value_set_pointer (value, lgw_actionable_get_actions (actionable));
        break;
      case PROP_PREFERENCES:
        g_value_set_object (value, lgw_vocabularywidget_get_preferences (vocabulary_widget));
        break;
      case PROP_VOCABULARYLISTSTORE:
        g_value_set_object (value, lgw_vocabularywidget_get_vocabularyliststore (vocabulary_widget));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lgw_vocabularywidget_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    LgwVocabularyWidget *vocabulary_widget = NULL;
    LgwVocabularyWidgetPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_vocabularywidget_parent_class)->constructed (object);
    }

    //Initializations
    vocabulary_widget = LGW_VOCABULARYWIDGET (object);
    priv = vocabulary_widget->priv;
    priv->ui.box = GTK_BOX (vocabulary_widget);

    {
      GtkWidget *paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);    
      priv->ui.paned = GTK_PANED (paned);
      gtk_box_pack_start (priv->ui.box, paned, TRUE, TRUE, 0);
      gtk_container_set_border_width (GTK_CONTAINER (paned), 6);
      gtk_widget_show (paned);

      {
        GtkWidget *vocabulary_list_view = lgw_vocabularylistview_new ();
        gtk_container_set_border_width (GTK_CONTAINER (vocabulary_list_view), 6);
        priv->ui.vocabulary_list_view = LGW_VOCABULARYLISTVIEW (vocabulary_list_view);
        gtk_paned_pack1 (priv->ui.paned, vocabulary_list_view, FALSE, FALSE);
        gtk_widget_show (vocabulary_list_view);
      }

      {
        GtkWidget *vocabulary_word_view = lgw_vocabularywordview_new ();
        gtk_container_set_border_width (GTK_CONTAINER (vocabulary_word_view), 6);
        priv->ui.vocabulary_word_view = LGW_VOCABULARYWORDVIEW (vocabulary_word_view);
        gtk_paned_pack2 (priv->ui.paned, vocabulary_word_view, TRUE, TRUE);
        gtk_widget_show (vocabulary_word_view);
      }
    }

    {
      priv->data.button_menu_model = G_MENU_MODEL (lgw_load_menu_model ("vocabularywidget-menumodel-button.ui"));
      priv->data.window_menu_model = G_MENU_MODEL (lgw_load_menu_model ("vocabularywidget-menumodel-window.ui"));
    }

    lgw_vocabularywidget_connect_signals (vocabulary_widget);
}


static void
lgw_vocabularywidget_dispose (GObject *object)
{
    //Declarations
    LgwVocabularyWidget *entry = NULL;
    LgwVocabularyWidgetPrivate *priv = NULL;

    //Initializations
    entry = LGW_VOCABULARYWIDGET (object);
    priv = entry->priv;

    lgw_vocabularywidget_disconnect_signals (entry);

    G_OBJECT_CLASS (lgw_vocabularywidget_parent_class)->dispose (object);
}


static void
lgw_vocabularywidget_class_init (LgwVocabularyWidgetClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LgwVocabularyWidgetClassPrivate *klasspriv = NULL;

    object_class = G_OBJECT_CLASS (klass);

    klass->priv = g_new0(LgwVocabularyWidgetClassPrivate, 1);
    klasspriv = klass->priv;

    object_class->constructed = lgw_vocabularywidget_constructed;
    object_class->set_property = lgw_vocabularywidget_set_property;
    object_class->get_property = lgw_vocabularywidget_get_property;
    object_class->dispose = lgw_vocabularywidget_dispose;
    object_class->finalize = lgw_vocabularywidget_finalize;

    g_type_class_add_private (object_class, sizeof (LgwVocabularyWidgetPrivate));

    g_object_class_override_property (object_class, PROP_ACTIONS, "actions");

    klasspriv->pspec[PROP_PREFERENCES] = g_param_spec_object (
        "preferences",
        "Preferences construct prop",
        "Set the preferences object",
        LW_TYPE_PREFERENCES,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERENCES, klasspriv->pspec[PROP_PREFERENCES]);

    klasspriv->pspec[PROP_VOCABULARYLISTSTORE] = g_param_spec_object (
        "vocabulary-list-store",
        "vocabulary list store",
        "Set the related vocabulary list",
        LGW_TYPE_VOCABULARYLISTSTORE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_VOCABULARYLISTSTORE, klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);
}


void
lgw_vocabularywidget_set_preferences (LgwVocabularyWidget *vocabulary_widget,
                                      LwPreferences       *preferences)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWIDGET (vocabulary_widget));

    //Declarations
    LgwVocabularyWidgetPrivate *priv = NULL;
    LgwVocabularyWidgetClass *klass = NULL;
    LgwVocabularyWidgetClassPrivate *klasspriv = NULL;

    //Initializations
    priv = vocabulary_widget->priv;
    klass = LGW_VOCABULARYWIDGET_GET_CLASS (vocabulary_widget);
    klasspriv = klass->priv;

    if (priv->config.preferences != NULL) {
      g_object_remove_weak_pointer (G_OBJECT (priv->config.preferences), (gpointer*) &(priv->config.preferences));
      g_object_unref (priv->config.preferences);
      priv->config.preferences = NULL;
    }

    priv->config.preferences = preferences;

    if (priv->config.preferences != NULL) {
      g_object_add_weak_pointer (G_OBJECT (priv->config.preferences), (gpointer*) &(priv->config.preferences));
    }

    g_object_notify_by_pspec (G_OBJECT (vocabulary_widget), klasspriv->pspec[PROP_PREFERENCES]);
}


LwPreferences*
lgw_vocabularywidget_get_preferences (LgwVocabularyWidget *vocabulary_widget)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWIDGET (vocabulary_widget));

    //Declarations
    LgwVocabularyWidgetPrivate *priv = NULL;

    //Initializations
    priv = vocabulary_widget->priv;

    if (priv->config.preferences == NULL)
    {
      lgw_vocabularywidget_set_preferences (vocabulary_widget, lw_preferences_get_default ());
    }

    return priv->config.preferences;
}


void
lgw_vocabularywidget_set_vocabularyliststore (LgwVocabularyWidget    *vocabulary_widget,
                                              LgwVocabularyListStore *vocabulary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWIDGET (vocabulary_widget));

    //Declarations
    LgwVocabularyWidgetPrivate *priv = NULL;
    LgwVocabularyWidgetClass *klass = NULL;
    LgwVocabularyWidgetClassPrivate *klasspriv = NULL;

    //Initializations
    priv = vocabulary_widget->priv;
    klass = LGW_VOCABULARYWIDGET_GET_CLASS (vocabulary_widget);
    klasspriv = klass->priv;
    
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

      lgw_vocabularylistview_set_liststore (priv->ui.vocabulary_list_view, vocabulary_list_store);

      g_object_notify_by_pspec (G_OBJECT (vocabulary_widget), klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);
    }
}


LgwVocabularyListStore*
lgw_vocabularywidget_get_vocabularyliststore (LgwVocabularyWidget *vocabulary_widget)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWIDGET (vocabulary_widget));

    //Declarations
    LgwVocabularyWidgetPrivate *priv = NULL;
    LgwVocabularyWidgetClass *klass = NULL;
    LgwVocabularyWidgetClassPrivate *klasspriv = NULL;

    //Initializations
    priv = vocabulary_widget->priv;
    klass = LGW_VOCABULARYWIDGET_GET_CLASS (vocabulary_widget);
    klasspriv = klass->priv;

    return priv->data.vocabulary_list_store;
}
