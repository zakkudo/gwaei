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
//!  @file file.c
//!
//!  @brief LwFilePath 
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


struct _LwFilePath {
  gchar *path;
  gchar *basename;
  const gchar *SUFFIX;
  gchar *suffixless;
};


LwFilePath*
lw_file_path_new (const gchar *PATH)
{
    //Sanity checks
    g_return_val_if_fail (PATH != NULL, NULL);

    //Declarations
    LwFilePath *self = NULL;

    //Initializations
    self = g_new0 (LwFilePath, 1);
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
lw_file_path_free (LwFilePath *self)
{
    //Sanity checks
    if (self == NULL) return;

    g_free (self->path);
    g_free (self->basename);
    g_free (self->suffixless);

    memset(self, 0, sizeof(self));

    g_free (self);
}


const gchar*
lw_file_path_get_path (LwFilePath *self)
{
    g_return_val_if_fail (self != NULL, NULL);

    return self->path;
}


const gchar*
lw_file_path_get_basename (LwFilePath *self)
{
    g_return_val_if_fail (self != NULL, NULL);

    return self->basename;
}


const gchar*
lw_file_path_get_suffixless (LwFilePath *self)
{
    g_return_val_if_fail (self != NULL, NULL);

    return self->suffixless;
}


const gchar*
lw_file_path_get_suffix (LwFilePath *self)
{
    g_return_val_if_fail (self != NULL, NULL);

    return self->SUFFIX;
}

