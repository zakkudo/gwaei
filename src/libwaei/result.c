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
#include <libwaei/utf8.h>
#include <libwaei/result.h>



LwResult *
lw_result_new (gint index)
{
    //Declarations
    LwResult * self = NULL;

    //Initializations
    self = g_new0 (LwResult, 1);
    if (self == NULL) goto errored;

    self->index = index;

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
