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


//Static declarations
static void lgw_searchwidget_attach_signals (LgwSearchWidget*);
static void lgw_searchwidget_remove_signals (LgwSearchWidget*);

static GMenuModel* lgw_searchwidget_get_menu_model (LgwStackWidget *widget);
static void lgw_searchwidget_initialize_interface (LgwStackWidgetInterface *iface);


G_DEFINE_TYPE_WITH_CODE (LgwSearchWidget, lgw_searchwidget, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_STACKWIDGET, lgw_searchwidget_initialize_interface));


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
        GtkWidget *entry = lgw_searchentry_new ();
        priv->ui.search_entry = LGW_SEARCHENTRY (entry);
        gtk_container_add (GTK_CONTAINER (priv->ui.search_bar), entry);
        gtk_widget_show (entry);
      }
    }

    {
      GtkWidget *paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);    
      priv->ui.paned = GTK_PANED (paned);
      gtk_box_pack_start (priv->ui.box, paned, TRUE, TRUE, 0);
      gtk_widget_show (paned);

      {
        GtkWidget *dictionary_view = lgw_dictionaryview_new ();
        priv->ui.dictionary_view = LGW_DICTIONARYVIEW (dictionary_view);
        gtk_paned_pack1 (priv->ui.paned, dictionary_view, FALSE, FALSE);
        gtk_widget_show (dictionary_view);
      }

      {
        GtkWidget *results_view = lgw_resultsview_new ();
        priv->ui.results_view = LGW_RESULTSVIEW (results_view);
        gtk_paned_pack2 (priv->ui.paned, results_view, TRUE, TRUE);
        gtk_widget_show (results_view);
      }
    }
}


static void
lgw_searchwidget_class_init (LgwSearchWidgetClass *klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = lgw_searchwidget_constructed;
    object_class->finalize = lgw_searchwidget_finalize;

    g_type_class_add_private (object_class, sizeof (LgwSearchWidgetPrivate));
/*
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


static void
lgw_searchwidget_initialize_interface (LgwStackWidgetInterface *iface) {
    iface->get_menu_model = lgw_searchwidget_get_menu_model;
}


gboolean
lgw_searchwidget_get_search_mode (LgwSearchWidget *widget)
{
    //Sanity checks
    g_return_val_if_fail (widget != NULL, FALSE);
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
    g_return_if_fail (widget != NULL);
    g_return_val_if_fail (widget->priv != NULL, FALSE);
    g_return_val_if_fail (widget->priv->ui.search_bar != NULL, FALSE);

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    priv = widget->priv;

    gtk_search_bar_set_search_mode (priv->ui.search_bar, search_mode);
}


void
lgw_searchwidget_set_dictionarylist (LgwSearchWidget   *search_widget, 
                                      LgwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_if_fail (search_widget != NULL);

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    if (search_widget->priv != NULL)
    {
        priv = search_widget->priv;
        lgw_dictionaryview_set_dictionarylist (priv->ui.dictionary_view, dictionary_list);
    }
}

static GMenuModel*
lgw_searchwidget_get_menu_model (LgwStackWidget *widget)
{
    return NULL;
}


