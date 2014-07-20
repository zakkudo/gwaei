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
//!  @file dependancy.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include <stdlib.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>


LwDependancy*
lw_dependancy_new ()
{
    LwDependancy *self = NULL;

    self = g_new0 (LwDependancy, 1);

    return self;
}


void
lw_dependancy_add_conditions (LwDependancy *self,
                              GList        *conditions)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    if (conditions == NULL) return;

    //Declarations
    GList  *sanitized_copy = NULL;
    
    //Initializations
    {
      GList *link = NULL;
      for (link = conditions; link != NULL; link = link->next)
      {
        LwCondition *condition = LW_CONDITION (link->data);
        sanitized_copy = g_list_prepend (sanitized_copy, condition);
      }
      sanitized_copy = g_list_reverse (sanitized_copy);
    }

    if (self->conditions == NULL)
    {
      self->conditions = sanitized_copy;
    }
    else
    {
      self->conditions = g_list_concat (self->conditions, sanitized_copy);
    }
}


gboolean
lw_dependancy_is_satisfied (LwDependancy *self,
                            GObject      *object)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    if (object == NULL) return FALSE;

    //Declarations
    gboolean conditions_match = FALSE;  
    gboolean has_object = FALSE;

    //Initializations
    conditions_match = lw_condition_matches_all (self->conditions, object);
    has_object = (self->object != NULL);

    return (conditions_match && has_object);
}


void
lw_dependancy_satisfy (LwDependancy *self,
                       GObject      *object)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (G_IS_OBJECT (object));

    if (object != NULL)
    {
      g_object_ref (object);
    }
    
    if (self->object != NULL)
    {
      g_object_remove_weak_pointer (self->object, (gpointer*) &self->object);
      g_object_unref (self->object);
    }

    if (object != NULL)
    {
      g_object_add_weak_pointer (self->object, (gpointer*) &self->object);
    }
}


void
lw_dependancy_free (LwDependancy *self)
{
    //Sanity checks
    if (self == NULL) return;

    lw_dependancy_satisfy (self, NULL);

    g_free (self->name);
    g_list_free_full (self->conditions, (GDestroyNotify) lw_condition_free);

    memset(self, 0, sizeof(LwDependancy));
}


