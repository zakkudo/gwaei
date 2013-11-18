#ifndef GW_SETTINGSWINDOW_PRIVATE_INCLUDED
#define GW_SETTINGSWINDOW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  GW_SETTINGSWINDOW_SIGNALID_ROMAJI_TO_FURIGANA,
  GW_SETTINGSWINDOW_SIGNALID_FURIGANA_INSENSITIVE,
  GW_SETTINGSWINDOW_SIGNALID_CASE_INSENSITIVE,
  GW_SETTINGSWINDOW_SIGNALID_STEM_INSENSITIVE,
  GW_SETTINGSWINDOW_SIGNALID_USE_GLOBAL_DOCUMENT_FONT,
  GW_SETTINGSWINDOW_SIGNALID_GLOBAL_DOCUMENT_FONT,
  GW_SETTINGSWINDOW_SIGNALID_CUSTOM_FONT,
  GW_SETTINGSWINDOW_SIGNALID_SEARCH_AS_YOU_TYPE,
  GW_SETTINGSWINDOW_SIGNALID_SPELLCHECK,
  GW_SETTINGSWINDOW_SIGNALID_MATCH_FG,
  GW_SETTINGSWINDOW_SIGNALID_MATCH_BG,
  GW_SETTINGSWINDOW_SIGNALID_COMMENT_FG,
  GW_SETTINGSWINDOW_SIGNALID_COMMENT_BG,
  GW_SETTINGSWINDOW_SIGNALID_HEADER_FG,
  GW_SETTINGSWINDOW_SIGNALID_HEADER_BG,
  TOTAL_GW_SETTINGSWINDOW_SIGNALIDS
} GwSettingsWindowSignalIds;

struct _GwSettingsWindowPrivate {
  GtkNotebook *notebook;
  GtkTreeView *manage_dictionaries_treeview;
  GtkButton *close_button;
  GtkToolButton *remove_dictionary_toolbutton;

  GtkColorButton *match_foreground;
  GtkColorButton *match_background;
  GtkColorButton *comment_foreground;
  GtkColorButton *header_foreground;
  GtkColorButton *header_background;

  GtkToggleButton *spellcheck_checkbutton;

  GtkComboBox *romaji_to_furigana_combobox;
  GtkCheckButton *furigana_insensitive_checkbutton;
  GtkCheckButton *case_insensitive_checkbutton;
  GtkCheckButton *stem_insensitive_checkbutton;
  GtkCheckButton *system_font_checkbutton;
  GtkBox *system_document_font_hbox;
  GtkCheckButton *search_as_you_type_checkbutton;

  GtkBox *please_install_dictionary_hbox;
  GtkFontButton *custom_font_fontbutton;

  guint signalids[TOTAL_GW_SETTINGSWINDOW_SIGNALIDS];
//  guint timeoutids[TOTAL_GW_SETTINGSWINDOW_TIMEOUTIDS];
};

#define GW_SETTINGSWINDOW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GW_TYPE_SETTINGSWINDOW, GwSettingsWindowPrivate))

G_END_DECLS

#endif

