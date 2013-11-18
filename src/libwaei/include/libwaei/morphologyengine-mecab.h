#ifndef LW_MORPHOLOGYENGINE_MECAB_INCLUDED
#define LW_MORPHOLOGYENGINE_MECAB_INCLUDED 

GList* lw_morphologyengine_mecab_analyze (LwMorphologyEngine*, const gchar*);
mecab_t* lw_morphologyengine_mecab_new (void);

#endif
