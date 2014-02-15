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
lgw_actiongroup_new (GtkWidget* widget)
{
    //Sanity checks
    g_return_val_if_fail (widget != NULL, NULL);

    //Declarations
    LgwActionGroup *group = NULL;
    
    //Initializations
    group = g_new0 (LgwActionGroup, 1);

    group->widget = GTK_WIDGET (widget);
    group->entries = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, (GDestroyNotify) g_free);
    group->changed = FALSE;

    return group;
}


void
lgw_actiongroup_free (LgwActionGroup *self)
{
    //Sanity checks
    if (self == NULL) return;

    if (self->entries != NULL) g_hash_table_unref (self->entries); self->entries = NULL;

    g_free (self);
}


void
lgw_actiongroup_add_entries (LgwActionGroup *self, ...)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    va_list va;
    GActionEntry* entries = NULL;
    gint length = 0;

    va_start(va, self);

    while (TRUE)
    {
      entries = va_arg(va, GActionEntry*);
      if (entries == NULL) goto done;
      length = va_arg(va, gint);
      if (length == 0) goto done;

      if (g_hash_table_lookup (self->entries, entries) == NULL)
      {
        LgwActionGroupData *data = g_new0 (LgwActionGroupData, 1);
        data->entries = entries;
        data->length = length;
        g_hash_table_insert (self->entries, entries, data);
        self->changed = TRUE;
      }
    }

done:

    va_end(va);
}

void
lgw_actiongroup_remove_entries (LgwActionGroup *self, ...)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    va_list va;
    GActionEntry* entries = NULL;
    gint length = 0;

    va_start(va, self);

    while (TRUE)
    {
      entries = va_arg(va, GActionEntry*);
      if (entries == NULL) goto done;

      gboolean removed = g_hash_table_remove (self->entries, entries);

      if (removed)
      {
        self->changed = TRUE;
      }
    }

done:

    va_end(va);
}


void
lgw_actiongroup_add_to_map (LgwActionGroup *self,
                            GActionMap     *action_map)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (G_IS_ACTION_MAP (action_map));

//    if (self->changed == FALSE) return;

    //Declarations
    GList *values = NULL;
    GtkWidget *widget = NULL;

    //Initializations
    values = g_hash_table_get_values (self->entries);
    widget = GTK_WIDGET (self->widget);

    {
      GList *link = NULL;
      LgwActionGroupData *data = NULL;
      const GActionEntry *entries = NULL;
      gint length = 0;
      for (link = values; link != NULL; link = link->next)
      {
        data = LGW_ACTIONGROUPDATA (link->data);
        entries = data->entries;
        length = data->length;

        g_action_map_add_action_entries (action_map, entries, length, widget);
      }
    }

    self->changed = FALSE;
}


void
lgw_actiongroup_remove_from_map (LgwActionGroup *self,
                                 GActionMap     *action_map)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (G_IS_ACTION_MAP (action_map));

//    if (self->changed == TRUE) return;

    //Declarations
    GList *values = NULL;
    GtkWidget *widget = NULL;

    //Initializations
    values = g_hash_table_get_values (self->entries);
    widget = GTK_WIDGET (self->widget);

    {
      GList *link = NULL;
      LgwActionGroupData *data = NULL;
      const GActionEntry *entries = NULL;
      gint length = 0;
      gint i = 0;
      for (link = values; link != NULL; link = link->next)
      {
        data = LGW_ACTIONGROUPDATA (link->data);
        entries = data->entries;
        length = data->length;

        for (i = 0; i < length; i++)
        {
          g_action_map_remove_action (action_map, entries[i].name);
        }
      }
    }

    self->changed = TRUE;
}


