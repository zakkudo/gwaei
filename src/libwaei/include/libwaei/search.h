#ifndef LW_SEARCH_INCLUDED
#define LW_SEARCH_INCLUDED

#include <stdio.h>

#include <libwaei/result.h>
#include <libwaei/dictionary.h>

G_BEGIN_DECLS

#define LW_SEARCH(object) (LwSearch*) object
#define LW_SEARCH_DATA_FREE_FUNC(object) (LwSearchDataFreeFunc)object
#define LW_SEARCH_MAX_CHUNK 1000

//!
//! @brief Search status types
//!
typedef enum
{
  LW_SEARCHSTATUS_IDLE,
  LW_SEARCHSTATUS_SEARCHING,
  LW_SEARCHSTATUS_FINISHING,
  LW_SEARCHSTATUS_CANCELING
} LwSearchStatus;


typedef enum
{
  LW_SEARCH_FLAG_RAW = (1 << 0),
  LW_SEARCH_FLAG_FURIGANA_INSENSITIVE = (1 << 1),
  LW_SEARCH_FLAG_CASE_INSENSITIVE = (1 << 2),
  LW_SEARCH_FLAG_STEM_INSENSITIVE = (1 << 3),
  LW_SEARCH_FLAG_ROMAJI_TO_FURIGANA = (1 << 4),
  LW_SEARCH_FLAG_USE_INDEX = (1 << 5),
  LW_SEARCH_FLAG_INSENSITIVE = (LW_SEARCH_FLAG_FURIGANA_INSENSITIVE | LW_SEARCH_FLAG_CASE_INSENSITIVE | LW_SEARCH_FLAG_STEM_INSENSITIVE),
} LwSearchFlag;

typedef void(*LwSearchDataFreeFunc)(gpointer);

//!
//! @brief Primitive for storing search item information
//!
//Object
struct _LwSearch {
    gchar *query;
    LwDictionary* dictionary;                 //!< Pointer to the dictionary used

    GThread *thread;                        //!< Thread the search is processed in
    GMutex mutex;                          //!< Mutext to help ensure threadsafe operation

    LwSearchStatus status;                  //!< Used to test if a search is in progress.
    LwSearchFlag flags;

    LwMorphologyEngine *morphologyengine;

    gint max;

    GHashTable *resulttable;

    gpointer data;                 //!< Pointer to a buffer that stays constant unlike when the target attribute is used

    LwProgress *progress;

    LwSearchDataFreeFunc free_data_func;
};
typedef struct _LwSearch LwSearch;

//Methods
LwSearch* lw_search_new (LwDictionary *dictionary, LwMorphologyEngine *morphologyengine, const gchar *QUERY, LwSearchFlag flags);
void lw_search_free (LwSearch*);

void lw_search_cleanup_search (LwSearch*);
void lw_search_clear_results (LwSearch*);
void lw_search_prepare_search (LwSearch*);

gboolean lw_search_compare (LwSearch *);
gboolean lw_search_is_equal (LwSearch*, LwSearch*);

void lw_search_set_data (LwSearch*, gpointer, LwSearchDataFreeFunc);
gpointer lw_search_get_data (LwSearch*);
void lw_search_free_data (LwSearch*);
gboolean lw_search_has_data (LwSearch*);

gboolean  lw_search_has_results (LwSearch*);
void lw_search_parse_result_string (LwSearch*);
void lw_search_cancel (LwSearch*);

void lw_search_lock (LwSearch*);
void lw_search_unlock (LwSearch*);

void lw_search_set_status (LwSearch*, LwSearchStatus);
LwSearchStatus lw_search_get_status (LwSearch*);

gboolean lw_search_read_line (LwSearch*);

void lw_search_start (LwSearch*, LwProgress*, gboolean);

gboolean lw_search_has_results (LwSearch *search);

LwSearchFlag lw_search_get_flags_from_preferences (LwPreferences*);

LwMorphologyList* lw_search_get_query_as_morphologylist (LwSearch* search);

LwProgress* lw_search_get_progress (LwSearch *search);

G_END_DECLS

#endif
