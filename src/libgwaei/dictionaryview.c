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
//! @file dictionaryview.c
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
#include <libgwaei/dictionaryview-private.h>


//Static declarations
static void lgw_dictionaryview_attach_signals (LgwDictionaryView*);
static void lgw_dictionaryview_remove_signals (LgwDictionaryView*);

G_DEFINE_TYPE (LgwDictionaryView, lgw_dictionaryview, GTK_TYPE_BOX)


//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_dictionaryview_new ()
{
    //Declarations
    LgwDictionaryView *widget = NULL;

    //Initializations
    widget = LGW_DICTIONARYVIEW (g_object_new (LGW_TYPE_DICTIONARYVIEW, NULL));

    return GTK_WIDGET (widget);
}


static void 
lgw_dictionaryview_init (LgwDictionaryView *widget)
{
    widget->priv = LGW_DICTIONARYVIEW_GET_PRIVATE (widget);
    memset(widget->priv, 0, sizeof(LgwDictionaryViewPrivate));

    LgwDictionaryViewPrivate *priv;
    priv = widget->priv;
}


static void 
lgw_dictionaryview_finalize (GObject *object)
{
    LgwDictionaryView *widget;
    LgwDictionaryViewPrivate *priv;

    widget = LGW_DICTIONARYVIEW (object);
    priv = widget->priv;

    G_OBJECT_CLASS (lgw_dictionaryview_parent_class)->finalize (object);
}


static void 
lgw_dictionaryview_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    LgwDictionaryView *widget = NULL;
    LgwDictionaryViewPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_dictionaryview_parent_class)->constructed (object);
    }

    //Initializations
    widget = LGW_DICTIONARYVIEW (object);
    priv = widget->priv;
    priv->ui.box = GTK_BOX (widget);

    {
      GtkWidget *tree_view = gtk_tree_view_new ();
      priv->ui.tree_view = GTK_TREE_VIEW (tree_view);
      gtk_widget_show (tree_view);
      gtk_box_pack_start (priv->ui.box, tree_view, TRUE, TRUE, 0);
    }

    {
      GtkWidget *toolbar = gtk_toolbar_new ();
      priv->ui.toolbar = GTK_TOOLBAR (toolbar);
      gtk_widget_show (toolbar);
      gtk_box_pack_start (priv->ui.box, toolbar, FALSE, FALSE, 0);
    }
}


static void
lgw_dictionaryview_class_init (LgwDictionaryViewClass *klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = lgw_dictionaryview_constructed;
    object_class->finalize = lgw_dictionaryview_finalize;

    g_type_class_add_private (object_class, sizeof (LgwDictionaryViewPrivate));
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


void
lgw_dictionaryview_set_dictionarylist (LgwDictionaryView *view, 
                                       LgwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_if_fail (view != NULL);

    //Declarations
    GtkTreeModel *model = NULL;
    LgwDictionaryViewPrivate *priv = NULL;

    //Initializations
    priv = view->priv;
    if (dictionary_list != NULL)
    {
        g_object_ref (dictionary_list);
        model = GTK_TREE_MODEL (lgw_dictionarylist_get_liststore (dictionary_list));
    }
    if (priv->data.dictionary_list != NULL)
    {
        g_object_unref (priv->data.dictionary_list);
    }

    gtk_tree_view_set_model (priv->ui.tree_view, model);
    priv->data.dictionary_list = dictionary_list;
}


LgwDictionaryList*
lgw_dictionaryview_get_dictionarylist (LgwDictionaryView *view)
{
    //Sanity checks
    g_return_val_if_fail (view != NULL, NULL);

    //Declarations
    LgwDictionaryViewPrivate *priv = NULL;
    LgwDictionaryList *dictionary_list = NULL;

    if (view->priv != NULL)
    {
        priv = view->priv; 
        dictionary_list = priv->data.dictionary_list;
    }

    return dictionary_list;
}

