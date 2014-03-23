#ifndef LW_PREFERENCES_INCLUDED
#define LW_PREFERENCES_INCLUDED

#include <gio/gio.h>
#define G_SETTINGS_ENABLE_BACKEND
#include <gio/gsettingsbackend.h>

G_BEGIN_DECLS

//GSettings
#define LW_SCHEMA_GNOME_INTERFACE   "org.gnome.desktop.interface"
#define LW_KEY_TOOLBAR_STYLE        "toolbar-style"
#define LW_KEY_DOCUMENT_FONT_NAME   "font-name"
#define LW_KEY_PROGRAM_VERSION      "version"

/////////////////////////
#define LW_SCHEMA_BASE               "org.gnome.gwaei"
#define LW_KEY_WINDOW_SIZE           "window-size"
#define LW_KEY_TABBAR_SHOW           "tabbar-show"
#define LW_KEY_MENUBAR_SHOW          "menubar-show"
#define LW_KEY_TOOLBAR_SHOW          "toolbar-show"
#define LW_KEY_STATUSBAR_SHOW        "statusbar-show"

#define LW_KEY_SPELLCHECK_DICTIONARY "spellcheck-dictionary"
#define LW_KEY_SEARCH_AS_YOU_TYPE    "search-as-you-type"

#define LW_KEY_SPELLCHECK           "query-spellcheck"
#define LW_KEY_FURIGANA_INSENSITIVE "query-furigana-insensitive"
#define LW_KEY_CASE_INSENSITIVE     "query-case-insensitive"
#define LW_KEY_STEM_INSENSITIVE     "query-stem-insensitive"
#define LW_KEY_ROMAJI_TO_FURIGANA   "query-romaji-to-furigana"

#define LW_KEY_INDEX_RESULTS        "index-results"
#define LW_KEY_ONTLY_EXACT_MATCHES  "only-exact-matches"

//////////////////////////
#define LW_SCHEMA_FONT               "org.gnome.gwaei.fonts"
#define LW_KEY_FONT_USE_GLOBAL_FONT  "use-global-document-font"
#define LW_KEY_FONT_CUSTOM_FONT      "custom-document-font"
#define LW_KEY_FONT_MAGNIFICATION    "magnification"

////////////////////////////
#define LW_SCHEMA_HIGHLIGHT     "org.gnome.gwaei.highlighting"
#define LW_KEY_MATCH_FG         "match-foreground"
#define LW_KEY_MATCH_BG         "match-background"
#define LW_KEY_HEADER_FG        "header-foreground"
#define LW_KEY_HEADER_BG        "header-background"
#define LW_KEY_COMMENT_FG       "comment-foreground"

#define LW_MATCH_FG_DEFAULT       "#000000"
#define LW_MATCH_BG_DEFAULT       "#CCEECC"
#define LW_HEADER_FG_DEFAULT      "#EE1111"
#define LW_HEADER_BG_DEFAULT      "#FFDEDE"
#define LW_COMMENT_FG_DEFAULT     "#2222DD"



//Boilerplate
typedef struct _LwPreferences LwPreferences;
typedef struct _LwPreferencesClass LwPreferencesClass;
typedef struct _LwPreferencesPrivate LwPreferencesPrivate;

#define LW_TYPE_PREFERENCES              (lw_preferences_get_type())
#define LW_PREFERENCES(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_PREFERENCES, LwPreferences))
#define LW_PREFERENCES_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_PREFERENCES, LwPreferencesClass))
#define LW_IS_PREFERENCES(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_PREFERENCES))
#define LW_IS_PREFERENCES_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_PREFERENCES))
#define LW_PREFERENCES_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_PREFERENCES, LwPreferencesClass))


struct _LwPreferences {
  GObject object;
  LwPreferencesPrivate *priv;
};


struct _LwPreferencesClass {
  GObjectClass parent_class;
};


LwPreferences* lw_preferences_new (GSettingsBackend*);
LwPreferences* lw_preferences_get_default (void);
GType lw_preferences_get_type (void) G_GNUC_CONST;

//Methods

gboolean lw_preferences_schema_is_installed (const gchar*);
GSettings* lw_preferences_get_settings_object (LwPreferences*, const gchar*);

void lw_preferences_reset_value (GSettings*, const gchar*);
void lw_preferences_reset_value_by_schema (LwPreferences*, const gchar*, const gchar*);

int lw_preferences_get_int (GSettings*, const gchar *);
int lw_preferences_get_int_by_schema (LwPreferences*, const gchar*, const gchar *);

void lw_preferences_set_int (GSettings*, const gchar*, const int);
void lw_preferences_set_int_by_schema (LwPreferences*, const gchar*, const gchar*, const int);

gboolean lw_preferences_get_boolean (GSettings*, const gchar *);
gboolean lw_preferences_get_boolean_by_schema (LwPreferences*, const gchar*, const gchar*);

void lw_preferences_set_boolean (GSettings*, const gchar*, const gboolean);
void lw_preferences_set_boolean_by_schema (LwPreferences*, const gchar*, const gchar*, const gboolean);

gchar* lw_preferences_get_string (GSettings*, const gchar*);
gchar* lw_preferences_get_string_by_schema (LwPreferences*, const gchar*, const gchar*);

void lw_preferences_set_string (GSettings*, const gchar*, const gchar*);
void lw_preferences_set_string_by_schema (LwPreferences*, const gchar*, const gchar*, const gchar*);

gchar** lw_preferences_get_strv (GSettings *settings, const gchar *KEY);
gchar** lw_preferences_get_strv_by_schema (LwPreferences *preferences, const gchar *SCHEMA, const gchar *KEY);

void lw_preferences_set_strv (GSettings  *settings, const gchar *KEY, const gchar *const *REQUEST);
void  lw_preferences_set_strv_by_schema (LwPreferences *preferences, const gchar *SCHEMA, const gchar *KEY, const gchar *const *REQUEST);

gulong lw_preferences_add_change_listener (GSettings*, const gchar*, void (*callback_function) (GSettings*, gchar*, gpointer), gpointer);
gulong lw_preferences_add_change_listener_by_schema (LwPreferences*, const gchar*, const gchar*, void (*callback_function) (GSettings*, gchar*, gpointer), gpointer);

void lw_preferences_remove_change_listener (GSettings*, gulong);
void lw_preferences_remove_change_listener_by_schema (LwPreferences*, const gchar*, gulong);

G_END_DECLS

#endif


