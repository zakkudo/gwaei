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
//! @file resultsview.c
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
#include <libgwaei/resultsview-private.h>


//Static declarations
static void lgw_resultsview_attach_signals (LgwResultsView*);
static void lgw_resultsview_remove_signals (LgwResultsView*);

G_DEFINE_TYPE (LgwResultsView, lgw_resultsview, GTK_TYPE_BOX)


//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_resultsview_new ()
{
    //Declarations
    LgwResultsView *widget = NULL;

    //Initializations
    widget = LGW_RESULTSVIEW (g_object_new (LGW_TYPE_RESULTSVIEW, NULL));

    return GTK_WIDGET (widget);
}


static void 
lgw_resultsview_init (LgwResultsView *widget)
{
    widget->priv = LGW_RESULTSVIEW_GET_PRIVATE (widget);
    memset(widget->priv, 0, sizeof(LgwResultsViewPrivate));

    LgwResultsViewPrivate *priv;
    priv = widget->priv;
}


static void 
lgw_resultsview_finalize (GObject *object)
{
    LgwResultsView *widget;
    LgwResultsViewPrivate *priv;

    widget = LGW_RESULTSVIEW (object);
    priv = widget->priv;

    G_OBJECT_CLASS (lgw_resultsview_parent_class)->finalize (object);
}


static void 
lgw_resultsview_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    LgwResultsView *widget = NULL;
    LgwResultsViewPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_resultsview_parent_class)->constructed (object);
    }

    //Initializations
    widget = LGW_RESULTSVIEW (object);
    priv = widget->priv;
    priv->ui.box = GTK_BOX (widget);

    {
        GtkWidget *notebook = gtk_notebook_new ();
        priv->ui.notebook = GTK_NOTEBOOK (notebook);
        gtk_box_pack_start (priv->ui.box, notebook, TRUE, TRUE, 0);
        gtk_widget_show (notebook);
    }
}


static void
lgw_resultsview_class_init (LgwResultsViewClass *klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = lgw_resultsview_constructed;
    object_class->finalize = lgw_resultsview_finalize;

    g_type_class_add_private (object_class, sizeof (LgwResultsViewPrivate));
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

