#ifndef LW_RESULTS_INCLUDED
#define LW_RESULTS_INCLUDED

#include "editable-list.h"
#include "dictionary-cache.h"
#include "dictionary.h"
#include "result.h"

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_RESULTS lw_results_get_type ()
G_DECLARE_FINAL_TYPE (LwResults, lw_results, LW, RESULTS, LwEditableList)

//Methods
LwResults * lw_results_new (LwDictionary * dictionary, LwDictionaryCache * dictionary_cache);

void lw_results_append (LwResults * self, LwResult * result);

/*
LwDictionaryCache * lw_results_get_dictionarycache (LwResults * self);
GSequence * lw_results_get_sequence (LwResults * self);

LwResultsIter * lw_results_append_result (LwResults * self, LwResult * result);

gint lw_results_compare_score_func (LwResult * a, LwResult * b);
void lw_results_sort_by_score (LwResults * self);
gint lw_results_compare_index_func (LwResult * a, LwResult * b);
void lw_results_sort_by_index (LwResults * self);
gint lw_results_compare_column_func (LwResult * a, LwResult * b, struct LwResultsCompareByColumnIdFuncData * data);
void lw_results_sort_by_column (LwResults * self, gint column);

gint lw_results_length (LwResults * self);
*/

G_END_DECLS

#endif

