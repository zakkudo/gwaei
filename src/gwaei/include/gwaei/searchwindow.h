#ifndef GW_SEARCHWINDOW_INCLUDED
#define GW_SEARCHWINDOW_INCLUDED

#include <libwaei/search.h>

G_BEGIN_DECLS

typedef enum {
  GW_SEARCHWINDOW_CLASS_SIGNALID_WORD_ADDED,
  TOTAL_GW_SEARCHWINDOW_CLASS_SIGNALIDS
} GwSearchWindowClassSignalId;


//Boilerplate
typedef struct _GwSearchWindow GwSearchWindow;
typedef struct _GwSearchWindowClass GwSearchWindowClass;
typedef struct _GwSearchWindowPrivate GwSearchWindowPrivate;

#define GW_TYPE_SEARCHWINDOW              (gw_searchwindow_get_type())
#define GW_SEARCHWINDOW(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GW_TYPE_SEARCHWINDOW, GwSearchWindow))
#define GW_SEARCHWINDOW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GW_TYPE_SEARCHWINDOW, GwSearchWindowClass))
#define GW_IS_SEARCHWINDOW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GW_TYPE_SEARCHWINDOW))
#define GW_IS_SEARCHWINDOW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GW_TYPE_SEARCHWINDOW))
#define GW_SEARCHWINDOW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GW_TYPE_SEARCHWINDOW, GwSearchWindowClass))

#define GW_SEARCHWINDOW_KEEP_SEARCHING_MAX_DELAY 3

struct _GwSearchWindow {
  GwWindow window;
  GwSearchWindowPrivate *priv;
};

struct _GwSearchWindowClass {
  GwWindowClass parent_class;
  guint signalid[TOTAL_GW_SEARCHWINDOW_CLASS_SIGNALIDS];
  void (*word_added) (GwSearchWindow* window, LwResult *result);
  GtkSizeGroup *tablabel_sizegroup;
  GtkCssProvider *tablabel_cssprovider;
};

GtkWindow* gw_searchwindow_new (GtkApplication *application);
GType gw_searchwindow_get_type (void) G_GNUC_CONST;

gboolean gw_searchwindow_update_progress_feedback_timeout (GwSearchWindow*);
gboolean gw_searchwindow_update_icons_for_selection_timeout (GwSearchWindow*);
gboolean gw_searchwindow_keep_searching_timeout (GwSearchWindow*);

void gw_searchwindow_update_history_popups (GwSearchWindow*);
void gw_searchwindow_update_toolbar_buttons (GwSearchWindow*);
void gw_searchwindow_update_total_results_label (GwSearchWindow*, LwSearch*);

char* gw_searchwindow_get_text_slice_from_buffer (GwSearchWindow*, int, int, int);
char* gw_searchwindow_get_text (GwSearchWindow*, GtkWidget*);

void gw_searchwindow_cancel_search_by_searchitem (GwSearchWindow*, LwSearch*);
void gw_searchwindow_cancel_search_by_tab_number (GwSearchWindow*, const int);
void gw_searchwindow_cancel_search_for_current_tab (GwSearchWindow*);
void gw_searchwindow_cancel_search_by_content (GwSearchWindow*, gpointer);
void gw_searchwindow_cancel_all_searches (GwSearchWindow*);

void gw_searchwindow_sync_tabbar_show (GwSearchWindow*);

void gw_searchwindow_initialize_buffer_by_searchitem (GwSearchWindow*, LwSearch*);

void gw_searchwindow_entry_set_text (GwSearchWindow*, const gchar*);
void gw_searchwindow_entry_insert_text (GwSearchWindow*, const gchar*);
void gw_searchwindow_set_entry_text_by_searchitem (GwSearchWindow*, LwSearch*);
void gw_searchwindow_set_total_results_label_by_searchresultiterator (GwSearchWindow*, LwSearchResultIterator*);
void gw_searchwindow_set_search_progressbar_by_searchitem (GwSearchWindow*, LwSearch*);
char* gw_searchwindow_get_title_by_searchresultiterator (GwSearchWindow*, LwSearchResultIterator*);
void gw_searchwindow_set_title_by_searchresultiterator (GwSearchWindow*, LwSearchResultIterator*);

LwDictionary* gw_searchwindow_get_dictionary (GwSearchWindow*);
void gw_searchwindow_set_dictionary_by_searchitem (GwSearchWindow *window, LwSearch*);
void gw_searchwindow_set_dictionary (GwSearchWindow*, int);

void gw_searchwindow_buffer_initialize_tags (GwSearchWindow*);
void gw_searchwindow_set_font (GwSearchWindow*);
void gw_searchwindow_buffer_initialize_marks (GtkTextBuffer*);

void gw_searchwindow_clear_search_entry (GwSearchWindow*);

gunichar gw_searchwindow_get_hovered_character (GwSearchWindow*, int*, int*, GtkTextIter*);
void gw_searchwindow_show_window (GwSearchWindow*, char*);
void gw_searchwindow_set_cursor (GwSearchWindow*, GdkCursorType);

void gw_searchwindow_paste_text (GwSearchWindow*, GtkWidget*);
void gw_searchwindow_cut_text (GwSearchWindow*, GtkWidget*);
void gw_searchwindow_copy_text (GwSearchWindow*, GtkWidget*);

void gw_searchwindow_cycle_dictionaries (GwSearchWindow*, gboolean);

void gw_searchwindow_select_all (GwSearchWindow*, GtkWidget*);
void gw_searchwindow_select_none (GwSearchWindow*, GtkWidget*);
gboolean gw_searchwindow_has_selection (GwSearchWindow*, GtkWidget*);

void gw_searchwindow_reload_tagtable_tags (GwSearchWindow*);

void gw_searchwindow_set_dictionary_by_searchitem (GwSearchWindow*, LwSearch*);

void gw_searchwindow_close_suggestion_box (GwSearchWindow*);
void gw_searchwindow_set_furigana_insensitive (GwSearchWindow*, gboolean);
void gw_searchwindow_set_romaji_to_furigana (GwSearchWindow*, int);
void gw_searchwindow_set_use_global_document_font_checkbox (GwSearchWindow*, gboolean);
void gw_searchwindow_set_toolbar_show (GwSearchWindow*, gboolean*);
void gw_searchwindow_set_toolbar_style (GwSearchWindow*, const char*);
void gw_searchwindow_set_statusbar_show (GwSearchWindow*, gboolean);
void gw_searchwindow_set_color_to_swatch (GwSearchWindow*, const char*, const char*);

void gw_searchwindow_guarantee_first_tab (GwSearchWindow*);

GtkTextView* gw_searchwindow_get_textview (GwSearchWindow*, int);
GtkInfoBar* gw_searchwindow_get_infobar (GwSearchWindow*, int);
GtkTextView* gw_searchwindow_get_current_textview (GwSearchWindow*);
GtkInfoBar* gw_searchwindow_get_current_infobar (GwSearchWindow*);

void gw_searchwindow_show_current_infobar (GwSearchWindow*, char*);
void gw_searchwindow_hide_current_infobar (GwSearchWindow*);
void gw_searchwindow_hide_infobars (GwSearchWindow*);

void gw_searchwindow_set_tab_text_by_searchitem (GwSearchWindow*, LwSearch*);

gint gw_searchwindow_get_current_tab_index (GwSearchWindow*);
void gw_searchwindow_set_searchresultiterator_by_index (GwSearchWindow*, gint, LwSearchResultIterator*);
LwSearchResultIterator* gw_searchwindow_get_searchresultiterator_by_index (GwSearchWindow*, gint);
LwSearchResultIterator* gw_searchwindow_steal_searchresultiterator_by_index (GwSearchWindow*, gint);

void gw_searchwindow_no_results_search_for_dictionary_cb (GtkWidget*, gpointer);

int gw_searchwindow_new_tab (GwSearchWindow*);
void gw_searchwindow_remove_tab_by_index (GwSearchWindow*, int);

void gw_searchwindow_start_search (GwSearchWindow*, LwSearch*, LwProgress*);
void gw_searchwindow_start_tooltip_search (GwSearchWindow *window, const gchar *QUERY, GdkEventButton *event, LwProgress *progress);

void gw_searchwindow_insert_resultpopup_button (GwSearchWindow*, LwSearch*, LwResult*, GtkTextIter*);

void gw_searchwindow_append_to_buffer (GwSearchWindow*, LwSearch*, const char *, char*, char*, int*, int*);

void gw_searchwindow_initialize_toolbar (GwSearchWindow*);
void gw_searchwindow_initialize_search_toolbar (GwSearchWindow*);
void gw_searchwindow_initialize_dictionary_menu (GwSearchWindow*);
void gw_searchwindow_initialize_menu_links (GwSearchWindow*);
void gw_searchwindow_initialize_dictionary_combobox (GwSearchWindow*);

void gw_searchwindow_sync_history (GwSearchWindow*);

void gw_searchwindow_go_back (GwSearchWindow*, gint, LwProgress*);
void gw_searchwindow_go_forward (GwSearchWindow*, gint, LwProgress*);

void gw_searchwindow_set_links (GwSearchWindow*, GMenuModel*);
GMenuModel* gw_searchwindow_get_popup_menu (GwSearchWindow*);

gint gw_searchwindow_entry_get_position (GwSearchWindow *window);

#include "searchwindow-callbacks.h"
#include "searchwindow-output.h"
#include "search-data.h"

G_END_DECLS

#endif
