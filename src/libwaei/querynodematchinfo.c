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
 * SECTION:querynodematchinfo
 * @short_description: A way to inspect deeper information on matches
 * @title: LwQueryNodeMatchInfo
 *
 * Provides a way to more deeply inspect matches from #LwQueryNode when you
 * run lw_querynode_match_parseline().  One of the primary purposes of this
 * library is to allow easy highlighting of substrings of the match.
 */

/*
              if (g_match_info_fetch_pos (match_info, 0, &start_pos, &end_post))
              {
              }
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <ctype.h>

#include <glib.h>

#include <libwaei/querynode.h>
#include <libwaei/querynodematchinfo-private.h>
#include <libwaei/gettext.h>


LwQueryNodeMatchInfo *
lw_querynodematchinfo_new ()
{
    //Declarations
    LwQueryNodeMatchInfo * self = NULL;

    //Initializations
    self = g_new0 (LwQueryNodeMatchInfo, 1);

    return self;
}


static void
lw_querynodematchinfo_free (LwQueryNodeMatchInfo * self)
{
    if (self == NULL) return;

    memset(self, 0, sizeof(LwQueryNodeMatchInfo));

    g_free (self);
}



LwQueryNodeMatchInfo*
lw_querynodematchinfo_ref (LwQueryNodeMatchInfo * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}


void
lw_querynodematchinfo_unref (LwQueryNodeMatchInfo *self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_querynodematchinfo_free (self);
    }
}

void
lw_querynodematchinfo_add (LwQueryNodeMatchInfo * self,
                           gint column,
                           gint strv_index,
                           GMatchInfo *match_info)
{
}
