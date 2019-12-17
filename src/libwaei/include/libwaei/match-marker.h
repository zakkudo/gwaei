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
 * @TOKEN: The full string that this marker references
 * @POSITION: The character position of this marker.  If the marker is of type #LW_QUERYNODEMATCHMARKERTYPE_OPEN it will be the same as @open. If the marker is of type #LW_QUERYNODEMATCHMARKERTYPE_CLOSE, it will be the same as @close.
 * @OPEN: The character marking the start of this selection
 * @CLOSE: The character marking the end of this selection
 * @type: Denotes if this object marks the start or end of a selection. @position will reflect the type.
 * @regex: The regex used to calculate the OPEN and CLOSE values
 * @refs: The current refcount
 */
struct _LwQueryNodeMatchMarker {
  gchar const * TOKEN;
  gchar const * POSITION;
  gchar const * OPEN;
  gchar const * CLOSE;
  LwQueryNodeMatchMarkerType type;
  GRegex * regex;
  gint refs;
};
typedef struct _LwQueryNodeMatchMarker LwQueryNodeMatchMarker;


/**
 * LW_QUERYNODEMATCHMARKER:
 * @obj: Object to cast to a #LwQueryNodeMatchMarker
 *
 * Returns: The object cast to a #LwQueryNodeMatchMarker
 */
#define LW_QUERYNODEMATCHMARKER(obj) ((LwQueryNodeMatchMarker*)(obj))

LwQueryNodeMatchMarker * lw_querynodematchmarker_new (LwQueryNodeMatchMarkerType type, GMatchInfo * match_info);

void lw_querynodematchmarker_unref (LwQueryNodeMatchMarker * self);
LwQueryNodeMatchMarker * lw_querynodematchmarker_ref (LwQueryNodeMatchMarker * self);


gchar const * lw_querynodematchmarker_get_position (LwQueryNodeMatchMarker * self, LwQueryNodeMatchMarkerType * type);
GRegex * lw_querynodematchmarker_get_regex (LwQueryNodeMatchMarker * self);
gchar const * lw_querynodematchmarker_get_string (LwQueryNodeMatchMarker * self);

G_END_DECLS

#endif
