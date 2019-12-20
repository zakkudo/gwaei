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

G_END_DECLS

#endif

