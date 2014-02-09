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
//! @file resultstabbedtextview.c
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
#include <libgwaei/resultstabbedtextview-private.h>


//Static declarations
static void lgw_resultstabbedtextview_attach_signals (LgwResultsTabbedTextView*);
static void lgw_resultstabbedtextview_remove_signals (LgwResultsTabbedTextView*);

G_DEFINE_TYPE (LgwResultsTabbedTextView, lgw_resultstabbedtextview, GTK_TYPE_BOX)


//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_resultstabbedtextview_new ()
{
    //Declarations
    LgwResultsTabbedTextView *widget = NULL;

    //Initializations
    widget = LGW_RESULTSTABBEDTEXTVIEW (g_object_new (LGW_TYPE_RESULTSTABBEDTEXTVIEW, NULL));

    return GTK_WIDGET (widget);
}


static void 
lgw_resultstabbedtextview_init (LgwResultsTabbedTextView *widget)
{
    widget->priv = LGW_RESULTSTABBEDTEXTVIEW_GET_PRIVATE (widget);
    memset(widget->priv, 0, sizeof(LgwResultsTabbedTextViewPrivate));

    LgwResultsTabbedTextViewPrivate *priv;
    priv = widget->priv;
}


static void 
lgw_resultstabbedtextview_finalize (GObject *object)
{
    LgwResultsTabbedTextView *widget;
    LgwResultsTabbedTextViewPrivate *priv;

    widget = LGW_RESULTSTABBEDTEXTVIEW (object);
    priv = widget->priv;

    G_OBJECT_CLASS (lgw_resultstabbedtextview_parent_class)->finalize (object);
}


static void 
lgw_resultstabbedtextview_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    LgwResultsTabbedTextView *widget = NULL;
    LgwResultsTabbedTextViewPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_resultstabbedtextview_parent_class)->constructed (object);
    }

    //Initializations
    widget = LGW_RESULTSTABBEDTEXTVIEW (object);
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
lgw_resultstabbedtextview_class_init (LgwResultsTabbedTextViewClass *klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = lgw_resultstabbedtextview_constructed;
    object_class->finalize = lgw_resultstabbedtextview_finalize;

    g_type_class_add_private (object_class, sizeof (LgwResultsTabbedTextViewPrivate));
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


