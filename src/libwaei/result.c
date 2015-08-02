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

/**
 * SECTION:result
 * @short_description: A result list pertaining to a specific dictionary
 * @title: LwResults
 * @include: libwaei/result.h
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/result.h>



LwResult *
lw_result_new (LwParsedLine * line)
{
    //Sanity checks
    g_return_val_if_fail (line != NULL, NULL);

    //Declarations
    LwResult * self = NULL;

    //Initializations
    self = g_new0 (LwResult, 1);
    if (self == NULL) goto errored;

    self->line = line;

errored:

    return self;
}


void
lw_result_free (LwResult * self)
{
    if (self == NULL) return;

    memset(self, 0, sizeof(LwResult));

    g_free (self);
}


gint
lw_result_compare_score_func (LwResult * a,
                              LwResult * b)
{
    return a->score - b->score;
}


gint
lw_result_compare_column_func (LwResult    * a,
                               LwResult    * b,
                               gint          columnid,
                               GQuark        language)
{
    //Declarations
    gchar const ** strva = NULL;
    gchar const ** strvb = NULL;
    gint i = 0;
    gint compare = 0;
    static GQuark number = 0;

    //Initializations
    strva = lw_parsedline_get_strv (a->line, columnid);
    if (strva == NULL) goto errored;
    strvb = lw_parsedline_get_strv (b->line, columnid);
    if (strvb == NULL) goto errored;
    
    if (G_UNLIKELY (number == 0))
    {
      number = g_quark_from_static_string ("number");
    }

    lw_utf8_cmpnumber ();
    /*
    if (language == number)
    {
      convert string to number
      if (g_ascii_ispunct (*c)){}
      if (
      number = g_ascii_strtoll (string, &endptr, 10);
      this should be done when the result is initialized
    }
    else
    {
      while (strva[i] != NULL && strvb[i] != NULL && compare == 0)
      {
        compare = g_strcmp0 (strva[i], strvb[i]);
      }
    }
    */

errored:

    return compare;
}

