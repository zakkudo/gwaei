#ifndef LGW_DELETEVOCABULARYLISTDIALOG_PRIVATE_INCLUDED
#define LGW_DELETEVOCABULARYLISTDIALOG_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    SIGNALID_RESPONSE,
    TOTAL_SIGNALIDS
} SignalId;


typedef enum {
    PROP_0,
    PROP_WORDSTORES,
    PROP_VOCABULARYLISTSTORE,
    PROP_DELETE_ON_RESPONSE,
    TOTAL_PROPS
} Props;

struct _UI {
  GtkButton *delete_button;
  GtkButton *cancel_button;
  GtkGrid *grid;
  GtkBox *layout_box;
  GtkLabel *primary_label;
  GtkLabel *secondary_label;
  GtkLabel *list_label;
};

struct _Data {
  guint signalid[TOTAL_SIGNALIDS];
  LgwVocabularyListStore *vocabulary_list_store;
  GList *treepaths;
  GList *wordstores;
  GHashTable *index;
};

struct _Config {
  gboolean delete_on_response;
};

struct _LgwDeleteVocabularyListDialogPrivate {
  struct _UI ui;
  struct _Data data;
  struct _Config config;
};

struct _LgwDeleteVocabularyListDialogClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LGW_DELETEVOCABULARYLISTDIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_DELETEVOCABULARYLISTDIALOG, LgwDeleteVocabularyListDialogPrivate));

void lgw_deletevocabularylistdialog_sync_labels (LgwDeleteVocabularyListDialog *self);

G_END_DECLS

#include <libgwaei/deletevocabularylistdialog-callbacks.h>

#endif

