#ifndef LW_MORPHOLOGYENGINE_HUNSPELL_INCLUDED
#define LW_MORPHOLOGYENGINE_HUNSPELL_INCLUDED 

GList* lw_morphologyengine_hunspell_analyze (LwMorphologyEngine *engine, const gchar* TEXT, gboolean include_spellcheck);
Hunhandle* lw_morphologyengine_hunspell_new (const gchar*);

#endif

