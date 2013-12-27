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

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/resultstextview-private.h>

static void lgw_resultstextview_init_resultsview_interface (LgwResultsViewInterface *iface);
static void lgw_resultstextview_init_actionable_interface (LgwActionableInterface *iface);


G_DEFINE_TYPE_WITH_CODE (LgwResultsTextView, lgw_resultstextview, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_RESULTSVIEW, lgw_resultstextview_init_resultsview_interface)
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_ACTIONABLE, lgw_resultstextview_init_actionable_interface));



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
lgw_resultstextview_init_resultsview_interface (LgwResultsViewInterface *iface)
{
    iface->set_search = lgw_resultstextview_set_searchlist;
}


static void
lgw_resultstextview_init_actionable_interface (LgwActionableInterface *iface)
{
    iface->get_actions = lgw_resultstextview_get_actions;
    iface->set_actiongroup = lgw_resultstextview_set_actiongroup;
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

    if (klasspriv->text_tag_table != NULL) {
        g_object_ref (klasspriv->text_tag_table);
    }

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

    if (klasspriv->text_tag_table == NULL)
    {
        klasspriv->text_tag_table = lgw_texttagtable_new ();
        g_object_add_weak_pointer (G_OBJECT (klasspriv->text_tag_table), (gpointer*) &(klasspriv->text_tag_table));
    }
    else
    {
        g_object_ref (klasspriv->text_tag_table);
    }

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

        GtkTextBuffer *text_buffer = gtk_text_buffer_new (klasspriv->text_tag_table);
        priv->ui.text_buffer = GTK_TEXT_BUFFER (text_buffer);
        gtk_text_view_set_buffer (priv->ui.text_view, text_buffer);
        g_object_unref (text_buffer);
      }
    }

    lgw_resultstextview_sync_actions (view); //TEMP TODO

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
    g_return_if_fail (view != NULL);

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

    printf("set search\n");
}


void
lgw_resultstextview_clear_searchlist (LgwResultsView *view)
{
    //Sanity checks
    g_return_if_fail (view != NULL);

    //Declarations
    LgwResultsTextView *text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    text_view = LGW_RESULTSTEXTVIEW (view);
    priv = text_view->priv;

    g_list_free_full (priv->data.searchiteratorlist, (GDestroyNotify) lw_searchiterator_free);
    g_list_free_full (priv->data.searchlist, (GDestroyNotify) lw_search_free);

    priv->data.searchiteratorlist = NULL;
    priv->data.searchlist = NULL;
}


void
lgw_resultstextview_set_searchlist (LgwResultsView *view,
                                    GList          *searchlist)
{
    //Sanity checks
    g_return_if_fail (view != NULL);

    //Declarations
    LgwResultsTextView *text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    text_view = LGW_RESULTSTEXTVIEW (view);
    priv = text_view->priv;

    lgw_resultstextview_clear_searchlist (view);

    {
      GList *link = searchlist;
      while (link != NULL)
      {
        LwSearch *search = LW_SEARCH (link->data);
        lgw_resultstextview_add_search (view, search);
        link = link->next;
      }
    }
    
    printf("set search\n");
}


GList*
lgw_resultstextview_get_searchlist (LgwResultsView *view)
{
    //Sanity checks
    g_return_if_fail (view != NULL);

    //Declarations
    LgwResultsTextView *text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    text_view = LGW_RESULTSTEXTVIEW (view);
    priv = text_view->priv;
}

static gboolean
_load_results (LgwResultsView *view)
{
    //Sanity checks
    g_return_if_fail (view != NULL);

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
    g_return_if_fail (view != NULL);

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


static void
lgw_resultstextview_set_actiongroup (LgwActionable  *actionable,
                                     LgwActionGroup *action_group)
{
    printf("BREAK lgw_resultstextview_set_actiongroup\n");
    //Sanity checks
    g_return_val_if_fail (actionable != NULL, NULL);

    //Declarations
    LgwResultsTextView *results_text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;
    LgwResultsTextViewClass *klass = NULL;
    LgwResultsTextViewClassPrivate *klasspriv = NULL;
    GList *list = NULL;

    //Initializations
    results_text_view = LGW_RESULTSTEXTVIEW (actionable);
    priv = results_text_view->priv;
    klass = LGW_RESULTSTEXTVIEW_GET_CLASS (results_text_view);
    klasspriv = klass->priv;

    if (priv->data.action_group_list != NULL)
    {
      g_list_free (priv->data.action_group_list);
      priv->data.action_group_list = NULL;
    }

    if (priv->data.action_group != NULL)
    {
        lgw_actiongroup_free (priv->data.action_group);
        priv->data.action_group = NULL;
    }
    priv->data.action_group = action_group;

    if (action_group != NULL)
    {
      priv->data.action_group_list = g_list_prepend (priv->data.action_group_list, action_group);
    }
}


void
lgw_resultstextview_sync_actions (LgwResultsTextView *results_text_view)
{
    printf("BREAK0 lgw_resultstextview_sync_actions\n");

    //Sanity checks
    g_return_val_if_fail (results_text_view != NULL, NULL);

    //Declarations
    LgwResultsTextViewPrivate *priv = NULL;
    GtkWidget *widget = NULL;
    gboolean has_focus = FALSE;

    //Initializations
    priv = results_text_view->priv;
    widget = GTK_WIDGET (results_text_view);
    has_focus = gtk_widget_is_focus (GTK_WIDGET (results_text_view));

    if (has_focus)
    {
      printf("BREAK1 lgw_resultstextview_sync_actions has focus\n");
      static GActionEntry entries[] = {
        { "copy", lgw_resultstextview_copy_cb, NULL, NULL, NULL }
      };
      if (priv->data.action_group == NULL || !lgw_actiongroup_contains_entries (priv->data.action_group, entries, G_N_ELEMENTS (entries)))
      {
        LgwActionGroup *action_group = lgw_actiongroup_static_new (entries, G_N_ELEMENTS (entries), widget);
        lgw_actionable_set_actiongroup (LGW_ACTIONABLE (results_text_view), action_group);
      }
    }
    else 
    {
      lgw_actionable_set_actiongroup (LGW_ACTIONABLE (results_text_view), NULL);
    }

    printf("BREAK2 lgw_resultstextview_sync_actions\n");
}


static GList*
lgw_resultstextview_get_actions (LgwActionable *actionable)
{
    printf("BREAK0 lgw_resultstextview_get_actions\n");

    //Sanity checks
    g_return_val_if_fail (actionable != NULL, NULL);

    //Declarations
    LgwResultsTextView *results_text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    results_text_view = LGW_RESULTSTEXTVIEW (actionable);
    priv = results_text_view->priv;

    printf("BREAK1 lgw_resultstextview_get_actions\n");

    return priv->data.action_group_list;
}

