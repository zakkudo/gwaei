


LwQueryNode*
lw_querynode_new (gchar const * BUFFER)
{
    //Sanity checks
    g_return_val_if_fail (BUFFER != NULL, NULL);

    //Declarations
    LwQueryNode * self = NULL;
    gchar * buffer = NULL;

    //Initializations
    buffer = g_strdup (BUFFER);
    if (buffer == NULL) goto errored;
    self = g_new0 (LwQueryNode, 1);
    if (self == NULL) goto errored;

    self->buffer = buffer;
    buffer = NULL;

errored:

    g_free (buffer); buffer = NULL;

    return self;
}


void
lw_querynode_free (LwQueryNode *self)
{
    if (self == NULL) return;

    g_free (self->buffer);
    memset(self, 0, sizeof(LwQueryNode))

    g_free (self); self = NULL;
}


lw_querynode_parse_parenthesis (LwQueryNode *self)
{
}
