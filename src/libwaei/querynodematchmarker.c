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
 * SECTION:querynodematchmarker
 * @short_description: A query tree structure for searches
 * @title: LwQueryNodeMatchMarker
 *
 * Creates a node structure than can easily be iterated to carry out a 
 * seach against a list of results.
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

#include <libwaei/querynodematchmarker.h>
#include <libwaei/gettext.h>


LwQueryNodeMatchMarker *
lw_querynodematchmarker_new (LwQueryNodeMatchMarkerType   type,
                             GMatchInfo                 * match_info)
{
    //Sanity checks
    g_return_val_if_fail (match_info != NULL, NULL);

    //Declarations
    LwQueryNodeMatchMarker * self = NULL;
    gchar const * TOKEN = NULL;
    gchar const * OPEN = NULL;
    gchar const * CLOSE = NULL;
    gint start_pos = NULL;
    gint end_pos = NULL;

    //Initializations
    match_info = g_match_info_ref (match_info);
    TOKEN = g_match_info_get_string (match_info);
    if (TOKEN == NULL) goto errored;
    g_match_info_fetch_pos (match_info, 0, &start_pos, &end_pos);
    OPEN = TOKEN + start_pos;
    CLOSE = TOKEN + end_pos;
    
    self = g_new0 (LwQueryNodeMatchMarker, 1);
    if (self == NULL) goto errored;

    self->match_info = match_info;
    match_info = NULL;
    self->OPEN = OPEN;
    self->CLOSE = CLOSE;
    self->type = type;
    self->refs = 1;

    switch (type)
    {
      case LW_QUERYNODEMATCHMARKERTYPE_OPEN:
        self->POSITION = OPEN;
        break;
      case LW_QUERYNODEMATCHMARKERTYPE_CLOSE:
        self->POSITION = CLOSE;
        break;
      default:
        g_assert_not_reached ();
        break;
    }
    
errored:

    if (match_info != NULL) g_match_info_unref (match_info);
    match_info = NULL;
}

static void
lw_querynodematchmarker_free (LwQueryNodeMatchMarker * self)
{
    if (self == NULL) return;

    if (self->match_info != NULL) g_match_info_unref (self->match_info);
    memset(self, 0, sizeof(LwQueryNodeMatchMarker));
    g_free (self);
}

void
lw_querynodematchmarker_unref (LwQueryNodeMatchMarker * self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_querynodematchmarker_free (self);
    }
}


LwQueryNodeMatchMarker *
lw_querynodematchmarker_ref (LwQueryNodeMatchMarker * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}


gchar const *
lw_querynodematchmarker_get_position (LwQueryNodeMatchMarker     * self,
                                      LwQueryNodeMatchMarkerType * type)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    if (type != NULL)
    {
      *type = self->type;
    }

    return self->POSITION;
}


GMatchInfo *
lw_querynodematchmarker_get_match_info (LwQueryNodeMatchMarker * self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    return self->match_info;
}

GRegex *
lw_querynodematchmarker_get_regex (LwQueryNodeMatchMarker * self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (self->match_info != NULL, NULL);

    return g_match_info_get_regex (self->match_info);
}


gchar const *
lw_querynodematchmarker_get_string (LwQueryNodeMatchMarker * self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (self->match_info != NULL, NULL);

    return g_match_info_get_string (self->match_info);
}
