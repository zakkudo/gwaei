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
//!  @file result.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>



//!
//! @brief Creates a new LwResult object
//! @return An allocated LwResult that will be needed to be freed by lw_result_free.
//!
LwResult* 
lw_result_new ()
{
    LwResult* temp;

    temp = g_new0 (LwResult, 1);

    return temp;
}

//!
//! @brief Releases a LwResult object from memory.
//! @param result A LwResult object created by lw_result_new.
//!
void 
lw_result_free (LwResult *result)
{
    if (result->text != NULL) g_free (result->text); 
    memset(result, 0, sizeof(LwResult));
    free (result);
}


gboolean 
lw_result_is_similar (LwResult *result1, LwResult *result2)
{
    //Declarations
    gboolean same_def_totals, same_first_def;

    if (result1 == NULL || result2 == NULL) return FALSE;

    //Initializations
    same_def_totals = (result1->def_total == result2->def_total);
    same_first_def = (strcmp(result1->def_start[0], result2->def_start[0]) == 0);

    return (same_first_def && same_def_totals);
}

