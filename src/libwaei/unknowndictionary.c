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
//!  @file unknowndictionary.c
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

#include <libwaei/libwaei.h>
#include <libwaei/dictionary-private.h>
#include <libwaei/gettext.h>

static gint lw_unknowndictionary_parse_result (LwDictionary*, LwResult*, const gchar*);


G_DEFINE_TYPE (LwUnknownDictionary, lw_unknowndictionary, LW_TYPE_DICTIONARY)


LwDictionary* lw_unknowndictionary_new (const gchar        *FILENAME,
                                        LwMorphologyEngine *morphologyengine)
{
    //Declarations
    LwDictionary *dictionary;

    //Initializations
    dictionary = LW_DICTIONARY (g_object_new (LW_TYPE_UNKNOWNDICTIONARY,
                                "filename", FILENAME,
                                "morphologyengine", morphologyengine,
                                 NULL));

    return dictionary;
}


static void 
lw_unknowndictionary_init (LwUnknownDictionary *dictionary)
{
}


static void
lw_unknowndictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_unknowndictionary_parent_class)->constructed (object);
    }

/*
    LwDictionary *dictionary;
    LwDictionaryPrivate *priv;

    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;
*/
}


static void 
lw_unknowndictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_unknowndictionary_parent_class)->finalize (object);
}


static void
lw_unknowndictionary_class_init (LwUnknownDictionaryClass *klass)
{
    //Declarations
    GObjectClass *object_class;
    LwDictionaryClass *dictionary_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_unknowndictionary_finalize;
    object_class->constructed = lw_unknowndictionary_constructed;

    dictionary_class = LW_DICTIONARY_CLASS (klass);
    dictionary_class->priv->parse_result = lw_unknowndictionary_parse_result;
}



//!
//! @brief Parses a string for an unknown format string
//! @param rl The Resultline object this method works on
//!
static gint
lw_unknowndictionary_parse_result (LwDictionary       *dictionary, 
                                   LwResult           *result, 
                                   const gchar        *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, 0);
    g_return_val_if_fail (result != NULL, 0);
    g_return_val_if_fail (TEXT != NULL, 0);

    //Declarations
    //gchar *ptr = result->text = g_strdup (TEXT);

    return 1;
}

