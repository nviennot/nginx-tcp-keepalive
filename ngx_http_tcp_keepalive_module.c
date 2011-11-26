/*
 * Copyright (C) 2011 Nicolas Viennot <nicolas@viennot.biz>
 *
 * This file is subject to the terms and conditions of the MIT License.
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
	ngx_flag_t	enable;
	ngx_uint_t	tcp_keepcnt;
	time_t		tcp_keepidle;
	time_t		tcp_keepintvl;
} ngx_http_tcp_keepalive_conf_t;

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
		NGX_HTTP_SRV_CONF_OFFSET,
		offsetof(ngx_http_tcp_keepalive_conf_t, enable),
		NULL
	},
	{
		ngx_string("tcp_keepcnt"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
		ngx_conf_set_num_slot,
		NGX_HTTP_SRV_CONF_OFFSET,
		offsetof(ngx_http_tcp_keepalive_conf_t, tcp_keepcnt),
		NULL
	},
	{
		ngx_string("tcp_keepalive"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
		ngx_conf_set_sec_slot,
		NGX_HTTP_SRV_CONF_OFFSET,
		offsetof(ngx_http_tcp_keepalive_conf_t, tcp_keepidle),
		NULL
	},
	{
		ngx_string("tcp_keepalive"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
		ngx_conf_set_sec_slot,
		NGX_HTTP_SRV_CONF_OFFSET,
		offsetof(ngx_http_tcp_keepalive_conf_t, tcp_keepintvl),
		NULL
	},
	ngx_null_command
};

static ngx_http_module_t  ngx_http_tcp_keepalive_module_ctx = {
    NULL,					/* preconfiguration */
    NULL,					/* postconfiguration */

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
