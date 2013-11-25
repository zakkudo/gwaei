#ifndef LGW_RESULTSVIEW_INCLUDED
#define LGW_RESULTSVIEW_INCLUDED

#include <glib-object.h>

#define LGW_TYPE_RESULTSVIEW                (lgw_resultsview_get_type ())
#define LGW_RESULTSVIEW(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), LGW_TYPE_RESULTSVIEW, LgwResultsView))
#define LGW_IS_RESULTSVIEW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LGW_TYPE_RESULTSVIEW))
#define LGW_RESULTSVIEW_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), LGW_TYPE_RESULTSVIEW, LgwResultsViewInterface))

typedef struct _LgwResultsView LgwResultsView; /* dummy object */
typedef struct _LgwResultsViewInterface LgwResultsViewInterface;

struct _LgwResultsViewInterface {
      GTypeInterface parent;
      void (*set_search) (LgwResultsView *self, GList *searchlist);
};

GType lgw_resultsview_get_type (void);

void lgw_resultsview_set_searchlist (LgwResultsView *self, GList *searchlist);


#endif
