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
//! @file stackwidget.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

G_DEFINE_INTERFACE (LgwActionable, lgw_actionable, 0);


typedef enum {
    PROP_0,
    PROP_ACTIONS,
    TOTAL_PROPS
} Props;

GParamSpec* _param_spec[TOTAL_PROPS];


static void
lgw_actionable_default_init (LgwActionableInterface *iface)
{
    memset(_param_spec, 0, (sizeof(GParamSpec*) * TOTAL_PROPS));

    _param_spec[PROP_ACTIONS] = g_param_spec_pointer (
      "actions",
      "actions",
      "A GList of LgwActionGroup items to be used iwth a GActionMap",
      G_PARAM_READABLE
    );

    g_object_interface_install_property (iface, _param_spec[PROP_ACTIONS]);
}


GList*
lgw_actionable_get_actions (LgwActionable *self)
{
    printf("BREAK actionable get actions\n");

    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    return LGW_ACTIONABLE_GET_INTERFACE (self)->get_actions (self);
}


void
lgw_actionable_set_actiongroup (LgwActionable  *self,
                                LgwActionGroup *action_group)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    printf("BREAK actionable set actiongroup\n");

    LGW_ACTIONABLE_GET_INTERFACE (self)->set_actiongroup (self, action_group);
}


void
lgw_actionable_notify_actions (LgwActionable *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    g_object_notify_by_pspec (G_OBJECT (self), _param_spec[PROP_ACTIONS]);
}


