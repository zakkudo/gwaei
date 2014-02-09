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


void
lgw_resultstextview_clear_searchlist (LgwResultsView *view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_RESULTSTEXTVIEW (view));

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
    g_return_if_fail (LGW_IS_RESULTSTEXTVIEW (view));

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
    g_return_if_fail (LGW_IS_RESULTSTEXTVIEW (view));

    //Declarations
    LgwResultsTextView *text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    text_view = LGW_RESULTSTEXTVIEW (view);
    priv = text_view->priv;
}


void
lgw_resultstextview_implement_resultsview_interface (LgwResultsViewInterface *iface)
{
    iface->set_search = lgw_resultstextview_set_searchlist;
    iface->get_search = lgw_resultstextview_get_searchlist;
    iface->clear_search = lgw_resultstextview_clear_searchlist;
}


