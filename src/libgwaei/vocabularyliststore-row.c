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
//! @file vocabularyliststore.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularyliststore-private.h>


LgwVocabularyListStoreRow*
lgw_vocabularyliststorerow_new ()
{
    LgwVocabularyListStoreRow *row = NULL;

    row = g_new0 (LgwVocabularyListStoreRow, 1);

    return row;
}

void
lgw_vocabularyliststorerow_free (LgwVocabularyListStoreRow *row)
{
    //Sanity checks
    if (row == NULL) return;

    if (row->filename != NULL) g_free (row->filename);
    if (row->vocabulary_word_store != NULL) i
    {
      g_object_remove_weak_pointer (G_OBJECT (row->vocabulary_word_store), (gpointer*) &(priv->vocabulary_word_store));
      g_object_unref (G_OBJECT (row->vocabulary_word_store));
    }

    memset(row->vocabulary_word_store, 0, sizeof(LgwVocabularyWordStoreRow));
}


void 
lgw_vocabularyliststorerow_get_wordstore (LgwVocabularyListStoreRow *row)
{
    //Sanity checks
    g_return_if_fail (row != NULL);
    g_return_if_fail (filename != NULL);

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;

    //Initializations
    vocabulary_word_store = lgw_vocabularywordstore_new (row->filename);

    return vocabulary_word_store;
}




