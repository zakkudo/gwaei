/******************************************************************************
    AUTHOR:
    File written and Copyrighted by Pauli Virtanen. All Rights Reserved.

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
//! @file dictionaryline.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>

struct _LwDictionaryLine {
  GTree *tree;
};

gint
_compare (gpoitner a, gpointer b)
{
  return GPOINTER_TO_INT (a) - GPOINTER_TO_INT (b);
}


void
lw_dictionaryline_take (LwDictionaryLine    *self,
                        gint                *id,
                        gchar const         *parsed_text)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    gchar **strv = NULL;

    //Initializations
    
    if (parsed_text == NULL)
    {
      g_tree_remove (self->data, GINT_TO_POINTER (id));
    }
    else
    {
      strv = g_new0 (gchar*, 2);
      if (strv == NULL) goto errored;
      strv[0] = (gchar*) parsed_text;

      g_tree_insert (self, GINT_TO_POINTER (id), strv);

      strv = NULL;
      parsed_text = NULL;
    }


errored:

    g_free (strv); strv = NULL;

    return;
}


void
lw_dictionaryline_take_strv (LwDictionaryLine    *self,
                             gint                 id,
                             gchar const * *      parsed_strv)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    if (parsed_strv == NULL) return;

    if (parsed_strv == NULL)
    {
      g_tree_remove (self->data, GINT_TO_POINTER (id));
    }
    else
    {
      g_tree_insert (self->data, GINT_TO_POINTER (id), parsed_strv);
    }
}


gchar const **
lw_dictionaryline_get (LwDictionaryLine *self,
                       gint              id)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    gchar **strv = NULL;
    gchar *value = NULL;

    //Initializations
    strv = g_tree_lookup (self->data, GINT_TO_POINTER (id));
    if (strv == NULL) goto errored;
    value = strv[0];

errored:

    return value;
}


gchar * const *
lw_dictionaryline_get_strv (LwDictionaryLine *self,
                            gint              id)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    return (g_tree_lookup (self->data, GINT_TO_POINTER (id)));
}



LwDictionaryLines*
lw_dictionarylines_new (gint length)
{
    LwDictionaryLines *self = NULL;

    self = g_new0(LwDictionaryLines*, 1);
    self->lines = g_new0(LwDictionaryLine, length);
    self->length = length;

    return self;
}


void
lw_dictionarylines_free (LwDictionaryLines *self)
{
    //Sanity checks
    if (self == NULL) return; 

    {
      gint i = 0;
      for (i = 0; i < self->length; i++)
      {
        lw_dictionaryline_clear (self->lines[i]);
      }
    }

    g_free (self);
}


void
lw_dictionarylines_foreach (LwDictionaryLines            *self,
                            LwDictionaryLinesForeachFunc  func,
                            gpointer                      data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (func != NULL);

    //Declarations
    gint i = 0;

    while (i < self->length)
    {
      func(self, self->lines[i], data);
      i++;
    }
}


LwDictionaryLine*
lw_dictionarylines_get_line (LwDictionaryLines *self,
                             gint               line_number)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (line < 0, NULL);
    g_return_val_if_fail (line_number > self->length, NULL);

    //Declarations
    LwDictionaryLine *line = NULL;

    if (line_number < 0 || line_number >= self->length) goto errored;

    line = self->lines + line_number;

errored:

    return line;
}


gint
lw_dictionarylines_length (LwDictionaryLines *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);

    return self->length;
}


