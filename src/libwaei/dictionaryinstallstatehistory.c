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


LwDictionaryInstallStateFile*
lw_dictionaryinstallstatefile_new (const gchar *PATH)
{
    //Sanity checks
    g_return_val_if_fail (PATH != NULL, NULL);

    //Declarations
    LwDictionaryInstallStateFile *self = NULL;

    //Initializations
    self = g_new0 (LwDictionaryInstallStateFile, 1);
    if (self == NULL) goto errored;
    self->path = g_strdup (PATH);
    self->basename = g_path_get_basename (PATH);
    self->SUFFIX = g_utf8_strrchr (self->basename, strlen(self->basename), '.');
    if (self->SUFFIX == NULL) self->SUFFIX = "";
    self->suffixless = g_strndup(self->path, strlen(self->path) - strlen(self->SUFFIX));

errored:

    return self;
}


void
lw_dictionaryinstallstatefile_free (LwDictionaryInstallStateFile *self)
{
    //Sanity checks
    if (self == NULL) return;

    g_free (self->path);
    g_free (self->basename);
    g_free (self->suffixless);

    memset(self, 0, sizeof(self));

    g_free (self);
}


LwDictionaryInstallState*
lw_dictionaryinstallstate_new_glist (const gchar *NAME,
                                     gboolean     is_temporary,
                                     GList       *paths)
{
    //Declarations
    LwDictionaryInstallState *self = NULL;
    gint length = 0;

    //Initializations
    self = g_new0 (LwDictionaryInstallState, 1);
    self->name = g_strdup (NAME);
    self->is_temporary = is_temporary;
    length = g_list_length (paths);

    self->files = g_new0 (LwDictionaryInstallStateFile*, length + 1);

    if (self->files != NULL) {
      GList *link = NULL;
      gint i = 0;
      const gchar *PATH = NULL;
      for (link = paths; link != NULL; link = link->next)
      {
        PATH = (gchar*) link->data;
        if (PATH != NULL)
        {
          LwDictionaryInstallStateFile *file = lw_dictionaryinstallstatefile_new (PATH);
          if (file != NULL)
          {
            self->files[i++] = file;
            file = NULL;
          }
          PATH = NULL;
        }
      }
      self->length = length;
      self->files[i++] = NULL;
    }

errored:

    return self;
}


LwDictionaryInstallState*
lw_dictionaryinstallstate_new_valist (const gchar *NAME,
                                      gboolean     is_temporary,
                                      va_list      va)
{
    //Declarations
    LwDictionaryInstallState *self = NULL;
    GList *paths = NULL;

    {
      while (TRUE)
      {
        const gchar *PATH = va_arg(va, gchar*);
        if (PATH == NULL || g_utf8_validate (PATH, -1, NULL) == FALSE) break;
        paths = g_list_prepend (paths, (gchar*) PATH);
      }
      paths = g_list_reverse (paths);
    }

    self = lw_dictionaryinstallstate_new_glist (NAME, is_temporary, paths);

errored:

    g_list_free (paths); paths = NULL;


    return self;
}


LwDictionaryInstallState*
lw_dictionaryinstallstate_new (const gchar *NAME,
                               gboolean     is_temporary,
                               ...)
{
    //Declarations
    va_list va;

    //Initializations
    va_start(va, is_temporary);

    lw_dictionaryinstallstate_new_valist (NAME, is_temporary, va);
}


void
lw_dictionaryinstallstate_free (LwDictionaryInstallState *self)
{
    //Sanity checks
    if (self == NULL) return;

    //Declarations
    g_free (self->name);

    {
      gint i = 0;
      for (i = 0; i < self->length; i++)
      {
        lw_dictionaryinstallstatefile_free (self->files[i]);
        self->files[i] = NULL;
      }
    }

    memset(self, 0, sizeof(self));

    g_free (self);
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
lw_dictionaryinstallstatehistory_add_paths (LwDictionaryInstallStateHistory *self,
                                            const gchar                     *NAME,
                                            gboolean                         is_temporary,
                                            GList                           *paths)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (NAME != NULL);
    g_return_if_fail (paths != NULL);
    if (NAME == NULL) return;
    if (paths == NULL) return;

    //Declarations
    LwDictionaryInstallState *state = NULL;

    //Initializations
    state = lw_dictionaryinstallstate_new_glist (NAME, is_temporary, paths);
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
lw_dictionaryinstallstatehistory_clear (LwDictionaryInstallStateHistory *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    g_list_free_full (self->states, (GDestroyNotify) lw_dictionaryinstallstate_free);
    self->states = NULL;
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

