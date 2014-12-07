#ifndef LW_VOCABULARY_INCLUDED
#define LW_VOCABULARY_INCLUDED

#include "row.h"
#include "word.h"

G_BEGIN_DECLS

//Preference Schema
#define LW_SCHEMA_VOCABULARY         "org.gnome.gwaei.vocabulary"

//Preference Settings
#define LW_KEY_TRIM_FLASHCARDS       "trim-flashcards"
#define LW_KEY_SHUFFLE_FLASHCARDS    "shuffle-flashcards"
#define LW_KEY_FLASHCARD_DECK_SIZE   "flashcard-deck-size"
#define LW_KEY_TRACK_RESULTS         "track-results"
#define LW_KEY_ORDER                 "order"
#define LW_KEY_POSITION_COLUMN_SHOW  "position-column-show"
#define LW_KEY_SCORE_COLUMN_SHOW     "score-column-show"
#define LW_KEY_TIMESTAMP_COLUMN_SHOW "timestamp-column-show"

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
  Row row; 
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

LwWord* lw_vocabulary_get_word_by_index (LwVocabulary *self, gint index);
gint* lw_vocabulary_get_indices (LwVocabulary *self, GList *words);

void lw_vocabulary_load (LwVocabulary *self);
void lw_vocabulary_save (LwVocabulary *self);

gint lw_vocabulary_length (LwVocabulary *self);

gchar* lw_vocabulary_generate_new_filename (void);
gchar* lw_vocabulary_generate_copied_filename (const gchar *FILENAME);
gboolean lw_vocabulary_has_file (LwVocabulary *self);
gboolean lw_vocabulary_filename_exists (const gchar *FILENAME);

gchar* lw_vocabulary_build_uri (const gchar *FILENAME);

gint* lw_vocabulary_insert (LwVocabulary *self, gint position, GList *wordlist);
GList* lw_vocabulary_remove (LwVocabulary *self, gint *indices);
void lw_vocabulary_remove_words (LwVocabulary *self, GList *words);

LwWord* lw_vocabulary_nth (LwVocabulary *self, gint index);

gchar* lw_vocabulary_to_string (LwVocabulary *self);
gchar* lw_vocabulary_load_from_string (LwVocabulary *self, const gchar *TEXT, gboolean take_filename_from_text);

void lw_vocabulary_set (LwVocabulary *self, gint position, ...);
void lw_vocabulary_set_valist (LwVocabulary *self, gint position, va_list va);

void lw_vocabulary_sort (LwVocabulary *self, GCompareDataFunc compare_func);

G_END_DECLS

#endif
