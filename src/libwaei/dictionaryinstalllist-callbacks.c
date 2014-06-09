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
//!  @file dictionary-callbacks.c
//!
//!  @brief LwDictionary objects represent a loaded dictionary that the program
//!         can use to carry out searches.  You can uninstall dictionaries
//!         by using the object, but you cannot install them. LwDictInst
//!         objects exist for that purpose.
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>

#include <libwaei/dictionaryinstalllist-private.h>


void
lw_dictionaryinstalllist_connect_signals (LwDictionaryInstallList *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALLLIST (self));

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
}


void
lw_dictionaryinstalllist_disconnect_signals (LwDictionaryInstallList *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALLLIST (self));

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
}
