#define	DEBUG	1	/* enable debugging code (needed for dig) */
#define	RESOLVSORT	/* allow sorting of addresses in gethostbyname */
#define	RFC1535		/* comply with RFC1535 (STRONGLY reccomended by vixie)*/
#undef	USELOOPBACK	/* res_init() bind to localhost */
#undef	SUNSECURITY	/* verify gethostbyaddr() calls - WE DONT NEED IT  */
#define MULTI_PTRS_ARE_ALIASES 1 /* fold multiple PTR records into aliases */
#define	CHECK_SRVR_ADDR 1 /* confirm that the server requested sent the reply */
#define BIND_UPDATE 1	/* update support */

#if defined(__rtems__)
u_int16_t _getshort(const u_char *src);
u_int32_t _getlong(const u_char *src);
int gethostname (char *name, size_t namelen);
int sethostname (char *name, size_t namelen);
int issetugid (void);
#endif
