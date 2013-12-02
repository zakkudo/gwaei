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
//! @file dictionarylist-iter.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib-object.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/dictionarylist-private.h>

gint stamp = 0;
gint valid_stamp = 0;

void
lgw_dictionarylist_initialize_tree_iter (LgwDictionaryList *dictionary_list,
                                         GtkTreeIter       *iter,
                                         gint               index_)
{
    //Sanity checks
    g_return_val_if_fail (dictionary_list != NULL, 0);

    //Declarations
    LgwDictionaryListClass *klass = NULL;

    //Initializations
    iter->stamp = stamp;
    iter->user_data = dictionary_list;
    iter->user_data2 = GINT_TO_POINTER (index);

    stamp++;
}


void
lgw_dictionarylist_invalidate_tree_iter (GtkTreeIter *iter)
{
    //Sanity checks
    g_return_if_fail (iter != NULL);

    memset(iter, 0, sizeof(GtkTreeIter));
    iter->stamp = -1;
}


gboolean
lgw_dictionarylist_tree_iter_is_valid (LgwDictionaryList *dictionary_list,
                                       GtkTreeIter       *iter)
{
    //Sanity checks
    g_return_val_if_fail (dictionary_list != NULL, FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    gint index = 0;
    gint total = 0;

    //Initializations
    index = GPOINTER_TO_INT (iter->user_data2);
    total = lw_dictionarylist_get_total (LW_DICTIONARYLIST (dictionary_list));

    return (iter->stamp > -1 && iter->stamp > valid_stamp && index >= 0 && index < total);
}


void
lgw_dictionarylist_invalidate_old_timestamps ()
{
  valid_stamp = stamp - 1;
}

