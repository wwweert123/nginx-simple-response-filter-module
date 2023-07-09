#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_flag_t enable;  
} ngx_http_fred_filter_loc_conf_t;

static void *ngx_http_fred_filter_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_fred_filter_merge_loc_conf(ngx_conf_t *cf,
    void *parent, void *child);
static ngx_int_t ngx_http_fred_filter_init(ngx_conf_t *cf);

static ngx_command_t  ngx_http_fred_filter_commands[] = {

    { ngx_string("upper"),
      NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_fred_filter_loc_conf_t, enable),
      NULL },

      ngx_null_command
};

static ngx_http_module_t  ngx_http_fred_filter_module_ctx = {
    NULL,                               /* proconfiguration */
    ngx_http_fred_filter_init,        /* postconfiguration */

    NULL,                               /* create main configuration */
    NULL,                               /* init main configuration */

    NULL,                               /* create server configuration */
    NULL,                               /* merge server configuration */

    ngx_http_fred_filter_create_loc_conf,    /* create location configuration */
    ngx_http_fred_filter_merge_loc_conf      /* merge location configuration */
};

ngx_module_t  ngx_http_fred_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_fred_filter_module_ctx, /* module context */
    ngx_http_fred_filter_commands,    /* module directives */
    NGX_HTTP_MODULE,                    /* module type */
    NULL,                               /* init master */
    NULL,                               /* init module */
    NULL,                               /* init process */
    NULL,                               /* init thread */
    NULL,                               /* exit thread */
    NULL,                               /* exit process */
    NULL,                               /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_http_output_header_filter_pt ngx_http_next_header_filter;
static ngx_http_output_body_filter_pt   ngx_http_next_body_filter;

// static ngx_int_t
// ngx_http_fred_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
// {
//     ngx_buf_t             *buf;
//     ngx_uint_t             last;
//     ngx_chain_t           *cl, *nl;
//     ngx_http_footer_ctx_t *ctx;

//     ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
//                    "http fred body filter");

//     ctx = ngx_http_get_module_ctx(r, ngx_http_footer_filter_module);
//     if (ctx == NULL) {
//         return ngx_http_next_body_filter(r, in);
//     }

//     last = 0;

//     for (cl = in; cl; cl = cl->next) {
//          if (cl->buf->last_buf) {
//              last = 1;
//              break;
//          }
//     }

//     if (!last) {
//         return ngx_http_next_body_filter(r, in);
//     }

//     buf = ngx_calloc_buf(r->pool);
//     if (buf == NULL) {
//         return NGX_ERROR;
//     }

//     buf->pos = ctx->footer.data;
//     buf->last = buf->pos + ctx->footer.len;
//     buf->start = buf->pos;
//     buf->end = buf->last;
//     buf->last_buf = 1;
//     buf->memory = 1;

//     if (ngx_buf_size(cl->buf) == 0) {
//         cl->buf = buf;
//     } else {
//         nl = ngx_alloc_chain_link(r->pool);
//         if (nl == NULL) {
//             return NGX_ERROR;
//         }

//         nl->buf = buf;
//         nl->next = NULL;
//         cl->next = nl;
//         cl->buf->last_buf = 0;
//     }

//     return ngx_http_next_body_filter(r, in);
// }

static ngx_int_t
ngx_http_fred_header_filter(ngx_http_request_t *r)
{
    // lcf = ngx_http_get_module_loc_conf(r, ngx_http_footer_filter_module);

    // if (r->headers_out.content_length_n != -1) {
    //     r->headers_out.content_length_n += sizeof("<!-- Served by Nginx -->") - 1;
    // }

    // if (r->headers_out.content_length) {
    //     r->headers_out.content_length->hash = 0;
    //     r->headers_out.content_length = NULL;
    // }

    // ngx_http_clear_accept_ranges(r);
    // r->headers_out.content_length_n = -1;
    // if (r->headers_out.content_length) {
    //     r->headers_out.content_length->hash = 0;
    //     r->headers_out.content_length = NULL;
    // }

    // ngx_http_clear_accept_ranges(r);

    return ngx_http_next_header_filter(r);
}

static ngx_int_t
ngx_http_fred_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                    "http fred body filter");

    ngx_http_fred_filter_loc_conf_t *conf;

    if (r->headers_out.status != NGX_HTTP_OK) {
        return ngx_http_next_body_filter(r, in);
    }

    conf = ngx_http_get_module_loc_conf(r, ngx_http_fred_filter_module);

    if (0 == conf->enable) {
        return ngx_http_next_body_filter(r, in);
    }

    // ngx_chain_t *chain_link;
    // int chain_contains_last_buffer = 0;
    u_char *p, c;
    ngx_buf_t* buf;
    
    buf = in-> buf;

    for (p = buf->pos; p < buf->last; p++) {
        c = *p;
        if (c >= 'a' && c <= 'z') {
            *p = ngx_toupper(c);
        }
    }

    // chain_link = in;
    // for ( ; ; ) {
    //     if (chain_link->buf->last_buf)
    //         chain_contains_last_buffer = 1;
    //     if (chain_link->next == NULL)
    //         break;
    //     chain_link = chain_link->next;
    // }

    // if (!chain_contains_last_buffer)
    // return ngx_http_next_body_filter(r, in);

    // ngx_buf_t    *b;
    // b = ngx_calloc_buf(r->pool);
    // if (b == NULL) {
    //     return NGX_ERROR;
    // }

    // b->pos = (u_char *) "<!-- Served by Nginx -->";
    // b->last = b->pos + sizeof("<!-- Served by Nginx -->") - 1;

    // ngx_chain_t   *added_link;

    // added_link = ngx_alloc_chain_link(r->pool);
    // if (added_link == NULL)
    //     return NGX_ERROR;

    // added_link->buf = b;
    // added_link->next = NULL;

    // chain_link->next = added_link;

    // chain_link->buf->last_buf = 0;
    // added_link->buf->last_buf = 1;

    return ngx_http_next_body_filter(r, in);
}

static void *
ngx_http_fred_filter_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_fred_filter_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_fred_filter_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->enable = NGX_CONF_UNSET;

    /*
     * set by ngx_pcalloc():
     *
     *     conf->types = { NULL };
     *     conf->types_keys = NULL;
     *     conf->variable = NULL;
     */

    return conf;
}

static char *
ngx_http_fred_filter_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_fred_filter_loc_conf_t  *prev = parent;
    ngx_http_fred_filter_loc_conf_t  *conf = child;

    // if (ngx_http_merge_types(cf, &conf->types_keys, &conf->types,
    //                          &prev->types_keys,&prev->types,
    //                          ngx_http_html_default_types)
    //     != NGX_OK)
    // {
    //    return NGX_CONF_ERROR;
    // } where did he get this? function 

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_fred_filter_init(ngx_conf_t *cf)
{
    ngx_http_next_body_filter = ngx_http_top_body_filter;
    ngx_http_top_body_filter = ngx_http_fred_body_filter;

    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_fred_header_filter;

    return NGX_OK;
}