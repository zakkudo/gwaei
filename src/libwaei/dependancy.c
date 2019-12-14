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

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>


struct _LwDependancy {
  GList *conditions;
  gchar *name;
  GObject *object;
};


LwDependancy*
lw_dependancy_new (const gchar *NAME)
{
    //Sanity checks
    g_return_val_if_fail (NAME != NULL, NULL);

    //Declarations
    LwDependancy *self = NULL;

    //Initializations
    self = g_new0 (LwDependancy, 1);
    self->name = g_strdup (NAME);

    return self;
}


GType
lw_dependancy_get_type ()
{
    static GType type = 0;

    if (type == 0)
    {
      type = g_boxed_type_register_static (
        "LwDependancy",
        (GBoxedCopyFunc) lw_dependancy_copy,
        (GBoxedFreeFunc) lw_dependancy_free
      );
    }

    return type;
}


LwDependancy*
lw_dependancy_copy (LwDependancy* self)
{
    //Sanity checks
    if (self == NULL) return NULL;

    //Declarations
    LwDependancy *copy = NULL;

    //Initializations
    copy = lw_dependancy_new (self->name);
    lw_dependancy_set_conditions (copy, self->conditions);
    lw_dependancy_satisfy (copy, self->object);

    return copy;
}


const gchar*
lw_dependancy_get_name (LwDependancy *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    return self->name;
}


void
lw_dependancy_set_conditions (LwDependancy *self,
                              GList        *conditions)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    if (conditions == NULL) return;

    //Declarations
    GList  *sanitized_list_copy = NULL;
    
    //Initializations
    {
      GList *link = NULL;
      for (link = conditions; link != NULL; link = link->next)
      {
        LwCondition *condition = lw_condition_copy (LW_CONDITION (link->data));
        if (condition != NULL)
        {
          sanitized_list_copy = g_list_prepend (sanitized_list_copy, condition);
        }
      }
      sanitized_list_copy = g_list_reverse (sanitized_list_copy);
    }

    g_list_free_full (self->conditions, (GDestroyNotify) lw_condition_free);
    self->conditions = sanitized_list_copy;
}


gboolean
lw_dependancy_is_satisfied_for (LwDependancy *self,
                                GObject      *object)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (object != NULL, FALSE);
    g_return_val_if_fail (G_IS_OBJECT (object), FALSE);

    //Declarations
    gboolean conditions_match = FALSE;  
    gboolean has_object = FALSE;

    //Initializations
    conditions_match = lw_condition_list_matches (self->conditions, object);
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

    self->object = object;

    if (object != NULL)
    {
      g_object_add_weak_pointer (self->object, (gpointer*) &self->object);
    }
}


GObject*
lw_dependancy_get_satisfaction (LwDependancy *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    return self->object;
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


