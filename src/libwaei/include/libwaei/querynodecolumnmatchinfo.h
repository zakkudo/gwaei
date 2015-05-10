#ifndef LW_QUERYNODECOLUMNMATCHINFO_INCLUDED
#define LW_QUERYNODECOLUMNMATCHINFO_INCLUDED

G_BEGIN_DECLS


/**
 * LwQueryNodeColumnMatchInfoIter:
 *
 * An opaque iterator used by lw_querynodecolumnmatchinfo_read() for reading simple tokens.
 */
struct _LwQueryNodeColumnMatchInfoIter {
  /*< private >*/
  gint match_level;
  gint i;
  GList * marker;
  gchar const * END;
};
typedef struct _LwQueryNodeColumnMatchInfoIter LwQueryNodeColumnMatchInfoIter;


/**
 * LwQueryNodeColumnMatchInfo:
 *
 * An opaque container for storing match information specific to a dictionary column.
 */
struct _LwQueryNodeColumnMatchInfo {
  /*< private >*/
  gint column;
  gchar const ** strv;
  GList * markers;
  gint refs;
};
typedef struct _LwQueryNodeColumnMatchInfo LwQueryNodeColumnMatchInfo;


LwQueryNodeColumnMatchInfo * lw_querynodecolumnmatchinfo_new (gint column, gchar const ** strv);
void lw_querynodecolumnmatchinfo_add (LwQueryNodeColumnMatchInfo * self, GMatchInfo * match_info);
LwQueryNodeColumnMatchInfo* lw_querynodecolumnmatchinfo_ref (LwQueryNodeColumnMatchInfo * self);
void lw_querynodecolumnmatchinfo_unref (LwQueryNodeColumnMatchInfo *self);
gboolean lw_querynodecolumnmatchinfo_read (LwQueryNodeColumnMatchInfo * self, LwQueryNodeColumnMatchInfoIter * iter, gint * i_out, gchar const ** START_OUT, gchar const ** END_OUT, gboolean * is_match_out);

G_END_DECLS

#endif
