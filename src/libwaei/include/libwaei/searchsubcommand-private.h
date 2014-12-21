#ifndef LW_SEARCHSUBCOMMAND_PRIVATE_INCLUDED
#define LW_SEARCHSUBCOMMAND_PRIVATE_INCLUDED

#include "search.h"

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  PROP_QUERY,
  PROP_DICTIONARY,
  PROP_SEARCH,
  TOTAL_PROPS
} LwSearchSubCommandProps;

struct _LwSearchSubCommandPrivate {
  gboolean exact_switch;
  gchar *query_text;
  gchar *dictionary_text;
  LwSearch *search;
  LwDictionary *dictionary;
};

struct _LwSearchSubCommandClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_SEARCHSUBCOMMAND_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_SEARCHSUBCOMMAND, LwSearchSubCommandPrivate));

//Properties

//Methods

static void lw_searchsubcommand_run (LwSubCommand *self, gchar *** argv, gint * argc);

G_END_DECLS

#endif

