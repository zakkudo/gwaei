#ifndef LGW_DELETEVOCABULARYLISTDIALOG_INCLUDED
#define LGW_DELETEVOCABULARYLISTDIALOG_INCLUDED

G_BEGIN_DECLS

typedef enum {
  LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_CANCEL,
  LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_DELETE,
  TOTAL_LGW_DELETEVOCABULARYLISTDIALOG_RESPONSES
} LgwDeleteVocabularyListDialogResponse;

//Boilerplate
typedef struct _LgwDeleteVocabularyListDialog LgwDeleteVocabularyListDialog;
typedef struct _LgwDeleteVocabularyListDialogClass LgwDeleteVocabularyListDialogClass;
typedef struct _LgwDeleteVocabularyListDialogPrivate LgwDeleteVocabularyListDialogPrivate;
typedef struct _LgwDeleteVocabularyListDialogClassPrivate LgwDeleteVocabularyListDialogClassPrivate;

#define LGW_TYPE_DELETEVOCABULARYLISTDIALOG              (lgw_deletevocabularylistdialog_get_type())
#define LGW_DELETEVOCABULARYLISTDIALOG(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_DELETEVOCABULARYLISTDIALOG, LgwDeleteVocabularyListDialog))
#define LGW_DELETEVOCABULARYLISTDIALOG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_DELETEVOCABULARYLISTDIALOG, LgwDeleteVocabularyListDialogClass))
#define LGW_IS_DELETEVOCABULARYLISTDIALOG(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_DELETEVOCABULARYLISTDIALOG))
#define LGW_IS_DELETEVOCABULARYLISTDIALOG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_DELETEVOCABULARYLISTDIALOG))
#define LGW_DELETEVOCABULARYLISTDIALOG_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_DELETEVOCABULARYLISTDIALOG, LgwDeleteVocabularyListDialogClass))

struct _LgwDeleteVocabularyListDialog {
  GtkDialog self;
  LgwDeleteVocabularyListDialogPrivate *priv;
};

struct _LgwDeleteVocabularyListDialogClass {
  GtkDialogClass parent_class;
  LgwDeleteVocabularyListDialogClassPrivate *priv;
  void (*word_added) (LgwDeleteVocabularyListDialog *dialog);
};

GtkWidget* lgw_deletevocabularylistdialog_new (GtkWindow *window);
GType lgw_deletevocabularylistdialog_get_type (void) G_GNUC_CONST;

LgwVocabularyListStore* lgw_deletevocabularylistdialog_get_liststore (LgwDeleteVocabularyListDialog *self);
void lgw_deletevocabularylistdialog_set_liststore (LgwDeleteVocabularyListDialog *self, LgwVocabularyListStore *vocabulary_list_store);

gboolean lgw_deletevocabularylistdialog_get_delete_on_response (LgwDeleteVocabularyListDialog *self);
void lgw_deletevocabularylistdialog_set_delete_on_response (LgwDeleteVocabularyListDialog *self, gboolean delete_on_response);


G_END_DECLS

#endif

