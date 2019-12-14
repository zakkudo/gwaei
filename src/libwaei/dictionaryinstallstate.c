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


struct _LwDictionaryInstallState {
  gchar *name;
  LwFilePath **filepaths;
  gint length;
  gboolean is_temporary;
};


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

    self->filepaths = g_new0 (LwFilePath*, length + 1);

    if (self->filepaths != NULL) {
      GList *link = NULL;
      gint i = 0;
      const gchar *PATH = NULL;
      for (link = paths; link != NULL; link = link->next)
      {
        PATH = (gchar*) link->data;
        if (PATH != NULL)
        {
          LwFilePath *filepath = lw_filepath_new (PATH);
          if (filepath != NULL)
          {
            self->filepaths[i++] = filepath;
            filepath = NULL;
          }
          PATH = NULL;
        }
      }
      self->length = length;
      self->filepaths[i++] = NULL;
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
        lw_filepath_free (self->filepaths[i]);
        self->filepaths[i] = NULL;
      }
    }

    memset(self, 0, sizeof(self));

    g_free (self);
}


void
lw_dictionaryinstallstate_remove_files (LwDictionaryInstallState *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    gint i = 0;
    gint length = -1;
    LwFilePath** filepaths = NULL;

    //Initializations
    filepaths = lw_dictionaryinstallstate_get_filepaths (self, &length);

    for (i = 0; i < length; i++)
    {
      LwFilePath *filepath = filepaths[i];
      if (filepath == NULL) continue;
      const gchar *PATH = lw_filepath_get_path (filepath);
      if (PATH == NULL) continue;
      if (g_file_test (PATH, G_FILE_TEST_IS_REGULAR))
      {
        g_remove (PATH);
      }
    }
}


void
lw_dictionaryinstallstate_foreach (LwDictionaryInstallState            *self, 
                                   LwDictionaryInstallStateForeachFunc  func,
                                   gpointer                             user_data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    gint i = 0;
    LwFilePath* filepath = NULL;

    //Initializations
    for (i = 0; i < self->length; i++)
    {
      filepath = LW_FILEPATH (self->filepaths[i]);
      func(self, filepath, user_data);
    }
}


gint
lw_dictionaryinstallstate_length (LwDictionaryInstallState *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, -1);

    return self->length;
}


LwFilePath**
lw_dictionaryinstallstate_get_filepaths (LwDictionaryInstallState *self,
                                         gint                     *length)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    if (length != NULL)
    {
      *length = self->length;
    }

    return self->filepaths;
}


gboolean
lw_dictionaryinstallstate_is_temporary (LwDictionaryInstallState *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    return self->is_temporary;
}


