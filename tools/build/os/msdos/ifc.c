/*
 *  $Id$
 */

#include <stdio.h>
#include <string.h>
#include <process.h>

#include <io.h>

void * xmalloc( int size )
{
    void * p = (void *)malloc( size );
    if ( !p )  {
        fprintf( stderr, "out of memory\n" );
        exit( 1 );
    }
    return p;
}
void * xrealloc( void * old, int size )
{
    void * p = (void *)realloc( old, size );
    if ( !p )  {
        fprintf( stderr, "out of memory\n" );
        exit( 1 );
    }
    return p;
}

char ** argv_fix( int * argc, char ** argv )
{
    char ** new = NULL;
    int     max = 20;
    int     cnt = 0;
    int     j;

    for ( j = 1; argv[j]; ++j )
        if ( argv[j][0] == '@' && access(argv[j]+1,0)==0 )
            break;
    if ( argv[j] == NULL )
        return argv;

    new = (char **)xmalloc( max * sizeof *new );
    new[cnt++] = *argv++;
    for ( ; *argv; ++argv )  {
        if ( cnt >= max )
            new = (char **)realloc( new, (max*=2) * sizeof *new );
            
        if ( argv[0][0] != '@' || access(argv[0]+1,0) )  {
            new[cnt++] = *argv;
        } else {
            char line[ 1000 ];
            FILE * f = fopen( argv[0]+1, "r" );
            if ( !f )  {
                perror( argv[0]+1 );
                exit( 2 );
            }
            while ( fgets( line, sizeof line, f ) )  {
                int len = strlen( line );
                /* delete trailing newlines */
                while ( line[len-1] == '\n' || line[len-1] == '\r' )
                    line[--len] = '\0';
                if ( cnt >= max )
                    new = (char **)xrealloc( new, (max*=2) * sizeof *new );
                new[cnt] = (char *)xmalloc( len+1 );
                strcpy( new[cnt], line );
                ++cnt;
            }
            fclose( f );
        }       
    }
    if ( cnt >= max )
        new = (char **)xrealloc( new, (max+1) * sizeof *new );
    new[cnt] = NULL;
    *argc = cnt;
    return new;
}


const char * USAGE = 
"usage: $progname [ -cNvmV ] file [ file ... ] dest-directory-or-file\n"
"        -v          -- verbose\n"
"        -V suffix   -- suffix to append to targets (before any . suffix)\n"
"                        eg: -V _g would change 'foo' to 'foo_g' and\n"
"                                               'libfoo.a' to 'libfoo_g.a'\n"
"        -m mode     -- mode for new file(s)\n"
"        -c          -- copy instead of move (always on)\n"
"        -N          -- copy only if source is newer than target\n"
;

void fatal( char * msg )
{
    if ( msg )
        fprintf( stderr, "%s\n", msg );
    fprintf( stderr, "%s", USAGE );
    exit( 1 );
}

char * basename( char * f )
{
    char * b = strrchr( f, '/' );
    if ( b )    ++b;
    else        b = f;
    return b;
}

#include <sys/stat.h>
int is_dir( char * path )
{
    struct stat buf;
    if ( stat( path, &buf ) )
        return 0;
    return buf.st_mode & S_IFDIR;
}
int is_file( char * path )
{
    struct stat buf;
    if ( stat( path, &buf ) )
        return 0;
    return buf.st_mode & S_IFREG;
}
int newer( char * p1, char * p2 )
{
    struct stat buf1;
    struct stat buf2;
    if ( stat( p1, &buf1 ) )
        return 0;
    if ( stat( p2, &buf2 ) )
        return 0;
    return buf1.st_mtime > buf2.st_mtime;
}

int filecopy( char * d, char * s, int preserve_time )
{
#if 0
    int         status;
    char      * argv[ 5 ];
    argv[0] = "cp";
    argv[1] = "-p";
    argv[2] = s;
    argv[3] = d;
    argv[4] = NULL;
    status = spawnvp( P_WAIT, argv[0], argv );
    if ( status )
        perror( "cp" );
    return status;
#else
    FILE      * fs;
    FILE      * fd;
    char        buffer[ 8192 ];
    int         n;
    struct ftime        When;
    struct stat         Stat;

    fs = fopen( s, "rb" );
    if ( fs == NULL )  {
        perror( s );
        return 1;
    }
    fd = fopen( d, "wb" );
    if ( fd == NULL )  {
        perror( d );
        fclose( fs );
        return 2;
    }

    if ( preserve_time )
        if ( getftime( fileno(fs), &When ) )  {
            perror( s );
            preserve_time = 0;
        }

    do {
        n = fread( buffer, 1, sizeof buffer, fs );
        if ( n > 0 )
            if ( fwrite( buffer, 1, n, fd ) < 0 )  {
                perror( d );
                return 3;
            }
    } while ( n > 0 );    

    fclose( fs );

    /* Fix time stamp */
    if ( preserve_time )
        if ( setftime( fileno(fd), &When ) )  {
            perror( s );
            preserve_time = 0;
        }
    fclose( fd );

    /* Fix access rights */
    if ( stat( s, &Stat ) )
        perror( s );
    else if ( chmod( d, Stat.st_mode ) )
        perror( d );

    return 0;
#endif
}





int main( int argc, char * argv[] )
{
    char *      progname;
    int         verbose = 0;
    int         only_if_newer= 0;
    char *      suffix  = NULL;
    char *      mode    = NULL;
    char *      dest;
    char **     pp;

    argv = argv_fix( &argc, argv );

    progname = basename( *argv++ );

    /* process the options */
    while ( argv[0]  &&  argv[0][0] == '-' )  {
        switch ( argv[0][1] )  {
            case 'N':
                ++argv;
                only_if_newer = 1;
                break;
            case 'c':
                ++argv;
                /* We always copy, regardless */
                break;
            case 'v':
                ++argv;
                verbose = 1;
                break;
            case 'V':
                ++argv;
                suffix = *argv;
                ++argv;
                break;
            case 'm':
                ++argv;
                mode = *argv;
                ++argv;
                break;
            default:
                fatal( NULL );
        }
    }

    /* Separate source file(s) from dest directory or file */
#if 0
    if ( !argv[0] || !argv[1] )
        fatal( "missing files or invalid destination" );
#else
    /* We used to require at least one file; not any more */
    if ( !argv[0] )
        fatal( "missing files or invalid destination" );
    if ( !argv[1] )
        return 0;
#endif
    for ( pp = argv; *pp; ++pp )
        continue;
    --pp;
    dest = *pp;
    *pp = NULL;

    /* Process the arguments */
    for (; *argv; ++argv )  {
        char * f = *argv;
        char * leaf = basename( f );
        char target[ 128 ];

        strcpy( target, dest );

        if ( is_dir( target ) )  {
            strcat( target, "/" );
            /* if we were given a suffix, then add it as appropriate */
            if ( suffix )  {
                char * dot = strchr( leaf, '.' );
                if ( dot )  {
                    strncat( target, leaf, dot-leaf );
                    strcat( target, suffix );
                    strcat( target, dot );
                    if ( verbose )
                        printf( "%s: %s will be installed as %s",
                               progname, f, strrchr(target,'/')+1 );
                } else {
                    strcat( target, leaf );
                    strcat( target, suffix );
                }
            } else {
                strcat( target, leaf );
            }
        }

        if ( access( f, 0 ) )  {
            char buf[200];
            sprintf( buf, "cannot read %s", f );
            fatal( buf );
        }

        if ( only_if_newer && is_file( target ) && !newer( f, target ) )  {
            if ( verbose )
                printf( "'%s' not newer than '%s'\n", f, target );
            continue;
        }

        if ( verbose )
            printf( "rm -f %s\n", target );
        if ( chmod( target, 0777 ) )
            if ( verbose )
                perror( target );
        if ( unlink( target ) )
            if ( verbose )
                perror( target );
        if ( verbose )
            printf( "cp -p %s %s\n", f, target );
        if ( filecopy( target, f, 1 ) )
            return 1;
        if ( mode )  {
            char buf[ 255 ];
            sprintf( buf, "chmod %s %s\n", mode, target );
            if ( verbose )
                printf( "%s\n", buf );
            system( buf );
        }
    }

    return 0;
}

