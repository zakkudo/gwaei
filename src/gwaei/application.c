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
//! @file application.c
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

#include <gio/gio.h>

#include <gtk/gtk.h>

#include <gwaei/gettext.h>
#include <gwaei/gwaei.h>
#include <gwaei/application-private.h>

static void gw_application_activate (GApplication*);
static gboolean gw_application_local_command_line (GApplication*, gchar***, gint*);
static int gw_application_command_line (GApplication*, GApplicationCommandLine*);
static void gw_application_startup (GApplication*);
static void gw_application_load_app_menu (GwApplication*);


G_DEFINE_TYPE (GwApplication, gw_application, GTK_TYPE_APPLICATION)

//!
//! @brief creates a new instance of the gwaei applicaiton
//!
GApplication* 
gw_application_new ()
{
    //Declarations
    GwApplication *application;
    const gchar *id;
    GApplicationFlags flags;

    //Initializations
    id = "gtk.org.gWaei";
    flags = G_APPLICATION_FLAGS_NONE;
    application = g_object_new (GW_TYPE_APPLICATION, 
                                "application-id", id, 
                                "flags", flags,
                                "register-session", TRUE,
                                NULL);

    return G_APPLICATION (application);
}


static void 
gw_application_init (GwApplication *application)
{
    application->priv = GW_APPLICATION_GET_PRIVATE (application);
    memset(application->priv, 0, sizeof(GwApplicationPrivate));
}

static void 
gw_application_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (gw_application_parent_class)->constructed (object);
    }

    lw_regex_initialize ();
    //gw_application_initialize_accelerators (GW_APPLICATION (object));

/*
#ifdef OS_MINGW
    GtkSettings *settings;
    settings = gtk_settings_get_default ();
    if (settings != NULL)
    {
      g_object_set (settings, "gtk-theme-name", "Raleigh", NULL);
      g_object_set (settings, "gtk-menu-images", FALSE, NULL);
      g_object_set (settings, "gtk-button-images", FALSE, NULL);
      g_object_set (settings, "gtk-cursor-blink", FALSE, NULL);
      g_object_set (settings, "gtk-alternative-button-order", TRUE, NULL);
      g_object_unref (settings);
      settings = NULL;
    }
#endif
*/
}

static void
gw_application_dispose (GObject *object)
{
    //Declarations
    GwApplication *application;

    application = GW_APPLICATION (object);

    gw_application_disconnect_signals (application);

    G_OBJECT_CLASS (gw_application_parent_class)->dispose (object);
}


static void 
gw_application_finalize (GObject *object)
{
    //Declarations
    GwApplication *application;
    GwApplicationPrivate *priv;

    application = GW_APPLICATION (object);
    priv = application->priv;

    gw_application_disconnect_signals (application);

    if (priv->data.dictionary_list_store.installable != NULL) g_object_unref (priv->data.dictionary_list_store.installable); 
    if (priv->data.dictionary_list_store.installed != NULL) g_object_unref (priv->data.dictionary_list_store.installed); 
    if (priv->data.vocabularyliststore != NULL) g_object_unref (priv->data.vocabularyliststore); 

    if (priv->config.context != NULL) g_option_context_free (priv->config.context); 
    if (priv->config.arguments.query != NULL) g_free(priv->config.arguments.query); 
    if (priv->data.morphologyengine != NULL) g_object_unref (priv->data.morphologyengine); 

    gw_application_set_preferences (application, NULL);

    lw_regex_free ();

    memset(priv, 0, sizeof(GwApplicationPrivate));

    G_OBJECT_CLASS (gw_application_parent_class)->finalize (object);
}


static void
gw_application_class_init (GwApplicationClass *klass)
{
  GObjectClass *object_class;
  GApplicationClass *application_class;

  object_class = G_OBJECT_CLASS (klass);
  application_class = G_APPLICATION_CLASS (klass);

  object_class->constructed = gw_application_constructed;
  object_class->finalize = gw_application_finalize;
  object_class->dispose = gw_application_dispose;
#ifndef OS_MINGW
  application_class->local_command_line = gw_application_local_command_line;
#endif
  application_class->command_line = gw_application_command_line;
  application_class->activate = gw_application_activate;
  application_class->startup = gw_application_startup;

  g_type_class_add_private (object_class, sizeof (GwApplicationPrivate));
}


//!
//! @brief Loads the arguments from the command line into the app instance
//!

void 
gw_application_parse_args (GwApplication *application, int *argc, char** argv[])
{
    GwApplicationPrivate *priv;

    priv = application->priv;

    //Reset the switches to their default state
    g_free (priv->config.arguments.dictionary);
    priv->config.arguments.dictionary = NULL;
    g_free (priv->config.arguments.query);
    priv->config.arguments.query = NULL;
    priv->config.arguments.version_switch = FALSE;
    priv->config.arguments.new_vocabulary_window_switch = FALSE;

    GOptionEntry entries[] =
    {
      { "dictionary", 'd', 0, G_OPTION_ARG_STRING, &(priv->config.arguments.dictionary), gettext("Choose the dictionary to use"), "English" },
      { "word", 'o', 0, G_OPTION_ARG_NONE, &(priv->config.arguments.new_vocabulary_window_switch), gettext("Open the vocabulary manager window"), NULL },
      { "version", 'v', 0, G_OPTION_ARG_NONE, &(priv->config.arguments.version_switch), gettext("Check the gWaei version information"), NULL },
      { NULL }
    };

    //Program flags setup
    GError *error = NULL;
    if (priv->config.context != NULL) g_option_context_free (priv->config.context);
    priv->config.context = g_option_context_new (gettext("- A dictionary program for Japanese-English translation."));
    g_option_context_add_main_entries (priv->config.context, entries, PACKAGE);
    g_option_context_set_ignore_unknown_options (priv->config.context, TRUE);
    g_option_context_parse (priv->config.context, argc, argv, &error);

    //g_log_set_always_fatal (G_LOG_LEVEL_WARNING);

    if (error != NULL)
    {
      //gw_application_handle_error (application, NULL, FALSE, &error);
      exit(EXIT_SUCCESS);
    }

    //Get the query after the flags have been parsed out
    priv->config.arguments.query = lw_util_get_query_from_args (*argc, *argv);
}


//!
//! @brief Prints to the terminal the about message for the program.
//!
void 
gw_application_print_about (GwApplication *application)
{
    const gchar *NAME = NULL; 

    NAME = gw_application_get_program_name (GW_APPLICATION (application));

    printf (gettext ("%s version %s"), NAME, VERSION);

    printf ("\n\n");

    printf ("Check for the latest updates at <http://gwaei.sourceforge.net/>\n");
    printf ("Code Copyright (C) 2009-2012 Zachary Dovel\n\n");

    printf ("License:\n");
    printf ("Copyright (C) 2008 Free Software Foundation, Inc.\nLicense GPLv3+: "
            "GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\nThis"
            " is free software: you are free to change and redistribute it.\nThe"
            "re is NO WARRANTY, to the extent permitted by law.\n\n"             );
}


void 
gw_application_quit (GwApplication *application)
{
    //Sanity checks
    g_return_if_fail (GW_IS_APPLICATION (application));

    //Declarations
    GList *link = NULL;
    gboolean should_close = FALSE;

    //initializations
    should_close = TRUE;

    if (should_close)
    {
      link = gtk_application_get_windows (GTK_APPLICATION (application));
      while (link != NULL)
      {
        gtk_widget_destroy (GTK_WIDGET (link->data));
        link = gtk_application_get_windows (GTK_APPLICATION (application));
      }
    }
}


//!
//! @brief Returns the program name.  It should not be freed or modified
//! @returns A constanst string representing the program name
//!
const gchar* 
gw_application_get_program_name (GwApplication *application) 
{
  return gettext("gWaei Japanese-English Dictionary");
}


//!
//!  @brief Will attempt to get the window of the specified type which is most at the front
//!
GtkWindow* 
gw_application_get_window_by_type (GwApplication *application, const GType TYPE)
{
    //Declarations
    GList *iter;
    GList *list;
    GtkWindow *window;
    GtkWindow *active;
    GtkWindow *fuzzy;

    //Initializations
    list = gtk_application_get_windows (GTK_APPLICATION (application));
    window = NULL;
    fuzzy = NULL;
    active = NULL;

    for (iter = list; iter != NULL; iter = iter->next)
    {
      fuzzy = GTK_WINDOW (iter->data);
      active = GTK_WINDOW (iter->data);

      if (fuzzy == NULL)
      {
        continue;
      }
      if (G_OBJECT_TYPE (active) == TYPE && gtk_window_is_active (active))
      {
        window = active;
        break;
      }
      if (G_OBJECT_TYPE (fuzzy) == TYPE)
      {
        window = fuzzy;
      }
    }

    return window;
}


LwMorphologyEngine*
gw_application_get_morphologyengine (GwApplication *application)
{
  //Sanity checks
  g_return_val_if_fail (GW_IS_APPLICATION (application), NULL);

  //Declarations
  GwApplicationPrivate *priv = NULL;
  gpointer* pointer = NULL;

  //Initializations
  priv = application->priv;

  if (priv->data.morphologyengine == NULL)
  {
    priv->data.morphologyengine = lw_morphologyengine_new ("en_US");
    pointer = (gpointer*) &(priv->data.morphologyengine);
    g_object_add_weak_pointer (G_OBJECT (priv->data.morphologyengine), pointer);
  }

  return priv->data.morphologyengine;
}


static void 
gw_application_activate (GApplication *application)
{
    //Sanity checks
    g_return_if_fail (GW_IS_APPLICATION (application));

    //Declarations
    GwApplicationPrivate *priv = NULL;
    GwMainWindow *mainwindow = NULL;

    //Initializations
    priv = GW_APPLICATION (application)->priv;

    mainwindow = GW_MAINWINDOW (gw_mainwindow_new (GTK_APPLICATION (application)));

    gtk_widget_show (GTK_WIDGET (mainwindow));
}


static int 
gw_application_command_line (GApplication *application, GApplicationCommandLine *command_line)
{
    //Declarations
    LwDictionary *dictionary;
    GwMainWindow *window;
    GwApplicationPrivate *priv;
    gint argc;
    gchar **argv;
    gint position;

    //Initializations
    priv = GW_APPLICATION (application)->priv;
    argv = NULL;

    if (command_line != NULL)
    {
      argv = g_application_command_line_get_arguments (command_line, &argc);

      gw_application_parse_args (GW_APPLICATION (application), &argc, &argv);
    }
    g_application_activate (G_APPLICATION (application));

    //Cleanup
    if (argv != NULL) g_strfreev (argv); argv = NULL;

    return 0;
}


static gboolean 
gw_application_local_command_line (GApplication *application, 
                                   gchar ***argv, gint *exit_status)
{
    //Declarations
    int argc;
    gboolean handled;
    int i;

    //Initializations
    argc = g_strv_length (*argv);
    handled = FALSE;

    for (i = 0; (*argv)[i] != NULL; i++)
    {
      if (strcmp((*argv)[i], "-v") == 0 || strcmp((*argv)[i], "--version") == 0)
      {
        gw_application_print_about (GW_APPLICATION (application));
        handled = TRUE;
        break;
      }
      else if (strcmp((*argv)[i], "-h") == 0 || strcmp((*argv)[i], "--help") == 0)
      {
        handled = TRUE;
        gw_application_parse_args (GW_APPLICATION (application), &argc, argv);
        break;
      }
    }

    return handled;
} 


static void
gw_application_startup (GApplication *application)
{
    G_APPLICATION_CLASS (gw_application_parent_class)->startup (application);

    gw_application_load_app_menu (GW_APPLICATION (application));

    gw_application_connect_signals (GW_APPLICATION (application));
}


gboolean
gw_application_should_quit (GwApplication *application)
{
    return TRUE;
/*
    GList *windowlist;
    GList *link;
    gboolean should_quit;

    windowlist = gtk_application_get_windows (GTK_APPLICATION (application));
    should_quit = TRUE;

    for (link = windowlist; should_quit && link != NULL; link = link->next)
      if (gw_window_is_important (GW_WINDOW (link->data))) should_quit = FALSE;

    return should_quit;
*/
}


static void
gw_application_initialize_menumodel_links (GwApplication *application)
{
/*
    //Sanity checks
    g_return_if_fail (GW_IS_APPLICATION (application));
  
    //Declarations
    GwVocabularyListStore *store;
    GMenuModel *menumodel;
    GMenuModel *link;

    //Initializations
    menumodel = gtk_application_get_app_menu (GTK_APPLICATION (application));
    g_return_if_fail (menumodel != NULL);

    store = GW_VOCABULARYLISTSTORE (gw_application_get_vocabularyliststore (application));
    link = gw_vocabularyliststore_get_menumodel (store);
    gw_menumodel_set_links (menumodel, "vocabulary-list-link", gettext ("Vocabulary"), G_MENU_LINK_SECTION, link);
*/
}


static void
gw_application_load_app_menu (GwApplication *application)
{
    //Sanity checks
    g_return_if_fail (GW_IS_APPLICATION (application));

    //Declarations
    GMenuModel *menu_model = NULL;

    //Initializations

    gw_application_map_actions (G_ACTION_MAP (application), application);

    menu_model = lgw_load_menu_model ("application-menumodel.ui");
    gtk_application_set_app_menu (GTK_APPLICATION (application), menu_model);

    gw_application_initialize_menumodel_links (application);
}


void
gw_application_map_actions (GActionMap *map, GwApplication *application)
{
    //Sanity checks
    g_return_if_fail (map != NULL);
    g_return_if_fail (GW_IS_APPLICATION (application));

    static GActionEntry entries[] = {
      //{ "new-window", gw_application_open_searchwindow_cb, NULL, NULL, NULL },
      { "show-about", gw_application_open_aboutdialog_cb, NULL, NULL, NULL },
      //{ "show-preferences", gw_application_open_settingswindow_cb, NULL, NULL, NULL },
      //{ "show-vocabulary", gw_application_open_vocabularywindow_cb, NULL, NULL, NULL },
      //{ "show-vocabulary-index", gw_application_open_vocabularywindow_index_cb, "s", NULL, NULL },
      { "show-help", gw_application_open_help_cb, NULL, NULL, NULL },
      { "show-glossary", gw_application_open_glossary_cb, NULL, NULL, NULL },
#ifdef HAVE_HUNSPELL
      { "toggle-spellcheck", gw_application_spellcheck_toggled_cb, NULL, "false", NULL },
#endif
      { "quit", gw_application_quit_cb, NULL, NULL, NULL }
    };

    g_action_map_add_action_entries (map, entries, G_N_ELEMENTS (entries), application);
}


void
gw_menumodel_set_links (GMenuModel *menumodel, const gchar *LABEL, const gchar *NEW_LABEL, const gchar *LINK_TYPE, GMenuModel *link)
{
    //Sanity checks
    g_return_if_fail (menumodel != NULL);
    g_return_if_fail (LABEL != NULL);
    g_return_if_fail (LINK_TYPE != NULL);

    //Declarations
    gint total_items = 0;
    gint index = 0;
    gchar *label = NULL;
    gboolean valid = FALSE;
    GMenuItem *menuitem = NULL;
    GMenuModel *sublink = NULL;

    //Initializations
    total_items = g_menu_model_get_n_items (menumodel);

    for (index = 0; index < total_items; index++)
    {
      valid = g_menu_model_get_item_attribute (menumodel, index, G_MENU_ATTRIBUTE_LABEL, "s", &label, NULL);
      if (valid == TRUE && label != NULL)
      {
        if (label != NULL && strcmp (label, LABEL) == 0)
        {
          menuitem = g_menu_item_new (NEW_LABEL, NULL);
          g_menu_item_set_link (menuitem, LINK_TYPE, link);
          g_menu_remove (G_MENU (menumodel), index);
          g_menu_insert_item (G_MENU (menumodel), index, menuitem);
          g_object_unref (menuitem); menuitem = NULL;
        }
        g_free (label); label = NULL;
      }

      //Recursive work
      sublink = g_menu_model_get_item_link (menumodel, index, G_MENU_LINK_SUBMENU);
      if (sublink != NULL) gw_menumodel_set_links (sublink, LABEL, NEW_LABEL, LINK_TYPE, link);
      sublink = g_menu_model_get_item_link (menumodel, index, G_MENU_LINK_SECTION);
      if (sublink != NULL) gw_menumodel_set_links (sublink, LABEL, NEW_LABEL, LINK_TYPE, link);
    }
}

static void
gw_application_initialize_accelerators (GwApplication *application)
{
    //Sanity checks
    g_return_if_fail (GW_IS_APPLICATION (application));

    //Declarations
    gchar *accel = NULL;
    gchar *action = NULL;
    gchar *detail = NULL;
    gint index = 0;

    //Initializations
    index = 1;

    while (index + 1 < 10)
    {
      accel = g_strdup_printf ("<Alt>%d", index);
      action = g_strdup_printf ("win.set-dictionary");
      detail = g_strdup_printf ("%d", index);
      if (accel != NULL && action != NULL && detail != NULL)
      {
        gtk_application_add_accelerator (GTK_APPLICATION (application), accel, action, g_variant_new_string (detail));
        index++;
      }
      if (accel != NULL) g_free (accel); accel = NULL;
      if (action != NULL) g_free (action); action = NULL;
      if (detail != NULL) g_free (detail); detail = NULL;
    }
}


LgwDictionaryListStore* 
gw_application_get_installed_dictionaryliststore (GwApplication *application)
{
    //Sanity checks
    g_return_val_if_fail (GW_IS_APPLICATION (application), NULL);

    //Declarations
    GwApplicationPrivate *priv = NULL;

    //Initializations;
    priv = application->priv;

    if (priv->data.dictionary_list_store.installed == NULL)
    {
      LwPreferences *preferences = gw_application_get_preferences (application);
      LgwDictionaryListStore *dictionary_list_store = lgw_dictionaryliststore_new (preferences);
      LwMorphologyEngine *morphologyengine = gw_application_get_morphologyengine (application);

      lw_dictionarylist_load_installed (LW_DICTIONARYLIST (dictionary_list_store), morphologyengine);
      lw_dictionarylist_load_order (LW_DICTIONARYLIST (dictionary_list_store));
      
      priv->data.dictionary_list_store.installed = dictionary_list_store;
      g_object_add_weak_pointer (G_OBJECT (priv->data.dictionary_list_store.installed), (gpointer*) &(priv->data.dictionary_list_store.installed));
    }

    return priv->data.dictionary_list_store.installed;
}


LgwDictionaryListStore* 
gw_application_get_installable_dictionaryliststore (GwApplication *application)
{
    //Sanity checks
    g_return_val_if_fail (GW_IS_APPLICATION (application), NULL);

    //Declarations
    GwApplicationPrivate *priv = NULL;

    //Initializations
    priv = application->priv;

    if (priv->data.dictionary_list_store.installable == NULL)
    {
      LwPreferences *preferences = gw_application_get_preferences (application);
      LgwDictionaryListStore *dictionary_list_store = lgw_dictionaryliststore_new (preferences);

      lw_dictionarylist_load_installable (LW_DICTIONARYLIST (dictionary_list_store));

      priv->data.dictionary_list_store.installable = dictionary_list_store;
      g_object_add_weak_pointer (G_OBJECT (priv->data.dictionary_list_store.installable), (gpointer*) &(priv->data.dictionary_list_store.installable));
    }

    return priv->data.dictionary_list_store.installable;
}


LwPreferences*
gw_application_get_preferences (GwApplication *application)
{
    //Sanity checks
    g_return_val_if_fail (GW_IS_APPLICATION (application), NULL);

    //Declarations
    GwApplicationPrivate *priv = NULL;

    //Initializations
    priv = application->priv;

    if (priv->config.preferences == NULL)
    {
      gw_application_set_preferences (application, lw_preferences_get_default ());
    }

    return priv->config.preferences;
}


void
gw_application_set_preferences (GwApplication *application,
                                LwPreferences *preferences)
{
    //Sanity checks
    g_return_if_fail (GW_IS_APPLICATION (application));

    //Declarations
    GwApplicationPrivate *priv = NULL;

    //Initializations
    priv = application->priv;

    if (priv->config.preferences != NULL)
    {
      g_object_remove_weak_pointer (G_OBJECT (priv->config.preferences), (gpointer*) &(priv->config.preferences));
      g_object_unref (priv->config.preferences);
      priv->config.preferences = NULL;
    }

    priv->config.preferences = preferences;

    if (priv->config.preferences != NULL) {
      g_object_ref (priv->config.preferences);
      g_object_add_weak_pointer (G_OBJECT (priv->config.preferences), (gpointer*) &(priv->config.preferences));
    }
}
