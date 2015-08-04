#ifndef LW_RESULTS_INCLUDED
#define LW_RESULTS_INCLUDED

#include "preferences.h"
#include "dictionary.h"
#include "utf8.h"
#include "querynode.h"
#include "result.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwResults LwResults;
typedef struct _LwResultsClass LwResultsClass;
typedef struct _LwResultsPrivate LwResultsPrivate;
typedef struct _LwResultsClassPrivate LwResultsClassPrivate;
typedef GSequenceIter LwResultsIter;

#define LW_TYPE_RESULTS              (lw_results_get_type())
#define LW_RESULTS(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_RESULTS, LwResults))
#define LW_RESULTS_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_RESULTS, LwResultsClass))
#define LW_IS_RESULTS(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_RESULTS))
#define LW_IS_RESULTS_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_RESULTS))
#define LW_RESULTS_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_RESULTS, LwResultsClass))


struct _LwResults {
  GObject object;
  LwResultsPrivate *priv;
};

struct _LwResultsClass {
  GObjectClass parent_class;
  LwResultsClassPrivate *priv;

  //Signals
  void (*row_changed) (LwResults * self, gint position, gpointer data);
  void (*row_inserted) (LwResults * self, gint position, gpointer data);
  void (*row_deleted) (LwResults * self, gint position, gpointer data);
  void (*rows_reordered) (LwResults * self, gint *order, gpointer data);
};


//Methods
LwResults * lw_results_new (LwDictionaryCache * dictionary_cache);

GType lw_results_get_type (void) G_GNUC_CONST;

LwDictionaryCache * lw_results_get_dictionarycache (LwResults * self);
GSequence * lw_results_get_sequence (LwResults * self);

LwResultsIter * lw_results_append_result (LwResults * self, LwResult * result);

G_END_DECLS

#endif

