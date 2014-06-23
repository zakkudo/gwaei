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
//!  @file dictionaryinstallstate.c
//!
//!  @brief LwDictionaryInstallState stores the self of the dictionary install
//!         that can be shared between states.
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>

#include <libwaei/dictionaryinstallstatehistory.h>


LwDictionaryInstallState*
lw_dictionaryinstallstate_new (const gchar *NAME,
                               const gchar *PATH)
{
    //Declarations
    LwDictionaryInstallState *self = NULL;

    //Initializations
    self = g_new0 (LwDictionaryInstallState, 1);

    self->name = g_strdup (NAME);
    self->path = g_strdup (PATH);

    return self;
}


void
lw_dictionaryinstallstate_free (LwDictionaryInstallState *self)
{
    //Sanity checks
    if (self == NULL) return;

    //Declarations
    g_free (self->name);
    g_free (self->path);

    memset(self, 0, sizeof(self));
}


LwDictionaryInstallStateHistory*
lw_dictionaryinstallstatehistory_new ()
{
    LwDictionaryInstallStateHistory *temp = NULL;

    temp = g_new0 (LwDictionaryInstallStateHistory, 1);

    return temp;
}


void
lw_dictionaryinstallstatehistory_free (LwDictionaryInstallStateHistory *self)
{
    //Sanity checks
    if (self == NULL) return;

    //Declarations

    //Initializations:
    g_list_free_full (self->states, (GDestroyNotify) lw_dictionaryinstallstate_free);
    self->states = NULL;
}


void
lw_dictionaryinstallstatehistory_add (LwDictionaryInstallStateHistory *self,
                                      LwDictionaryInstallState        *state)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (state != NULL);

    self->states = g_list_prepend (self->states, state);
}


void
lw_dictionaryinstallstatehistory_add_full (LwDictionaryInstallStateHistory *self,
                                           const gchar  *NAME,
                                           const gchar  *PATH)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (NAME != NULL);
    g_return_if_fail (PATH != NULL);
    if (NAME == NULL) return;
    if (PATH == NULL) return;

    //Declarations
    LwDictionaryInstallState *state = NULL;

    //Initializations
    state = lw_dictionaryinstallstate_new (NAME, PATH);
    if (state == NULL) goto errored;

    lw_dictionaryinstallstatehistory_add (self, state);
    
errored:

    return;
}


LwDictionaryInstallState*
lw_dictionaryinstallstatehistory_current (LwDictionaryInstallStateHistory *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryInstallState *state = NULL;

    //Initializations
    state = (LwDictionaryInstallState*) (g_list_nth_data (self->states, 0));

    return state;
}


void
lw_dictionaryinstallstatehistory_foreach (LwDictionaryInstallStateHistory            *self, 
                                          LwDictionaryInstallStateHistoryForeachFunc  func,
                                          gpointer                                    user_data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    GList *link = NULL;
    LwDictionaryInstallState *state = NULL;

    //Initializations
    for (link = self->states; link != NULL; link = link->next)
    {
      state = (LwDictionaryInstallState*) (link->data);
      func(self, state, user_data);
    }
}

