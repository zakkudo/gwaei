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
//! @file resultstextview.c
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

#include <libgwaei/resultstextview-private.h>


G_DEFINE_TYPE_WITH_CODE (LgwResultsTextView, lgw_resultstextview, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_RESULTSVIEW, lgw_resultstextview_implement_resultsview_interface)
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_ACTIONABLE, lgw_resultstextview_implement_actionable_interface));


//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_resultstextview_new ()
{
    //Declarations
    LgwResultsTextView *widget = NULL;

    //Initializations
    widget = LGW_RESULTSTEXTVIEW (g_object_new (LGW_TYPE_RESULTSTEXTVIEW, NULL));

    return GTK_WIDGET (widget);
}


static void 
lgw_resultstextview_init (LgwResultsTextView *widget)
{
    widget->priv = LGW_RESULTSTEXTVIEW_GET_PRIVATE (widget);
    memset(widget->priv, 0, sizeof(LgwResultsTextViewPrivate));

    LgwResultsTextViewPrivate *priv;
    priv = widget->priv;
}


static void 
lgw_resultstextview_finalize (GObject *object)
{
    LgwResultsTextView *view;
    LgwResultsTextViewPrivate *priv;
    LgwResultsTextViewClass *klass = NULL;
    LgwResultsTextViewClassPrivate *klasspriv = NULL;

    view = LGW_RESULTSTEXTVIEW (object);
    priv = view->priv;
    klass = LGW_RESULTSTEXTVIEW_GET_CLASS (view);
    klasspriv = klass->priv;

    lgw_resultstextview_set_tagtable (view, NULL);

    G_OBJECT_CLASS (lgw_resultstextview_parent_class)->finalize (object);
}


static void
lgw_resultstextview_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LgwResultsTextView *results_text_view = NULL;
    LgwActionable *actionable = NULL;
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    results_text_view = LGW_RESULTSTEXTVIEW (object);
    actionable = LGW_ACTIONABLE (object);
    priv = results_text_view->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        lgw_actionable_set_actiongroup (actionable, g_value_get_pointer (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_resultstextview_get_property (GObject      *object,
                                  guint         property_id,
                                  GValue       *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LgwResultsTextView *results_text_view = NULL;
    LgwActionable *actionable = NULL;
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    results_text_view = LGW_RESULTSTEXTVIEW (object);
    actionable = LGW_ACTIONABLE (object);
    priv = results_text_view->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        g_value_set_pointer (value, lgw_actionable_get_actions (actionable));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}



static void 
lgw_resultstextview_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    LgwResultsTextView *view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;
    LgwResultsTextViewClass *klass = NULL;
    LgwResultsTextViewClassPrivate *klasspriv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_resultstextview_parent_class)->constructed (object);
    }

    //Initializations
    view = LGW_RESULTSTEXTVIEW (object);
    priv = view->priv;
    priv->ui.box = GTK_BOX (view);
    klass = LGW_RESULTSTEXTVIEW_GET_CLASS (view);
    klasspriv = klass->priv;

    {
      GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
      priv->ui.scrolled_window = GTK_SCROLLED_WINDOW (scrolled_window);
      gtk_scrolled_window_set_shadow_type (priv->ui.scrolled_window, GTK_SHADOW_IN);
      gtk_box_pack_start (priv->ui.box, scrolled_window, TRUE, TRUE, 0);
      gtk_widget_show (scrolled_window);

      {
        GtkWidget *text_view = gtk_text_view_new ();
        priv->ui.text_view = GTK_TEXT_VIEW (text_view);
        gtk_container_add (GTK_CONTAINER (scrolled_window), text_view);
        gtk_widget_show (text_view);

        LgwTextTagTable *text_tag_table = lgw_resultstextview_get_tagtable (view);
        GtkTextBuffer *text_buffer = gtk_text_buffer_new (GTK_TEXT_TAG_TABLE (text_tag_table));
        priv->ui.text_buffer = GTK_TEXT_BUFFER (text_buffer);
        gtk_text_view_set_buffer (priv->ui.text_view, text_buffer);
        g_object_unref (text_buffer);
      }
    }

    gtk_text_view_set_editable (priv->ui.text_view, FALSE);
    gtk_text_view_set_cursor_visible (priv->ui.text_view, FALSE);

    lgw_resultstextview_connect_signals (view);
}


static void
lgw_resultstextview_dispose (GObject *object)
{
    //Declarations
    LgwResultsTextView *results_text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    results_text_view = LGW_RESULTSTEXTVIEW (object);
    priv = results_text_view->priv;

    lgw_resultstextview_disconnect_signals (results_text_view);

    G_OBJECT_CLASS (lgw_resultstextview_parent_class)->dispose (object);
}



static void
lgw_resultstextview_class_init (LgwResultsTextViewClass *klass)
{
    //Sanity checks
    g_return_if_fail (klass != NULL);

    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->constructed = lgw_resultstextview_constructed;
    object_class->set_property = lgw_resultstextview_set_property;
    object_class->get_property = lgw_resultstextview_get_property;
    object_class->dispose = lgw_resultstextview_dispose;
    object_class->finalize = lgw_resultstextview_finalize;
    klass->priv = g_new0 (LgwResultsTextViewClassPrivate, 1);

    g_type_class_add_private (object_class, sizeof (LgwResultsTextViewPrivate));

    g_object_class_override_property (object_class, PROP_ACTIONS, "actions");
}


void
lgw_resultstextview_add_search (LgwResultsView *view,
                                LwSearch       *search)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_RESULTSTEXTVIEW (view));

    //Declarations
    LgwResultsTextView *text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;
    gboolean found = FALSE;

    //Initializations
    text_view = LGW_RESULTSTEXTVIEW (view);
    priv = text_view->priv;

    {
      GList *link = priv->data.searchlist;
      while (link != NULL)
      {
        if (lw_search_is_equal (LW_SEARCH (link->data), search)) {
          found = TRUE;
        }
        link = link->next;
      }
      
    }

    if (!found)
    {
      priv->data.searchlist = g_list_append (priv->data.searchlist, search);
      priv->data.searchiteratorlist = g_list_append (priv->data.searchiteratorlist, lw_searchiterator_new (search, "raw"));
    }

}


static gboolean
_load_results (LgwResultsView *view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_RESULTSTEXTVIEW (view));

    //Declarations
    LgwResultsTextView *text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    text_view = LGW_RESULTSTEXTVIEW (view);
    priv = text_view->priv;

    priv->data.timeoutid = 0;
    return FALSE;
}


static void
lgw_resultstextview_set_timeout (LgwResultsView *view,
                                 guint           milliseconds)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_RESULTSTEXTVIEW (view));

    //Declarations
    LgwResultsTextView *text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    text_view = LGW_RESULTSTEXTVIEW (view);
    priv = text_view->priv;

    if (priv->data.timeoutid != 0)
    {
      priv->data.timeoutid = g_timeout_add (milliseconds, (GSourceFunc) _load_results, view);
    }
}


void
lgw_resultstextview_set_tagtable (LgwResultsTextView *results_text_view,
                                  LgwTextTagTable    *tag_table)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_RESULTSTEXTVIEW (results_text_view));

    //Declarations
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    priv = results_text_view->priv;

    if (tag_table != NULL)
    {
      g_object_ref (tag_table);
    }

    if (priv->config.text_tag_table != NULL)
    {
      g_object_remove_weak_pointer (G_OBJECT (priv->config.text_tag_table), (gpointer*) &(priv->config.text_tag_table));
      g_object_unref (priv->config.text_tag_table);
      priv->config.text_tag_table = NULL;
    }

    priv->config.text_tag_table = tag_table;

    if (priv->config.text_tag_table != NULL)
    {
      g_object_add_weak_pointer (G_OBJECT (priv->config.text_tag_table), (gpointer*) &(priv->config.text_tag_table));
    }
}

LgwTextTagTable*
lgw_resultstextview_get_tagtable (LgwResultsTextView *results_text_view)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_RESULTSTEXTVIEW (results_text_view), NULL);

    //Declarations
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    priv = results_text_view->priv;

    return priv->config.text_tag_table;
}

