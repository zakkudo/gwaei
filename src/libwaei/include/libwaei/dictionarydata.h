#ifndef LW_DICTIONARYDATA_INCLUDED
#define LW_DICTIONARYDATA_INCLUDED

G_BEGIN_DECLS

struct _LwDictionaryData {
    gchar *buffer;
    gchar *checksum;
    LwOffset length;
    gchar *path;
};
typedef struct _LwDictionaryData LwDictionaryData;

LwDictionaryData* lw_dictionarydata_new (void);
void lw_dictionarydata_create (LwDictionaryData *dictionarydata, const gchar *PATH);
const gchar* lw_dictionarydata_get_string (LwDictionaryData *dictionarydata, LwOffset offset);
const gchar* lw_dictionarydata_get_buffer (LwDictionaryData *dictionarydata);
const gchar* lw_dicionarydata_buffer_next (LwDictionaryData *dictionarydata, const gchar *BUFFER);
GList* lw_dictionarydata_get_strings (LwDictionaryData *dictionarydata, GList *offsets);
gint lw_dictionarydata_get_accuracy_weight_delta (LwDictionaryData *dictionarydata, LwOffset offset, LwMorphologyList *morphologylist);
void lw_dictionarydata_free (LwDictionaryData* dictionarydata);
const gchar* lw_dictionarydata_get_checksum (LwDictionaryData *dictionarydata);
LwOffset lw_dictionarydata_get_length (LwDictionaryData *dictionarydata);
const gchar* lw_dictionarydata_buffer_next (LwDictionaryData *dictionarydata, const gchar *BUFFER);
LwOffset lw_dictionarydata_get_offset (LwDictionaryData *dictionarydata, const gchar *BUFFER);

G_END_DECLS

#endif
