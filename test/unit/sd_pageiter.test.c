
/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

#include <sophia.h>
#include <libss.h>
#include <libsf.h>
#include <libsr.h>
#include <libsv.h>
#include <libsd.h>
#include <libst.h>

static void
addv(sdbuild *b, sr *r, uint64_t lsn, uint8_t flags, int *key)
{
	sfv pv[8];
	memset(pv, 0, sizeof(pv));
	pv[0].pointer = (char*)key;
	pv[0].size = sizeof(uint32_t);
	pv[1].pointer = NULL;
	pv[1].size = 0;
	svv *v = sv_vbuild(r, pv);
	sf_lsnset(r->scheme, sv_vpointer(v), lsn);
	sf_flagsset(r->scheme, sv_vpointer(v), flags);
	sd_buildadd(b, r, sv_vpointer(v), flags & SVDUP);
	sv_vunref(r, v);
}

static void
sd_pageiter_lte_empty(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, NULL);
	t( ss_iteratorhas(&it) == 0 );
	char *v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lte_eq0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, i, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, j, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, k, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lte_minmax0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, 6, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) == 0 );
	t( ss_iteratorof(&it) == NULL);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 16, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lte_mid0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, 8, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 10, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 555, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lte_iterate0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, NULL);
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lte_iterate1(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, k, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lte_dup_iterate0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 90, 0, &i);
	addv(&b, &st_r.r, 80, 0, &j);
	addv(&b, &st_r.r, 70, 0|SVDUP, &j);
	addv(&b, &st_r.r, 60, 0|SVDUP, &j);
	addv(&b, &st_r.r, 50, 0, &k);
	addv(&b, &st_r.r, 40, 0|SVDUP, &k);
	addv(&b, &st_r.r, 30, 0|SVDUP, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, 100, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));

	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 50);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	t( sf_flags(st_r.r.scheme, v) == SVDUP);
	t( sf_lsn(st_r.r.scheme, v) == 40);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	t( sf_flags(st_r.r.scheme, v) == SVDUP);
	t( sf_lsn(st_r.r.scheme, v) == 30);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 80);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	t( sf_flags(st_r.r.scheme, v) == SVDUP);
	t( sf_lsn(st_r.r.scheme, v) == 70);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	t( sf_flags(st_r.r.scheme, v) == SVDUP);
	t( sf_lsn(st_r.r.scheme, v) == 60);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 90);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lte_dup_mid(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 90, 0, &i);
	addv(&b, &st_r.r, 80, 0, &j);
	addv(&b, &st_r.r, 70, 0|SVDUP, &j);
	addv(&b, &st_r.r, 60, 0|SVDUP, &j);
	addv(&b, &st_r.r, 50, 0, &k);
	addv(&b, &st_r.r, 40, 0|SVDUP, &k);
	addv(&b, &st_r.r, 30, 0|SVDUP, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key;
	char *v;
	ssiter it;

	/* i */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, i, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 90);
	ss_iteratorclose(&it);

	/* j */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, j, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 80);
	ss_iteratorclose(&it);

	/* k */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, k, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 50);
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lt_eq(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, i, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, sv_vpointer(key));
	t( ss_iteratorhas(&it) == 0 );
	char *v = ss_iteratorof(&it);
	t( v == NULL);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, j, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, k, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lt_minmax(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, 7, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, sv_vpointer(key));
	t( ss_iteratorhas(&it) == 0 );
	t( ss_iteratorof(&it) == NULL);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 16, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lt_mid(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, 8, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 10, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 555, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lt_iterate0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, NULL);
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lt_iterate1(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, k, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_lt_dup_mid(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 90, 0, &i);
	addv(&b, &st_r.r, 80, 0, &j);
	addv(&b, &st_r.r, 70, 0|SVDUP, &j);
	addv(&b, &st_r.r, 60, 0|SVDUP, &j);
	addv(&b, &st_r.r, 50, 0, &k);
	addv(&b, &st_r.r, 40, 0|SVDUP, &k);
	addv(&b, &st_r.r, 30, 0|SVDUP, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key;
	char *v;
	ssiter it;

	/* i */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, i, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, sv_vpointer(key));
	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	/* j */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, j, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 90);
	ss_iteratorclose(&it);

	/* k */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, k, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_LT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 80);
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gte_eq0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, i, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, j, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, k, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gte_minmax0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, 6, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 16, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gte_mid0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, 8, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 10, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 2, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 555, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gte_mid1(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, NULL);
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gte_iterate0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, NULL);
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gte_iterate1(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, i, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gte_dup_iterate0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 90, 0, &i);
	addv(&b, &st_r.r, 80, 0, &j);
	addv(&b, &st_r.r, 70, 0|SVDUP, &j);
	addv(&b, &st_r.r, 60, 0|SVDUP, &j);
	addv(&b, &st_r.r, 50, 0, &k);
	addv(&b, &st_r.r, 40, 0|SVDUP, &k);
	addv(&b, &st_r.r, 30, 0|SVDUP, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, 1, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));

	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	t( sf_lsn(st_r.r.scheme, v) == 90);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	t( sf_lsn(st_r.r.scheme, v) == 80);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	t( sf_lsn(st_r.r.scheme, v) == 70);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	t( sf_lsn(st_r.r.scheme, v) == 60);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	t( sf_lsn(st_r.r.scheme, v) == 50);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	t( sf_lsn(st_r.r.scheme, v) == 40);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	t( sf_lsn(st_r.r.scheme, v) == 30);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gte_dup_mid(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 90, 0, &i);
	addv(&b, &st_r.r, 80, 0, &j);
	addv(&b, &st_r.r, 70, 0|SVDUP, &j);
	addv(&b, &st_r.r, 60, 0|SVDUP, &j);
	addv(&b, &st_r.r, 50, 0, &k);
	addv(&b, &st_r.r, 40, 0|SVDUP, &k);
	addv(&b, &st_r.r, 30, 0|SVDUP, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key;
	char *v;
	ssiter it;

	/* i */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, i, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 90);
	ss_iteratorclose(&it);

	/* j */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, j, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 80);
	ss_iteratorclose(&it);

	/* k */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, k, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GTE, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 50);
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gt_eq(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, i, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( v != NULL);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, j, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, k, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gt_minmax(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, 7, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 15, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gt_mid(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, 8, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 10, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);

	key = st_svv(&st_r.g, &st_r.gc, 0, 0, 555, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gt_iterate0(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, NULL);
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == i);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gt_iterate1(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 3, 0, &i);
	addv(&b, &st_r.r, 2, 0, &j);
	addv(&b, &st_r.r, 1, 0, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key = st_svv(&st_r.g, &st_r.gc, 0, 0, i, 0, 0);
	ssiter it;
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	char *v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	ss_iteratornext(&it);

	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

static void
sd_pageiter_gt_dup_mid(void)
{
	sdbuild b;
	sd_buildinit(&b);
	t( sd_buildbegin(&b, &st_r.r, 1, 0, NULL) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, &st_r.r, 90, 0, &i);
	addv(&b, &st_r.r, 80, 0, &j);
	addv(&b, &st_r.r, 70, 0|SVDUP, &j);
	addv(&b, &st_r.r, 60, 0|SVDUP, &j);
	addv(&b, &st_r.r, 50, 0, &k);
	addv(&b, &st_r.r, 40, 0|SVDUP, &k);
	addv(&b, &st_r.r, 30, 0|SVDUP, &k);
	sd_buildend(&b, &st_r.r);

	ssbuf buf;
	ss_bufinit(&buf);
	t( sd_commitpage(&b, &st_r.r, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	svv *key;
	char *v;
	ssiter it;

	/* i */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, i, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == j);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 80);
	ss_iteratorclose(&it);

	/* j */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, j, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) != 0 );
	v = ss_iteratorof(&it);
	t( *(int*)sf_field(st_r.r.scheme, 0, v, &st_r.size) == k);
	t( sf_flags(st_r.r.scheme, v) == 0);
	t( sf_lsn(st_r.r.scheme, v) == 50);
	ss_iteratorclose(&it);

	/* k */
	key = st_svv(&st_r.g, &st_r.gc, 0, 0, k, 0, 0);
	ss_iterinit(sd_pageiter, &it);
	ss_iteropen(sd_pageiter, &it, &st_r.r, &page, SS_GT, sv_vpointer(key));
	t( ss_iteratorhas(&it) == 0 );
	v = ss_iteratorof(&it);
	t( v == NULL );
	ss_iteratorclose(&it);

	sd_buildfree(&b, &st_r.r);
	ss_buffree(&buf, &st_r.a);
}

stgroup *sd_pageiter_group(void)
{
	stgroup *group = st_group("sdpageiter");
	st_groupadd(group, st_test("lte_empty", sd_pageiter_lte_empty));
	st_groupadd(group, st_test("lte_eq0", sd_pageiter_lte_eq0));
	st_groupadd(group, st_test("lte_minmax0",  sd_pageiter_lte_minmax0));
	st_groupadd(group, st_test("lte_mid0", sd_pageiter_lte_mid0));
	st_groupadd(group, st_test("lte_iterate0", sd_pageiter_lte_iterate0));
	st_groupadd(group, st_test("lte_iterate1", sd_pageiter_lte_iterate1));
	st_groupadd(group, st_test("lte_dup_iterate0", sd_pageiter_lte_dup_iterate0));
	st_groupadd(group, st_test("lte_dup_mid", sd_pageiter_lte_dup_mid));
	st_groupadd(group, st_test("lt_eq", sd_pageiter_lt_eq));
	st_groupadd(group, st_test("lt_minmax", sd_pageiter_lt_minmax));
	st_groupadd(group, st_test("lt_mid", sd_pageiter_lt_mid));
	st_groupadd(group, st_test("lt_iterate0", sd_pageiter_lt_iterate0));
	st_groupadd(group, st_test("lt_iterate1", sd_pageiter_lt_iterate1));
	st_groupadd(group, st_test("lt_dup_mid", sd_pageiter_lt_dup_mid));
	st_groupadd(group, st_test("gte_eq0", sd_pageiter_gte_eq0));
	st_groupadd(group, st_test("gte_minmax0", sd_pageiter_gte_minmax0));
	st_groupadd(group, st_test("gte_mid0", sd_pageiter_gte_mid0));
	st_groupadd(group, st_test("gte_mid1", sd_pageiter_gte_mid1));
	st_groupadd(group, st_test("gte_iterate0", sd_pageiter_gte_iterate0));
	st_groupadd(group, st_test("gte_iterate1", sd_pageiter_gte_iterate1));
	st_groupadd(group, st_test("gte_dup_iterate0", sd_pageiter_gte_dup_iterate0));
	st_groupadd(group, st_test("gte_dup_mid", sd_pageiter_gte_dup_mid));
	st_groupadd(group, st_test("gt_eq", sd_pageiter_gt_eq));
	st_groupadd(group, st_test("gt_minmax", sd_pageiter_gt_minmax));
	st_groupadd(group, st_test("gt_mid", sd_pageiter_gt_mid));
	st_groupadd(group, st_test("gt_iterate0", sd_pageiter_gt_iterate0));
	st_groupadd(group, st_test("gt_iterate1", sd_pageiter_gt_iterate1));
	st_groupadd(group, st_test("gt_dup_mid", sd_pageiter_gt_dup_mid));
	return group;
}
