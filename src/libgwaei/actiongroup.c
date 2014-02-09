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
//! @file actiongroup.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib-object.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>


LgwActionGroup*
lgw_actiongroup_static_new (GActionEntry* const entries,
                            gint                length,
                            GtkWidget*          widget)
{
    //Sanity checks
    g_return_val_if_fail (entries != NULL, NULL);
    g_return_val_if_fail (length > 0, NULL);
    g_return_val_if_fail (widget != NULL, NULL);

    //Declarations
    LgwActionGroup *group = NULL;
    
    //Initializations
    group = g_new0 (LgwActionGroup, 1);

    group->entries = entries;
    group->length = length;
    group->widget = GTK_WIDGET (widget);

    return group;
}


void
lgw_actiongroup_free (LgwActionGroup *group)
{
    //Sanity checks
    if (group == NULL) return;

    g_free (group);
}


void
lgw_actiongroup_add_to_map (LgwActionGroup *action_group,
                            GActionMap     *action_map)
{
    //Sanity checks
    g_return_if_fail (action_group != NULL);
    g_return_if_fail (action_map != NULL);

    //Declarations
    GActionEntry *entries = NULL;
    gint length = 0;
    GtkWidget *widget = NULL;

    //Initializations
    entries = (GActionEntry*) action_group->entries;
    length = action_group->length;
    widget = GTK_WIDGET (action_group->widget);

    g_action_map_add_action_entries (action_map, entries, length, widget);
}


void
lgw_actiongroup_remove_from_map (LgwActionGroup *action_group,
                                 GActionMap     *action_map)
{
    //Sanity checks
    g_return_if_fail (action_group != NULL);
    g_return_if_fail (action_map != NULL);

    //Declarations
    GActionEntry *entries = NULL;
    gint length = 0;
    GtkWidget *widget = NULL;

    //Initializations
    entries = (GActionEntry*) action_group->entries;
    length = action_group->length;

    {
      gint i = 0;
      for (i = 0; i < length; i++)
      {
        g_action_map_remove_action (action_map, entries[i].name);
      }
    }
}


gboolean
lgw_actiongroup_contains_entries (LgwActionGroup *action_group,
                                  GActionEntry   *other_entries,
                                  gint            other_length)
{
    //Sanity checks
    g_return_val_if_fail (action_group != NULL, FALSE);
    g_return_val_if_fail (other_entries != NULL, FALSE);

    //Declarations
    GActionEntry *entries = NULL;
    gint length = 0;

    //Initializations
    entries = (GActionEntry*) action_group->entries;
    length = action_group->length;

    return (entries == other_entries && length == other_length);
}

