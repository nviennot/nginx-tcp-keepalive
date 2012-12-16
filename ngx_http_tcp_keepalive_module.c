/*
 * Copyright (C) 2011 Nicolas Viennot <nicolas@viennot.biz>
 *
 * This file is subject to the terms and conditions of the MIT License.
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <sys/socket.h>

extern ngx_module_t ngx_http_tcp_keepalive_module;

typedef struct {
	ngx_flag_t	enable;
	ngx_uint_t	tcp_keepcnt;
	time_t		tcp_keepidle;
	time_t		tcp_keepintvl;
} ngx_http_tcp_keepalive_conf_t;

static ngx_int_t
ngx_http_tcp_keepalive_handler(ngx_http_request_t *r)
{
	ngx_http_tcp_keepalive_conf_t *conf;
	int fd = r->connection->fd;

	conf = ngx_http_get_module_loc_conf(r, ngx_http_tcp_keepalive_module);
	if (!conf->enable)
		return NGX_DECLINED;

#define SSO(level, optname, val) ({					\
	if (setsockopt(fd, level, optname, &(val), sizeof(int)) < 0) {	\
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,	\
			      "setsockopt(" #level ", " #optname ", %d) failed", val);	\
		return NGX_HTTP_INTERNAL_SERVER_ERROR;			\
	}								\
})
	SSO(SOL_SOCKET, SO_KEEPALIVE, conf->enable);
#ifdef NGX_DARWIN
	SSO(IPPROTO_TCP, TCP_KEEPALIVE, conf->tcp_keepidle);
#else
	SSO(IPPROTO_TCP, TCP_KEEPCNT, conf->tcp_keepcnt);
	SSO(IPPROTO_TCP, TCP_KEEPIDLE, conf->tcp_keepidle);
	SSO(IPPROTO_TCP, TCP_KEEPINTVL, conf->tcp_keepintvl);
#endif
#undef SSO

	return NGX_DECLINED;
}

static ngx_int_t
ngx_http_tcp_keepalive_init(ngx_conf_t *cf)
{
	ngx_http_handler_pt *h;
	ngx_http_core_main_conf_t *cmcf;

	cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

	h = ngx_array_push(&cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers);
	if (h == NULL)
		return NGX_ERROR;

	*h = ngx_http_tcp_keepalive_handler;

	return NGX_OK;
}

static void *
ngx_http_tcp_keepalive_create_loc_conf(ngx_conf_t *cf)
{
	ngx_http_tcp_keepalive_conf_t *conf;

	conf = ngx_palloc(cf->pool, sizeof(*conf));
	if (conf == NULL)
		return NULL;

	conf->enable = NGX_CONF_UNSET;
	conf->tcp_keepcnt = NGX_CONF_UNSET_UINT;
	conf->tcp_keepidle = NGX_CONF_UNSET;
	conf->tcp_keepintvl = NGX_CONF_UNSET;

	return conf;
}

static char *
ngx_http_tcp_keepalive_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
	ngx_http_tcp_keepalive_conf_t *prev = parent;
	ngx_http_tcp_keepalive_conf_t *conf = child;

	ngx_conf_merge_value(conf->enable, prev->enable, 0);
	ngx_conf_merge_uint_value(conf->tcp_keepcnt, prev->tcp_keepcnt, 2);
	ngx_conf_merge_value(conf->tcp_keepidle, prev->tcp_keepidle, 60);
	ngx_conf_merge_value(conf->tcp_keepintvl, prev->tcp_keepintvl, 60);

	return NGX_CONF_OK;
}

static ngx_command_t ngx_http_tcp_keepalive_commands[] = {
	{
		ngx_string("tcp_keepalive"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
		ngx_conf_set_flag_slot,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_tcp_keepalive_conf_t, enable),
		NULL
	},
	{
		ngx_string("tcp_keepcnt"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
		ngx_conf_set_num_slot,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_tcp_keepalive_conf_t, tcp_keepcnt),
		NULL
	},
	{
		ngx_string("tcp_keepidle"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
		ngx_conf_set_sec_slot,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_tcp_keepalive_conf_t, tcp_keepidle),
		NULL
	},
	{
		ngx_string("tcp_keepintvl"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
		ngx_conf_set_sec_slot,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_tcp_keepalive_conf_t, tcp_keepintvl),
		NULL
	},
	ngx_null_command
};

static ngx_http_module_t  ngx_http_tcp_keepalive_module_ctx = {
	NULL,					/* preconfiguration */
	ngx_http_tcp_keepalive_init,		/* postconfiguration */

	NULL,					/* create main configuration */
	NULL,					/* init main configuration */

	NULL,					/* create server configuration */
	NULL,					/* merge server configuration */

	ngx_http_tcp_keepalive_create_loc_conf,	/* create location configuration */
	ngx_http_tcp_keepalive_merge_loc_conf 	/* merge location configuration */
};

ngx_module_t ngx_http_tcp_keepalive_module = {
	NGX_MODULE_V1,
	&ngx_http_tcp_keepalive_module_ctx,	/* module context */
	ngx_http_tcp_keepalive_commands,	/* module directives */ 
	NGX_HTTP_MODULE,			/* module type */
	NULL,					/* init master */
	NULL,					/* init module */
	NULL,					/* init process */
	NULL,					/* init thread */
	NULL,					/* exit thread */
	NULL,					/* exit process */
	NULL,					/* exit master */
	NGX_MODULE_V1_PADDING
};
