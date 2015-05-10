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
 * @short_description: Mark the start and end of #GMatchInfo matches
 * @title: LwQueryNodeMatchMarker
 *
 * Designed to assist in finding match start and stop endpoints so appropriate result highlighting
 * can be easily shown when printing query results.
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


/**
 * lw_querynodematchmarker_new:
 * @type: The type of marker, denoting the start or end of a match
 * @match_info: (transfer none): The #GMatchInfo used to calculate the pointers for the start and end of the match.  The object is only used for calculating values during construction, so it can be freed after calling this method.
 *
 * Designed to assist in finding match start and stop endpoints so appropriate match highlighting
 * can be shown in the view for query results. The #GRegex that #GMatchInfo is stored on the new
 * LwQueryNodeMatchMarker with it's ref count incremented such that the caller freeing the
 * #GMatchInfo will not free the #GRegex.
 *
 * Returns: (transfer full): A new #LwQueryNodeMatchMarker that can be freed with lw_querynodematchmarker_unref()
 */
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
    TOKEN = g_match_info_get_string (match_info);
    if (TOKEN == NULL) goto errored;
    g_match_info_fetch_pos (match_info, 0, &start_pos, &end_pos);
    OPEN = TOKEN + start_pos;
    CLOSE = TOKEN + end_pos;
    
    self = g_new0 (LwQueryNodeMatchMarker, 1);
    if (self == NULL) goto errored;

    self->TOKEN = TOKEN;
    self->regex = g_regex_ref (g_match_info_get_regex (match_info));
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

    return self;
}

static void
lw_querynodematchmarker_free (LwQueryNodeMatchMarker * self)
{
    if (self == NULL) return;

    if (self->regex != NULL) g_regex_unref (self->regex);
    memset(self, 0, sizeof(LwQueryNodeMatchMarker));
    g_free (self);
}


/**
 * lw_querynodematchmarker_unref:
 * @self: A #LwQueryNodeMatchMarker
 *
 * Reduces the ref count on the object and frees it once it reaches 0
 */
void
lw_querynodematchmarker_unref (LwQueryNodeMatchMarker * self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_querynodematchmarker_free (self);
    }
}


/**
 * lw_querynodematchmarker_ref:
 * 
 * Increases the reference count on an object.
 *
 * Returns: The #LwQueryNodeMatchMarker with its ref count incremented
 */
LwQueryNodeMatchMarker *
lw_querynodematchmarker_ref (LwQueryNodeMatchMarker * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}


/**
 * lw_querynodematchmarker_get_position:
 * @self: A #LwQueryNodeMatchMarker
 * @type: (out) (allow-none): A location to write the #LwQueryNodeMatchMarkerType or #NULL
 *
 * Gets the string pointed to by #LwQueryNodeMatchMarker, which will always be an offset of the
 * string returned by lw_querynodematchmarker_get_string(). When #LwQueryNodeMatchMarkerType
 * is written, it will tell you if this marker denotes the start or end of a match.  All of
 * this together allows you to do pointer arithmetic to print substrings without having to
 * make copies of the original.
 *
 * Returns: (transfer none): The substring pointed to by the marker.  It is owned by #LwQueryNodeMatchMarker and should not be freed or modified.
 */
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


/**
 * lw_querynodematchmarker_get_regex:
 * @self: A #LwQueryNodeMatchMarker
 * Returns: (transfer none) The #GRegex used to create this marker. The returned #GRegex should not be unrefed or freed.
 */
GRegex *
lw_querynodematchmarker_get_regex (LwQueryNodeMatchMarker * self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    return self->regex;
}


/**
 * lw_querynodematchmarker_get_string:
 * @self: A #LwQueryNodeMatchMarker
 * Returns: (transfer none): The substring that this marker references. This string is owned internally by the #LwQueryNodeMatchMarker and should not be freed or modified.  lw_querynodematchmarker_get_position() will reference an offset of this same string in memory.
 */
gchar const *
lw_querynodematchmarker_get_string (LwQueryNodeMatchMarker * self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    return self->TOKEN;
}
