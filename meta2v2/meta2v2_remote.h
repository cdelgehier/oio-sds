/*
OpenIO SDS meta2v2
Copyright (C) 2014 Worldine, original work as part of Redcurrant
Copyright (C) 2015 OpenIO, modified as part of OpenIO Software Defined Storage

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.
*/

#ifndef OIO_SDS__meta2v2__meta2v2_remote_h
# define OIO_SDS__meta2v2__meta2v2_remote_h 1

# include <glib.h>
# include <meta2v2/autogen.h>

#define M2V2_FLAG_NODELETED        0x00000001
#define M2V2_FLAG_ALLVERSION       0x00000002
#define M2V2_FLAG_NOPROPS          0x00000004
#define M2V2_FLAG_NOFORMATCHECK    0x00000008
#define M2V2_FLAG_ALLPROPS         0x00000010

/* when listing */
#define M2V2_FLAG_HEADERS          0x00000020

/* when getting an alias, do not follow the foreign keys toward
 * headers, contents and chunks. */
#define M2V2_FLAG_NORECURSION      0x00000080

/* when getting an alias, ignores the version in the URL and
 * return the latest alias only. */
#define M2V2_FLAG_LATEST           0x00000100

/* flush the properties */
#define M2V2_FLAG_FLUSH            0x00000200

/* Request N spare chunks which should not be on provided blacklist */
#define M2V2_SPARE_BY_BLACKLIST "SPARE_BLACKLIST"
/* Request N spare chunks according to a storage policy */
#define M2V2_SPARE_BY_STGPOL "SPARE_STGPOL"

struct oio_url_s;

struct list_params_s;
struct list_result_s;

void m2v2_list_result_clean (struct list_result_s *p);

struct m2v2_create_params_s
{
	const char *storage_policy; /**< Will override the (maybe present) stgpol property. */
	const char *version_policy; /**< idem for the verpol property. */
	gchar **properties; /**< A NULL-terminated sequence of strings where:
						  * properties[i*2] is the i-th key and
						  * properties[(i*2)+1] is the i-th value */
	gboolean local; /**< Do not try to replicate, do not call get_peers() */
};

#define M2V2_MODE_DRYRUN  0x10000000

GError* m2v2_remote_execute_CREATE(const char *target, struct oio_url_s *url,
		struct m2v2_create_params_s *pols);

enum m2v2_destroy_flag_e {
	/* send a destruction event */
	M2V2_DESTROY_EVENT = 0x01,
	M2V2_DESTROY_FLUSH = 0x02,
	M2V2_DESTROY_FORCE = 0x04,
};

GError* m2v2_remote_execute_DESTROY(const char *target, struct oio_url_s *url,
		guint32 flags);

/* Locally destroy a container on several services. */
GError* m2v2_remote_execute_DESTROY_many(gchar **targets, struct oio_url_s *url,
		guint32 flags);

GError* m2v2_remote_execute_PURGE(const char *m2, struct oio_url_s *u, gdouble to);
GError* m2v2_remote_execute_DEDUP(const char *m2, struct oio_url_s *u, gdouble to);
GError* m2v2_remote_execute_FLUSH(const char *m2, struct oio_url_s *u, gdouble to);

/* returns an error of not empty (or if an other error occured) */
GError* m2v2_remote_execute_ISEMPTY(const char *m2, struct oio_url_s *u);

GError* m2v2_remote_execute_HAS(const char *target, struct oio_url_s *url);

GError* m2v2_remote_execute_BEANS(const char *target, struct oio_url_s *url,
		const char *pol, gint64 size, gboolean append, GSList **out);

/**
 * Get spare chunks. The number of spare chunks returned
 * will be the one defined by the policy minus the length of notin_list.
 *
 * @param target
 * @param sid
 * @param url
 * @param stgpol The storage policy that the spare chunks should match
 * @param notin_list The list of already known chunks that should be taken into
 *   account when computing distance between chunks.
 * @param broken_list The list of broken chunks, provided to prevent getting
 *   spare chunks from the same rawx (not taken into account when computing
 *   distance between chunks).
 * @param[out] out The output list of spare chunks
 * @return A GError in case of error
 *
 * @note notin_list and broken_list may contain beans other than
 *   (struct bean_CHUNKS_s *) with no harm (they will be ignored).
 */
GError* m2v2_remote_execute_SPARE(const char *target, struct oio_url_s *url,
		const char *stgpol, GSList *notin_list, GSList *broken_list,
		GSList **out);

GError* m2v2_remote_execute_PUT(const char *target, struct oio_url_s *url,
		GSList *in, GSList **out);

GError* m2v2_remote_execute_OVERWRITE(const char *target, struct oio_url_s *url,
		GSList *in);

GError* m2v2_remote_execute_APPEND(const char *target, struct oio_url_s *url,
		GSList *in, GSList **out);

GError* m2v2_remote_execute_COPY(const char *target, struct oio_url_s *url,
		const char *src);

GError* m2v2_remote_execute_GET(const char *target, struct oio_url_s *url,
		guint32 flags, GSList **out);

GError* m2v2_remote_execute_DEL(const char *target, struct oio_url_s *url);

GError* m2v2_remote_execute_RAW_ADD(const char *target, struct oio_url_s *url,
		GSList *beans);

GError* m2v2_remote_execute_RAW_DEL(const char *target, struct oio_url_s *url,
		GSList *beans);

GError* m2v2_remote_execute_LINK(const char *target, struct oio_url_s *url);

/** Substitute chunks by another one in meta2 database.
 * TODO: return number of substitutions */
GError* m2v2_remote_execute_RAW_SUBST(const char *target,
		struct oio_url_s *url, GSList *new_chunks, GSList *old_chunks);

GError* m2v2_remote_execute_RAW_SUBST_single(const char *target,
		struct oio_url_s *url,
		struct bean_CHUNKS_s *new_chunk, struct bean_CHUNKS_s *old_chunk);

GError* m2v2_remote_execute_LIST(const char *target,
		struct oio_url_s *url, struct list_params_s *p,
		struct list_result_s *out, gchar ***out_properties);

/** Get the aliases the chunk belongs to. */
GError* m2v2_remote_execute_LIST_BY_CHUNKID(const char *target,
		struct oio_url_s *url, const char *chunk, struct list_params_s *p,
		struct list_result_s *out);

/** Get the aliases the header belongs to. */
GError* m2v2_remote_execute_LIST_BY_HEADERHASH(const char *target, 
		struct oio_url_s *url, GBytes *h, struct list_params_s *p,
		struct list_result_s *out);

/** idem, but with the content ID as the key */
GError* m2v2_remote_execute_LIST_BY_HEADERID(const char *target, struct oio_url_s *url,
		GBytes *h, struct list_params_s *p, struct list_result_s *out);

/* works for contents only. for container props, @see sqlx_pack_PROPDEL() */
GError* m2v2_remote_execute_PROP_DEL(const char *target,
		struct oio_url_s *url, GSList *names);

/* works for contents only. for container props, @see sqlx_pack_PROPSET() */
GError* m2v2_remote_execute_PROP_SET(const char *target,
		struct oio_url_s *url, guint32 flags, GSList *in);

/* works for contents only. for container props, @see sqlx_pack_PROPGET() */
GError* m2v2_remote_execute_PROP_GET(const char *target,
		struct oio_url_s *url, guint32 flags, GSList **out);

GError* m2v2_remote_execute_EXITELECTION(const char *target,
		struct oio_url_s *url);

GError* m2v2_remote_touch_content(const char *target, struct oio_url_s *url);

GError* m2v2_remote_touch_container_ex(const char *target, struct oio_url_s *url, guint32 flags);

#endif /*OIO_SDS__meta2v2__meta2v2_remote_h*/
