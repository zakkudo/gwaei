#ifndef LGW_ADDVOCABULARYDIALOG_PRIVATE_INCLUDED
#define LGW_ADDVOCABULARYDIALOG_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    SIGNALID_KANJI_CHANGED,
    SIGNALID_READING_CHANGED,
    SIGNALID_DEFINITION_CHANGED,
    SIGNALID_DEFINITION_FOCUS_IN_EVENT,
    SIGNALID_DEFINITION_FOCUS_OUT_EVENT,
    SIGNALID_RESPONSE,
    TOTAL_SIGNALIDS
} SignalId;


typedef enum {
    PROP_0,
    PROP_KANJI,
    PROP_READING,
    PROP_DEFINITION,
    PROP_VOCABULARYLISTSTORE,
    PROP_VOCABULARYWORDSTORE,
    PROP_SAVE_ON_ADD,
    TOTAL_PROPS
} Props;

struct _Definition {
  GtkTextBuffer *placeholder_text_buffer;
  GtkTextBuffer *text_buffer;
};

struct _Store {
  LgwVocabularyListStore *vocabulary_list;
  LgwVocabularyWordStore *vocabulary_word;
};

struct _UI {
  GtkComboBox *combo_box;
  GtkEntry *kanji_entry;
  GtkEntry *reading_entry;
  GtkTextView *definition_text_view;
  GtkButton *add_button;
  GtkButton *cancel_button;
  GtkGrid *grid;
  GtkBox *layout_box;
};

struct _Data {
  LwWord *word;
  struct _Store store;
  struct _Definition definition;
  guint signalid[TOTAL_SIGNALIDS];
};

struct _Config {
  gboolean save_on_add;
};

struct _LgwAddVocabularyDialogPrivate {
  struct _UI ui;
  struct _Data data;
  struct _Config config;
};

struct _LgwAddVocabularyDialogClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  gchar *last_selected_list_name;
};

#define LGW_ADDVOCABULARYDIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_ADDVOCABULARYDIALOG, LgwAddVocabularyDialogPrivate));

void lgw_addvocabularydialog_sync_definition_text_buffer (LgwAddVocabularyDialog *self);
void lgw_addvocabularydialog_sync_list_store (LgwAddVocabularyDialog *self);

G_END_DECLS

#include <libgwaei/addvocabularydialog-callbacks.h>

#endif

