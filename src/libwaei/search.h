#ifndef LW_SEARCH_INCLUDED
#define LW_SEARCH_INCLUDED

#include "preferences.h"
#include "dictionary.h"
#include "utf8.h"
#include "querynode.h"
#include "results.h"

G_BEGIN_DECLS

#define LW_TYPE_SEARCH lw_search_get_type ()
G_DECLARE_FINAL_TYPE (LwSearch, lw_search, LW, SEARCH, LwList)

//Methods
LwSearch * lw_search_new (gchar const * QUERY, LwSearchFlag flags);
void lw_search_match (LwSearch * self, LwList * haystack);

//Properties

gchar const * lw_search_get_query (LwSearch * self);
LwSearchFlag lw_search_get_flags (LwSearch * self);

G_END_DECLS

#endif
