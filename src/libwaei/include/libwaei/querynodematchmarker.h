#ifndef LW_QUERYNODEMATCHMARKER_INCLUDED
#define LW_QUERYNODEMATCHMARKER_INCLUDED

G_BEGIN_DECLS

/**
 * LwQueryNodeMatchMarkerType:
 * @LW_QUERYNODEMATCHMARKERTYPE_OPEN: The first character matching in a selection
 * @LW_QUERYNODEMATCHMARKERTYPE_CLOSE: The last character matching in a selection
 */
typedef enum {
  LW_QUERYNODEMATCHMARKERTYPE_OPEN,
  LW_QUERYNODEMATCHMARKERTYPE_CLOSE
} LwQueryNodeMatchMarkerType;

/**
 * LwQueryNodeMatchMarker:
 * @position: The character position of this marker.  If the marker is of type #LW_QUERYNODEMATCHMARKERTYPE_OPEN it will be the same as @open. If the marker is of type #LW_QUERYNODEMATCHMARKERTYPE_CLOSE, it will be the same as @close.
 * @open: The character marking the start of this selection
 * @close: The character marking the end of this selection
 * @type: Denotes if this object marks the start or end of a selection. @position will reflect the type.
 * @match_info: The #GMatchInfo used to calculate this marker selection.  You can use this to get the original search pattern from the #GRegex
 * @refs: The current refcount
 */
struct _LwQueryNodeMatchMarker {
  gchar const * POSITION;
  gchar const * OPEN;
  gchar const * CLOSE;
  LwQueryNodeMatchMarkerType type;
  GMatchInfo * match_info;
  gint refs;
};
typedef struct _LwQueryNodeMatchMarker LwQueryNodeMatchMarker;

#define LW_QUERYNODEMATCHMARKER(obj) ((LwQueryNodeMatchMarker*)(obj))

LwQueryNodeMatchMarker * lw_querynodematchmarker_new (LwQueryNodeMatchMarkerType type, GMatchInfo * match_info);

void lw_querynodematchmarker_unref (LwQueryNodeMatchMarker * self);
LwQueryNodeMatchMarker * lw_querynodematchmarker_ref (LwQueryNodeMatchMarker * self);


gchar const * lw_querynodematchmarker_get_position (LwQueryNodeMatchMarker * self, LwQueryNodeMatchMarkerType * type);
GMatchInfo * lw_querynodematchmarker_get_match_info (LwQueryNodeMatchMarker * self);
GRegex * lw_querynodematchmarker_get_regex (LwQueryNodeMatchMarker * self);
gchar const * lw_querynodematchmarker_get_string (LwQueryNodeMatchMarker * self);

G_END_DECLS

#endif
