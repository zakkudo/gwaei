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
//!  @file condition.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include <stdlib.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>


LwCondition*
lw_condition_new_boolean (const gchar *property_name,
                          gboolean     expected_value)
{
    //Declarations
    LwCondition *self = NULL;

    //Initializations
    self = g_new0 (LwCondition, 1);
    self->property_name = g_strdup (property_name);
    g_value_init (&self->value, G_TYPE_BOOLEAN);
    g_value_set_boolean (&self->value, expected_value);
}

void
lw_condition_free (LwCondition *self)
{
    //Sanity checks
    if (self == NULL) return;

    g_free (self->property_name);
    if (G_IS_VALUE (&self->value)) g_value_unset (&self->value);
    memset(self, 0, sizeof(LwCondition));
    g_free (self);
}


LwCondition*
lw_condition_copy (LwCondition *self)
{
    //Sanity checks
    if (self == NULL) return NULL;

    //Declarations
    LwCondition *copy = NULL;

    //Initializations
    copy = g_new0 (LwCondition, 1);
    copy->property_name = g_strdup (self->property_name);
    g_value_copy (&self->value, &copy->value);

    return copy;
}
                   


gboolean
lw_condition_matches (LwCondition *self,
                      GObject     *object)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (G_IS_OBJECT (object), FALSE);

    //Declarations
    GValue value = G_VALUE_INIT;
    gboolean matches = FALSE;
    GType type = G_TYPE_INVALID;

    //Initializations
    type = G_VALUE_TYPE (&self->value);
    g_object_get_property (object, self->property_name, &value);
    if (G_VALUE_TYPE (&value) != G_VALUE_TYPE (&self->value)) goto errored;
    
    if (type == G_TYPE_STRING)
    {
      matches = (g_strcmp0 (g_value_get_string (&self->value), g_value_get_string (&value)) == 0);
    }
    else if (type == G_TYPE_UINT)
    {
      matches = (g_value_get_uint (&self->value) == g_value_get_uint (&value));
    }
    else if (type == G_TYPE_INT)
    {
      matches = (g_value_get_int (&self->value) == g_value_get_int (&value));
    }
    else if (type == G_TYPE_ULONG)
    {
      matches = (g_value_get_ulong (&self->value) == g_value_get_ulong (&value));
    }
    else if (type == G_TYPE_LONG)
    {
      matches = (g_value_get_long (&self->value) == g_value_get_long (&value));
    }
    else if (type == G_TYPE_BOOLEAN)
    {
      matches = (g_value_get_boolean (&self->value) == g_value_get_boolean (&value));
    }
    else if (type == G_TYPE_FLOAT)
    {
      matches = (g_value_get_float (&self->value) == g_value_get_float (&value));
    }
    else if (type == G_TYPE_DOUBLE)
    {
      matches = (g_value_get_double (&self->value) == g_value_get_double (&value));
    }
    else if (type == G_TYPE_POINTER)
    {
      matches = (g_value_get_pointer (&self->value) == g_value_get_pointer (&value));
    }
    else if (type == G_TYPE_GTYPE)
    {
      matches = (g_value_get_gtype (&self->value) == g_value_get_gtype (&value));
    }
    else {
      g_assert_not_reached ();
    }

errored:

    if (G_IS_VALUE (&value)) g_value_unset (&value);
}

gboolean
lw_condition_matches_all (GList   *conditions,
                          GObject *object)
{
    //Sanity checks
    g_return_val_if_fail (G_IS_OBJECT (object), FALSE);
    if (conditions == NULL) return FALSE;

    //Declarations
    gboolean matches = TRUE;

    {
      GList *link = NULL;
      for (link = conditions; link != NULL && matches == TRUE; link = link->next)
      {
        LwCondition *condition = LW_CONDITION (link->data);
        if (condition != NULL)
        {
          if (!lw_condition_matches (condition, object))
          {
            matches = FALSE;
          }
        }
      }
    }

    return matches;
}

