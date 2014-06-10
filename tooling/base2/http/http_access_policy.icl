<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 -->
<class
    name    = "http_access_policy"
    comment = "HTTP access policy module"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    Applies access control policies to the HTTP request.
</doc>

<inherit class = "http_access_module_back" />

<import class = "http" />

<method name = "announce">
    smt_log_print (log, "I: X5 access policy module loaded");
</method>

<method name = "before">
    <doc>
    Checks the request against the defined access policies.  If the policies
    decided a firm allow/deny, returns 1.  For a deny, prepares the response
    object accordingly.

    Sets the response accordingly, and returns 1 if the request was allowed
    or denied, and -1 if the request is still in progress.  In that case the
    calling thread will receive a deny or allow method at some later time.
    </doc>
    <local>
    ipr_config_t
        *config;
    Bool
        decision = FALSE;
    </local>
    //
    //  Get a threadsafe handle of the config for our work here
    config = ipr_config_dup (http_config_table (http_config));
    s_trace (context, "P: starting policy check on request '%s'", context->request->request_line);

    //  Execute all policies except the default
    ipr_config_locate (config, "/config/access/policy", NULL);
    while (config->located && !decision) {
        if (strneq (ipr_config_get (config, "name", ""), "default")) {
            s_trace (context, "P: executing '%s' policy", ipr_config_get (config, "name", ""));
            decision = s_execute_policy (context, config);
        }
        ipr_config_next (config);
    }
    //  If no decision, execute the default policy
    if (!decision) {
        ipr_config_locate (config, "/config/access/policy", "default");
        if (config->located) {
            s_trace (context, "P: no decision - executing default policy");
            decision = s_execute_policy (context, config);
        }
    }
    //  If still decision, deny the URI
    if (decision)
        s_trace (context, "P: ending policy check, reply_code=%d", context->response->reply_code);
    else {
        s_trace (context, "P: no decision - denying access to resource");
        http_response_set_error (context->response, HTTP_REPLY_FORBIDDEN, "No access policy");
    }
    //  Drop our handle to the config
    ipr_config_destroy (&config);
</method>

<private name = "header">
static void
s_trace (http_access_context_t *context, char *format, ...);
static Bool
s_execute_policy (http_access_context_t *context, ipr_config_t *config);
static int
s_execute_rule (http_access_context_t *context, ipr_config_t *config);
static int
s_execute_actions (http_access_context_t *context, ipr_config_t *config);
static int
s_execute_authenticate (http_access_context_t *context, ipr_config_t *config);
</private>

<private name = "footer">
//  Traces a policy execution step, if the config asks for it
static void
s_trace (http_access_context_t *context, char *format, ...)
{
    icl_shortstr_t
        log_line;
    va_list
        va;
    va_start (va, format);
    if (http_config_policy_trace (http_config)) {
        apr_vsnprintf (log_line, ICL_SHORTSTR_MAX, format, va);
        smt_log_print (context->debug_log, log_line);
    }
    va_end (va);
}

//  Executes a policy, returns TRUE if decision was taken, else FALSE.
//  If the policy decided to deny, prepares the response object.
//
static Bool
s_execute_policy (http_access_context_t *context, ipr_config_t *config)
{
    Bool
        decision = FALSE;
    char
        *item_name = ipr_config_item (config);

    if (streq (item_name, "policy")) {
        char
            *uri = ipr_config_get (config, "uri", NULL);
        //  Check if policy uri matches request uri
        if (uri == NULL || ipr_str_prefixed (context->request->path, uri)) {
            ipr_config_locate (config, "+", NULL);
            if (config->located) {
                while (config->located && !decision) {
                    decision = s_execute_rule (context, config);
                    ipr_config_next (config);
                }
                ipr_config_locate (config, "..", NULL);
            }
        }
        else
            s_trace (context, "P: policy uri '%s' does not match - skipping", uri);
    }
    else {
        icl_system_panic ("$(selfname)",
            "E: invalid access config, '%s' not expected (abort)",
            ipr_config_item (config));
        exit (EXIT_FAILURE);
    }
    //  Rules can return 0 or -1 (skip), neither are a policy decision
    return (decision == 1);
}

//  Executes a rule within the policy.
//  Returns 1 if decision was taken, 0 if not, -1 to skip rest of policy.
//  If the rule decided to deny, prepares the response object.
//
//    blacklist - check if the IP address of the client application is blacklisted
//    from value = "pattern" - check the IP address of the client application
//    header name = "name" value = "pattern" - check the value of a specific request header
//    local - check if the referrer is the current host
//    detect value = "string" comment = "text" - check if the request contains string
//    group value = "pattern" - check if the user is authenticated and in some group
//    always - always execute the actions
//    default - executes the actions for any matching rule that has //no// actions

static int
s_execute_rule (http_access_context_t *context, ipr_config_t *config)
{
    int
        decision = 0;
    char
        *rule = ipr_config_item (config);

    s_trace (context, "P: executing rule '%s'", rule);
    if (streq (rule, "blacklist")) {
        ipr_dict_t
            *entry;
        entry = ipr_dict_table_search (context->blacklist, context->request->address);
        if (entry) {
            s_trace (context, "P: rule match: address '%s' found in blacklist", context->request->address);
            decision = s_execute_actions (context, config);
        }
    }
    else
    if (streq (rule, "from")) {
        char
            *value = ipr_config_get (config, "value", NULL);
        if (value
        && (ipr_str_matches (context->request->address, value)
        ||  ipr_net_cidr_eq (context->request->address, value))) {
            s_trace (context, "P: rule match: address '%s' matches '%s'", context->request->address, value);
            decision = s_execute_actions (context, config);
        }
    }
    else
    if (streq (rule, "header")) {
        char
            *name = ipr_config_get (config, "name", NULL),
            *value = ipr_config_get (config, "value", NULL);
        if (name && value
        && ipr_str_lex_matches (http_request_get_header (context->request, name), value)) {
            s_trace (context, "P: rule match: header '%s=%s' matches '%s'",
                name, http_request_get_header (context->request, name), value);
            decision = s_execute_actions (context, config);
        }
    }
    else
    if (streq (rule, "local")) {
        char
            *referer = http_request_get_header (context->request, "referer");
        if (strnull (referer) || streq (referer, context->response->root_uri)) {
            s_trace (context, "P: rule match: referrer '%s' considered local", referer);
            decision = s_execute_actions (context, config);
        }
    }
    else
    if (streq (rule, "detect")) {
        char
            *limit = ipr_config_get (config, "limit", NULL),
            *value = ipr_config_get (config, "value", NULL);
        if (value
        &&  ipr_str_find (context->request->request_line, value)) {
            s_trace (context, "P: rule match: request contains '%s'", value);
            apr_env_set ("comment", ipr_config_get (config, "comment", ""), icl_global_pool);
            decision = s_execute_actions (context, config);
        }
        if (!decision
        &&  limit
        &&  strlen (context->request->request_line) > (uint) atoi (limit)) {
            s_trace (context, "P: rule match: request exceeds %d chars", atoi (limit));
            apr_env_set ("comment", ipr_config_get (config, "comment", ""), icl_global_pool);
            decision = s_execute_actions (context, config);
        }
    }
    else
    if (streq (rule, "group")) {
        char
            *value = ipr_config_get (config, "value", NULL);
        if (value && context->auth_group
        &&  ipr_str_lex_matches (context->auth_group, value)) {
            s_trace (context, "P: rule match: user is in group '%s'", value);
            decision = s_execute_actions (context, config);
        }
    }
    else
    if (streq (rule, "always")) {
        s_trace (context, "P: rule match: always");
            decision = s_execute_actions (context, config);
    }
    else
    if (streq (rule, "default"))
        ;                               //  Do nothing
    else {
        icl_system_panic ("$(selfname)",
            "E: invalid policy rule: '%s' (abort)", rule);
        exit (EXIT_FAILURE);
    }
    return (decision);
}

//  Executes action list for current rule
//  Returns 1 if decision was taken, 0 if not, -1 to skip rest of policy.
//  If the action decided to deny, prepares the response object.
//    deny [ code = "reply-code" [ text = "reply-text" ] ]
//    allow
//    authenticate mechanism = "basic|digest" [ realm = "realmname" ]
//    redirect uri = "uri"
//    ban
//    skip
//    echo Message text

static int
s_execute_actions (http_access_context_t *context, ipr_config_t *config)
{
    int
        decision = 0;
    char
        *action;

    //  Push local copy of config state
    config = ipr_config_dup (config);

    //  Look for first action, if none then look in default rule
    ipr_config_locate (config, "+", NULL);
    if (!config->located) {
        ipr_config_locate (config, "../default", NULL);
        ipr_config_locate (config, "+", NULL);
    }
    while (config->located && !decision) {
        action = ipr_config_item (config);
        s_trace (context, "P: execute action '%s'", action);

        if (streq (action, "deny")) {
            http_response_set_error (context->response,
                atoi (ipr_config_get (config, "code", "503")),
                ipr_config_get (config, "text", "Access denied"));
            decision = 1;
        }
        else
        if (streq (action, "allow"))
            decision = 1;
        else
        if (streq (action, "authenticate"))
            decision = s_execute_authenticate (context, config);
        else
        if (streq (action, "redirect")) {
            http_response_set_redirect (context->response, ipr_config_get (config, "uri", "/"));
            decision = 1;
        }
        else
        if (streq (action, "ban")) {
            http_server_ban_address (context->response->server, context->request->address);
            http_response_set_error (context->response,
                atoi (ipr_config_get (config, "code", "503")),
                ipr_config_get (config, "text", "Access denied"));
            decision = 1;
        }
        else
        if (streq (action, "skip"))
            decision = -1;
        else
        if (streq (action, "echo")) {
            char
                *formatted = http_access_context_format (context, ipr_config_value (config));
            smt_log_print (context->alert_log, "%s", formatted);
            icl_mem_free (formatted);
        }
        else {
            icl_system_panic ("$(selfname)",
                "E: invalid rule action: '%s' (abort)", action);
            exit (EXIT_FAILURE);
        }
        ipr_config_next (config);
    }
    ipr_config_destroy (&config);
    return (decision);
}

//  All the heavy work is done by ipr_password_table, which implements the
//  Basic and Digest authentication mechanisms.

static int
s_execute_authenticate (http_access_context_t *context, ipr_config_t *config)
{
    int
        decision = 0;
    char
        *mechanism,
        *realm;
    char
        *credentials,           //  Authorization header
        *challenge;             //  Response from authentication

    mechanism = ipr_config_get (config, "mechanism", "basic"),
    realm = ipr_config_get (config, "realm", context->response->hostname);
    credentials = http_request_get_header (context->request, "authorization");
    if (streq (mechanism, "basic")) {
        if (http_config_policy_trace (http_config))
            ipr_password_table_set_debug (context->basic, TRUE);
        challenge = ipr_password_table_basic (
            context->basic,
            context->request->address,
            realm,
            credentials);
        if (context->basic->result == IPR_PASSWORD_OK) {
            http_access_context_set_auth (context, context->basic, "users");
            s_trace (context, "P: successful Basic authentication for '%s'",
                context->auth_user);
        }
        else
            http_access_context_set_fail (context, context->basic);
    }
    else
    if (streq (mechanism, "digest")) {
        if (http_config_policy_trace (http_config)) {
            ipr_password_table_set_debug (context->digest, TRUE);
            //  If we're debugging, use the RFC2617 example nonce
            ipr_password_table_set_nonce_test (context->digest, "dcd98b7102dd2f0e8b11d0f600bfb0c093");
        }
        context->digest_auth = TRUE;
        challenge = ipr_password_table_digest (
            context->digest,
            context->request->address,
            realm,
            credentials,
            context->request->request_method,
            context->request->path);
        if (context->digest->result == IPR_PASSWORD_OK) {
            http_access_context_set_auth (context, context->digest, "users");
            s_trace (context, "P: successful Digest authentication for '%s'",
                context->auth_user);
        }
        else
            http_access_context_set_fail (context, context->digest);
    }
    else {
        icl_system_panic ("$(selfname)",
            "E: invalid authentication mechanism: '%s' (abort)", mechanism);
        exit (EXIT_FAILURE);
    }
    if (challenge) {
        http_response_set_header (context->response, "www-authenticate", challenge);
        http_response_set_error  (context->response, HTTP_REPLY_UNAUTHORIZED,
            "Authenticate yourself to access this resource");
        icl_mem_strfree (&challenge);
        decision = 1;                       //  Deny access
    }
    return (decision);
}
</private>

<method name = "selftest" />

</class>
