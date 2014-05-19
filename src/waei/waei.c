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
//! @file waei.c
//!
//! @brief Main entrance into the program.
//!
//! Main entrance into the program.
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>

#include <glib.h>

#include <waei/gettext.h>
#include <waei/waei.h>


static void
guarantee_server (int argc, char *argv[])
{
    //Declarations
    gchar **command = NULL;
    GError *error = NULL;
    gboolean need_new_instance = FALSE;
    GPid pid = 0;

    {
      gint i = 1;
      for (i = 1; i < argc; i++)
      {
        if (g_strcmp0 (argv[i], "--start-server") == 0)
        {
          printf("BREAK server is started\n");
          goto errored;
        }
      }
    }

    //Initializations
    command = g_new0(gchar*, 3);
    command[0] = g_strdup (argv[0]);
    command[1] = g_strdup ("--start-server");
    command[2] = NULL;
    if (command == NULL) goto errored;

    printf("BREAK starting server\n");
    g_spawn_async (NULL, command, NULL, G_SPAWN_SEARCH_PATH | G_SPAWN_STDERR_TO_DEV_NULL | G_SPAWN_STDOUT_TO_DEV_NULL, NULL, NULL, &pid, &error);

    g_spawn_close_pid (pid);

errored:

    if (error != NULL)
    {
      g_error (error->message);
      g_clear_error (&error);
    }
    g_strfreev (command); command = NULL;

    return;
}


int 
main (int argc, char *argv[])
{
    GApplication *application = NULL;
    int resolution = -1;
    GApplicationFlags flags = 0;
    gint tries = 10;

    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, GWAEI_LOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    guarantee_server (argc, argv);

    {
      gint i = 1;
      for (i = 1; i < argc; i++)
      {
        if (g_strcmp0 (argv[i], "--start-server") == 0)
        {
          printf("BREAK is service\n");
          flags |= G_APPLICATION_IS_SERVICE;
          break;
        }
      }
      if ((G_APPLICATION_IS_SERVICE & flags) == 0)
      {
        printf("BREAK is launch\n");
        flags |= G_APPLICATION_IS_LAUNCHER;
      }
    }

    application = G_APPLICATION (w_application_new (flags));

    if (flags & G_APPLICATION_IS_LAUNCHER)
    {
      
      while (tries-- > 0 && !g_application_register (G_APPLICATION (application), NULL, NULL))
      {
        g_usleep (10);
      }
      printf("BREAK run launcher\n");
      resolution = g_application_run (G_APPLICATION (application), argc, argv);
    }
    else
    {
      printf("BREAK run service\n");
      resolution = g_application_run (G_APPLICATION (application), argc, argv);
    }

    g_object_unref (application);
    application = NULL;

    if ((flags & G_APPLICATION_IS_SERVICE) != 0) printf("BREAK service is exiting\n");
    if ((flags & G_APPLICATION_IS_LAUNCHER) != 0) printf("BREAK launcher is exiting\n");

    return resolution;
}


