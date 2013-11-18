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
//! @file spellcheck.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include <gtk/gtk.h>

#include <gwaei/gwaei.h>
#include <gwaei/gettext.h>
#include <gwaei/spellcheck-private.h>

static void gw_spellcheck_attach_signals (GwSpellcheck*);
static void gw_spellcheck_remove_signals (GwSpellcheck*);

G_DEFINE_TYPE (GwSpellcheck, gw_spellcheck, G_TYPE_OBJECT)


typedef enum
{
  PROP_0,
  PROP_APPLICATION
} GwSpellcheckProps;


GwSpellcheck*
gw_spellcheck_new (GwApplication *application)
{
    GwSpellcheck *spellcheck;

    spellcheck = GW_SPELLCHECK (g_object_new (GW_TYPE_SPELLCHECK, "application", application, NULL));

    return spellcheck;
}


GwSpellcheck* 
gw_spellcheck_new_with_entry (GwApplication *application, GtkEntry *entry)
{
    GwSpellcheck *spellcheck;
    
    spellcheck = gw_spellcheck_new (application);

    if (spellcheck != NULL)
      gw_spellcheck_set_entry (spellcheck, entry);

    return spellcheck;
}



static void
gw_spellcheck_init (GwSpellcheck *spellcheck)
{
    spellcheck->priv = GW_SPELLCHECK_GET_PRIVATE (spellcheck);
    memset(spellcheck->priv, 0, sizeof(GwSpellcheckPrivate));

    gw_spellcheck_set_timeout_threshold (spellcheck, 2);

    gw_spellcheck_attach_signals (spellcheck);
}


static void
gw_spellcheck_finalize (GObject *object)
{
    GwSpellcheck *spellcheck;
    GwSpellcheckPrivate *priv;

    spellcheck = GW_SPELLCHECK (object);
    priv = spellcheck->priv;

    gw_spellcheck_remove_signals (spellcheck);
    gw_spellcheck_clear (spellcheck);

    if (priv->entry != NULL) gtk_widget_queue_draw (GTK_WIDGET (priv->entry));

    G_OBJECT_CLASS (gw_spellcheck_parent_class)->finalize (object);
}


static void 
gw_spellcheck_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    GwSpellcheck *spellcheck;
    GwSpellcheckPrivate *priv;

    spellcheck = GW_SPELLCHECK (object);
    priv = spellcheck->priv;

    switch (property_id)
    {
      case PROP_APPLICATION:
        priv->application = GW_APPLICATION (g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
gw_spellcheck_get_property (GObject      *object,
                            guint         property_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    GwSpellcheck *spellcheck;
    GwSpellcheckPrivate *priv;

    spellcheck = GW_SPELLCHECK (object);
    priv = spellcheck->priv;

    switch (property_id)
    {
      case PROP_APPLICATION:
        g_value_set_object (value, priv->application);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
gw_spellcheck_class_init (GwSpellcheckClass *klass)
{
    //Declarations
    GParamSpec *pspec;
    GObjectClass *object_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = gw_spellcheck_set_property;
    object_class->get_property = gw_spellcheck_get_property;
    object_class->finalize = gw_spellcheck_finalize;

    g_type_class_add_private (object_class, sizeof (GwSpellcheckPrivate));

    pspec = g_param_spec_object ("application",
                                 "Application construct prop",
                                 "Set GwSpellcheck's Application",
                                 GW_TYPE_APPLICATION,
                                 G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_APPLICATION, pspec);
}


static void  
gw_spellcheck_attach_signals (GwSpellcheck *spellcheck)
{
}


static void 
gw_spellcheck_remove_signals (GwSpellcheck *spellcheck)
{
    //Declarations
    GwSpellcheckPrivate *priv;
    LwPreferences *preferences;
    GtkEntry *entry;
    GSource *source;
    int i;

    priv = spellcheck->priv;
    preferences = gw_application_get_preferences (priv->application);
    entry = priv->entry;

    if (priv->signalid[GW_SPELLCHECK_SIGNALID_DICTIONARY] != 0)
    {
      lw_preferences_remove_change_listener_by_schema (
          preferences, 
          LW_SCHEMA_BASE, 
          priv->signalid[GW_SPELLCHECK_SIGNALID_DICTIONARY]
      );
      priv->signalid[GW_SPELLCHECK_SIGNALID_DICTIONARY] = 0;
    }

    if (priv->signalid[GW_SPELLCHECK_SIGNALID_RK_CONV] != 0)
    {
      lw_preferences_remove_change_listener_by_schema (
          preferences, 
          LW_SCHEMA_BASE, 
          priv->signalid[GW_SPELLCHECK_SIGNALID_RK_CONV]
      );
      priv->signalid[GW_SPELLCHECK_SIGNALID_RK_CONV] = 0;
    }

    for (i = 0; i < TOTAL_GW_SPELLCHECK_SIGNALIDS && entry != NULL; i++)
    {
      if (priv->signalid[i] > 0)
      {
        g_signal_handler_disconnect (G_OBJECT (entry), priv->signalid[i]);
        priv->signalid[i] = 0;
      }
    }

    for (i = 0; i < TOTAL_GW_SPELLCHECK_TIMEOUTIDS; i++)
    {
      if (g_main_current_source () != NULL &&
          !g_source_is_destroyed (g_main_current_source ()) &&
          priv->timeoutid[i] > 0
         )
      {
        source = g_main_context_find_source_by_id (NULL, priv->timeoutid[i]);
        if (source != NULL)
        {
          g_source_destroy (source);
        }
      }
      priv->timeoutid[i] = 0;
    }
}


void
gw_spellcheck_set_timeout_threshold (GwSpellcheck *spellcheck, guint threshold)
{
   GwSpellcheckPrivate *priv;

   priv = spellcheck->priv;

   priv->threshold = threshold;
}


void
gw_spellcheck_set_entry (GwSpellcheck *spellcheck, GtkEntry *entry)
{

    GwSpellcheckPrivate *priv;
    LwPreferences *preferences;

    priv = spellcheck->priv;
    preferences = gw_application_get_preferences (priv->application);

    //Remove the old signals
    if (priv->entry != NULL)
    {

      if (priv->signalid[GW_SPELLCHECK_SIGNALID_DRAW] != 0)
        g_signal_handler_disconnect (G_OBJECT (priv->entry), priv->signalid[GW_SPELLCHECK_SIGNALID_DRAW]);

      if (priv->signalid[GW_SPELLCHECK_SIGNALID_CHANGED] != 0)
        g_signal_handler_disconnect (G_OBJECT (priv->entry), priv->signalid[GW_SPELLCHECK_SIGNALID_CHANGED]);

      if (priv->signalid[GW_SPELLCHECK_SIGNALID_POPULATE_POPUP] != 0)
        g_signal_handler_disconnect (G_OBJECT (priv->entry), priv->signalid[GW_SPELLCHECK_SIGNALID_POPULATE_POPUP]);

      if (priv->signalid[GW_SPELLCHECK_SIGNALID_DESTROY] != 0)
        g_signal_handler_disconnect (G_OBJECT (priv->entry), priv->signalid[GW_SPELLCHECK_SIGNALID_DESTROY]);

      g_object_remove_weak_pointer (G_OBJECT (priv->entry), (gpointer*) (&(priv->entry)));
    }

    //Set the entry pointer
    priv->entry = entry;

    if (entry == NULL) return;

    g_object_add_weak_pointer (G_OBJECT (priv->entry), (gpointer*) (&(priv->entry)));

    //set the new signals
    priv->signalid[GW_SPELLCHECK_SIGNALID_DICTIONARY] = lw_preferences_add_change_listener_by_schema (
        preferences,
        LW_SCHEMA_BASE,
        LW_KEY_SPELLCHECK_DICTIONARY,
        gw_spellcheck_sync_dictionary_cb,
        spellcheck
    );
    priv->signalid[GW_SPELLCHECK_SIGNALID_RK_CONV] = lw_preferences_add_change_listener_by_schema (
        preferences,
        LW_SCHEMA_BASE,
        LW_KEY_ROMAJI_TO_FURIGANA,
        gw_spellcheck_sync_rk_conv_cb,
        spellcheck
    );
    priv->signalid[GW_SPELLCHECK_SIGNALID_DRAW] = g_signal_connect_after (
        G_OBJECT (entry), 
        "draw", 
        G_CALLBACK (gw_spellcheck_draw_underline_cb), 
        spellcheck
    );
    priv->signalid[GW_SPELLCHECK_SIGNALID_CHANGED] = g_signal_connect (
        G_OBJECT (entry), 
        "changed", 
        G_CALLBACK (gw_spellcheck_queue_cb), 
        spellcheck
    );

    priv->signalid[GW_SPELLCHECK_SIGNALID_BUTTON_PRESS_EVENT] = g_signal_connect (
        G_OBJECT (entry), 
        "button-press-event", 
        G_CALLBACK (gw_spellcheck_button_press_event_cb), 
        spellcheck
    );

    priv->signalid[GW_SPELLCHECK_SIGNALID_POPULATE_POPUP] = g_signal_connect (
        G_OBJECT (entry), 
        "populate-popup", 
        G_CALLBACK (gw_spellcheck_populate_popup_cb), 
        spellcheck
    );

    priv->signalid[GW_SPELLCHECK_SIGNALID_DESTROY] = g_signal_connect_swapped (
        G_OBJECT (entry), 
        "destroy", 
        G_CALLBACK (gw_spellcheck_remove_signals), 
        spellcheck
    );

    gw_spellcheck_queue (spellcheck);
}


gboolean
gw_spellcheck_clear (GwSpellcheck *spellcheck)
{
    g_return_val_if_fail (spellcheck != NULL, FALSE);

    GwSpellcheckPrivate *priv;
    gboolean changed;

    priv = spellcheck->priv;
    changed = (priv->misspelled != NULL);

    priv->timeout = 0;
    
    if (priv->tokens != NULL)
      g_strfreev (priv->tokens); priv->tokens = NULL;
    if (priv->misspelled != NULL)
      g_list_free (priv->misspelled); priv->misspelled = NULL;

    return changed;
}


gint
gw_spellcheck_get_layout_y_offset (GwSpellcheck *spellcheck)
{
    g_return_val_if_fail (spellcheck != NULL, 0);

    //Declarations
    GwSpellcheckPrivate *priv;
    PangoRectangle rect;
    PangoLayout *layout;
    gint layout_offset;

    //Initializations
    priv = spellcheck->priv;
    layout = gtk_entry_get_layout (priv->entry);
    pango_layout_get_pixel_extents (layout, &rect, NULL);
    gtk_entry_get_layout_offsets (priv->entry, NULL, &layout_offset);

    return (layout_offset + 1);
}


gint
gw_spellcheck_get_layout_x_offset (GwSpellcheck *spellcheck)
{
    g_return_val_if_fail (spellcheck != NULL, 0);

    //Declarations
    GwSpellcheckPrivate *priv;
    PangoRectangle rect;
    PangoLayout *layout;
    int layout_offset;

    //Initializations
    priv = spellcheck->priv;
    layout = gtk_entry_get_layout (priv->entry);
    pango_layout_get_pixel_extents (layout, &rect, NULL);
    gtk_entry_get_layout_offsets (priv->entry, &layout_offset, NULL);

    return (layout_offset);
}


gboolean
gw_spellcheck_has_hiragana_conversion (GwSpellcheck *spellcheck)
{
    //Declarations
    GwSpellcheckPrivate *priv;
    const gint MAX = 300;
    gchar kana[MAX];
    const gchar *query;
    gboolean has_hiragana_conversion;
    gint rk_conv_setting;
    gboolean want_conv;

    priv = spellcheck->priv;
    rk_conv_setting = priv->rk_conv_setting;
    want_conv = (rk_conv_setting == 0 || (rk_conv_setting == 2 && !lw_util_is_japanese_locale()));
    query = gtk_entry_get_text (priv->entry);
    has_hiragana_conversion = (want_conv && lw_util_str_roma_to_hira (query, kana, MAX));
  
    return has_hiragana_conversion;
}


gboolean
gw_spellcheck_should_check (GwSpellcheck *spellcheck)
{
    //Declarations
    GwSpellcheckPrivate *priv = NULL;
    gboolean has_hiragana_conversion = FALSE;
    gboolean should_check = FALSE;
    const gchar *query = NULL;
    LwMorphologyEngine *engine = NULL;
    

    //Initializations
    priv = spellcheck->priv;
    has_hiragana_conversion = gw_spellcheck_has_hiragana_conversion (spellcheck);
    query = gtk_entry_get_text (priv->entry);
    engine = gw_application_get_morphologyengine (priv->application);
    should_check = (engine != NULL && query != NULL && *query != '\0' && !has_hiragana_conversion);

    return should_check;
}


void
gw_spellcheck_queue (GwSpellcheck *spellcheck)
{
    g_return_if_fail (spellcheck != NULL);

    //Declarations
    GwSpellcheckPrivate *priv = NULL;;
    gboolean should_check = FALSE;
    gboolean should_redraw = FALSE;
    LwMorphologyEngine *engine = NULL;

    //Initializations
    priv = spellcheck->priv;
    should_check = gw_spellcheck_should_check (spellcheck);
    engine = gw_application_get_morphologyengine (priv->application);

    if (engine == NULL) return;

    if (should_check)
    {
      priv->timeout = 0;
      if (priv->timeoutid[GW_SPELLCHECK_TIMEOUTID_UPDATE] == 0)
        priv->timeoutid[GW_SPELLCHECK_TIMEOUTID_UPDATE] = g_timeout_add_full (
          G_PRIORITY_LOW, 
          100, (GSourceFunc) 
          gw_spellcheck_update_timeout, 
          spellcheck, 
          NULL
      );
    }

    should_redraw = gw_spellcheck_clear (spellcheck);
    if (should_redraw) 
    {
      gtk_widget_queue_draw (GTK_WIDGET (priv->entry));
    }
}


static gboolean
gw_spellcheck_is_common_script (const gchar *TEXT)
{
    if (TEXT == NULL) return FALSE;

    GUnicodeScript script;
    gunichar c;
    const gchar *ptr;
    gboolean is_script;

    ptr = TEXT;
    is_script = TRUE;

    while (*ptr != '\0' && is_script == TRUE)
    {
      c = g_utf8_get_char (ptr);
      script = g_unichar_get_script (c);
      if (script != G_UNICODE_SCRIPT_COMMON && script != G_UNICODE_SCRIPT_LATIN)
      {
        is_script = FALSE;
      }
      ptr = g_utf8_next_char (ptr);
    }

    return is_script;
}


gboolean
gw_spellcheck_update (GwSpellcheck *spellcheck)
{
    //Sanity checks
    g_return_val_if_fail (spellcheck != NULL, FALSE);

    //Declarations
    GwSpellcheckPrivate *priv = NULL;
    gboolean should_check = FALSE;
    gboolean should_redraw = FALSE;
    const gchar *query = NULL;
    gchar **iter = NULL;
    LwMorphologyEngine *morphologyengine = NULL;

    //Initializations
    priv = spellcheck->priv;
    morphologyengine = gw_application_get_morphologyengine (priv->application);

    if (priv->timeoutid[GW_SPELLCHECK_TIMEOUTID_UPDATE] == 0)
    {
      return FALSE;
    }
    else if (priv->timeout < priv->threshold) 
    {
      priv->timeout++;
      return TRUE;
    }

    should_check = gw_spellcheck_should_check (spellcheck);
    should_redraw = gw_spellcheck_clear (spellcheck); //Make sure the memory is freed

    if (should_check)
    {
      query = gtk_entry_get_text (priv->entry);
      priv->tokens = g_strsplit (query, " ", -1);

      for (iter = priv->tokens; *iter != NULL; iter++)
      {
        if (**iter != '\0' && gw_spellcheck_is_common_script (*iter))
        {
          LwMorphologyList *morphologylist = lw_morphologyengine_analyze (morphologyengine, *iter, TRUE);
          if (morphologylist != NULL)
          {
            LwMorphology *morphology = lw_morphologylist_read (morphologylist);
            if (morphology != NULL && morphology->spellcheck != NULL)
              priv->misspelled = g_list_append (priv->misspelled, *iter);
            lw_morphologylist_free (morphologylist); morphologylist = NULL;

          }
        }
      }
    }

    if (should_redraw) gtk_widget_queue_draw (GTK_WIDGET (priv->entry));

    priv->timeoutid[GW_SPELLCHECK_TIMEOUTID_UPDATE] = 0;
    priv->timeout = 0;

    return FALSE;
}


void
gw_spellcheck_record_mouse_cordinates (GwSpellcheck *spellcheck, 
                                       GdkEvent     *event)
{
    GwSpellcheckPrivate *priv;
    GtkWidget *toplevel;
    gint toplevel_x, toplevel_y;

    priv = spellcheck->priv;
    toplevel = GTK_WIDGET (gtk_widget_get_ancestor (GTK_WIDGET (priv->entry), GTK_TYPE_WINDOW));

    gdk_window_get_device_position (
      gtk_widget_get_window (GTK_WIDGET (priv->entry)),
      gdk_event_get_device (event),
      &toplevel_x, 
      &toplevel_y, 
      NULL
    );

    gtk_widget_translate_coordinates (
      toplevel, 
      GTK_WIDGET (priv->entry), 
      toplevel_x, 
      toplevel_y, 
      &priv->x, 
      &priv->y
    );
}


static int 
_get_string_index (GtkEntry *entry, 
                   gint      x, 
                   gint      y)
{
    //Declarations
    int layout_index;
    int entry_index;
    int trailing;
    PangoLayout *layout;

    //Initalizations
    layout = gtk_entry_get_layout (GTK_ENTRY (entry));
    if (pango_layout_xy_to_index (layout, x * PANGO_SCALE, y * PANGO_SCALE, &layout_index, &trailing))
      entry_index = gtk_entry_layout_index_to_text_index (GTK_ENTRY (entry), layout_index);
    else
      entry_index = -1;

    return entry_index;
}


void 
gw_spellcheck_populate_popup (GwSpellcheck *spellcheck, 
                              GtkMenu      *menu)
{
    //Sanity checks
    g_return_if_fail (spellcheck != NULL);
    g_return_if_fail (menu != NULL);

    //Declarations
    GwSpellcheckPrivate *priv = NULL;
    GtkWidget *menuitem = NULL, *spellmenuitem = NULL;
    GtkWidget *spellmenu = NULL;

    gint index;
    gint xoffset, yoffset, x, y;
    gint start_offset, end_offset;
    gint i;
    gchar **iter = NULL;

    LwMorphologyEngine *morphologyengine = NULL;

    //Initializations
    priv = spellcheck->priv;
    if (priv->tokens == NULL) return;
    morphologyengine = gw_application_get_morphologyengine (priv->application);
    g_return_if_fail (morphologyengine != NULL);

    xoffset = gw_spellcheck_get_layout_x_offset (spellcheck);
    yoffset = gw_spellcheck_get_layout_y_offset (spellcheck);
    x = priv->x - xoffset;
    y = priv->y - yoffset; //Since a GtkEntry is single line, we want the y to always be in the area
    index =  _get_string_index (priv->entry, x, y);

    start_offset = 0;
    iter = priv->tokens;
    while (*iter != NULL && start_offset + strlen(*iter) < index)
    {
      start_offset += strlen(*iter) + 1;
      iter++;
    }
    if (*iter == NULL) return;
    end_offset = start_offset + strlen(*iter);

    LwMorphologyList *morphologylist = NULL;
    LwMorphology *morphology = NULL;
    gchar **suggestions = NULL;

    morphologylist = lw_morphologyengine_analyze (morphologyengine, *iter, TRUE); if (morphologylist == NULL) goto errored;
    morphology = lw_morphologylist_read (morphologylist); if (morphology == NULL) goto errored;

    if (morphology->spellcheck != NULL)
    {
      suggestions = g_strsplit (morphology->spellcheck, LW_MORPHOLOGY_SPELLCHECK_DELIMITOR, -1);
      gsize total_suggestions = g_strv_length (suggestions);

      if (total_suggestions > 0 && suggestions != NULL)
      {
        menuitem = gtk_separator_menu_item_new ();
        gtk_menu_shell_prepend (GTK_MENU_SHELL (menu), menuitem);
        gtk_widget_show (menuitem);

        spellmenu = gtk_menu_new ();
        spellmenuitem = gtk_menu_item_new_with_label (gettext("_Spellcheck"));
        gtk_menu_item_set_submenu (GTK_MENU_ITEM (spellmenuitem), spellmenu);
        gtk_menu_shell_prepend (GTK_MENU_SHELL (menu), spellmenuitem);
        gtk_widget_show (spellmenuitem);

        gchar *text = g_strdup_printf (gettext("Add \"%s\" to the dictionary"), *iter);
        if (text != NULL)
        {
          GtkWidget *image = gtk_image_new_from_icon_name ("gtk-add", GTK_ICON_SIZE_MENU);
          menuitem = gtk_menu_item_new_with_label (text);
          g_object_set_data_full (G_OBJECT (menuitem), "word", g_strdup (*iter), g_free);
          g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (gw_spellcheck_add_menuitem_activated_cb), spellcheck);
          gtk_menu_shell_append (GTK_MENU_SHELL (spellmenu), menuitem);
          g_free (text); text = NULL;
          gtk_widget_show (menuitem);
        }

        menuitem = gtk_separator_menu_item_new ();
        gtk_menu_shell_append (GTK_MENU_SHELL (spellmenu), menuitem);
        gtk_widget_show (menuitem);

        //Menuitems
        for (i = 0; i < total_suggestions; i++)
        {
          menuitem = gtk_menu_item_new_with_label (suggestions[i]);
          g_object_set_data (G_OBJECT (menuitem), "start-offset", GINT_TO_POINTER (start_offset));
          g_object_set_data (G_OBJECT (menuitem), "end-offset", GINT_TO_POINTER (end_offset));
          g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (gw_spellcheck_menuitem_activated_cb), spellcheck);
          gtk_widget_show (GTK_WIDGET (menuitem));
          gtk_menu_shell_append (GTK_MENU_SHELL (spellmenu), menuitem);
        }

      }
    }

errored:

    if (suggestions != NULL) g_strfreev (suggestions); suggestions = NULL;
    if (morphologylist != NULL) lw_morphologylist_free (morphologylist); morphologylist = NULL;
}



