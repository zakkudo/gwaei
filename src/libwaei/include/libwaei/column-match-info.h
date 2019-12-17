#ifndef LW_COLUMN_MATCH_INFO_INCLUDED
#define LW_COLUMN_MATCH_INFO_INCLUDED

G_BEGIN_DECLS


/**
 * LwColumnMatchInfoIter:
 *
 * An opaque iterator used by lw_column_match_info_read() for reading simple tokens.
 */
struct _LwColumnMatchInfoIter {
  /*< private >*/
  gint match_level;
  gint i;
  GList * marker;
  gchar const * END;
};
typedef struct _LwColumnMatchInfoIter LwColumnMatchInfoIter;


/**
 * LwColumnMatchInfo:
 *
 * An opaque container for storing match information specific to a dictionary column.
 */
struct _LwColumnMatchInfo {
  /*< private >*/
  gint column;
  gchar const ** strv;
  GList * markers;
  gint refs;
};
typedef struct _LwColumnMatchInfo LwColumnMatchInfo;


LwColumnMatchInfo * lw_column_match_info_new (gint column, gchar const ** strv);
void lw_column_match_info_add (LwColumnMatchInfo * self, GMatchInfo * match_info);
LwColumnMatchInfo* lw_column_match_info_ref (LwColumnMatchInfo * self);
void lw_column_match_info_unref (LwColumnMatchInfo *self);
gint lw_column_match_info_get_column (LwColumnMatchInfo * self);
gboolean lw_column_match_info_read (LwColumnMatchInfo * self, LwColumnMatchInfoIter * iter, gint * i_out, gchar const ** START_OUT, gchar const ** END_OUT, gboolean * is_match_out);

G_END_DECLS

#endif
