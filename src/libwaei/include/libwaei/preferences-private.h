#ifndef LW_PREFERENCES_PRIVATE_INCLUDED
#define LW_PREFERENCES_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _LwPreferencesPrivate {
  GList *settingslist;
  GSettingsBackend *backend;

  gboolean toolbar_show; 
  gboolean statusbar_show;
  gboolean query_katakana_to_hiragana;
  gboolean query_hiragana_to_katakana;
  gint query_romaji_to_kana;
  gboolean query_spellcheck;
  gboolean search_as_you_type;
  gchar* window_positions;

  gchar *dictionary_load_order;
  gchar *dictionary_english_source;
  gchar *dictionary_kanji_source;
  gchar *dictionary_names_places_source;
  gchar *dictionary_examples_source;

  gboolean use_global_document_font;
  gchar *custom_document_font;
  gint magnification;
  
  gchar *comment_foreground;
  gchar *comment_background;
  gchar *match_foreground;
  gchar *match_background;
  gchar *header_foreground;
  gchar *header_background;
};

#define LW_PREFERENCES_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_PREFERENCES, LwPreferencesPrivate));

G_END_DECLS

#endif
