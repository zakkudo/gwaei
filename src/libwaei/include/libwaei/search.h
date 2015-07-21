#ifndef LW_SEARCH_INCLUDED
#define LW_SEARCH_INCLUDED

#include "preferences.h"
#include "dictionary.h"
#include "utf8.h"
#include "querynode.h"
#include "results.h"

G_BEGIN_DECLS

#define LW_SEARCH_DEFAULT_MAX_CHUNK 1000

#define LW_SEARCHSTATUSNAME_UNSTARTED "Unstarted"
#define LW_SEARCHSTATUSNAME_SEARCHING "Searching"
#define LW_SEARCHSTATUSNAME_CANCELING "Canceling"
#define LW_SEARCHSTATUSNAME_CANCELLED "Cancelled"
#define LW_SEARCHSTATUSNAME_FINISHED "Finished"

#define LW_SEARCHSTATUSNICK_UNSTARTED "unstarted"
#define LW_SEARCHSTATUSNICK_SEARCHING "searching"
#define LW_SEARCHSTATUSNICK_CANCELING "canceling"
#define LW_SEARCHSTATUSNICK_CANCELLED "cancelled"
#define LW_SEARCHSTATUSNICK_FINISHED "finished"

//!
//! @brief Search status types
//!
typedef enum {
  LW_SEARCHSTATUS_UNSTARTED,
  LW_SEARCHSTATUS_SEARCHING,
  LW_SEARCHSTATUS_CANCELING,
  LW_SEARCHSTATUS_CANCELLED,
  LW_SEARCHSTATUS_ERRORED,
  LW_SEARCHSTATUS_FINISHED,
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
  LwResults object;
  LwSearchPrivate *priv;
};

struct _LwSearchClass {
  LwResultsClass parent_class;
  LwSearchClassPrivate *priv;
};


//Methods
LwSearch* lw_search_new (gchar const *QUERY, LwDictionary * dictionary, LwSearchFlag flags);
LwSearch* lw_search_new_by_preferences (gchar const *QUERY, LwDictionary  * dictionary, LwPreferences * preferences);
GType lw_search_get_type (void) G_GNUC_CONST;

LwResults * lw_search_query_results (LwSearch * self);
LwResults * lw_search_query_results_async (LwSearch * self, GError ** error);

//Properties

LwDictionary* lw_search_get_dictionary (LwSearch * self);
gchar const * lw_search_get_query (LwSearch * self);
LwQueryNode * lw_search_get_query_tree (LwSearch * self);

void lw_search_set_max_results (LwSearch * self, gint max_results);
gint lw_search_get_max_results (LwSearch * self);

LwSearchStatus lw_search_get_status (LwSearch * self);

LwProgress* lw_search_get_progress (LwSearch * self);

LwSearchFlag lw_search_get_flags (LwSearch * self);
LwSearchFlag lw_search_build_flags_from_preferences (LwPreferences * preferences);
LwUtf8Flag lw_search_build_utf8flags (LwSearch * self);

LwResults * lw_search_get_results (LwSearch * self);

void lw_search_cancel (LwSearch * self);

G_END_DECLS

#endif
