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
//! @file utilities.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <ctype.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>


//!
//! @brief Checks for a Japanese local messages setting
//! @returns A boolean stating whether the locale is a Japanese utf8 one
//! @return Returns true if it is a japanese local
//!
gboolean 
lw_util_is_japanese_locale ()
{
    return (setlocale(LC_ALL, NULL) != NULL &&
             (
               strcmp(setlocale(LC_ALL, NULL), "ja_JP.UTF8")  == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "ja_JP.UTF-8") == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "ja_JP.utf8")  == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "ja_JP.utf-8") == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "ja_JP")       == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "ja")          == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "japanese")    == 0
             )
           );
}


//!
//! @brief Converts the arguments passed to the program into a query search string
//! @param argc The argc argument passed to main
//! @param argv The argv argument passed to main
//! @returns An allocated string that must be freed with g_free
//!
gchar* 
lw_util_get_query_from_args (int argc, char** argv)
{
    //Sanity check
    if (argc < 2) return NULL;

    //Declarations
    char *text;
    char *query;
    int i;
    int length;
    char *ptr;

    //Initializations
    text = NULL;
    query = NULL;
    length = 0;

    //Get the required length of the combined string
    for (i = 1; i < argc; i++) 
    {
      length += strlen (argv[i]) + 1;
    }

    //Allocate it and set up the iterator
    text = (char*) malloc(length * sizeof(char) + 1);
    if (text == NULL) return NULL;
    ptr = text;

    //Copy the argument words into the query
    for (i = 1; i < argc; i++)
    { 
      strcpy(ptr, argv[i]);
      ptr += strlen(argv[i]);

      if (i == argc - 1) break;

      strcpy(ptr, " ");
      ptr += strlen(" ");
    }

    query = g_strdup (text);
    lw_utf8_sanitize (query);

    //Cleanup
    if (text != NULL) free (text);

    return query;
}


gchar*
lw_util_get_environment_data_folder ()
{
    gchar **environment = g_get_environ ();
    if (environment == NULL) return NULL;
    gchar *folder = g_strdup (g_environ_getenv (environment, "LIBWAEI_DATA_FOLDER"));
    g_strfreev (environment); environment = NULL;
    return folder;
}


