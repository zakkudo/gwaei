#ifndef LW_SEARCH_INCLUDED
#define LW_SEARCH_INCLUDED

#include "preferences.h"
#include "dictionary.h"
#include "utf8.h"

G_BEGIN_DECLS

#define LW_SEARCH_MAX_CHUNK 1000

#define LW_SEARCHSTATUSNAME_IDLE "Idle"
#define LW_SEARCHSTATUSNAME_SEARCHING "Searching"
#define LW_SEARCHSTATUSNAME_FINISHING "Finishing"
#define LW_SEARCHSTATUSNAME_CANCELING "Canceling"

#define LW_SEARCHSTATUSNICK_IDLE "idle"
#define LW_SEARCHSTATUSNICK_SEARCHING "searching"
#define LW_SEARCHSTATUSNICK_FINISHING "finishing"
#define LW_SEARCHSTATUSNICK_CANCELING "canceling"

//!
//! @brief Search status types
//!
typedef enum {
  LW_SEARCHSTATUS_IDLE,
  LW_SEARCHSTATUS_SEARCHING,
  LW_SEARCHSTATUS_FINISHING,
  LW_SEARCHSTATUS_CANCELING
} LwSearchStatus;
GType lw_searchstatus_get_type (void);
#define LW_TYPE_SEARCHSTATUS (lw_searchstatus_get_type ())

#define LW_SEARCHFLAGNAME_RAW "Raw"
#define LW_SEARCHFLAGNAME_FURIGANA_INSENSITIVE "Furigana Insensitive"
#define LW_SEARCHFLAGNAME_CASE_INSENSITIVE "Case Insensitive"
#define LW_SEARCHFLAGNAME_STEM_INSENSITIVE "Stem Insensitive"
#define LW_SEARCHFLAGNAME_ROMAJI_TO_FURIGANA "Romaji to Furigana"
#define LW_SEARCHFLAGNAME_USE_INDEX "Use Index"
#define LW_SEARCHFLAGNAME_INSENSITIVE "Insensitive"

#define LW_SEARCHFLAGNICK_RAW "raw"
#define LW_SEARCHFLAGNICK_FURIGANA_INSENSITIVE "furigana-insensitive"
#define LW_SEARCHFLAGNICK_CASE_INSENSITIVE "case-insensitive"
#define LW_SEARCHFLAGNICK_STEM_INSENSITIVE "stem-insensitive"
#define LW_SEARCHFLAGNICK_ROMAJI_TO_FURIGANA "romaji-to furigana"
#define LW_SEARCHFLAGNICK_USE_INDEX "use-index"
#define LW_SEARCHFLAGNICK_INSENSITIVE "insensitive"

typedef enum {
  LW_SEARCHFLAG_RAW = (1 << 0),
  LW_SEARCHFLAG_FURIGANA_INSENSITIVE = (1 << 1),
  LW_SEARCHFLAG_CASE_INSENSITIVE = (1 << 2),
  LW_SEARCHFLAG_STEM_INSENSITIVE = (1 << 3),
  LW_SEARCHFLAG_ROMAJI_TO_FURIGANA = (1 << 4),
  LW_SEARCHFLAG_USE_INDEX = (1 << 5),
  LW_SEARCHFLAG_INSENSITIVE = (LW_SEARCHFLAG_FURIGANA_INSENSITIVE | LW_SEARCHFLAG_CASE_INSENSITIVE | LW_SEARCHFLAG_STEM_INSENSITIVE),
} LwSearchFlag;
GType lw_searchflag_get_type (void);
#define LW_TYPE_SEARCHFLAG (lw_searchflag_get_type ())

//Boilerplate
typedef struct _LwSearch LwSearch;
typedef struct _LwSearchClass LwSearchClass;
typedef struct _LwSearchPrivate LwSearchPrivate;
typedef struct _LwSearchClassPrivate LwSearchClassPrivate;

#define LW_TYPE_SEARCH              (lw_search_get_type())
#define LW_SEARCH(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_SEARCH, LwSearch))
#define LW_SEARCH_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_SEARCH, LwSearchClass))
#define LW_IS_SEARCH(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_SEARCH))
#define LW_IS_SEARCH_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_SEARCH))
#define LW_SEARCH_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_SEARCH, LwSearchClass))


struct _LwSearch {
  GObject object;
  LwSearchPrivate *priv;
};

struct _LwSearchClass {
  GObjectClass parent_class;
  LwSearchClassPrivate *priv;

  //Signals
  void (*row_changed) (LwSearch* search, gint position, gpointer data);
  void (*row_inserted) (LwSearch* search, gint position, gpointer data);
  void (*row_deleted) (LwSearch* search, gint position, gpointer data);
  void (*rows_reordered) (LwSearch* search, gint *order, gpointer data);
};


//Methods
LwSearch* lw_search_new (const gchar *QUERY, LwDictionary *dictionary, LwSearchFlag flags);
LwSearch* lw_search_new_by_preferences (const gchar *QUERY, LwDictionary  *dictionary, LwPreferences *preferences);
GType lw_search_get_type (void) G_GNUC_CONST;


void lw_search_start (LwSearch *self);
void lw_search_start_async (LwSearch *self);

//Properties

void lw_search_set_dictionary (LwSearch *self, LwDictionary *dictionary);
LwDictionary* lw_search_get_dictionary (LwSearch *self);

void lw_search_set_query (LwSearch *self, const gchar *QUERY);
const gchar* lw_search_get_query (LwSearch *self);

void lw_search_set_max_results (LwSearch *self, gint max_results);
gint lw_search_get_max_results (LwSearch *self);

void lw_search_set_status (LwSearch *self, LwSearchStatus status);
LwSearchStatus lw_search_get_status (LwSearch *self);

void lw_search_set_progress (LwSearch *self, LwProgress *progress);
LwProgress* lw_search_get_progress (LwSearch *self);

GRegex* lw_search_get_regex (LwSearch *self);
void lw_search_set_regex (LwSearch *self, GRegex *regex);
void lw_search_sync_regex (LwSearch *self);

LwSearchFlag lw_search_get_flags (LwSearch *self);
void lw_search_set_flags (LwSearch *self, LwSearchFlag flags);
LwSearchFlag lw_search_build_flags_from_preferences (LwPreferences *preferences);

G_END_DECLS

#endif
