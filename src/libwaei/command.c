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
#include <locale.h>

#include <glib.h>

#include "gettext.h"
#include "command.h"

#include "utf8.h"

typedef enum {
  PROP_0,
  PROP_NAME,
  PROP_APPLICATION,
  PROP_APPLICATION_COMMAND_LINE,
  PROP_PARAMETER_STRING,
  PROP_DESCRIPTION,
  PROP_SUMMARY,
  PROP_OPTION_ENTRIES,
  TOTAL_PROPS
} Prop;

typedef enum {
  TOTAL_SIGNALIDS
} SignalId;

typedef enum {
  CLASS_SIGNALID_RUN,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwCommandPrivate {
  gint argc;
  gchar * * argv;
  GApplication *application;
  GApplicationCommandLine *command_line;
  gchar *parameter_string;
  GOptionEntry * option_entries;
  guint signalid[TOTAL_SIGNALIDS];

  gchar * name;
  gchar * description;
  gchar * summary;

  GTree * subcommands;
  LwCommand * default_subcommand;
};

struct _LwCommandClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

static void lw_command_set_application (LwCommand *self, GApplication *application);
static void lw_command_set_command_line (LwCommand *self, GApplicationCommandLine *command_line);

static void lw_command_set_name (LwCommand * self, gchar const  * NAME);
gchar const * lw_command_get_name (LwCommand * self);

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (LwCommand, lw_command, G_TYPE_OBJECT)

static void 
lw_command_init (LwCommand *self)
{
    //Declarations
    LwCommandPrivate *priv = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);

    priv->subcommands = g_tree_new_full ((GCompareDataFunc) g_strcmp0, NULL, NULL, g_object_unref);
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
    priv = lw_command_get_instance_private (self);

    switch (property_id)
    {
      case PROP_APPLICATION:
        lw_command_set_application (self, g_value_get_object (value));
        break;
      case PROP_APPLICATION_COMMAND_LINE:
        lw_command_set_command_line (self, g_value_get_object (value));
        break;
      case PROP_PARAMETER_STRING:
        lw_command_set_parameter_string (self, g_value_get_string (value));
        break;
      case PROP_DESCRIPTION:
        lw_command_set_description (self, g_value_get_string (value));
        break;
      case PROP_SUMMARY:
        lw_command_set_summary (self, g_value_get_string (value));
        break;
      case PROP_NAME:
        lw_command_set_name (self, g_value_get_string (value));
        break;
      case PROP_OPTION_ENTRIES:
        lw_command_set_option_entries (self, g_value_get_pointer (value));
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
    priv = lw_command_get_instance_private (self);

    switch (property_id)
    {
      case PROP_APPLICATION:
        g_value_set_object (value, lw_command_get_application (self));
        break;
      case PROP_APPLICATION_COMMAND_LINE:
        g_value_set_object (value, lw_command_get_command_line (self));
        break;
      case PROP_PARAMETER_STRING:
        g_value_set_string (value, lw_command_get_parameter_string (self));
        break;
      case PROP_DESCRIPTION:
        g_value_set_string (value, lw_command_get_summary (self));
        break;
      case PROP_SUMMARY:
        g_value_set_string (value, lw_command_get_summary (self));
        break;
      case PROP_NAME:
        g_value_set_string (value, lw_command_get_name (self));
        break;
      case PROP_OPTION_ENTRIES:
        g_value_set_pointer (value, (gpointer) lw_command_get_option_entries (self));
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
    priv = lw_command_get_instance_private (self);

    lw_command_set_application (self, NULL);
    lw_command_set_command_line (self, NULL);
    lw_command_set_command_line (self, NULL);
    lw_command_set_parameter_string (self, NULL);
    lw_command_set_name (self, NULL);

    g_tree_unref (priv->subcommands);
    priv->subcommands = NULL;

    G_OBJECT_CLASS (lw_command_parent_class)->finalize (object);
}


static void
lw_command_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_command_parent_class)->constructed (object);
    }

    LwCommand *self = NULL;
    LwCommandPrivate *priv = NULL;

    self = LW_COMMAND (object);
    priv = lw_command_get_instance_private (self);
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
    object_class->constructed = lw_command_constructed;

    //Signals

    klass->priv->signalid[CLASS_SIGNALID_RUN] = g_signal_new (
        "run",
        G_OBJECT_CLASS_TYPE (klass),
        G_SIGNAL_RUN_FIRST | G_SIGNAL_DETAILED,
        G_STRUCT_OFFSET (LwCommandClass, run),
        NULL, NULL,
        g_cclosure_marshal_VOID__OBJECT,
        G_TYPE_NONE, 0
    );

    //Properties

    klass->priv->pspec[PROP_APPLICATION] = g_param_spec_object (
        "application",
        "Application",
        "Application associated with the command line.",
        G_TYPE_APPLICATION,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    g_object_class_install_property (
        object_class,
        PROP_APPLICATION,
        klass->priv->pspec[PROP_APPLICATION]
    );

    klass->priv->pspec[PROP_APPLICATION_COMMAND_LINE] = g_param_spec_object (
        "command-line",
        "Command Line",
        "The current command line context.",
        G_TYPE_APPLICATION_COMMAND_LINE,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    g_object_class_install_property (
        object_class,
        PROP_APPLICATION_COMMAND_LINE,
        klass->priv->pspec[PROP_APPLICATION_COMMAND_LINE]
    );

    klass->priv->pspec[PROP_PARAMETER_STRING] = g_param_spec_string (
        "parameter-string",
        "Parameter String",
        "The parameters in the commandline as a string",
        "",
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );

    g_object_class_install_property (
        object_class,
        PROP_PARAMETER_STRING,
        klass->priv->pspec[PROP_PARAMETER_STRING]
    );

    klass->priv->pspec[PROP_DESCRIPTION] = g_param_spec_string (
        "description",
        "Description",
        "Description of the command",
        "",
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );

    g_object_class_install_property (
        object_class,
        PROP_DESCRIPTION,
        klass->priv->pspec[PROP_DESCRIPTION]
    );

    klass->priv->pspec[PROP_SUMMARY] = g_param_spec_string (
        "summary",
        "Summary",
        "Summary of the command",
        "",
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );

    g_object_class_install_property (
        object_class,
        PROP_SUMMARY,
        klass->priv->pspec[PROP_SUMMARY]
    );

    klass->priv->pspec[PROP_NAME] = g_param_spec_string (
        "name",
        "Name",
        "The name of the command",
        NULL,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );

    g_object_class_install_property (
        object_class,
        PROP_NAME,
        klass->priv->pspec[PROP_NAME]
    );

    klass->priv->pspec[PROP_OPTION_ENTRIES] = g_param_spec_pointer (
        "option-entries",
        "Option Entries",
        "Options of this command",
        G_PARAM_READWRITE
    );

    g_object_class_install_property (
        object_class,
        PROP_OPTION_ENTRIES,
        klass->priv->pspec[PROP_OPTION_ENTRIES]
    );
}


//Properties

static void
lw_command_set_application (LwCommand    *self,
                            GApplication *application)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));
    g_return_if_fail (application == NULL || G_IS_APPLICATION (application));

    //Declarations
    LwCommandPrivate *priv = NULL;
    LwCommandClass *klass = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);
    klass = LW_COMMAND_GET_CLASS (self);
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

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_APPLICATION]);

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
    priv = lw_command_get_instance_private (self);

    return priv->application;
}


static void
lw_command_set_command_line (LwCommand               *self,
                             GApplicationCommandLine *command_line)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));
    g_return_if_fail (command_line == NULL || G_IS_APPLICATION_COMMAND_LINE (command_line));

    //Declarations
    LwCommandPrivate *priv = NULL;
    LwCommandClass *klass = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);
    klass = LW_COMMAND_GET_CLASS (self);
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

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_APPLICATION_COMMAND_LINE]);

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
    priv = lw_command_get_instance_private (self);

    return priv->command_line;
}


static GOptionContext*
lw_command_context_new (LwCommand * self)
{
    printf("lw_command_contex_new\n");
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;
    GOptionContext *context = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);

    if (!priv->command_line)
    {
      return NULL;
    }

    context = g_option_context_new (priv->parameter_string);
    if (context == NULL) goto errored;

    g_option_context_set_summary (context, priv->summary);
    g_option_context_set_description (context, priv->description);
    g_option_context_set_ignore_unknown_options (context, TRUE);

errored:

    return context;
}


static gchar const *
lw_command_parse_command_name (LwCommand   * self,
                               gchar     *** argv,
                               gint        * argc)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);
    g_return_val_if_fail (argv != NULL, NULL);
    g_return_val_if_fail (argc != NULL, NULL);
    if (*argc < 2) return NULL;

    //Declarations
    LwCommandPrivate *priv = NULL;
    gchar *command_name = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);
    command_name = *argv[1];
    argv[1] = NULL;

    if (*argc > 2)
    {
      memmove(*argv[2], *argv[1], *argc - 2), 
      *argc -= 1;
    }

errored:

    return command_name;
}


/*
static LwCommand*
lw_command_parse_optional_subcommand (LwCommand        * self,
                                      GOptionContext   * option_context,
                                      gchar          *** argv,
                                      gint             * argc)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);
    g_return_val_if_fail (option_context != NULL, NULL);
    g_return_val_if_fail (argv != NULL, NULL);
    g_return_val_if_fail (argc != NULL, NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;
    gchar const * COMMAND_NAME = NULL;
    LwCommand * subcommand = NULL;
    GOptionGroup * option_group = NULL;

    //Initialization
    priv = lw_command_get_instance_private (self);
    COMMAND_NAME = lw_command_parse_command_name (self, argv, argc);
    if (COMMAND_NAME == NULL) goto errored;
    subcommand = lw_command_lookup_subcommand (self, COMMAND_NAME);
    if (subcommand == NULL) goto errored;
    option_group = lw_command_build_option_group (subcommand);
    if (option_group)
    {
      g_option_context_add_group (option_context, option_group);
      option_group = NULL;
    }

errored:

    g_option_group_unref (option_group);
    option_group = NULL;

    return subcommand;
}


static LwCommand*
lw_command_parse_default_subcommand (LwCommand        * self,
                                     GOptionContext   * option_context,
                                     gchar          *** argv,
                                     gint             * argc)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);
    g_return_val_if_fail (option_context != NULL, NULL);
    g_return_val_if_fail (argv != NULL, NULL);
    g_return_val_if_fail (argc != NULL, NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);

    if (priv->default_subcommand != NULL)
    {
      GOptionEntry const * option_entries = lw_command_get_option_entries (priv->default_subcommand);
      if (option_entries)
      {
        g_option_context_add_main_entries (option_context, option_entries, PACKAGE);
        option_entries = NULL;
      }
    }

errored:

    return priv->default_subcommand;
}
*/

GOptionEntry const *
lw_command_get_option_entries (LwCommand *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);

    return priv->option_entries;
}


void
lw_command_set_option_entries (LwCommand       * self,
                               GOptionEntry const * option_entries)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));

    //Declarations
    LwCommandPrivate *priv = NULL;
    LwCommandClass *klass = NULL;
    gint length = 0;
    GOptionEntry* copy = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);
    if (priv->option_entries == option_entries) goto errored;
    klass = LW_COMMAND_GET_CLASS (self);

    length = 1;
    if (option_entries == NULL) goto errored;
    while (option_entries[length].long_name != NULL) length++;
    copy = g_new (GOptionEntry, length);
    if (copy == NULL) goto errored;
    memcpy(copy, option_entries, sizeof(GOptionEntry) * length);

    g_free (priv->option_entries);
    priv->option_entries = copy;
    copy = NULL;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_OPTION_ENTRIES]);

errored:

    g_free (copy); copy = NULL;

    return;
}


/*
static LwCommand*
lw_command_parse_subcommand (LwCommand        * self,
                             GOptionContext   * option_context,
                             gchar          *** argv,
                             gint             * argc)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);
    g_return_val_if_fail (option_context != NULL, NULL);
    g_return_val_if_fail (argv != NULL, NULL);
    g_return_val_if_fail (argc != NULL, NULL);

    //Declarations
    LwCommand *optional_subcommand = NULL;
    LwCommand *default_subcommand = NULL;
    LwCommand *subcommand = NULL;

    //Initializations
    default_subcommand = lw_command_parse_default_subcommand (self, option_context, argv, argc);
    optional_subcommand = lw_command_parse_optional_subcommand (self, option_context, argv, argc);

    if (optional_subcommand != NULL)
    {
      subcommand = optional_subcommand;
    }
    else
    {
      subcommand = default_subcommand;
    }

    return subcommand; 
}
*/


int
lw_command_run (LwCommand *   self,
                gchar     *** argv, 
                gint      *   argc)
{
    printf("lw_command_run\n");
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));

    //Declarations
    LwCommandPrivate * priv = NULL;
    LwCommandClass * klass = NULL;
    GError *error = NULL;
    GOptionContext * option_context = NULL;
    LwCommand *subcommand = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);
    klass = LW_COMMAND_GET_CLASS (self);
    if (argv == NULL) goto errored;  
    option_context = lw_command_context_new (self);
    if (option_context == NULL) goto errored;
    /*
    subcommand = lw_command_parse_subcommand (self, option_context, &argv, &argc);
    g_option_context_parse (option_context, &argc, &argv, &error);

    printf("lw_command_run %s\n", priv->command_name);

    if (subcommand != NULL)
    {
      lw_command_run (subcommand, &argv, &argc);
      if (priv->command_name)
      {
        GQuark detail = g_quark_from_string (priv->command_name);
        g_signal_emit (self, klass->priv->signalid[CLASS_SIGNALID_RUN], detail, subcommand);
      }
      else
      {
        g_signal_emit (self, klass->priv->signalid[CLASS_SIGNALID_RUN], 0, subcommand);
      }
    }
    else if (klass->run != NULL)
    {
        klass->run(self, &argv, &argc);
    }
    
    if (priv->argv != argv) g_strfreev (priv->argv);
    priv->argv = argv;
    argv = NULL;
    priv->argc = argc;

    option_context = NULL;
    */

errored:

    g_option_context_free (option_context);
    option_context = NULL;

    if (error != NULL)
    {
      fprintf(stderr, "%s\n", error->message);
      g_clear_error (&error);
      error = NULL;
    }

}


void
lw_command_set_parameter_string (LwCommand   * self,
                                 gchar const * parameter_string)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));

    //Declarations
    LwCommandPrivate *priv = NULL;
    LwCommandClass *klass = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);
    klass = LW_COMMAND_GET_CLASS (self);
    if (g_strcmp0(priv->parameter_string, parameter_string) == 0) goto errored;

    priv->parameter_string = g_strdup (parameter_string);

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_PARAMETER_STRING]);

errored:

    return;
}


gchar const *
lw_command_get_parameter_string (LwCommand * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);

    return priv->parameter_string;
}


void
lw_command_set_description (LwCommand   * self,
                            gchar const * DESCRIPTION)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));

    //Declarations
    LwCommandPrivate *priv = NULL;
    LwCommandClass *klass = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);
    klass = LW_COMMAND_GET_CLASS (self);
    if (g_strcmp0(priv->description, DESCRIPTION) == 0) goto errored;

    priv->description = g_strdup (DESCRIPTION);

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_DESCRIPTION]);

errored:

    return;
}


gchar const *
lw_command_get_description (LwCommand * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);

    return priv->description;
}


void
lw_command_set_summary (LwCommand   * self,
                        gchar const * SUMMARY)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));

    //Declarations
    LwCommandPrivate *priv = NULL;
    LwCommandClass *klass = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);
    klass = LW_COMMAND_GET_CLASS (self);
    if (g_strcmp0(priv->summary, SUMMARY) == 0) goto errored;

    priv->summary = g_strdup (SUMMARY);

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_SUMMARY]);

errored:

    return;
}


gchar const *
lw_command_get_summary (LwCommand * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);

    return priv->summary;
}


gchar const *
lw_command_get_name (LwCommand * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);

    return priv->name;
}


static void
lw_command_set_name (LwCommand * self,
                     gchar const  * NAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));

    //Declarations
    LwCommandPrivate *priv = NULL;
    LwCommandClass *klass = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);
    klass = LW_COMMAND_GET_CLASS (self);
    
    if (g_strcmp0 (priv->name, NAME) == 0) goto errored;

    g_free (priv->name);
    priv->name = g_strdup (NAME);
    
    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_NAME]);

errored:

    return;
}


//Methods


gchar const*
lw_command_get_locale ()
{
    return setlocale(LC_ALL, NULL);
}


gboolean 
lw_command_is_japanese_locale ()
{
    //Declarations
    const gchar *LOCALE = NULL;

    //Initialization
    LOCALE = lw_command_get_locale ();

    return (LOCALE != NULL && strncmp(LOCALE, "ja", 2) == 0);
}


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

    priv = lw_command_get_instance_private (self);
    command_line = priv->command_line;
    if (command_line == NULL) goto errored;
    va_start (ap, format);
    message = g_strdup_vprintf (format, ap);
    va_end (ap);

    G_APPLICATION_COMMAND_LINE_GET_CLASS (command_line)->print_literal (command_line, message);

errored:

    g_free (message); message = NULL;
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

    //Initializations
    priv = lw_command_get_instance_private (self);
    command_line = priv->command_line;
    if (command_line == NULL) goto errored;
    va_start (ap, format);
    message = g_strdup_vprintf (format, ap);
    va_end (ap);

    G_APPLICATION_COMMAND_LINE_GET_CLASS (command_line)->printerr_literal (command_line, message);

errored:

    g_free (message); message = NULL;
}


void
lw_command_add_subcommand (LwCommand    * self,
                           LwCommand * subcommand)
{
    //Sanity checks
    g_return_if_fail (LW_IS_COMMAND (self));
    g_return_if_fail (LW_IS_COMMAND (subcommand));

    //Declarations
    LwCommandPrivate *priv = NULL;
    gchar const *SUBCOMMAND_NAME = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);
    SUBCOMMAND_NAME = lw_command_get_name (subcommand);
    if (SUBCOMMAND_NAME == NULL) goto errored;
    if (g_tree_lookup (priv->subcommands, SUBCOMMAND_NAME) != NULL) goto errored;

    g_tree_insert (priv->subcommands, (gchar*) SUBCOMMAND_NAME, subcommand);

errored:

    return;
}


LwCommand*
lw_command_lookup_subcommand (LwCommand   * self,
                              gchar const * SUBCOMMAND_NAME)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_COMMAND (self), NULL);
    g_return_val_if_fail (SUBCOMMAND_NAME != NULL, NULL);

    //Declarations
    LwCommandPrivate *priv = NULL;
    LwCommand *subcommand = NULL;

    //Initializations
    priv = lw_command_get_instance_private (self);
    subcommand = LW_COMMAND (g_tree_lookup(priv->subcommands, SUBCOMMAND_NAME));

errored:

    return subcommand;
}
