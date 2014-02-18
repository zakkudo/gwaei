#ifndef LGW_ADDVOCABULARYDIALOG_PRIVATE_INCLUDED
#define LGW_ADDVOCABULARYDIALOG_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    UNUSED_SIGNALID,
    TOTAL_SIGNALIDS
} SignalId;


typedef enum {
  CLASS_SIGNALID_WORD_ADDED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;


typedef enum {
    PROP_KANJI,
    PROP_READING,
    PROP_DEFINITION,
    PROP_VOCABULARYLISTSTORE,
    TOTAL_PROPS
} Props;

struct _UI {
  GtkComboBox *combobox;
  GtkEntry *kanji_entry;
  GtkEntry *reading_entry;
  GtkEntry *definition_entry;
  GtkButton *add_button;
  GtkButton *cancel_button;
};

struct _Data {
  LgwVocabularyListStore *vocabulary_list_store;
  LwWord *word;
  guint signalid[TOTAL_SIGNALIDS];
};

struct _Config {
};

struct _LgwAddVocabularyDialogPrivate {
  struct _UI ui;
  struct _Data data;
  struct _Config config;
};

struct _LgwAddVocabularyDialogClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  gchar *last_selected_list_name;
};

#define LGW_ADDVOCABULARYDIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_ADDVOCABULARYDIALOG, LgwAddVocabularyDialogPrivate));

G_END_DECLS

#include <libgwaei/addvocabularydialog-callbacks.h>

#endif

