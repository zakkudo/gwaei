#ifndef LGW_ADDVOCABULARYDIALOG_INCLUDED
#define LGW_ADDVOCABULARYDIALOG_INCLUDED

G_BEGIN_DECLS

typedef enum {
  LGW_ADDVOCABULARYDIALOG_RESPONSE_CANCEL,
  LGW_ADDVOCABULARYDIALOG_RESPONSE_ADD,
  TOTAL_LGW_ADDVOCABULARYDIALOG_RESPONSES
} LgwAddVocabularyDialogResponse;

//Boilerplate
typedef struct _LgwAddVocabularyDialog LgwAddVocabularyDialog;
typedef struct _LgwAddVocabularyDialogClass LgwAddVocabularyDialogClass;
typedef struct _LgwAddVocabularyDialogPrivate LgwAddVocabularyDialogPrivate;
typedef struct _LgwAddVocabularyDialogClassPrivate LgwAddVocabularyDialogClassPrivate;

#define LGW_TYPE_ADDVOCABULARYDIALOG              (lgw_addvocabularydialog_get_type())
#define LGW_ADDVOCABULARYDIALOG(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_ADDVOCABULARYDIALOG, LgwAddVocabularyDialog))
#define LGW_ADDVOCABULARYDIALOG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_ADDVOCABULARYDIALOG, LgwAddVocabularyDialogClass))
#define LGW_IS_ADDVOCABULARYDIALOG(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_ADDVOCABULARYDIALOG))
#define LGW_IS_ADDVOCABULARYDIALOG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_ADDVOCABULARYDIALOG))
#define LGW_ADDVOCABULARYDIALOG_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_ADDVOCABULARYDIALOG, LgwAddVocabularyDialogClass))

struct _LgwAddVocabularyDialog {
  GtkDialog self;
  LgwAddVocabularyDialogPrivate *priv;
};

struct _LgwAddVocabularyDialogClass {
  GtkDialogClass parent_class;
  LgwAddVocabularyDialogClassPrivate *priv;
  void (*word_added) (LgwAddVocabularyDialog *dialog);
};

GtkWidget* lgw_addvocabularydialog_new (GtkWindow *window);
GType lgw_addvocabularydialog_get_type (void) G_GNUC_CONST;


const gchar* lgw_addvocabularydialog_get_kanji (LgwAddVocabularyDialog *self);
void lgw_addvocabularydialog_set_kanji (LgwAddVocabularyDialog *self, const gchar *KANJI);

const gchar* lgw_addvocabularydialog_get_reading (LgwAddVocabularyDialog *self);
void lgw_addvocabularydialog_set_reading (LgwAddVocabularyDialog *self, const gchar *READING);

const gchar* lgw_addvocabularydialog_get_definition (LgwAddVocabularyDialog *self);
void lgw_addvocabularydialog_set_definition (LgwAddVocabularyDialog *self, const gchar *DEFINITION);

void lgw_addvocabularydialog_set_liststore (LgwAddVocabularyDialog *self, LgwVocabularyListStore *vocabulary_list_store);
LgwVocabularyListStore* lgw_addvocabularydialog_get_liststore (LgwAddVocabularyDialog *self);

LgwVocabularyWordStore* lgw_addvocabularydialog_get_wordstore (LgwAddVocabularyDialog *self);
void lgw_addvocabularydialog_set_wordstore (LgwAddVocabularyDialog *self, LgwVocabularyWordStore *vocabulary_word_store);

gboolean lgw_addvocabularydialog_get_save_on_add (LgwAddVocabularyDialog *self);
void lgw_addvocabularydialog_set_save_on_add (LgwAddVocabularyDialog *self, gboolean save_on_add);

LwWord* lgw_addvocabularydialog_steal_word (LgwAddVocabularyDialog *self);

G_END_DECLS

#endif

