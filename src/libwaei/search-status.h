
#define LW_SEARCH_STATUS_NAME_UNSTARTED "Unstarted"
#define LW_SEARCH_STATUS_NAME_SEARCHING "Searching"
#define LW_SEARCH_STATUS_NAME_CANCELING "Canceling"
#define LW_SEARCH_STATUS_NAME_CANCELLED "Cancelled"
#define LW_SEARCH_STATUS_NAME_FINISHED "Finished"

#define LW_SEARCH_STATUS_NICK_UNSTARTED "unstarted"
#define LW_SEARCH_STATUS_NICK_SEARCHING "searching"
#define LW_SEARCH_STATUS_NICK_CANCELING "canceling"
#define LW_SEARCH_STATUS_NICK_CANCELLED "cancelled"
#define LW_SEARCH_STATUS_NICK_FINISHED "finished"

//!
//! @brief Search status types
//!
typedef enum {
  LW_SEARCH_STATUS_UNSTARTED,
  LW_SEARCH_STATUS_SEARCHING,
  LW_SEARCH_STATUS_CANCELING,
  LW_SEARCH_STATUS_CANCELLED,
  LW_SEARCH_STATUS_ERRORED,
  LW_SEARCH_STATUS_FINISHED,
} LwSearchStatus;
GType lw_search_status_get_type (void);
#define LW_TYPE_SEARCH_STATUS (lw_search_status_get_type ())
