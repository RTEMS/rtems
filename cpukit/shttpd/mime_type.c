/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */

#include "defs.h"

static const struct mime_type default_mime_types[] = {
	{"html",	4,	"text/html"			},
	{"htm",		3,	"text/html"			},
	{"txt",		3,	"text/plain"			},
	{"css",		3,	"text/css"			},
	{"ico",		3,	"image/x-icon"			},
	{"gif",		3,	"image/gif"			},
	{"jpg",		3,	"image/jpeg"			},
	{"jpeg",	4,	"image/jpeg"			},
	{"png",		3,	"image/png"			},
	{"svg",		3,	"image/svg+xml"			},
	{"torrent",	7,	"application/x-bittorrent"	},
	{"wav",		3,	"audio/x-wav"			},
	{"mp3",		3,	"audio/x-mp3"			},
	{"mid",		3,	"audio/mid"			},
	{"m3u",		3,	"audio/x-mpegurl"		},
	{"ram",		3,	"audio/x-pn-realaudio"		},
	{"ra",		2,	"audio/x-pn-realaudio"		},
	{"doc",		3,	"application/msword",		},
	{"exe",		3,	"application/octet-stream"	},
	{"zip",		3,	"application/x-zip-compressed"	},
	{"xls",		3,	"application/excel"		},
	{"tgz",		3,	"application/x-tar-gz"		},
	{"tar.gz",	6,	"application/x-tar-gz"		},
	{"tar",		3,	"application/x-tar"		},
	{"gz",		2,	"application/x-gunzip"		},
	{"arj",		3,	"application/x-arj-compressed"	},
	{"rar",		3,	"application/x-arj-compressed"	},
	{"rtf",		3,	"application/rtf"		},
	{"pdf",		3,	"application/pdf"		},
	{"mpg",		3,	"video/mpeg"			},
	{"mpeg",	4,	"video/mpeg"			},
	{"asf",		3,	"video/x-ms-asf"		},
	{"avi",		3,	"video/x-msvideo"		},
	{"bmp",		3,	"image/bmp"			},
	{NULL,		0,	NULL				}
};

const char *
get_mime_type(struct shttpd_ctx *ctx, const char *uri, int len)
{
	struct llhead		*lp;
	const struct mime_type	*mt;
	struct mime_type_link	*mtl;
	const char		*s;

	/* Firt, loop through the custom mime types if any */
	LL_FOREACH(&ctx->mime_types, lp) {
		mtl = LL_ENTRY(lp, struct mime_type_link, link);
		s = uri + len - mtl->ext_len;
		if (s > uri && s[-1] == '.' &&
		    !my_strncasecmp(mtl->ext, s, mtl->ext_len))
			return (mtl->mime);
	}

	/* If no luck, try built-in mime types */
	for (mt = default_mime_types; mt->ext != NULL; mt++) {
		s = uri + len - mt->ext_len;
		if (s > uri && s[-1] == '.' &&
		    !my_strncasecmp(mt->ext, s, mt->ext_len))
			return (mt->mime);
	}

	/* Oops. This extension is unknown to us. Fallback to text/plain */
	return ("text/plain");
}

void
set_mime_types(struct shttpd_ctx *ctx, const char *path)
{
	FILE	*fp;
	char	line[512], ext[sizeof(line)], mime[sizeof(line)], *s;

	if ((fp = fopen(path, "r")) == NULL)
		elog(E_FATAL, NULL, "set_mime_types: fopen(%s): %s",
		    path, strerror(errno));

	while (fgets(line, sizeof(line), fp) != NULL) {
		/* Skip empty lines */
		if (line[0] == '#' || line[0] == '\n')
			continue;
		if (sscanf(line, "%s", mime)) {
			s = line + strlen(mime);
			while (*s && *s != '\n' && sscanf(s, "%s", ext)) {
				shttpd_add_mime_type(ctx, ext, mime);
				s += strlen(mime);
			}
		}
	}

	(void) fclose(fp);
}
