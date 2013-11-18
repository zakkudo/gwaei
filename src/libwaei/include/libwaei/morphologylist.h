#ifndef LW_MORPHOLOGYLIST_INCLUDED
#define LW_MORPHOLOGYLIST_INCLUDED 

#include <glib.h>

G_BEGIN_DECLS

struct _LwMorphologyList {
    GList *list;
    GList *link;
};
typedef struct _LwMorphologyList LwMorphologyList;


LwMorphologyList* lw_morphologylist_new_from_list (GList *list);
void  lw_morphologylist_free (LwMorphologyList *morphologylist);

void lw_morphologylist_rewind (LwMorphologyList *morphologylist);
LwMorphology* lw_morphologylist_read (LwMorphologyList *morphologylist);

LwMorphology* lw_morphologylist_find (LwMorphologyList *morphologylist, const gchar* WORD);
gchar* lw_morphologylist_to_string (LwMorphologyList *morphologylist);

gchar const** lw_morphologylist_get_words (LwMorphologyList *morphologylist);
gchar const** lw_morphologylist_get_normalized (LwMorphologyList *morphologylist);
gchar const** lw_morphologylist_get_stems (LwMorphologyList *morphologylist);
gchar const** lw_morphologylist_get_canonical (LwMorphologyList *morphologylist);

gint lw_morphologylist_length (LwMorphologyList *morphologylist);

GList* lw_morphologylist_get_morphologyindexlist (LwMorphologyList *morphologylist, const gchar *HAYSTACK); 
gint lw_morphologylist_get_score (LwMorphologyList *morphologylist, const gchar *HAYSTACK);

G_END_DECLS

#endif
