#ifndef LW_QUERYNODEMATCHINFO_INCLUDED
#define LW_QUERYNODEMATCHINFO_INCLUDED

G_BEGIN_DECLS


/**
 * LwQueryNodeMatchInfo:
 * 
 * An opaque object holding the fields for #LwQueryNodeMatchInfo
 */
struct _LwQueryNodeMatchInfo {
  /*< private >*/
  GTree * tree;
  GList * markers;
  gboolean compiled;
  gint refs;
};
typedef struct _LwQueryNodeMatchInfo LwQueryNodeMatchInfo;


typedef enum {
  LW_QUERYNODEMATCHMARKERTYPE_OPEN,
  LW_QUERYNODEMATCHMARKERTYPE_CLOSE
} LwQueryNodeMatchMarkerType;


struct _LwQueryNodeMatchMarker {
  gchar const * position;
  gchar const * open;
  gchar const * close;
  LwQueryNodeMatchMarkerType type;
  GMatchInfo * match_info;
  gint refs;
};
typedef struct _LwQueryNodeMatchMarker LwQueryNodeMatchMarker;

LwQueryNodeMatchMarker * lw_querynodematchmarker_new (gchar * open, gchar * close, LwQueryNodeMatchMarkerType type, GMatchInfo * match_info);
void lw_querynodematchmarker_unref (LwQueryNodeMatchMarker * self);
LwQueryNodeMatchMarker * lw_querynodematchmarker_ref (LwQueryNodeMatchMarker * self);
void lw_querynodematchinfo_insert (LwQueryNodeMatchInfo * self, gchar const * token, GMatchInfo * match_info);
void lw_querynodematchinfo_compile (LwQueryNodeMatchInfo * self);


/**
 * LW_QUERYNODEMATCHINFO:
 * @obj: Object to cast to a #LwQueryNodeMatchInfo
 *
 * Returns: The object cast to a #LwQueryNodeMatchInfo
 */
#define LW_QUERYNODEMATCHINFO(ob) ((LwQueryNodeMatchInfo*)(obj))

LwQueryNodeMatchInfo * lw_querynodematchinfo_new (void);
void lw_querynodematchinfo_unref (LwQueryNodeMatchInfo * self);
LwQueryNodeMatchInfo * lw_querynodematchinfo_ref (LwQueryNodeMatchInfo * self);


G_END_DECLS

#endif
