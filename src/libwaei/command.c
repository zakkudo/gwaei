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
//! @file command.c
//!
//! @brief Abstraction layer for the command
//!
//! Used as a go between for functions interacting with the console.
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/command.h>

#include <libwaei/command-private.h>


G_DEFINE_TYPE (LwCommand, lw_command, G_TYPE_OBJECT)


LwCommand*
lw_command_new (GApplication            *application,
                GApplicationCommandLine *command_line)
{
    //Sanity checks
    g_return_val_if_fail (G_IS_APPLICATION (application), NULL);
    g_return_val_if_fail (G_IS_APPLICATION_COMMAND_LINE (command_line), NULL);

    //Declarations
    LwCommand *self = NULL;

    //Initializations
    self = g_object_new (LW_TYPE_COMMAND, "application", application, "command-line", command_line, NULL);

    return self;
}


static void 
lw_command_init (LwCommand *self)
{
    self->priv = LW_COMMAND_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwCommandPrivate));

    //Declarations
    LwCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
}


static void 
lw_command_set_property (GObject      *object,
                         guint         property_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
    //Declarations
    LwCommand *self = NULL;
    LwCommandPrivate *priv = NULL;

    //Initializations
    self = LW_COMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_APPLICATION:
        lw_command_set_application (self, g_value_get_object (value));
        break;
      case PROP_APPLICATION_COMMAND_LINE:
        lw_command_set_command_line (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_command_get_property (GObject    *object,
                         guint       property_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
    //Declarations
    LwCommand *self = NULL;
    LwCommandPrivate *priv = NULL;

    //Initializations
    self = LW_COMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_APPLICATION:
        g_value_set_object (value, lw_command_get_application (self));
        break;
      case PROP_APPLICATION_COMMAND_LINE:
        g_value_set_object (value, lw_command_get_command_line (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_command_dispose (GObject *object)
{
    //Declarations
    LwCommand *self = NULL;

    //Initializations
    self = LW_COMMAND (object);

    G_OBJECT_CLASS (lw_command_parent_class)->dispose (object);
}



static void 
lw_command_finalize (GObject *object)
{
    //Declarations
    LwCommand *self = NULL;
    LwCommandPrivate *priv = NULL;

    //Initializations
    self = LW_COMMAND (object);
    priv = self->priv;

    if (priv->application != NULL)
    {
      lw_command_set_application (self, NULL);
    }
    if (priv->command_line) {
      lw_command_set_command_line (self, NULL);
    }

    G_OBJECT_CLASS (lw_command_parent_class)->finalize (object);
}


static void
lw_command_class_init (LwCommandClass *klass)
{
    GObjectClass *object_class = NULL;

    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwCommandClassPrivate, 1);

    object_class->set_property = lw_command_set_property;
    object_class->get_property = lw_command_get_property;
    object_class->dispose = lw_command_dispose;
    object_class->finalize = lw_command_finalize;

    g_type_class_add_private (object_class, sizeof (LwCommandPrivate));

    LwCommandClassPrivate *klasspriv = klass->priv;

    klasspriv->pspec[PROP_APPLICATION] = g_param_spec_object (
        "application",
        "Application",
        "Application associated with the command line.",
        G_TYPE_APPLICATION,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_APPLICATION, klasspriv->pspec[PROP_APPLICATION]);

    klasspriv->pspec[PROP_APPLICATION_COMMAND_LINE] = g_param_spec_object (
        "command-line",
        "Command Line",
        "The current command line context.",
        G_TYPE_APPLICATION_COMMAND_LINE,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_APPLICATION_COMMAND_LINE, klasspriv->pspec[PROP_APPLICATION_COMMAND_LINE]);
}

//Properties

static void
lw_command_set_application (LwCommand    *self,
                            GApplication *application)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));
    g_return_if_fail (G_IS_APPLICATION (application));

    //Declarations
    LwCommandPrivate *priv = NULL;
    LwCommandClass *klass = NULL;
    LwCommandClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_COMMAND_CLASS (self);
    klasspriv = klass->priv;
    if (application == priv->application) goto errored;

    if (application != NULL)
    {
      g_application_hold (G_APPLICATION (application));
    }
    
    if (priv->application != NULL)
    {
      g_application_release (G_APPLICATION (priv->application));
      g_object_remove_weak_pointer (
        G_OBJECT (priv->application),
        (gpointer*) &(priv->application)
      );
    }

    priv->application = application;

    if (priv->application != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->application),
        (gpointer*) &(priv->application)
      );
    }

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_APPLICATION]);

errored:
    
    return;
}


GApplication*
lw_command_get_application (LwCommand *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->application;
}


static void
lw_command_set_command_line (LwCommand               *self,
                             GApplicationCommandLine *command_line)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));
    g_return_if_fail (G_IS_APPLICATION_COMMAND_LINE (command_line));

    //Declarations
    LwCommandPrivate *priv = NULL;
    LwCommandClass *klass = NULL;
    LwCommandClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_COMMAND_CLASS (self);
    klasspriv = klass->priv;
    if (command_line == priv->command_line) goto errored;

    if (command_line != NULL)
    {
      g_object_ref (command_line);
    }
    
    if (priv->command_line != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->command_line),
        (gpointer*) &(priv->command_line)
      );
      g_object_unref (priv->command_line);
    }

    priv->command_line = command_line;

    if (priv->command_line != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->command_line),
        (gpointer*) &(priv->command_line)
      );
    }

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_APPLICATION_COMMAND_LINE]);

errored:
    
    return;
}


GApplicationCommandLine*
lw_command_get_command_line (LwCommand *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->command_line;
}


//Methods


/*TODO

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
    gchar *text = NULL;
    gchar *query = NULL;
    gint i = 0;
    gint length = 0;
    gchar *ptr = NULL;

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


lw_application_parse_arguments (LwApplication *self)
{
    //Reset the switches to their default state
    if (priv->data.context != NULL) g_option_context_free (priv->data.context); 
    priv->data.context = g_option_context_new (gettext("- A dictionary program for Japanese-English translation."));
    if (priv->data.context == NULL) goto errored;

    g_option_context_parse (priv->data.context, &argc, &argv, &error);

    query_switch_text = lw_util_get_query_from_args (argc, argv);
}

*/


void
lw_command_print (LwCommand *self,
                  const gchar *format,
                  ...)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));
    g_return_if_fail (format != NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;
    GApplicationCommandLine *command_line = NULL;
    gchar *message = NULL;
    va_list ap;

    priv = self->priv;
    command_line = priv->command_line;
    if (command_line == NULL) goto errored;
    va_start (ap, format);
    message = g_strdup_vprintf (format, ap);
    va_end (ap);

    G_APPLICATION_COMMAND_LINE_GET_CLASS (command_line)->print_literal (command_line, message);

errored:

    g_free (message);
}


void
lw_command_printerr (LwCommand *self,
                     const gchar *format,
                     ...)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));
    g_return_if_fail (format != NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;
    GApplicationCommandLine *command_line = NULL;
    gchar *message = NULL;
    va_list ap;

    priv = self->priv;
    command_line = priv->command_line;
    if (command_line == NULL) goto errored;
    va_start (ap, format);
    message = g_strdup_vprintf (format, ap);
    va_end (ap);

    G_APPLICATION_COMMAND_LINE_GET_CLASS (command_line)->printerr_literal (command_line, message);

errored:

    g_free (message);
}

