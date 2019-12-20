#ifndef LW_MATCH_MARKER_INCLUDED
#define LW_MATCH_MARKER_INCLUDED

G_BEGIN_DECLS

/**
 * LwMatchMarkerType:
 * @LW_MATCH_MARKER_TYPE_OPEN: The first character matching in a selection
 * @LW_MATCH_MARKER_TYPE_CLOSE: The last character matching in a selection
 */
typedef enum {
  LW_MATCH_MARKER_TYPE_OPEN,
  LW_MATCH_MARKER_TYPE_CLOSE
} LwMatchMarkerType;

/**
 * LwMatchMarker:
 * @TOKEN: The full string that this marker references
 * @POSITION: The character position of this marker.  If the marker is of type #LW_MATCH_MARKER_TYPE_OPEN it will be the same as @open. If the marker is of type #LW_MATCH_MARKER_TYPE_CLOSE, it will be the same as @close.
 * @OPEN: The character marking the start of this selection
 * @CLOSE: The character marking the end of this selection
 * @type: Denotes if this object marks the start or end of a selection. @position will reflect the type.
 * @regex: The regex used to calculate the OPEN and CLOSE values
 * @refs: The current refcount
 */
struct _LwMatchMarker {
  gchar const * TOKEN;
  gchar const * POSITION;
  gchar const * OPEN;
  gchar const * CLOSE;
  LwMatchMarkerType type;
  GRegex * regex;
  gint refs;
};
typedef struct _LwMatchMarker LwMatchMarker;


/**
 * LW_MATCH_MARKER:
 * @obj: Object to cast to a #LwMatchMarker
 *
 * Returns: The object cast to a #LwMatchMarker
 */
#define LW_MATCH_MARKER(obj) ((LwMatchMarker*)(obj))

LwMatchMarker * lw_match_marker_new (LwMatchMarkerType type, GMatchInfo * match_info);

void lw_match_marker_unref (LwMatchMarker * self);
LwMatchMarker * lw_match_marker_ref (LwMatchMarker * self);


gchar const * lw_match_marker_get_position (LwMatchMarker * self, LwMatchMarkerType * type);
GRegex * lw_match_marker_get_regex (LwMatchMarker * self);
gchar const * lw_match_marker_get_string (LwMatchMarker * self);

G_END_DECLS

#endif
