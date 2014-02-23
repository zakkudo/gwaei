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
  struct _Row row; 
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
  void (*filename_changed) (LwVocabulary* vocabulary, const gchar* OLD_FILENAME, gpointer data);
};


//Methods
LwVocabulary* lw_vocabulary_new (const gchar* NAME);
GType lw_vocabulary_get_type (void) G_GNUC_CONST;

gchar** lw_vocabulary_get_filenames (void);

void lw_vocabulary_set_changed (LwVocabulary *self, gboolean changed);
gboolean lw_vocabulary_has_changes (LwVocabulary *self);

void lw_vocabulary_set_filename (LwVocabulary *self, const gchar *FILENAME);
const gchar* lw_vocabulary_get_filename (LwVocabulary *self);

void lw_vocabulary_set_loaded (LwVocabulary *self, gboolean loaded);
gboolean lw_vocabulary_is_loaded (LwVocabulary *self);

gboolean lw_vocabulary_rename (LwVocabulary *self, const gchar *FILENAME);

LwWord* lw_vocabulary_get_word_by_index (LwVocabulary *self, gint index);

void lw_vocabulary_load (LwVocabulary *self, LwProgressCallback cb);

gint lw_vocabulary_length (LwVocabulary *self);

gchar* lw_vocabulary_generate_filename (void);
gboolean lw_vocabulary_file_exists (LwVocabulary *self);

gchar* lw_vocabulary_build_uri (const gchar *FILENAME);

G_END_DECLS

#endif
