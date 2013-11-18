#ifndef LW_MORPHOLOGYINDEX_INCLUDED
#define LW_MORPHOLOGYINDEX_INCLUDED 


struct LwMorphologyIndex {
  gint index;
  const gchar *BUFFER;
  gint start_offset;
  gint end_offset;
}; 
typedef struct LwMorphologyIndex LwMorphologyIndex;


struct LwMorphologyIndexList {
  const gchar *BUFFER;
  GList *sectionlist;
  GList *indexlist;
};
typedef struct LwMorphologyIndexList LwMorphologyIndexList;


LwMorphologyIndex* lw_morphologyindex_new (gint index, const gchar *BUFFER, gint start_offset, gint end_offset);
void lw_morphologyindex_free (LwMorphologyIndex *morphologyindex);

gint lw_morphologyindexlist_get_accuracy_weight (LwMorphologyIndexList *morphologyindexlist);

LwMorphologyIndexList* lw_morphologyindexlist_new (const gchar *HAYSTACK);
void lw_morphologyindexlist_free (LwMorphologyIndexList *morphologyindexlist);

#endif
