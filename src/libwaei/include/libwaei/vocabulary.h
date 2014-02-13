#ifndef LW_VOCABULARY_INCLUDED
#define LW_VOCABULARY_INCLUDED

#include <libwaei/word.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwVocabulary LwVocabulary;
typedef struct _LwVocabularyClass LwVocabularyClass;
typedef struct _LwVocabularyPrivate LwVocabularyPrivate;
typedef struct _LwVocabularyClassPrivate LwVocabularyClassPrivate;

#define LW_TYPE_VOCABULARY              (lw_vocabulary_get_type())
#define LW_VOCABULARY(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_VOCABULARY, LwVocabulary))
#define LW_VOCABULARY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_VOCABULARY, LwVocabularyClass))
#define LW_IS_VOCABULARY(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_VOCABULARY))
#define LW_IS_VOCABULARY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_VOCABULARY))
#define LW_VOCABULARY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_VOCABULARY, LwVocabularyClass))


struct _LwVocabulary {
  GObject object;
  LwVocabularyPrivate *priv;
};

struct _LwVocabularyClass {
  GObjectClass parent_class;
  LwVocabularyClassPrivate *priv;

  //Signals
  void (*row_changed) (LwVocabulary* vocabulary, gint position, gpointer data);
  void (*row_inserted) (LwVocabulary* vocabulary, gint position, gpointer data);
  void (*row_deleted) (LwVocabulary* vocabulary, gint position, gpointer data);
  void (*rows_reordered) (LwVocabulary* vocabulary, gint *order, gpointer data);
};


//Methods
LwVocabulary* lw_vocabulary_new (const gchar* NAME);
GType lw_vocabulary_get_type (void) G_GNUC_CONST;

gchar** lw_vocabulary_get_filenames (void);

void lw_vocabulary_set_changed (LwVocabulary *vocabulary, gboolean changed);
gboolean lw_vocabulary_has_changes (LwVocabulary *vocabulary);

void lw_vocabulary_set_filename (LwVocabulary *vocabulary, const gchar *FILENAME);
const gchar* lw_vocabulary_get_filename (LwVocabulary *vocabulary);

void lw_vocabulary_set_loaded (LwVocabulary *vocabulary, gboolean loaded);
gboolean lw_vocabulary_is_loaded (LwVocabulary *vocabulary);

LwWord* lw_vocabulary_get_word_by_index (LwVocabulary *vocabulary, gint index);

void lw_vocabulary_load (LwVocabulary *vocabulary, LwProgressCallback cb);

gint lw_vocabulary_length (LwVocabulary *vocabulary);

gchar* lgw_vocabulary_generate_filename ();

G_END_DECLS

#endif
