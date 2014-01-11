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

/*
    GType types[] = { 
      G_TYPE_UINT,   //LGW_VOCABULARYWORDSTORE_COLUMN_POSITION_INTEGER
      G_TYPE_STRING, //LGW_VOCABULARYWORDSTORE_COLUMN_POSITION_STRING
      G_TYPE_STRING, //LGW_VOCABULARYWORDSTORE_COLUMN_KANJI
      G_TYPE_STRING, //LGW_VOCABULARYWORDSTORE_COLUMN_FURIGANA
      G_TYPE_STRING, //LGW_VOCABULARYWORDSTORE_COLUMN_DEFINITIONS
      G_TYPE_INT,    //LGW_VOCABULARYWORDSTORE_COLUMN_CORRECT_GUESSES
      G_TYPE_INT,    //LGW_VOCABULARYWORDSTORE_COLUMN_INCORRECT_GUESSES
      G_TYPE_STRING, //LGW_VOCABULARYWORDSTORE_COLUMN_SCORE
      G_TYPE_UINT,   //LGW_VOCABULARYWORDSTORE_COLUMN_TIMESTAMP
      G_TYPE_STRING, //LGW_VOCABULARYWORDSTORE_COLUMN_DAYS
      G_TYPE_INT     //LGW_VOCABULARYWORDSTORE_COLUMN_WEIGHT
    };

    gtk_list_store_set_column_types (GTK_LIST_STORE (vocabulary_word_store), TOTAL_LGW_VOCABULARYWORDSTORE_COLUMNS, types);
*/

void
lgw_vocabularyliststore_init_interface (LgwVocabularyListStore *vocabulary_list_store)
{
    //TODO
}
