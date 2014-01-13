#ifndef LGW_VOCABULARYLISTSTORE_ROW_INCLUDED
#define LGW_VOCABULARYLISTSTORE_ROW_INCLUDED

G_BEGIN_DECLS


struct _LgwVocabularyListStoreRow {
    gchar *filename;
    LgwVocabularyWordStore *vocabulary_word_store;
};


LgwVocabularyListStoreRow* lgw_vocabularyliststorerow_new (void);
void lgw_vocabularyliststorerow_free (LgwVocabularyListStoreRow *row);

void lgw_vocabularyliststorerow_get_wordstore (LgwVocabularyListStoreRow *row);

G_END_DECLS

#endif

