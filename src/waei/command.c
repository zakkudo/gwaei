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

#include <waei/gettext.h>
#include <waei/waei.h>

#include <waei/command-private.h>


static WCommandClass *_klass = NULL;
static WCommandClassPrivate *_klasspriv = NULL;

G_DEFINE_TYPE (WCommand, w_command, G_TYPE_OBJECT)


//!
//! @brief creates a new instance of the gwaei applicaiton
//!
WCommand*
w_command_new (WApplication            *application,
               GApplicationCommandLine *command_line)
{
    //Sanity checks
    g_return_val_if_fail (command_line != NULL, NULL);
    g_return_val_if_fail (command_line != NULL, NULL);

    //Declarations
    WCommand *self = NULL;

    //Initializations
    self = g_object_new (W_TYPE_COMMAND, "application", application, "command-line", command_line, NULL);

    return self;
}


static void 
w_command_init (WCommand *self)
{
    self->priv = W_COMMAND_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(WCommandPrivate));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    priv->data.loop = g_main_loop_new (NULL, FALSE);
}


static void 
w_command_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (w_command_parent_class)->constructed (object);
    }

    //Declarations
    WCommand *self = NULL;
    WCommandPrivate *priv = NULL;
    WApplication *application = NULL;

    //Initializations
    self = W_COMMAND (object);
    priv = self->priv;
    application = w_command_get_application (self);
/*

    if (priv->argument.dictionary_switch_text)
    {
        LwDictionaryList *list = w_application_get_installed_dictionarylist (self);
        LwDictionary *dictionary = lw_dictionarylist_get_dictionary_by_filename (list, priv->argument.dictionary_switch_text);
        w_command_set_dictionary (self, dictionary);
    }


    lw_regex_initialize ();
*/
}


static void 
w_command_set_property (GObject      *object,
                        guint         property_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
    //Declarations
    WCommand *self = NULL;
    WCommandPrivate *priv = NULL;

    //Initializations
    self = W_COMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_APPLICATION:
        w_command_set_application (self, g_value_get_object (value));
        break;
      case PROP_COMMAND_LINE:
        w_command_set_command_line (self, g_value_get_object (value));
        break;
      case PROP_DICTIONARY_SWITCH_TEXT:
        w_command_set_dictionary_switch_text (self, g_value_get_string (value));
        break;
      case PROP_DICTIONARY_INSTALL_SWITCH_TEXT:
        w_command_set_dictionary_install_switch_text (self, g_value_get_string (value));
        break;
      case PROP_DICTIONARY_UNINSTALL_SWITCH_TEXT:
        w_command_set_dictionary_uninstall_switch_text (self, g_value_get_string (value));
        break;
      case PROP_QUERY_SWITCH_TEXT:
        w_command_set_query_switch_text (self, g_value_get_string (value));
        break;
      case PROP_QUIET_SWITCH:
        w_command_set_quiet_switch (self, g_value_get_boolean (value));
        break;
      case PROP_EXACT_SWITCH:
        w_command_set_quiet_switch (self, g_value_get_boolean (value));
        break;
      case PROP_LIST_SWITCH:
        w_command_set_quiet_switch (self, g_value_get_boolean (value));
        break;
      case PROP_VERSION_SWITCH:
        w_command_set_version_switch (self, g_value_get_boolean (value));
        break;
      case PROP_COLOR_SWITCH:
        w_command_set_color_switch (self, g_value_get_boolean (value));
        break;
      case PROP_FORCE_INDEX_REBUILD_SWITCH:
        w_command_set_force_index_rebuild_switch (self, g_value_get_boolean (value));
        break;
      case PROP_START_SERVER_SWITCH:
        w_command_set_start_server_switch (self, g_value_get_boolean (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
w_command_get_property (GObject    *object,
                        guint       property_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
    //Declarations
    WCommand *self = NULL;
    WCommandPrivate *priv = NULL;

    //Initializations
    self = W_COMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_APPLICATION:
        g_value_set_object (value, w_command_get_application (self));
        break;
      case PROP_COMMAND_LINE:
        g_value_set_object (value, w_command_get_command_line (self));
        break;
      case PROP_DICTIONARY_SWITCH_TEXT:
        g_value_set_string (value, w_command_get_dictionary_switch_text (self));
        break;
      case PROP_DICTIONARY_INSTALL_SWITCH_TEXT:
        g_value_set_string (value, w_command_get_dictionary_install_switch_text (self));
        break;
      case PROP_DICTIONARY_UNINSTALL_SWITCH_TEXT:
        g_value_set_string (value, w_command_get_dictionary_uninstall_switch_text (self));
        break;
      case PROP_QUERY_SWITCH_TEXT:
        g_value_set_string (value, w_command_get_query_switch_text (self));
        break;
      case PROP_QUIET_SWITCH:
        g_value_set_boolean (value, w_command_get_quiet_switch (self));
        break;
      case PROP_EXACT_SWITCH:
        g_value_set_boolean (value, w_command_get_exact_switch (self));
        break;
      case PROP_LIST_SWITCH:
        g_value_set_boolean (value, w_command_get_list_switch (self));
        break;
      case PROP_VERSION_SWITCH:
        g_value_set_boolean (value, w_command_get_version_switch (self));
        break;
      case PROP_COLOR_SWITCH:
        g_value_set_boolean (value, w_command_get_color_switch (self));
        break;
      case PROP_FORCE_INDEX_REBUILD_SWITCH:
        g_value_set_boolean (value, w_command_get_force_index_rebuild_switch (self));
        break;
      case PROP_START_SERVER_SWITCH:
        g_value_set_boolean (value, w_command_get_start_server_switch (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
w_command_dispose (GObject *object)
{
    //Declarations
    WCommand *self = NULL;

    //Initializations
    self = W_COMMAND (object);

    G_OBJECT_CLASS (w_command_parent_class)->dispose (object);
}



static void 
w_command_finalize (GObject *object)
{
    //Declarations
    WCommand *self = NULL;
    WCommandPrivate *priv = NULL;

    //Initializations
    self = W_COMMAND (object);
    priv = self->priv;

    w_command_set_application (self, NULL);
    w_command_set_command_line (self, NULL);

    G_OBJECT_CLASS (w_command_parent_class)->finalize (object);
}


static void
w_command_class_init (WCommandClass *klass)
{
    GObjectClass *object_class = NULL;

    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (WCommandClassPrivate, 1);

    object_class->constructed = w_command_constructed;
    object_class->set_property = w_command_set_property;
    object_class->get_property = w_command_get_property;
    object_class->dispose = w_command_dispose;
    object_class->finalize = w_command_finalize;

    g_type_class_add_private (object_class, sizeof (WCommandPrivate));

    _klass = klass;
    _klasspriv = klass->priv;

    _klasspriv->pspec[PROP_APPLICATION] = g_param_spec_object (
        "application",
        "FIlename construct prop",
        "Set the filename",
        W_TYPE_APPLICATION,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_APPLICATION, _klasspriv->pspec[PROP_APPLICATION]);

    _klasspriv->pspec[PROP_COMMAND_LINE] = g_param_spec_object (
        "command-line",
        "FIlename construct prop",
        "Set the filename",
        G_TYPE_APPLICATION_COMMAND_LINE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_COMMAND_LINE, _klasspriv->pspec[PROP_COMMAND_LINE]);

    _klasspriv->pspec[PROP_DICTIONARY_SWITCH_TEXT] = g_param_spec_string (
        "dictionary-switch-text",
        "FIlename construct prop",
        "Set the filename",
        "",
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DICTIONARY_SWITCH_TEXT, _klasspriv->pspec[PROP_DICTIONARY_SWITCH_TEXT]);

    _klasspriv->pspec[PROP_DICTIONARY_INSTALL_SWITCH_TEXT] = g_param_spec_string (
        "dictionary-install-switch-text",
        "FIlename construct prop",
        "Set the filename",
        "",
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DICTIONARY_INSTALL_SWITCH_TEXT, _klasspriv->pspec[PROP_DICTIONARY_INSTALL_SWITCH_TEXT]);

    _klasspriv->pspec[PROP_DICTIONARY_UNINSTALL_SWITCH_TEXT] = g_param_spec_string (
        "dictionary-uninstall-switch-text",
        "FIlename construct prop",
        "Set the filename",
        "",
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DICTIONARY_UNINSTALL_SWITCH_TEXT, _klasspriv->pspec[PROP_DICTIONARY_UNINSTALL_SWITCH_TEXT]);

    _klasspriv->pspec[PROP_QUERY_SWITCH_TEXT] = g_param_spec_string (
        "query-switch-text",
        "FIlename construct prop",
        "Set the filename",
        "",
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_QUERY_SWITCH_TEXT, _klasspriv->pspec[PROP_QUERY_SWITCH_TEXT]);


    _klasspriv->pspec[PROP_QUIET_SWITCH] = g_param_spec_boolean (
        "quiet-switch",
        "FIlename construct prop",
        "Set the filename",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_QUIET_SWITCH, _klasspriv->pspec[PROP_QUIET_SWITCH]);

    _klasspriv->pspec[PROP_EXACT_SWITCH] = g_param_spec_boolean (
        "exact-switch",
        "FIlename construct prop",
        "Set the filename",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_EXACT_SWITCH, _klasspriv->pspec[PROP_EXACT_SWITCH]);

    _klasspriv->pspec[PROP_LIST_SWITCH] = g_param_spec_boolean (
        "list-switch",
        "FIlename construct prop",
        "Set the filename",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_LIST_SWITCH, _klasspriv->pspec[PROP_LIST_SWITCH]);

    _klasspriv->pspec[PROP_FORCE_INDEX_REBUILD_SWITCH] = g_param_spec_boolean (
        "force-index-rebuild-switch",
        "FIlename construct prop",
        "Set the filename",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_FORCE_INDEX_REBUILD_SWITCH, _klasspriv->pspec[PROP_FORCE_INDEX_REBUILD_SWITCH]);

    _klasspriv->pspec[PROP_START_SERVER_SWITCH] = g_param_spec_boolean (
        "start-server-switch",
        "FIlename construct prop",
        "Set the filename",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_START_SERVER_SWITCH, _klasspriv->pspec[PROP_START_SERVER_SWITCH]);
}


//!
//! @brief Loads the arguments from the command line into the app instance
//!
static void 
w_command_parse_args (WCommand  *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;
    GApplicationCommandLine *command_line = NULL;
    gint argc = -1;
    gchar **argv = NULL;
    const gchar *SUMMARY_TEXT = NULL;
    gchar *description_text = NULL;
    GError *error = NULL;

    gboolean exact_switch = FALSE;
    gboolean quiet_switch = FALSE;
    gboolean color_switch = FALSE;
    gboolean list_switch = FALSE;
    gboolean version_switch = FALSE;
    gboolean force_index_rebuild_switch = FALSE;
    gboolean start_server_switch = FALSE;

    gchar *query_switch_text = NULL;
    gchar *dictionary_switch_text = NULL;
    gchar *dictionary_install_switch_text = NULL;
    gchar *dictionary_uninstall_switch_text = NULL;

    //Initializations
    priv = self->priv;
    command_line = priv->data.command_line;
    if (command_line == NULL) goto errored;
    argv = g_application_command_line_get_arguments (command_line, &argc);

    //Reset the switches to their default state
    if (priv->data.context != NULL) g_option_context_free (priv->data.context); 
    priv->data.context = g_option_context_new (gettext("- A dictionary program for Japanese-English translation."));
    if (priv->data.context == NULL) goto errored;

    SUMMARY_TEXT = gettext("waei generally outputs directly to the console.");
    description_text = g_strdup_printf(
        gettext(
           "Examples:\n"
           "  waei English               Search for the english word English\n"
           "  waei \"cats&dogs\"           Search for results containing cats and dogs\n"
           "  waei \"cats|dogs\"           Search for results containing cats or dogs\n"
           "  waei cats dogs             Search for results containing \"cats dogs\"\n"
           "  waei %s                Search for the Japanese word %s\n"
           "  waei -e %s               Search for %s and ignore similar results\n"
           "  waei %s                 When you don't know a kanji character\n"
           "  waei -d Kanji %s           Find a kanji character in the kanji dictionary\n"
           "  waei -d Names %s       Look up a name in the names dictionary\n"
           "  waei -d Places %s       Look up a place in the places dictionary"
         )
         , "にほん", "にほん", "日本", "日本", "日.語", "魚", "Miyabe", "Tokyo"
    );

    GOptionEntry entries[] = 
    {
      { "exact", 'e', 0, G_OPTION_ARG_NONE, &exact_switch, gettext("Do not display less relevant results"), NULL },
      { "quiet", 'q', 0, G_OPTION_ARG_NONE, &quiet_switch, gettext("Display less information"), NULL },
      { "color", 'c', 0, G_OPTION_ARG_NONE, &color_switch, gettext("Display results with color"), NULL },
      { "dictionary", 'd', 0, G_OPTION_ARG_STRING, &dictionary_switch_text, gettext("Search using a chosen dictionary"), NULL },
      { "list", 'l', 0, G_OPTION_ARG_NONE, &list_switch, gettext("Show available dictionaries for searches"), NULL },
      { "install", 'i', 0, G_OPTION_ARG_STRING, &dictionary_install_switch_text, gettext("Install dictionary"), NULL },
      { "uninstall", 'u', 0, G_OPTION_ARG_STRING, &dictionary_uninstall_switch_text, gettext("Uninstall dictionary"), NULL },
      { "rebuild-index", 0, 0, G_OPTION_ARG_NONE, &force_index_rebuild_switch, gettext("Rebuild dictionary indexes"), NULL },
      { "version", 'v', 0, G_OPTION_ARG_NONE, &version_switch, gettext("Check the waei version information"), NULL },
      { "start-server", 0, 0, G_OPTION_ARG_NONE, &start_server_switch, gettext("Star the waei server"), NULL },
      { NULL }
    };

    g_option_context_set_description (priv->data.context, description_text);
    g_option_context_set_summary (priv->data.context, SUMMARY_TEXT);
    g_option_context_add_main_entries (priv->data.context, entries, PACKAGE);
    g_option_context_set_ignore_unknown_options (priv->data.context, TRUE);
    g_option_context_parse (priv->data.context, &argc, &argv, &error);

    query_switch_text = lw_util_get_query_from_args (argc, argv);

    w_command_set_exact_switch (self, exact_switch);
    w_command_set_quiet_switch (self, quiet_switch);
    w_command_set_color_switch (self, color_switch);
    w_command_set_list_switch (self, list_switch);
    w_command_set_version_switch (self, version_switch);
    w_command_set_force_index_rebuild_switch (self, force_index_rebuild_switch);
    w_command_set_start_server_switch (self, start_server_switch);

    w_command_set_query_switch_text (self, query_switch_text);
    w_command_set_dictionary_switch_text (self, dictionary_switch_text);
    w_command_set_dictionary_install_switch_text (self, dictionary_install_switch_text);
    w_command_set_dictionary_uninstall_switch_text (self, dictionary_uninstall_switch_text);

errored:

    g_free (query_switch_text); query_switch_text = NULL;
    g_free (dictionary_switch_text); dictionary_switch_text = NULL;
    g_free (dictionary_install_switch_text); dictionary_install_switch_text = NULL;
    g_free (dictionary_uninstall_switch_text); dictionary_uninstall_switch_text = NULL;

    if (error != NULL)
    {
      w_command_printerr (self, "%s\n", error->message);
      g_clear_error (&error);
    }

    //Get the query after the flags have been parsed out
    if (argv != NULL) g_strfreev (argv);
    g_free (description_text); description_text = NULL;

    return;
}


gint
w_command_run (WCommand *self)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_COMMAND (self), -1);

    //Declarations
    WCommandPrivate *priv = NULL;
    gint resolution = -1;

    //Initializations
    priv = self->priv;

    //User wants to see what dictionaries are available
    if (w_command_get_list_switch (self))
    {
      w_command_list (self);
    }

    //User wants to see the version of waei
    else if (w_command_get_version_switch (self))
    {
      w_command_about (self);
    }

    //User wants to install a dictionary
    else if (w_command_has_dictionary_install_request (self))
    {
      resolution = w_command_install_dictionary (self);
    }

    //User wants to uninstall a dictionary
    else if (w_command_has_dictionary_uninstall_request (self))
    {
      printf("BREAK uninstall\n");
      resolution = w_command_uninstall_dictionary (self);
    }

    //User wants to do a search
    else if (w_command_has_query_request (self))
    {
      resolution = w_command_search (self);
    }

    //User didn't specify enough information for an action
    else 
    {
      gchar *text = g_option_context_get_help (priv->data.context, FALSE, NULL);
      if (text != NULL)
      {
        w_command_print (self, "%s\n", text);
        g_free (text); text = NULL;
      }
    }

errored:

    //Cleanup
    //w_application_handle_error (self, &progress->error); TODO

    return resolution;
}


//!
//! @brief Uninstalls the named dictionary, deleting it.
//!
//! @param name A string of the name of the dictionary to uninstall.
//!
gint 
w_command_uninstall_dictionary (WCommand *self)
{
    //Sanity check
    g_return_val_if_fail (W_IS_COMMAND (self), -1);

    //Declarations
    WCommandPrivate *priv = NULL;
    WApplication *application = NULL;
    LwDictionary *dictionary = NULL;
    gint resolution = -1;
    const gchar *DICTIONARY_UNINSTALL_SWITCH_TEXT = NULL;

    //Initializations
    priv = self->priv;
    application = priv->data.application;
    if (application == NULL) goto errored;
    DICTIONARY_UNINSTALL_SWITCH_TEXT = w_command_get_dictionary_uninstall_switch_text (self);
    dictionary = w_application_fuzzy_find_dictionary (application, DICTIONARY_UNINSTALL_SWITCH_TEXT);

    if (dictionary != NULL)
    {
      LwProgress *progress = lw_dictionary_get_progress (dictionary);
      w_command_watch_progress (self, progress);
      lw_dictionary_uninstall (dictionary);
      w_command_unwatch_current_progress (self);

      if (lw_progress_errored (progress))
      {
        resolution = 1;
      }
    }
    else
    {
      w_command_print (self, "\n\"%s\" Dictionary was not found!\n\n", DICTIONARY_UNINSTALL_SWITCH_TEXT);
      w_command_print_available_dictionaries (self);
    }

errored:

    return resolution;
}


void
w_command_unwatch_current_progress (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.progress == NULL) goto errored;

    if (priv->data.signalid[SIGNALID_PROGRESS_PRIMARY_MESSAGE_CHANGED] != 0)
    {
      g_signal_handler_disconnect (
        priv->data.progress,
        priv->data.signalid[SIGNALID_PROGRESS_PRIMARY_MESSAGE_CHANGED]
      );
      priv->data.signalid[SIGNALID_PROGRESS_PRIMARY_MESSAGE_CHANGED] = 0;
    }

    if (priv->data.signalid[SIGNALID_PROGRESS_SECONDARY_MESSAGE_CHANGED] != 0)
    {
      g_signal_handler_disconnect (
        priv->data.progress,
        priv->data.signalid[SIGNALID_PROGRESS_SECONDARY_MESSAGE_CHANGED]
      );
      priv->data.signalid[SIGNALID_PROGRESS_SECONDARY_MESSAGE_CHANGED] = 0;
    }

    if (priv->data.signalid[SIGNALID_PROGRESS_COMPLETED_CHANGED] != 0)
    {
      g_signal_handler_disconnect (
        priv->data.progress,
        priv->data.signalid[SIGNALID_PROGRESS_COMPLETED_CHANGED]
      );
      priv->data.signalid[SIGNALID_PROGRESS_COMPLETED_CHANGED] = 0;
    }

    if (priv->data.signalid[SIGNALID_PROGRESS_FRACTION_CHANGED] != 0)
    {
      g_signal_handler_disconnect (
        priv->data.progress,
        priv->data.signalid[SIGNALID_PROGRESS_FRACTION_CHANGED]
      );
      priv->data.signalid[SIGNALID_PROGRESS_FRACTION_CHANGED] = 0;
    }

errored:
 
    return;
}


void
w_command_watch_progress (WCommand   *self,
                          LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));
    g_return_if_fail (LW_IS_PROGRESS (progress));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (progress != NULL)
    {
      g_object_ref (progress);
    }

    w_command_unwatch_current_progress (self);

    if (priv->data.progress != NULL)
    {
      g_object_unref (priv->data.progress);
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.progress), 
        (gpointer*) &priv->data.progress
      );
    }

    priv->data.progress = progress;
    g_object_add_weak_pointer (
      G_OBJECT (priv->data.progress), 
      (gpointer*) &priv->data.progress
    );

    priv->data.signalid[SIGNALID_PROGRESS_PRIMARY_MESSAGE_CHANGED] = g_signal_connect_swapped (
      G_OBJECT (progress),
      "notify::primary-message",
      G_CALLBACK (w_command_progress_primary_message_changed_cb),
      self
    );

    priv->data.signalid[SIGNALID_PROGRESS_SECONDARY_MESSAGE_CHANGED] = g_signal_connect_swapped (
      G_OBJECT (progress),
      "notify::secondary-message",
      G_CALLBACK (w_command_progress_secondary_message_changed_cb),
      self
    );

    priv->data.signalid[SIGNALID_PROGRESS_COMPLETED_CHANGED] = g_signal_connect_swapped (
      G_OBJECT (progress),
      "notify::completed",
      G_CALLBACK (w_command_progress_completed_changed_cb),
      self
    );

    priv->data.signalid[SIGNALID_PROGRESS_FRACTION_CHANGED] = g_signal_connect_swapped (
      G_OBJECT (progress),
      "notify::progress-fraction",
      G_CALLBACK (w_command_progress_fraction_changed_cb),
      self
    );
}


void
_install (WCommand *self,
          GList    *dictionaryinstalls) 
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));
    if (dictionaryinstalls == NULL) return;

    {
      GList *link = NULL;

      for (link = dictionaryinstalls; link != NULL; link = link->next)
      {
        LwDictionaryInstall *dictionaryinstall = LW_DICTIONARYINSTALL (link->data);
        if (dictionaryinstall != NULL)
        {
          LwProgress *progress = lw_dictionaryinstall_get_progress (dictionaryinstall);
          w_command_watch_progress (self, progress);
          lw_dictionaryinstall_install (dictionaryinstall);
          if (lw_progress_should_abort (progress)) goto errored;
        }
      }
    }

errored:

    w_command_unwatch_current_progress (self);
}

GList*
_find_dictionaryinstalls (WCommand *self)
{
    g_return_val_if_fail (W_IS_COMMAND (self), -1);

    //Declarations
    WCommandPrivate *priv = NULL;
    WApplication *application = NULL;
    GList *dictionaryinstalls = NULL;
    const gchar *install_switch_text = NULL;

    //Initializations
    priv = self->priv;
    application = priv->data.application;
    if (application == NULL) goto errored;
    install_switch_text = w_command_get_dictionary_install_switch_text (self);
    if (install_switch_text == NULL) goto errored;

    {
      LwDictionaryInstall *dictionaryinstall = w_application_fuzzy_find_dictionaryinstall (application, install_switch_text);
      if (dictionaryinstall != NULL)
      {
        g_object_ref (dictionaryinstall);
        dictionaryinstalls = g_list_prepend (dictionaryinstalls, dictionaryinstall);
      }
      dictionaryinstalls = g_list_reverse (dictionaryinstalls);
    }

errored:

    return dictionaryinstalls;
}

gint 
w_command_install_dictionary (WCommand *self)
{
    //Sanity check
    g_return_val_if_fail (W_IS_COMMAND (self), -1);

    //Declarations
    WCommandPrivate *priv = NULL;
    WApplication *application = NULL;
    gint resolution = -1;
    LwProgress *progress = NULL;
    GList *dictionaryinstalls = NULL;

    //Initializations
    priv = self->priv;
    application = priv->data.application;
    if (application == NULL) goto errored;
    progress = lw_progress_new ();
    if (progress == NULL) goto errored;
    resolution = 0;
    dictionaryinstalls = _find_dictionaryinstalls (self);

    if (dictionaryinstalls != NULL)
    {
      _install (self, dictionaryinstalls);

      if (lw_progress_errored (progress)) 
      {
        w_command_printerr (self, "\n%s\n", gettext("Installation failed!"));
      }
      else
      {
        w_command_printerr (self, "%s\n", gettext("Installation complete."));
      }
    }
    else
    {
      const gchar *install_switch_text = w_command_get_dictionary_install_switch_text (self);
      w_command_print (self, "\n%s \"was not\" found!\n\n", install_switch_text);
      w_command_print_installable_dictionaries (self);
    }

    if (lw_progress_errored (progress))
    {
      resolution = 1;
    }

errored:

    g_list_free_full (dictionaryinstalls, (GDestroyNotify) g_object_unref); dictionaryinstalls = NULL;

    return resolution;
}


void
w_command_rebuild_index (WCommand *self)
{
/* TODO
    LwDictionaryList dictionarylist = w_application_get_dictionarylist (self);
    gint total = lw_dictionarylist_get_total (dictionarylist);
    gint i = 0;

    for (i = 0; i < total; i++)
    {
      lw_dictionary_index (lw_dictionarylist_get_dictionary_by_position (dictionarylist, i));
    }
*/
}


//!
//! @brief Prints to the terminal the about message for the program.
//!
void 
w_command_about (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    w_command_print (self, "waei version %s", VERSION);

    w_command_print (self, "\n\n");

    w_command_print (self, "Check for the latest updates at <http://gwaei.sourceforge.net/>\n");
    w_command_print (self, "Code Copyright (C) 2009-2013 Zachary Dovel\n\n");

    w_command_print (self, "License:\n");
    w_command_print (self, "Copyright (C) 2008 Free Software Foundation, Inc.\nLicense GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\n\n");

errored:

    return;
}


//!
//! @brief Prints out the yet uninstalled available dictionaries.
//!
void 
w_command_print_installable_dictionaries (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;
    WApplication *application = NULL;
    LwDictionaryInstallList *dictionaryinstalllist = NULL;
    GList *dictionaryinstalls = NULL;

    //Initializations
    priv = self->priv;
    application = priv->data.application;
    if (application == NULL) goto errored;
    dictionaryinstalllist = w_application_get_dictionaryinstalllist (application);
    if (dictionaryinstalllist == NULL) goto errored;
    dictionaryinstalls = lw_dictionaryinstalllist_dictionaryinstalls (dictionaryinstalllist);

    w_command_print (self, gettext("Installable dictionaries are:\n"));

    if (dictionaryinstalls == NULL)
    {
      w_command_print (self, "  %s\n", gettext("none"));
    }
    else {
      GList *link = NULL;
      for (link = dictionaryinstalls; link != NULL; link = link->next)
      {
        LwDictionaryInstall *dictionaryinstall = LW_DICTIONARYINSTALL (link->data);
        {
          const gchar *FILENAME = lw_dictionaryinstall_get_name (dictionaryinstall);
          const gchar *NAME = lw_dictionaryinstall_get_name (dictionaryinstall);
          gint j = 0;

          w_command_print (self, "  %s", FILENAME);
          for (j = strlen(FILENAME); j < 20; j++) w_command_print (self, " ");
          w_command_print (self, "(AKA: %s Dictionary)\n", NAME);
        }
      }
    }

errored:

    if (dictionaryinstalls != NULL) g_list_free (dictionaryinstalls); dictionaryinstalls = NULL;
}


//!
//! @brief Not yet written
//!
void 
w_command_print_available_dictionaries (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;
    WApplication *application = NULL;
    LwDictionaryList *dictionary_list = NULL;
    GList *dictionaries = NULL;

    //Initializations
    priv = self->priv;
    application = priv->data.application;
    if (application == NULL) goto errored;
    dictionary_list = w_application_get_dictionarylist (application);
    if (dictionary_list == NULL) goto errored;
    dictionaries = lw_dictionarylist_dictionaries (dictionary_list);

    w_command_print (self, gettext("Available dictionaries are:\n"));

    if (dictionaries == NULL)
    {
      w_command_print (self, "  %s\n", gettext("none"));
    }
    else {
      GList *link = NULL;
      for (link = dictionaries; link != NULL; link = link->next) {
        LwDictionary *dictionary = LW_DICTIONARY (link->data);
        if (dictionary != NULL)
        {
          const gchar *FILENAME = lw_dictionary_get_filename (dictionary);
          gint j = 0;
          w_command_print (self, "  %s", FILENAME);
          for (j = strlen(FILENAME); j < 20; j++) w_command_print (self, " ");
          w_command_print (self, "(AKA: %s Dictionary)\n", lw_dictionary_get_name (dictionary));
        }
      }
    }


errored:

    if (dictionaries != NULL) g_list_free (dictionaries); dictionaries = NULL;

    return;
}


//!
//! @brief Lists the available and installable dictionaries
//!
void 
w_command_list (WCommand *self)
{
    w_command_print_available_dictionaries (self);
    w_command_print_installable_dictionaries (self);
}


//!
//! @brief If the GError is set, it prints it and frees the memory
//! @param error A pointer to a gerror pointer
//!
void 
w_command_handle_error (WCommand  *self, 
                        GError    **error)
{
    if (error != NULL && *error != NULL)
    {
      fprintf(stderr, "Error: %s\n", (*error)->message);
      g_error_free (*error);
      *error = NULL;
    }
}


gint 
w_command_search (WCommand *self)
{
    //Sanity check
    g_return_val_if_fail (W_IS_COMMAND (self), FALSE);

    //Declarations
    LwSearch *search = NULL;
    LwPreferences* preferences = NULL;

    WApplication *application = NULL;
    const gchar* dictionary_switch_text = NULL;
    const gchar* query_switch_text = NULL;

    gchar *message_total = NULL;
    LwDictionary *dictionary = NULL;
    gint resolution = -1;
    GMainLoop *loop = NULL;
    LwSearchFlag flags = 0;

    //Initializations
    application = w_command_get_application (self);
    if (application == NULL) goto errored;
    preferences = w_application_get_preferences (application);
    if (preferences == NULL) goto errored;
    dictionary_switch_text = w_command_get_dictionary_switch_text (self);
    if (dictionary_switch_text == NULL) goto errored;
    query_switch_text = w_command_get_query_switch_text (self);
    if (query_switch_text == NULL) goto errored;
    flags = lw_search_build_flags_from_preferences (preferences);

    search = lw_search_new_by_preferences (query_switch_text, dictionary, preferences);
    if (search == NULL) goto errored;

    if (w_command_get_quiet_switch (self)) 
    {
      //flags &= ~LW_SEARCH_FLAG_INSENSITIVE;
    }
    resolution = 0;

    dictionary = w_application_fuzzy_find_dictionary (application, dictionary_switch_text);
    if (dictionary == NULL)
    {
      resolution = 1;
      fprintf (stderr, gettext("\"%s\" Dictionary was not found!\n"), dictionary_switch_text);
      return resolution;
    }
/*TODO

    search = lw_search_new (dictionary, w_application_get_morphologyengine (self), query_switch_text, flags);
    searchiterator = lw_searchiterator_new (search, "raw");

    if (search == NULL)
    {
      resolution = 1;
      return resolution;
    }

    //Print the search intro
    if (!quiet_switch)
    {
      // TRANSLATORS: 'Searching for "${query}" in ${dictionary long name}'
      printf(gettext("Searching for \"%s\" in %s Dictionary...\n"), query_switch_text, lw_dictionary_get_name (dictionary));
    }

    loop = g_main_loop_new (NULL, FALSE); 
    sdata = w_searchdata_new (loop, self);
    lw_search_set_data (search, sdata, LW_SEARCH_DATA_FREE_FUNC (w_searchdata_free));

    //Print the results
    lw_search_start (search, progress, FALSE);

    g_timeout_add_full (
        G_PRIORITY_LOW,
        100,
        (GSourceFunc) w_command_append_result_timeout,
        searchiterator,
        NULL
    );

    g_main_loop_run (loop);

    //Print final header
    if (quiet_switch == FALSE)
    {
      total_results = lw_searchiterator_count (searchiterator);
      message_total = ngettext("Found %d result", "Found %d results", total_results);
      printf(message_total, total_results);

      printf("\n");
    }
*/

errored:

    //Cleanup
    g_object_unref (search); search = NULL;
    g_main_loop_unref (loop); loop = NULL;

    return 0;
}

/*TODO
void
w_application_set_dictionary (WApplication *self,
                              LwDictionary *dictionary)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (priv->data.dictionary != dictionary);

    if (dictionary != NULL)
    {
      g_object_ref (dictionary);
    }

    if (priv->data.dictionary != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.dictionary),
        (gpointer*) &priv->data.dictionary
      );
      g_object_unref (priv->data.dictionary);
    }

    priv->data.dictionary = dictionary;

    if (priv->data.dictionary != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->data.dictionary),
        (gpointer*) &priv->data.dictionary
      );
    }

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DICTIONARY]);
}


LwDictionary*
w_application_get_dictionary (WApplication *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.dictionary;
}
*/


void
w_command_set_application (WCommand     *self,
                           WApplication *application)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;
    gboolean changed = NULL;

    //Initializations
    priv = self->priv;
    changed = (application != priv->data.application);

    if (application != NULL)
    {
      g_application_hold (G_APPLICATION (application));
    }
    
    if (priv->data.application != NULL)
    {
      g_application_release (G_APPLICATION (priv->data.application));
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.application),
        (gpointer*) &(priv->data.application)
      );
    }

    priv->data.application = application;

    if (priv->data.application != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->data.application),
        (gpointer*) &(priv->data.application)
      );
    }

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_APPLICATION]);
}


WApplication*
w_command_get_application (WCommand *self)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_COMMAND (self), NULL);

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.application;
}


void
w_command_set_command_line (WCommand                *self,
                            GApplicationCommandLine *command_line)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_COMMAND (self), NULL);

    //Declarations
    WCommandPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (priv->data.command_line != command_line);

    if (command_line != NULL)
    {
      g_object_ref (command_line);
    }

    if (priv->data.command_line != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.command_line),
        (gpointer*) &(priv->data.command_line)
      );
      g_object_unref (priv->data.command_line);
      priv->data.command_line = NULL;
    }

    priv->data.command_line = command_line;

    if (priv->data.command_line != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->data.command_line),
        (gpointer*) &(priv->data.command_line)
      );
      if (changed) w_command_parse_args (self);
    }

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_COMMAND_LINE]);
}


GApplicationCommandLine*
w_command_get_command_line (WCommand *self)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_COMMAND (self), NULL);

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.command_line;
}


void
w_command_set_quiet_switch (WCommand *self,
                            gboolean quiet_switch)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (priv->argument.quiet_switch == quiet_switch) goto errored;

    priv->argument.quiet_switch = quiet_switch;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_QUIET_SWITCH]);

errored:

    return;
}


gboolean
w_command_get_quiet_switch (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->argument.quiet_switch;
}


void
w_command_set_exact_switch (WCommand *self,
                            gboolean exact_switch)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (priv->argument.exact_switch == exact_switch) goto errored;

    priv->argument.exact_switch = exact_switch;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_EXACT_SWITCH]);

errored:

    return;
}


gboolean
w_command_get_exact_switch (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->argument.exact_switch;
}


void
w_command_set_list_switch (WCommand *self,
                            gboolean list_switch)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (priv->argument.list_switch == list_switch) goto errored;

    priv->argument.list_switch = list_switch;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_LIST_SWITCH]);

errored:

    return;
}


gboolean
w_command_get_list_switch (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->argument.list_switch;
}


void
w_command_set_version_switch (WCommand *self,
                            gboolean version_switch)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (priv->argument.version_switch == version_switch) goto errored;

    priv->argument.version_switch = version_switch;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_VERSION_SWITCH]);

errored:

    return;
}


gboolean
w_command_get_version_switch (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->argument.version_switch;
}


void
w_command_set_color_switch (WCommand *self,
                            gboolean color_switch)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (priv->argument.color_switch == color_switch) goto errored;

    priv->argument.color_switch = color_switch;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_COLOR_SWITCH]);

errored:

    return;
}


gboolean
w_command_get_color_switch (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->argument.color_switch;
}



void
w_command_set_force_index_rebuild_switch (WCommand *self,
                                          gboolean force_index_rebuild_switch)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (priv->argument.force_index_rebuild_switch == force_index_rebuild_switch) goto errored;

    priv->argument.force_index_rebuild_switch = force_index_rebuild_switch;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_FORCE_INDEX_REBUILD_SWITCH]);

errored:

    return;
}


gboolean
w_command_get_force_index_rebuild_switch (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->argument.force_index_rebuild_switch;
}


void
w_command_set_start_server_switch (WCommand *self,
                                   gboolean start_server_switch)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (priv->argument.start_server_switch == start_server_switch) goto errored;

    priv->argument.start_server_switch = start_server_switch;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_START_SERVER_SWITCH]);

errored:

    return;
}


gboolean
w_command_get_start_server_switch (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->argument.start_server_switch;
}


void
w_command_set_dictionary_switch_text (WCommand    *self,
                                      const gchar *dictionary_switch_text)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (g_strcmp0 (dictionary_switch_text, priv->argument.dictionary_switch_text) == 0) goto errored;

    g_free (priv->argument.dictionary_switch_text);
    priv->argument.dictionary_switch_text = g_strdup (dictionary_switch_text);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DICTIONARY_SWITCH_TEXT]);

errored:

    return;
}


const gchar*
w_command_get_dictionary_switch_text (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->argument.dictionary_switch_text;
}


void
w_command_set_dictionary_install_switch_text (WCommand    *self,
                                              const gchar *dictionary_install_switch_text)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (g_strcmp0 (dictionary_install_switch_text, priv->argument.dictionary_install_switch_text) == 0) goto errored;

    g_free (priv->argument.dictionary_install_switch_text);
    priv->argument.dictionary_install_switch_text = g_strdup (dictionary_install_switch_text);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DICTIONARY_INSTALL_SWITCH_TEXT]);

errored:

    return;
}


const gchar*
w_command_get_dictionary_install_switch_text (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->argument.dictionary_install_switch_text;
}


gboolean
w_command_has_dictionary_install_request (WCommand *self)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_COMMAND (self), FALSE);

    //Declarations
    const gchar *DICTIONARY_INSTALL_SWITCH_TEXT = NULL;
    gboolean has_request = FALSE;

    //Initializations
    DICTIONARY_INSTALL_SWITCH_TEXT = w_command_get_dictionary_install_switch_text (self);
    has_request = (DICTIONARY_INSTALL_SWITCH_TEXT != NULL && *DICTIONARY_INSTALL_SWITCH_TEXT != '\0');

    return has_request;
}


void
w_command_set_dictionary_uninstall_switch_text (WCommand    *self,
                                                const gchar *dictionary_uninstall_switch_text)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (g_strcmp0 (dictionary_uninstall_switch_text, priv->argument.dictionary_uninstall_switch_text) == 0) goto errored;

    g_free (priv->argument.dictionary_uninstall_switch_text);
    priv->argument.dictionary_uninstall_switch_text = g_strdup (dictionary_uninstall_switch_text);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DICTIONARY_UNINSTALL_SWITCH_TEXT]);

errored:

    return;
}


const gchar*
w_command_get_dictionary_uninstall_switch_text (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->argument.dictionary_uninstall_switch_text;
}


gboolean
w_command_has_dictionary_uninstall_request (WCommand *self)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_COMMAND (self), FALSE);

    //Declarations
    const gchar *DICTIONARY_UNINSTALL_SWITCH_TEXT = NULL;
    gboolean has_request = FALSE;

    //Initializations
    DICTIONARY_UNINSTALL_SWITCH_TEXT = w_command_get_dictionary_uninstall_switch_text (self);
    has_request = (DICTIONARY_UNINSTALL_SWITCH_TEXT != NULL && *DICTIONARY_UNINSTALL_SWITCH_TEXT != '\0');

    return has_request;
}


void
w_command_set_query_switch_text (WCommand    *self,
                                 const gchar *query_switch_text)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (g_strcmp0 (query_switch_text, priv->argument.query_switch_text) == 0) goto errored;

    g_free (priv->argument.query_switch_text);
    priv->argument.query_switch_text = g_strdup (query_switch_text);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_QUERY_SWITCH_TEXT]);

errored:

    return;
}


const gchar*
w_command_get_query_switch_text (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->argument.query_switch_text;
}


gboolean
w_command_has_query_request (WCommand *self)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    const gchar *QUERY_SWITCH_TEXT = NULL;
    gboolean has_request = FALSE;

    //Initializations
    QUERY_SWITCH_TEXT = w_command_get_query_switch_text (self);
    has_request = (QUERY_SWITCH_TEXT != NULL && *QUERY_SWITCH_TEXT != '\0');

    return has_request;
}


void
w_command_print (WCommand *self,
                  const gchar *format,
                  ...)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));
    g_return_if_fail (format != NULL);

    //Declarations
    WCommandPrivate *priv = NULL;
    GApplicationCommandLine *command_line = NULL;
    gchar *message = NULL;
    va_list ap;

    priv = self->priv;
    command_line = priv->data.command_line;
    if (command_line == NULL) goto errored;
    va_start (ap, format);
    message = g_strdup_vprintf (format, ap);
    va_end (ap);

    G_APPLICATION_COMMAND_LINE_GET_CLASS (command_line)->print_literal (command_line, message);

errored:

    g_free (message);
}


void
w_command_printerr (WCommand *self,
                    const gchar *format,
                    ...)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));
    g_return_if_fail (format != NULL);

    //Declarations
    WCommandPrivate *priv = NULL;
    GApplicationCommandLine *command_line = NULL;
    gchar *message = NULL;
    va_list ap;

    priv = self->priv;
    command_line = priv->data.command_line;
    if (command_line == NULL) goto errored;
    va_start (ap, format);
    message = g_strdup_vprintf (format, ap);
    va_end (ap);

    G_APPLICATION_COMMAND_LINE_GET_CLASS (command_line)->printerr_literal (command_line, message);

errored:

    g_free (message);
}


