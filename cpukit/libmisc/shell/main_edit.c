//
// edit.c
//
// Text editor
//
// Copyright (C) 2002 Michael Ringgaard. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the project nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//

#include <signal.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef SANOS
#include <os.h>
#endif

#ifdef __rtems__
#include <assert.h>
#include <rtems.h>
#include <rtems/shell.h>
#endif

#if defined(__linux__) || defined(__rtems__)
#include <sys/ioctl.h>
#include <termios.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif
static int linux_console;
#endif

#define MINEXTEND      32768
#define LINEBUF_EXTRA  32

#ifndef TABSIZE
#define TABSIZE        8
#endif

#ifndef INDENT
#define INDENT "  "
#endif

#define CLRSCR           "\033[0J\x1b[H\x1b[J"
#define CLREOL           "\033[K"
#define GOTOXY           "\033[%d;%dH"
#define RESET_COLOR      "\033[0m"

#ifdef COLOR
#define TEXT_COLOR       "\033[44m\033[37m\033[1m"
#define SELECT_COLOR     "\033[47m\033[37m\033[1m"
#define STATUS_COLOR     "\033[0m\033[47m\033[30m"
#else
#define TEXT_COLOR       "\033[0m"
#define SELECT_COLOR     "\033[7m\033[1m"
#define STATUS_COLOR     "\033[1m\033[7m"
#endif

//
// Key codes
//

#define KEY_BACKSPACE        0x101
#define KEY_ESC              0x102
#define KEY_INS              0x103
#define KEY_DEL              0x104
#define KEY_LEFT             0x105
#define KEY_RIGHT            0x106
#define KEY_UP               0x107
#define KEY_DOWN             0x108
#define KEY_HOME             0x109
#define KEY_END              0x10A
#define KEY_ENTER            0x10B
#define KEY_TAB              0x10C
#define KEY_PGUP             0x10D
#define KEY_PGDN             0x10E

#define KEY_CTRL_LEFT        0x10F
#define KEY_CTRL_RIGHT       0x110
#define KEY_CTRL_UP          0x111
#define KEY_CTRL_DOWN        0x112
#define KEY_CTRL_HOME        0x113
#define KEY_CTRL_END         0x114
#define KEY_CTRL_TAB         0x115

#define KEY_SHIFT_LEFT       0x116
#define KEY_SHIFT_RIGHT      0x117
#define KEY_SHIFT_UP         0x118
#define KEY_SHIFT_DOWN       0x119
#define KEY_SHIFT_PGUP       0x11A
#define KEY_SHIFT_PGDN       0x11B
#define KEY_SHIFT_HOME       0x11C
#define KEY_SHIFT_END        0x11D
#define KEY_SHIFT_TAB        0x11E

#define KEY_SHIFT_CTRL_LEFT  0x11F
#define KEY_SHIFT_CTRL_RIGHT 0x120
#define KEY_SHIFT_CTRL_UP    0x121
#define KEY_SHIFT_CTRL_DOWN  0x122
#define KEY_SHIFT_CTRL_HOME  0x123
#define KEY_SHIFT_CTRL_END   0x124

#define KEY_F1               0x125
#define KEY_F2               0x126
#define KEY_F3               0x127
#define KEY_F4               0x128
#define KEY_F5               0x129
#define KEY_F6               0x12a
#define KEY_F7               0x12b
#define KEY_F8               0x12c
#define KEY_F9               0x12d
#define KEY_F10              0x12e

#define KEY_UNKNOWN          0xFFF

#define ctrl(c) ((c) - 0x60)

//
// Editor data block
//
// Structure of split buffer:
//
//    +------------------+------------------+------------------+
//    | text before gap  |        gap       |  text after gap  |
//    +------------------+------------------+------------------+
//    ^                  ^                  ^                  ^
//    |                  |                  |                  |
//  start               gap                rest               end
//

struct env;

struct undo {
  int pos;                 // Editor position
  int erased;              // Size of erased contents
  int inserted;            // Size of inserted contents
  unsigned char *undobuf;  // Erased contents for undo
  unsigned char *redobuf;  // Inserted contents for redo
  struct undo *next;       // Next undo buffer
  struct undo *prev;       // Previous undo buffer
};

struct editor {
  unsigned char *start;      // Start of text buffer
  unsigned char *gap;        // Start of gap
  unsigned char *rest;       // End of gap
  unsigned char *end;        // End of text buffer

  int toppos;                // Text position for current top screen line
  int topline;               // Line number for top of screen
  int margin;                // Position for leftmost column on screen

  int linepos;               // Text position for current line
  int line;                  // Current document line
  int col;                   // Current document column
  int lastcol;               // Remembered column from last horizontal navigation
  int anchor;                // Anchor position for selection

  struct undo *undohead;     // Start of undo buffer list
  struct undo *undotail;     // End of undo buffer list
  struct undo *undo;         // Undo/redo boundary

  int refresh;               // Flag to trigger screen redraw
  int lineupdate;            // Flag to trigger redraw of current line
  int dirty;                 // Dirty flag is set when the editor buffer has been changed

  int newfile;               // File is a new file
  int permissions;           // File permissions

  int selecting;             // Selecting active in the edtor.

  struct env *env;           // Reference to global editor environment
  struct editor *next;       // Next editor
  struct editor *prev;       // Previous editor

  char filename[FILENAME_MAX];
};

struct env {
  struct editor *current;   // Current editor

  unsigned char *clipboard; // Clipboard
  int clipsize;             // Clipboard size

  unsigned char *search;    // Search text
  unsigned char *linebuf;   // Scratch buffer

  int cols;                 // Console columns
  int lines;                // Console lines

  int untitled;             // Counter for untitled files
};

//
// Editor buffer functions
//

static void clear_undo(struct editor *ed) {
  struct undo *undo = ed->undohead;
  while (undo) {
    struct undo *next = undo->next;
    free(undo->undobuf);
    free(undo->redobuf);
    free(undo);
    undo = next;
  }
  ed->undohead = ed->undotail = ed->undo = NULL;
}

static void reset_undo(struct editor *ed) {
  while (ed->undotail != ed->undo) {
    struct undo *undo = ed->undotail;
    if (!undo) {
      ed->undohead = NULL;
      ed->undotail = NULL;
      break;
    }
    ed->undotail = undo->prev;
    if (undo->prev) undo->prev->next = NULL;
    free(undo->undobuf);
    free(undo->redobuf);
    free(undo);
  }
  ed->undo = ed->undotail;
}

static struct editor *create_editor(struct env *env) {
  struct editor *ed = (struct editor *) malloc(sizeof(struct editor));
  memset(ed, 0, sizeof(struct editor));
  if (env->current) {
    ed->next = env->current->next;
    ed->prev = env->current;
    env->current->next->prev = ed;
    env->current->next = ed;
  } else {
    ed->next = ed->prev = ed;
  }
  ed->env = env;
  env->current = ed;
  return ed;
}

static void delete_editor(struct editor *ed) {
  if (ed->next == ed) {
    ed->env->current = NULL;
  } else {
    ed->env->current = ed->prev;
  }
  ed->next->prev = ed->prev;
  ed->prev->next = ed->next;
  if (ed->start) free(ed->start);
  clear_undo(ed);
  free(ed);
}

static struct editor *find_editor(struct env *env, char *filename) {
  char fnbuf[PATH_MAX];
  char *fn = fnbuf;
  struct editor *ed = env->current;
  struct editor *start = ed;

  /* Note: When realpath() == NULL, usually the file doesn't exist */
  if (!realpath(filename, fn)) { fn = filename; }

  do {
    if (strcmp(fn, ed->filename) == 0) return ed;
    ed = ed->next;
  } while (ed != start);
  return NULL;
}

static int new_file(struct editor *ed, char *filename) {
  if (*filename) {
    strlcpy(ed->filename, filename, sizeof(ed->filename));
  } else {
    sprintf(ed->filename, "Untitled-%d", ++ed->env->untitled);
    ed->newfile = 1;
  }
  ed->permissions = 0644;

  ed->start = (unsigned char *) malloc(MINEXTEND);
  if (!ed->start) return -1;
#ifdef DEBUG
  memset(ed->start, 0, MINEXTEND);
#endif

  ed->gap = ed->start;
  ed->rest = ed->end = ed->gap + MINEXTEND;
  ed->anchor = -1;

  return 0;
}

static int load_file(struct editor *ed, char *filename) {
  struct stat statbuf;
  int length;
  int f;

  if (!realpath(filename, ed->filename)) return -1;
  f = open(ed->filename, O_RDONLY | O_BINARY);
  if (f < 0) return -1;

  if (fstat(f, &statbuf) < 0) goto err;
  length = statbuf.st_size;
  ed->permissions = statbuf.st_mode & 0777;

  ed->start = (unsigned char *) malloc(length + MINEXTEND);
  if (!ed->start) goto err;
#ifdef DEBUG
  memset(ed->start, 0, length + MINEXTEND);
#endif
  if (read(f, ed->start, length) != length) goto err;

  ed->gap = ed->start + length;
  ed->rest = ed->end = ed->gap + MINEXTEND;
  ed->anchor = -1;

  close(f);
  return 0;

err:
  close(f);
  if (ed->start) {
    free(ed->start);
    ed->start = NULL;
  }
  return -1;
}

static int save_file(struct editor *ed) {
  int f;

  f = open(ed->filename, O_CREAT | O_TRUNC | O_WRONLY, ed->permissions);
  if (f < 0) return -1;

  if (write(f, ed->start, ed->gap - ed->start) != ed->gap - ed->start) goto err;
  if (write(f, ed->rest, ed->end - ed->rest) != ed->end - ed->rest) goto err;

  close(f);
  ed->dirty = 0;
  clear_undo(ed);
  return 0;

err:
  close(f);
  return -1;
}

static int text_length(struct editor *ed) {
  return (ed->gap - ed->start) + (ed->end - ed->rest);
}

static unsigned char *text_ptr(struct editor *ed, int pos) {
  unsigned char *p = ed->start + pos;
  if (p >= ed->gap) p += (ed->rest - ed->gap);
  return p;
}

static void move_gap(struct editor *ed, int pos, int minsize) {
  int gapsize = ed->rest - ed->gap;
  unsigned char *p = text_ptr(ed, pos);
  if (minsize < 0) minsize = 0;

  if (minsize <= gapsize) {
    if (p != ed->rest) {
      if (p < ed->gap) {
        memmove(p + gapsize, p, ed->gap - p);
      } else {
        memmove(ed->gap, ed->rest, p - ed->rest);
      }
      ed->gap = ed->start + pos;
      ed->rest = ed->gap + gapsize;
    }
  } else {
    int newsize;
    unsigned char *start;
    unsigned char *gap;
    unsigned char *rest;
    unsigned char *end;

    if (gapsize + MINEXTEND > minsize) minsize = gapsize + MINEXTEND;
    newsize = (ed->end - ed->start) - gapsize + minsize;
    start = (unsigned char *) malloc(newsize); // TODO check for out of memory
    if (start == NULL) {
        return;
    }
    gap = start + pos;
    rest = gap + minsize;
    end = start + newsize;

    if (p < ed->gap) {
      memcpy(start, ed->start, pos);
      memcpy(rest, p, ed->gap - p);
      memcpy(end - (ed->end - ed->rest), ed->rest, ed->end - ed->rest);
    } else {
      memcpy(start, ed->start, ed->gap - ed->start);
      memcpy(start + (ed->gap - ed->start), ed->rest, p - ed->rest);
      memcpy(rest, p, ed->end - p);
    }

    free(ed->start);
    ed->start = start;
    ed->gap = gap;
    ed->rest = rest;
    ed->end = end;
  }

#ifdef DEBUG
  memset(ed->gap, 0, ed->rest - ed->gap);
#endif
}

static void close_gap(struct editor *ed) {
  int len = text_length(ed);
  move_gap(ed, len, 1);
  ed->start[len] = 0;
}

static int get(struct editor *ed, int pos) {
  unsigned char *p = text_ptr(ed, pos);
  if (p >= ed->end) return -1;
  return *p;
}

static int compare(struct editor *ed, unsigned char *buf, int pos, int len) {
  unsigned char *bufptr = buf;
  unsigned char *p = ed->start + pos;
  if (p >= ed->gap) p += (ed->rest - ed->gap);

  while (len > 0) {
    if (p == ed->end) return 0;
    if (*bufptr++ != *p) return 0;
    len--;
    if (++p == ed->gap) p = ed->rest;
  }

  return 1;
}

static int copy(struct editor *ed, unsigned char *buf, int pos, int len) {
  unsigned char *bufptr = buf;
  unsigned char *p = ed->start + pos;
  if (p >= ed->gap) p += (ed->rest - ed->gap);

  while (len > 0) {
    if (p == ed->end) break;
    *bufptr++ = *p;
    len--;
    if (++p == ed->gap) p = ed->rest;
  }

  return bufptr - buf;
}

static void replace(struct editor *ed, int pos, int len, unsigned char *buf, int bufsize, int doundo) {
  unsigned char *p;
  struct undo *undo;

  // Store undo information
  if (doundo) {
    reset_undo(ed);
    undo = ed->undotail;
    if (undo && len == 0 && bufsize == 1 && undo->erased == 0 && pos == undo->pos + undo->inserted) {
      // Insert character at end of current redo buffer
      undo->redobuf = realloc(undo->redobuf, undo->inserted + 1);
      undo->redobuf[undo->inserted] = *buf;
      undo->inserted++;
    } else if (undo && len == 1 && bufsize == 0 && undo->inserted == 0 && pos == undo->pos) {
      // Erase character at end of current undo buffer
      undo->undobuf = realloc(undo->undobuf, undo->erased + 1);
      undo->undobuf[undo->erased] = get(ed, pos);
      undo->erased++;
    } else if (undo && len == 1 && bufsize == 0 && undo->inserted == 0 && pos == undo->pos - 1) {
      // Erase character at beginning of current undo buffer
      undo->pos--;
      undo->undobuf = realloc(undo->undobuf, undo->erased + 1);
      memmove(undo->undobuf + 1, undo->undobuf, undo->erased);
      undo->undobuf[0] = get(ed, pos);
      undo->erased++;
    } else {
      // Create new undo buffer
      undo = (struct undo *) malloc(sizeof(struct undo));
      if (ed->undotail) ed->undotail->next = undo;
      undo->prev = ed->undotail;
      undo->next = NULL;
      ed->undotail = ed->undo = undo;
      if (!ed->undohead) ed->undohead = undo;

      undo->pos = pos;
      undo->erased = len;
      undo->inserted = bufsize;
      undo->undobuf = undo->redobuf = NULL;
      if (len > 0) {
        undo->undobuf = malloc(len);
        copy(ed, undo->undobuf, pos, len);
      }
      if (bufsize > 0) {
        undo->redobuf = malloc(bufsize);
        memcpy(undo->redobuf, buf, bufsize);
      }
    }
  }

  p = ed->start + pos;
  if (bufsize == 0 && p <= ed->gap && p + len >= ed->gap) {
    // Handle deletions at the edges of the gap
    ed->rest += len - (ed->gap - p);
    ed->gap = p;
  } else {
    // Move the gap
    move_gap(ed, pos + len, bufsize - len);

    // Replace contents
    memcpy(ed->start + pos, buf, bufsize);
    ed->gap = ed->start + pos + bufsize;
  }

  // Mark buffer as dirty
  ed->dirty = 1;
}

static void insert(struct editor *ed, int pos, unsigned char *buf, int bufsize) {
  replace(ed, pos, 0, buf, bufsize, 1);
}

static void erase(struct editor *ed, int pos, int len) {
  replace(ed, pos, len, NULL, 0, 1);
}

//
// Navigation functions
//

static int line_length(struct editor *ed, int linepos) {
  int pos = linepos;
  while (1) {
    int ch = get(ed, pos);
    if (ch < 0 || ch == '\n' || ch == '\r') break;
    pos++;
  }

  return pos - linepos;
}

static int line_start(struct editor *ed, int pos) {
  while (1) {
    if (pos == 0) break;
    if (get(ed, pos - 1) == '\n') break;
    pos--;
  }

  return pos;
}

static int next_line(struct editor *ed, int pos) {
  while (1) {
    int ch = get(ed, pos);
    if (ch < 0) return -1;
    pos++;
    if (ch == '\n') return pos;
  }
}

static int prev_line(struct editor *ed, int pos) {
  if (pos == 0) return -1;

  while (pos > 0) {
    int ch = get(ed, --pos);
    if (ch == '\n') break;
  }

  while (pos > 0) {
    int ch = get(ed, --pos);
    if (ch == '\n') return pos + 1;
  }

  return 0;
}

static int column(struct editor *ed, int linepos, int col) {
  unsigned char *p = text_ptr(ed, linepos);
  int c = 0;
  while (col > 0) {
    if (p == ed->end) break;
    if (*p == '\t') {
      int spaces = TABSIZE - c % TABSIZE;
      c += spaces;
    } else {
      c++;
    }
    col--;
    if (++p == ed->gap) p = ed->rest;
  }
  return c;
}

static void moveto(struct editor *ed, int pos, int center) {
  int scroll = 0;
  for (;;) {
    int cur = ed->linepos + ed->col;
    if (pos < cur) {
      if (pos >= ed->linepos) {
        ed->col = pos - ed->linepos;
      } else {
        ed->col = 0;
        ed->linepos = prev_line(ed, ed->linepos);
        ed->line--;

        if (ed->topline > ed->line) {
          ed->toppos = ed->linepos;
          ed->topline--;
          ed->refresh = 1;
          scroll = 1;
        }
      }
    } else if (pos > cur) {
      int next = next_line(ed, ed->linepos);
      if (next == -1) {
        ed->col = line_length(ed, ed->linepos);
        break;
      } else if (pos < next) {
        ed->col = pos - ed->linepos;
      } else {
        ed->col = 0;
        ed->linepos = next;
        ed->line++;

        if (ed->line >= ed->topline + ed->env->lines) {
          ed->toppos = next_line(ed, ed->toppos);
          ed->topline++;
          ed->refresh = 1;
          scroll = 1;
        }
      }
    } else {
      break;
    }
  }

  if (scroll && center) {
    int tl = ed->line - ed->env->lines / 2;
    if (tl < 0) tl = 0;
    for (;;) {
      if (ed->topline > tl) {
        ed->toppos = prev_line(ed, ed->toppos);
        ed->topline--;
      } else if (ed->topline < tl) {
        ed->toppos = next_line(ed, ed->toppos);
        ed->topline++;
      } else {
        break;
      }
    }
  }
}

//
// Text selection
//

static int get_selection(struct editor *ed, size_t *start, size_t *end) {
  if (ed->anchor == -1) {
    *start = *end = -1;
    return 0;
  } else {
    int pos = ed->linepos + ed->col;
    if (pos == ed->anchor) {
      *start = *end = -1;
      return 0;
    } else if (pos < ed->anchor) {
      *start = pos;
      *end = ed->anchor;
    } else {
      *start = ed->anchor;
      *end = pos;
    }
  }
  return 1;
}

static int get_selected_text(struct editor *ed, char *buffer, int size) {
  size_t selstart, selend, len;

  if (!get_selection(ed, &selstart, &selend)) return 0;
  len = selend - selstart;
  if (len >= size) return 0;
  copy(ed, (unsigned char*) buffer, selstart, len);
  buffer[len] = 0;
  return len;
}

static void update_selection(struct editor *ed, int select) {
  if (select) {
    if (ed->anchor == -1) ed->anchor = ed->linepos + ed->col;
    ed->refresh = 1;
  } else {
    if (ed->anchor != -1) ed->refresh = 1;
    ed->anchor = -1;
  }
}

static int erase_selection(struct editor *ed) {
  size_t selstart, selend;

  if (!get_selection(ed, &selstart, &selend)) return 0;
  moveto(ed, selstart, 0);
  erase(ed, selstart, selend - selstart);
  ed->anchor = -1;
  ed->refresh = 1;
  return 1;
}

static void select_all(struct editor *ed) {
  ed->anchor = 0;
  ed->refresh = 1;
  moveto(ed, text_length(ed), 0);
}

//
// Screen functions
//

static void get_console_size(struct env *env) {
#ifdef __linux__
  struct winsize ws;
  ioctl(0, TIOCGWINSZ, &ws);
  env->cols = ws.ws_col;
  env->lines = ws.ws_row - 1;
#elif defined(__rtems__)
  char* e;
  env->lines = 25;
  env->cols = 80;
  e = getenv("LINES");
  if (e != NULL) {
    int lines = strtol(e, 0, 10);
    if (lines > 0) {
      env->lines = lines - 1;
    }
  }
  e = getenv("COLUMNS");
  if (e != NULL) {
    int cols = strtol(e, 0, 10);
    if (cols > 0) {
      env->cols = cols;
    }
  }
#else
  struct term *term = gettib()->proc->term;
  env->cols = term->cols;
  env->lines = term->lines - 1;
#endif
  env->linebuf = realloc(env->linebuf, env->cols + LINEBUF_EXTRA);
}

static void outch(char c) {
  putchar(c);
}

static void outbuf(unsigned char *buf, int len) {
  fwrite(buf, 1, len, stdout);
}

static void outstr(char *str) {
  fputs(str, stdout);
}

static void clear_screen(void) {
  outstr(CLRSCR);
}

static void gotoxy(int col, int line) {
  char buf[32];

  sprintf(buf, GOTOXY, line + 1, col + 1);
  outstr(buf);
}

//
// Keyboard functions
//

static void get_modifier_keys(int *shift, int *ctrl) {
  *shift = *ctrl = 0;
#ifdef __linux__
  if (linux_console) {
    char modifiers = 6;
    if (ioctl(0, TIOCLINUX, &modifiers) >= 0) {
      if (modifiers & 1) *shift = 1;
      if (modifiers & 4) *ctrl = 1;
    }
  }
#endif
}

static int getachar(void)
{
  int ch = getchar();
  return ch;
}

static int getkey(void) {
  int ch, shift, ctrl;

  ch = getachar();
  if (ch < 0) return ch;

  switch (ch) {
    case 0x08: return KEY_BACKSPACE;
    case 0x09:
      get_modifier_keys(&shift, &ctrl);
      if (shift) return KEY_SHIFT_TAB;
      if (ctrl) return KEY_CTRL_TAB;
      return KEY_TAB;
#ifdef SANOS
    case 0x0D: return gettib()->proc->term->type == TERM_CONSOLE ? KEY_ENTER : KEY_UNKNOWN;
    case 0x0A: return gettib()->proc->term->type != TERM_CONSOLE ? KEY_ENTER : KEY_UNKNOWN;
#else
    case 0x0D: return KEY_ENTER;
    case 0x0A: return KEY_ENTER;
#endif
    case 0x1B:
      ch = getachar();
      switch (ch) {
        case 0x1B: return KEY_ESC;
        case 0x4F:
          ch = getachar();
          switch (ch) {
            case 0x46: return KEY_END;
            case 0x48: return KEY_HOME;
            case 0x50: return KEY_F1;
            case 0x51: return KEY_F2;
            case 0x52: return KEY_F3;
            case 0x53: return KEY_F4;
            case 0x54: return KEY_F5;
            default: return KEY_UNKNOWN;
          }
          break;

        case 0x5B:
          get_modifier_keys(&shift, &ctrl);
          ch = getachar();
          if (ch == 0x31) {
            ch = getachar();
            switch (ch) {
              case 0x35:
                return getachar() == 0x7E ? KEY_F5 : KEY_UNKNOWN;
              case 0x37:
                return getachar() == 0x7E ? KEY_F6 : KEY_UNKNOWN;
              case 0x3B:
                ch = getachar();
                if (ch == 0x7E) return KEY_F7;
                if (ch == 0x32) shift = 1;
                if (ch == 0x35) ctrl = 1;
                if (ch == 0x36) shift = ctrl = 1;
                ch = getachar();
                break;
              case 0x39:
                return getachar() == 0x7E ? KEY_F8 : KEY_UNKNOWN;
              case 0x7E:
                if (shift && ctrl) return KEY_SHIFT_CTRL_HOME;
                if (shift) return KEY_SHIFT_HOME;
                if (ctrl) return KEY_CTRL_HOME;
                return KEY_HOME;
              default:
                return KEY_UNKNOWN;
            }
          }

          switch (ch) {
            case 0x31:
              ch = getachar();
              if (ch != 0x7E) return KEY_UNKNOWN;
              if (shift && ctrl) return KEY_SHIFT_CTRL_HOME;
              if (shift) return KEY_SHIFT_HOME;
              if (ctrl) return KEY_CTRL_HOME;
              return KEY_HOME;
            case 0x32:
              ch = getachar();
              switch (ch) {
                case 0x30: ch = getachar(); return KEY_F9;
                case 0x31: ch = getachar(); return KEY_F10;
                case 0x7E: return KEY_INS;
                default: break;
              }
              return KEY_UNKNOWN;
            case 0x33: return getachar() == 0x7E ? KEY_DEL : KEY_UNKNOWN;
            case 0x34:
              if (getachar() != 0x7E) return KEY_UNKNOWN;
              if (shift && ctrl) return KEY_SHIFT_CTRL_END;
              if (shift) return KEY_SHIFT_END;
              if (ctrl) return KEY_CTRL_END;
              return KEY_END;
            case 0x35:
              if (getachar() != 0x7E) return KEY_UNKNOWN;
              if (shift) return KEY_SHIFT_PGUP;
              return KEY_PGUP;
            case 0x36:
              if (getachar() != 0x7E) return KEY_UNKNOWN;
              if (shift) return KEY_SHIFT_PGDN;
              return KEY_PGDN;
            case 0x41:
              if (shift && ctrl) return KEY_SHIFT_CTRL_UP;
              if (shift) return KEY_SHIFT_UP;
              if (ctrl) return KEY_CTRL_UP;
              return KEY_UP;
            case 0x42:
              if (shift && ctrl) return KEY_SHIFT_CTRL_DOWN;
              if (shift) return KEY_SHIFT_DOWN;
              if (ctrl) return KEY_CTRL_DOWN;
              return KEY_DOWN;
            case 0x43:
              if (shift && ctrl) return KEY_SHIFT_CTRL_RIGHT;
              if (shift) return KEY_SHIFT_RIGHT;
              if (ctrl) return KEY_CTRL_RIGHT;
              return KEY_RIGHT;
            case 0x44:
              if (shift && ctrl) return KEY_SHIFT_CTRL_LEFT;
              if (shift) return KEY_SHIFT_LEFT;
              if (ctrl) return KEY_CTRL_LEFT;
              return KEY_LEFT;
            case 0x46:
              if (shift && ctrl) return KEY_SHIFT_CTRL_END;
              if (shift) return KEY_SHIFT_END;
              if (ctrl) return KEY_CTRL_END;
              return KEY_END;
            case 0x48:
              if (shift && ctrl) return KEY_SHIFT_CTRL_HOME;
              if (shift) return KEY_SHIFT_HOME;
              if (ctrl) return KEY_CTRL_HOME;
              return KEY_HOME;
            case 0x5A:
              return KEY_SHIFT_TAB;
            case 0x5B:
              ch = getachar();
              switch (ch) {
                case 0x41: return KEY_F1;
                case 0x43: return KEY_F3;
                case 0x45: return KEY_F5;
              }
              return KEY_UNKNOWN;

            default: return KEY_UNKNOWN;
          }
          break;

        default: return KEY_UNKNOWN;
      }
      break;

    case 0x00:
    case 0xE0:
      ch = getachar();
      switch (ch) {
        case 0x0F: return KEY_SHIFT_TAB;
        case 0x3B: return KEY_F1;
        case 0x3D: return KEY_F3;
        case 0x3F: return KEY_F5;
        case 0x47: return KEY_HOME;
        case 0x48: return KEY_UP;
        case 0x49: return KEY_PGUP;
        case 0x4B: return KEY_LEFT;
        case 0x4D: return KEY_RIGHT;
        case 0x4F: return KEY_END;
        case 0x50: return KEY_DOWN;
        case 0x51: return KEY_PGDN;
        case 0x52: return KEY_INS;
        case 0x53: return KEY_DEL;
        case 0x73: return KEY_CTRL_LEFT;
        case 0x74: return KEY_CTRL_RIGHT;
        case 0x75: return KEY_CTRL_END;
        case 0x77: return KEY_CTRL_HOME;
        case 0x8D: return KEY_CTRL_UP;
        case 0x91: return KEY_CTRL_DOWN;
        case 0x94: return KEY_CTRL_TAB;
        case 0xB8: return KEY_SHIFT_UP;
        case 0xB7: return KEY_SHIFT_HOME;
        case 0xBF: return KEY_SHIFT_END;
        case 0xB9: return KEY_SHIFT_PGUP;
        case 0xBB: return KEY_SHIFT_LEFT;
        case 0xBD: return KEY_SHIFT_RIGHT;
        case 0xC0: return KEY_SHIFT_DOWN;
        case 0xC1: return KEY_SHIFT_PGDN;
        case 0xDB: return KEY_SHIFT_CTRL_LEFT;
        case 0xDD: return KEY_SHIFT_CTRL_RIGHT;
        case 0xD8: return KEY_SHIFT_CTRL_UP;
        case 0xE0: return KEY_SHIFT_CTRL_DOWN;
        case 0xD7: return KEY_SHIFT_CTRL_HOME;
        case 0xDF: return KEY_SHIFT_CTRL_END;

        default: return KEY_UNKNOWN;
      }
      break;

    case 0x7F: return KEY_BACKSPACE;

    default: return ch;
  }
}

static int prompt(struct editor *ed, char *msg, int selection) {
  int maxlen, len, ch;
  char *buf = (char*) ed->env->linebuf;

  gotoxy(0, ed->env->lines);
  outstr(STATUS_COLOR);
  outstr(msg);
  outstr(CLREOL);

  len = 0;
  maxlen = ed->env->cols - strlen(msg) - 1;
  if (selection) {
    len = get_selected_text(ed, buf, maxlen);
    outbuf((unsigned char*) buf, len);
  }

  for (;;) {
    fflush(stdout);
    ch = getkey();
    if (ch == KEY_ESC) {
      return 0;
    } else if (ch == KEY_ENTER) {
      buf[len] = 0;
      return len > 0;
    } else if (ch == KEY_BACKSPACE) {
      if (len > 0) {
        outstr("\b \b");
        len--;
      }
    } else if (ch >= ' ' && ch < 0x100 && len < maxlen) {
      outch(ch);
      buf[len++] = ch;
    }
  }
}

static int ask(void) {
  int ch = getachar();
  return ch == 'y' || ch == 'Y';
}

//
// Display functions
//

static void display_message(struct editor *ed, char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  gotoxy(0, ed->env->lines);
  outstr(STATUS_COLOR);
  vprintf(fmt, args);
  outstr(CLREOL TEXT_COLOR);
  fflush(stdout);
  va_end(args);
}

static void draw_full_statusline(struct editor *ed) {
  struct env *env = ed->env;
  int namewidth = env->cols - 29;
  gotoxy(0, env->lines);
  sprintf((char*) env->linebuf, STATUS_COLOR "%*.*sF1=Help %c%c Ln %-6dCol %-4d" CLREOL TEXT_COLOR, -namewidth, namewidth, ed->filename, ed->selecting ? '+' : ' ', ed->dirty ? '*' : ' ', ed->line + 1, column(ed, ed->linepos, ed->col) + 1);
  outstr((char*) env->linebuf);
}

static void draw_statusline(struct editor *ed) {
  gotoxy(ed->env->cols - 20, ed->env->lines);
  sprintf((char*) ed->env->linebuf, STATUS_COLOR "%c Ln %-6dCol %-4d" CLREOL TEXT_COLOR, ed->dirty ? '*' : ' ', ed->line + 1, column(ed, ed->linepos, ed->col) + 1);
  outstr((char*) ed->env->linebuf);
}

static void display_line(struct editor *ed, int pos, int fullline) {
  int hilite = 0;
  int col = 0;
  int margin = ed->margin;
  int maxcol = ed->env->cols + margin;
  unsigned char *bufptr = ed->env->linebuf;
  unsigned char *p = text_ptr(ed, pos);
  size_t selstart, selend, ch;
  char *s;

  (void) get_selection(ed, &selstart, &selend);
  while (col < maxcol) {
    if (margin == 0) {
      if (!hilite && pos >= selstart && pos < selend) {
        for (s = SELECT_COLOR; *s; s++) *bufptr++ = *s;
        hilite = 1;
      } else if (hilite && pos >= selend) {
        for (s = TEXT_COLOR; *s; s++) *bufptr++ = *s;
        hilite = 0;
      }
    }

    if (p == ed->end) break;
    ch = *p;
    if (ch == '\r' || ch == '\n') break;

    if (ch == '\t') {
      int spaces = TABSIZE - col % TABSIZE;
      while (spaces > 0 && col < maxcol) {
        if (margin > 0) {
          margin--;
        } else {
          *bufptr++ = ' ';
        }
        col++;
        spaces--;
      }
    } else {
      if (margin > 0) {
        margin--;
      } else {
        *bufptr++ = ch;
      }
      col++;
    }

    if (++p == ed->gap) p = ed->rest;
    pos++;
  }

#if defined(__linux__)
  if (hilite) {
    while (col < maxcol) {
      *bufptr++ = ' ';
      col++;
    }
  } else {
    if (col == margin) *bufptr++ = ' ';
  }
#endif

  if (col < maxcol) {
    for (s = CLREOL; *s; s++) *bufptr++ = *s;
    if (fullline) {
      memcpy(bufptr, "\r\n", 2);
      bufptr += 2;
    }
  }

  if (hilite) {
    for (s = TEXT_COLOR; *s; s++) *bufptr++ = *s;
  }

  outbuf(ed->env->linebuf, bufptr - ed->env->linebuf);
}

static void update_line(struct editor *ed) {
  gotoxy(0, ed->line - ed->topline);
  display_line(ed, ed->linepos, 0);
}

static void draw_screen(struct editor *ed) {
  int pos;
  int i;

  gotoxy(0, 0);
  outstr(TEXT_COLOR);
  pos = ed->toppos;
  for (i = 0; i < ed->env->lines; i++) {
    if (pos < 0) {
      outstr(CLREOL "\r\n");
    } else {
      display_line(ed, pos, 1);
      pos = next_line(ed, pos);
    }
  }
}

static void position_cursor(struct editor *ed) {
  int col = column(ed, ed->linepos, ed->col);
  gotoxy(col - ed->margin, ed->line - ed->topline);
}

//
// Cursor movement
//

static void adjust(struct editor *ed) {
  int col;
  int ll = line_length(ed, ed->linepos);
  ed->col = ed->lastcol;
  if (ed->col > ll) ed->col = ll;

  col = column(ed, ed->linepos, ed->col);
  while (col < ed->margin) {
    ed->margin -= 4;
    if (ed->margin < 0) ed->margin = 0;
    ed->refresh = 1;
  }

  while (col - ed->margin >= ed->env->cols) {
    ed->margin += 4;
    ed->refresh = 1;
  }
}

static void select_toggle(struct editor *ed) {
  ed->selecting = ed->selecting ? 0 : 1;
  update_selection(ed, ed->selecting);
  adjust(ed);
}

static void up(struct editor *ed, int select) {
  int newpos;

  update_selection(ed, select);

  newpos = prev_line(ed, ed->linepos);
  if (newpos < 0) return;

  ed->linepos = newpos;
  ed->line--;
  if (ed->line < ed->topline) {
    ed->toppos = ed->linepos;
    ed->topline = ed->line;
    ed->refresh = 1;
  }

  adjust(ed);
}

static void down(struct editor *ed, int select) {
  int newpos;

  update_selection(ed, select);

  newpos = next_line(ed, ed->linepos);
  if (newpos < 0) return;

  ed->linepos = newpos;
  ed->line++;

  if (ed->line >= ed->topline + ed->env->lines) {
    ed->toppos = next_line(ed, ed->toppos);
    ed->topline++;
    ed->refresh = 1;
  }

  adjust(ed);
}

static void left(struct editor *ed, int select) {
  update_selection(ed, select);
  if (ed->col > 0) {
    ed->col--;
  } else {
    int newpos = prev_line(ed, ed->linepos);
    if (newpos < 0) return;

    ed->col = line_length(ed, newpos);
    ed->linepos = newpos;
    ed->line--;
    if (ed->line < ed->topline) {
      ed->toppos = ed->linepos;
      ed->topline = ed->line;
      ed->refresh = 1;
    }
  }

  ed->lastcol = ed->col;
  adjust(ed);
}

static void right(struct editor *ed, int select) {
  update_selection(ed, select);
  if (ed->col < line_length(ed, ed->linepos)) {
    ed->col++;
  } else {
    int newpos = next_line(ed, ed->linepos);
    if (newpos < 0) return;

    ed->col = 0;
    ed->linepos = newpos;
    ed->line++;

    if (ed->line >= ed->topline + ed->env->lines) {
      ed->toppos = next_line(ed, ed->toppos);
      ed->topline++;
      ed->refresh = 1;
    }
  }

  ed->lastcol = ed->col;
  adjust(ed);
}

static int wordchar(int ch) {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9');
}

static void wordleft(struct editor *ed, int select) {
  int pos, phase;

  update_selection(ed, select);
  pos = ed->linepos + ed->col;
  phase = 0;
  while (pos > 0) {
    int ch = get(ed, pos - 1);
    if (phase == 0) {
      if (wordchar(ch)) phase = 1;
    } else {
      if (!wordchar(ch)) break;
    }

    pos--;
    if (pos < ed->linepos) {
      ed->linepos = prev_line(ed, ed->linepos);
      ed->line--;
      ed->refresh = 1;
    }
  }
  ed->col = pos - ed->linepos;
  if (ed->line < ed->topline) {
    ed->toppos = ed->linepos;
    ed->topline = ed->line;
  }

  ed->lastcol = ed->col;
  adjust(ed);
}

static void wordright(struct editor *ed, int select) {
  int pos, end, phase, next;

  update_selection(ed, select);
  pos = ed->linepos + ed->col;
  end = text_length(ed);
  next = next_line(ed, ed->linepos);
  phase = 0;
  while (pos < end) {
    int ch = get(ed, pos);
    if (phase == 0) {
      if (wordchar(ch)) phase = 1;
    } else {
      if (!wordchar(ch)) break;
    }

    pos++;
    if (pos == next) {
      ed->linepos = next;
      next = next_line(ed, ed->linepos);
      ed->line++;
      ed->refresh = 1;
    }
  }
  ed->col = pos - ed->linepos;
  if (ed->line >= ed->topline + ed->env->lines) {
    ed->toppos = next_line(ed, ed->toppos);
    ed->topline++;
  }

  ed->lastcol = ed->col;
  adjust(ed);
}

static void home(struct editor *ed, int select) {
  update_selection(ed, select);
  ed->col = ed->lastcol = 0;
  adjust(ed);
}

static void end(struct editor *ed, int select) {
  update_selection(ed, select);
  ed->col = ed->lastcol = line_length(ed, ed->linepos);
  adjust(ed);
}

static void top(struct editor *ed, int select) {
  update_selection(ed, select);
  ed->toppos = ed->topline = ed->margin = 0;
  ed->linepos = ed->line = ed->col = ed->lastcol = 0;
  ed->refresh = 1;
}

static void bottom(struct editor *ed, int select) {
  update_selection(ed, select);
  for (;;) {
    int newpos = next_line(ed, ed->linepos);
    if (newpos < 0) break;

    ed->linepos = newpos;
    ed->line++;

    if (ed->line >= ed->topline + ed->env->lines) {
      ed->toppos = next_line(ed, ed->toppos);
      ed->topline++;
      ed->refresh = 1;
    }
  }
  ed->col = ed->lastcol = line_length(ed, ed->linepos);
  adjust(ed);
}

static void pageup(struct editor *ed, int select) {
  int i;

  update_selection(ed, select);
  if (ed->line < ed->env->lines) {
    ed->linepos = ed->toppos = 0;
    ed->line = ed->topline = 0;
  } else {
    for (i = 0; i < ed->env->lines; i++) {
      int newpos = prev_line(ed, ed->linepos);
      if (newpos < 0) return;

      ed->linepos = newpos;
      ed->line--;

      if (ed->topline > 0) {
        ed->toppos = prev_line(ed, ed->toppos);
        ed->topline--;
      }
    }
  }

  ed->refresh = 1;
  adjust(ed);
}

static void pagedown(struct editor *ed, int select) {
  int i;

  update_selection(ed, select);
  for (i = 0; i < ed->env->lines; i++) {
    int newpos = next_line(ed, ed->linepos);
    if (newpos < 0) break;

    ed->linepos = newpos;
    ed->line++;

    ed->toppos = next_line(ed, ed->toppos);
    ed->topline++;
  }

  ed->refresh = 1;
  adjust(ed);
}

//
// Text editing
//

static void insert_char(struct editor *ed, unsigned char ch) {
  erase_selection(ed);
  insert(ed, ed->linepos + ed->col, &ch, 1);
  ed->col++;
  ed->lastcol = ed->col;
  adjust(ed);
  if (!ed->refresh) ed->lineupdate = 1;
}

static void newline(struct editor *ed) {
  int p;
  unsigned char ch;

  erase_selection(ed);
#if defined(__linux__) || defined(__rtems__)
  insert(ed, ed->linepos + ed->col, (unsigned char*) "\n", 1);
#else
  insert(ed, ed->linepos + ed->col, "\r\n", 2);
#endif
  ed->col = ed->lastcol = 0;
  ed->line++;
  p = ed->linepos;
  ed->linepos = next_line(ed, ed->linepos);
  for (;;) {
    ch = get(ed, p++);
    if (ch == ' ' || ch == '\t') {
      insert(ed, ed->linepos + ed->col, &ch, 1);
      ed->col++;
    } else {
      break;
    }
  }
  ed->lastcol = ed->col;

  ed->refresh = 1;

  if (ed->line >= ed->topline + ed->env->lines) {
    ed->toppos = next_line(ed, ed->toppos);
    ed->topline++;
    ed->refresh = 1;
  }

  adjust(ed);
}

static void backspace(struct editor *ed) {
  if (erase_selection(ed)) return;
  if (ed->linepos + ed->col == 0) return;
  if (ed->col == 0) {
    int pos = ed->linepos;
    erase(ed, --pos, 1);
    if (get(ed, pos - 1) == '\r') erase(ed, --pos, 1);

    ed->line--;
    ed->linepos = line_start(ed, pos);
    ed->col = pos - ed->linepos;
    ed->refresh = 1;

    if (ed->line < ed->topline) {
      ed->toppos = ed->linepos;
      ed->topline = ed->line;
    }
  } else {
    ed->col--;
    erase(ed, ed->linepos + ed->col, 1);
    ed->lineupdate = 1;
  }

  ed->lastcol = ed->col;
  adjust(ed);
}

static void del(struct editor *ed) {
  int pos, ch;

  if (erase_selection(ed)) return;
  pos = ed->linepos + ed->col;
  ch = get(ed, pos);
  if (ch < 0) return;

  erase(ed, pos, 1);
  if (ch == '\r') {
    ch = get(ed, pos);
    if (ch == '\n') erase(ed, pos, 1);
  }

  if (ch == '\n') {
    ed->refresh = 1;
  } else {
    ed->lineupdate = 1;
  }
}

static void indent(struct editor *ed, unsigned char *indentation) {
  size_t start, end, i, lines, toplines, newline, ch;
  unsigned char *buffer, *p;
  size_t buflen;
  int width = strlen((const char*) indentation);
  int pos = ed->linepos + ed->col;

  if (!get_selection(ed, &start, &end)) {
    insert_char(ed, '\t');
    return;
  }

  lines = 0;
  toplines = 0;
  newline = 1;
  for (i = start; i < end; i++) {
    if (i == ed->toppos) toplines = lines;
    if (newline) {
      lines++;
      newline = 0;
    }
    if (get(ed, i) == '\n') newline = 1;
  }
  buflen = end - start + lines * width;
  buffer = malloc(buflen);
  if (!buffer) return;

  newline = 1;
  p = buffer;
  for (i = start; i < end; i++) {
    if (newline) {
      memcpy(p, indentation, width);
      p += width;
      newline = 0;
    }
    ch = get(ed, i);
    *p++ = ch;
    if (ch == '\n') newline = 1;
  }

  replace(ed, start, end - start, buffer, buflen, 1);
  free(buffer);

  if (ed->anchor < pos) {
    pos += width * lines;
  } else {
    ed->anchor += width * lines;
  }

  ed->toppos += width * toplines;
  ed->linepos = line_start(ed, pos);
  ed->col = ed->lastcol = pos - ed->linepos;

  adjust(ed);
  ed->refresh = 1;
}

static void unindent(struct editor *ed, unsigned char *indentation) {
  size_t start, end, i, newline, ch, shrinkage, topofs;
  unsigned char *buffer, *p;
  int width = strlen((const char*) indentation);
  int pos = ed->linepos + ed->col;

  if (!get_selection(ed, &start, &end)) return;

  buffer = malloc(end - start);
  if (!buffer) return;

  newline = 1;
  p = buffer;
  i = start;
  shrinkage = 0;
  topofs = 0;
  while (i < end) {
    if (newline) {
      newline = 0;
      if (compare(ed, indentation, i, width)) {
        i += width;
        shrinkage += width;
        if (i < ed->toppos) topofs -= width;
        continue;
      }
    }
    ch = get(ed, i++);
    *p++ = ch;
    if (ch == '\n') newline = 1;
  }

  if (!shrinkage) {
    free(buffer);
    return;
  }

  replace(ed, start, end - start, buffer, p - buffer, 1);
  free(buffer);

  if (ed->anchor < pos) {
    pos -= shrinkage;
  } else {
    ed->anchor -= shrinkage;
  }

  ed->toppos += topofs;
  ed->linepos = line_start(ed, pos);
  ed->col = ed->lastcol = pos - ed->linepos;

  ed->refresh = 1;
  adjust(ed);
}

static void undo(struct editor *ed) {
  if (!ed->undo) return;
  moveto(ed, ed->undo->pos, 0);
  replace(ed, ed->undo->pos, ed->undo->inserted, ed->undo->undobuf, ed->undo->erased, 0);
  ed->undo = ed->undo->prev;
  if (!ed->undo) ed->dirty = 0;
  ed->anchor = -1;
  ed->lastcol = ed->col;
  ed->refresh = 1;
}

static void redo(struct editor *ed) {
  if (ed->undo) {
    if (!ed->undo->next) return;
    ed->undo = ed->undo->next;
  } else {
    if (!ed->undohead) return;
    ed->undo = ed->undohead;
  }
  replace(ed, ed->undo->pos, ed->undo->erased, ed->undo->redobuf, ed->undo->inserted, 0);
  moveto(ed, ed->undo->pos, 0);
  ed->dirty = 1;
  ed->anchor = -1;
  ed->lastcol = ed->col;
  ed->refresh = 1;
}

//
// Clipboard
//

static void copy_selection(struct editor *ed) {
  size_t selstart, selend;

  if (!get_selection(ed, &selstart, &selend)) return;
  ed->env->clipsize = selend - selstart;
  ed->env->clipboard = (unsigned char *) realloc(ed->env->clipboard, ed->env->clipsize);
  if (!ed->env->clipboard) return;
  copy(ed, ed->env->clipboard, selstart, ed->env->clipsize);
}

static void cut_selection(struct editor *ed) {
  copy_selection(ed);
  erase_selection(ed);
  select_toggle(ed);
}

static void paste_selection(struct editor *ed) {
  erase_selection(ed);
  insert(ed, ed->linepos + ed->col, ed->env->clipboard, ed->env->clipsize);
  moveto(ed, ed->linepos + ed->col + ed->env->clipsize, 0);
  ed->refresh = 1;
}

//
// Editor Commands
//

static void open_editor(struct editor *ed) {
  int rc;
  char *filename;
  struct env *env = ed->env;

  if (!prompt(ed, "Open file: ", 1)) {
    ed->refresh = 1;
    return;
  }
  filename = (char*) ed->env->linebuf;

  ed = find_editor(ed->env, filename);
  if (ed) {
    env->current = ed;
  } else {
    ed = create_editor(env);
    rc = load_file(ed, filename);
    if (rc < 0) {
      display_message(ed, "Error %d opening %s (%s)", errno, filename, strerror(errno));
      sleep(5);
      delete_editor(ed);
      ed = env->current;
    }
  }
  ed->refresh = 1;
}

static void new_editor(struct editor *ed) {
  ed = create_editor(ed->env);
  new_file(ed, "");
  ed->refresh = 1;
}

static void read_from_stdin(struct editor *ed) {
  char buffer[512];
  int n, pos;

  pos = 0;
  while ((n = fread(buffer, 1, sizeof(buffer), stdin)) > 0) {
    insert(ed, pos, (unsigned char*) buffer, n);
    pos += n;
  }
  strncpy(ed->filename, "<stdin>", FILENAME_MAX);
  ed->newfile = 1;
  ed->dirty = 0;
}

static void save_editor(struct editor *ed) {
  int rc;

  if (!ed->dirty && !ed->newfile) return;

  if (ed->newfile) {
    if (!prompt(ed, "Save as: ", 1)) {
      ed->refresh = 1;
      return;
    }

    if (access((const char*) ed->env->linebuf, F_OK) == 0) {
      display_message(ed, "Overwrite %s (y/n)? ", ed->env->linebuf);
      if (!ask()) {
        ed->refresh = 1;
        return;
      }
    }
    strlcpy(
      ed->filename, (const char*) ed->env->linebuf, sizeof(ed->filename));
    ed->newfile = 0;
  }

  rc = save_file(ed);
  if (rc < 0) {
    display_message(ed, "Error %d saving document (%s)", errno, strerror(errno));
    sleep(5);
  }

  ed->refresh = 1;
}

static struct editor* close_editor(struct editor *ed) {
  struct env *env = ed->env;

  if (ed->dirty) {
    display_message(ed, "Close %s without saving changes (y/n)? ", ed->filename);
    if (!ask()) {
      ed->refresh = 1;
      return ed;
    }
  }

  delete_editor(ed);

  ed = env->current;
  if (!ed) {
    ed = create_editor(env);
    new_file(ed, "");
  }
  ed->refresh = 1;
  return ed;
}

static void pipe_command(struct editor *ed) {
#ifdef __rtems__
    display_message(ed, "Not supported");
    sleep(3);
#else
  FILE *f;
  char buffer[512];
  int n;
  int pos;

  if (!prompt(ed, "Command: ", 1)) {
    ed->refresh = 1;
    return;
  }

#ifdef SANOS
  f = popen(ed->env->linebuf, "r2");
#else
  f = popen(ed->env->linebuf, "r");
#endif
  if (!f) {
    display_message(ed, "Error %d running command (%s)", errno, strerror(errno));
    sleep(5);
  } else {
    erase_selection(ed);
    pos = ed->linepos + ed->col;
    while ((n = fread(buffer, 1, sizeof(buffer), f)) > 0) {
      insert(ed, pos, buffer, n);
      pos += n;
    }
    moveto(ed, pos, 0);
    pclose(f);
  }
  ed->refresh = 1;
#endif
}

static void find_text(struct editor *ed, int next) {
  int slen;

  if (!next) {
    if (!prompt(ed, "Find: ", 1)) {
      ed->refresh = 1;
      return;
    }
    if (ed->env->search) free(ed->env->search);
    ed->env->search = (unsigned char*) strdup((const char*) ed->env->linebuf);
  }

  if (!ed->env->search) return;
  slen = strlen((const char*) ed->env->search);
  if (slen > 0) {
    unsigned char *match;

    close_gap(ed);
    match = (unsigned char*) strstr((char*) ed->start + ed->linepos + ed->col, (char*) ed->env->search);
    if (match != NULL) {
      int pos = match - ed->start;
      ed->anchor = pos;
      moveto(ed, pos + slen, 1);
    } else {
      outch('\007');
    }
  }
  ed->refresh = 1;
}

static void goto_line(struct editor *ed) {
  int lineno, l, pos;

  ed->anchor = -1;
  if (prompt(ed, "Goto line: ", 1)) {
    lineno = atoi((char*) ed->env->linebuf);
    if (lineno > 0) {
      pos = 0;
      for (l = 0; l < lineno - 1; l++) {
        pos = next_line(ed, pos);
        if (pos < 0) break;
      }
    } else {
      pos = -1;
    }

    if (pos >= 0) {
      moveto(ed, pos, 1);
    } else {
      outch('\007');
    }
  }
  ed->refresh = 1;
}

static struct editor *next_file(struct editor *ed) {
  ed = ed->env->current = ed->next;
  ed->refresh = 1;
  return ed;
}

static void jump_to_editor(struct editor *ed) {
  struct env *env = ed->env;
  char filename[FILENAME_MAX];
  int lineno = 0;

  if (!get_selected_text(ed, filename, FILENAME_MAX)) {
    int pos = ed->linepos + ed->col;
    char *p = filename;
    int left = FILENAME_MAX - 1;
    while (left > 0) {
      int ch = get(ed, pos);
      if (ch < 0) break;
      if (strchr("!@\"'#%&()[]{}*?+:;\r\n\t ", ch)) break;
      *p++ = ch;
      left--;
      pos++;
    }
    *p = 0;

    if (get(ed, pos) == ':') {
      pos++;
      for (;;) {
        int ch = get(ed, pos);
        if (ch < 0) break;
        if (ch >= '0' && ch <= '9') {
          lineno = lineno * 10 + (ch - '0');
        } else {
          break;
        }
        pos++;
      }
    }
  }
  if (!*filename) return;

  ed = find_editor(env, filename);
  if (ed) {
    env->current = ed;
  } else {
    ed = create_editor(env);
    if (load_file(ed, filename) < 0) {
      outch('\007');
      delete_editor(ed);
      ed = env->current;
    }
  }

  if (lineno > 0) {
    int pos = 0;
    while (--lineno > 0) {
      pos = next_line(ed, pos);
      if (pos < 0) break;
    }
    if (pos >= 0) moveto(ed, pos, 1);
  }

  ed->refresh = 1;
}

static void redraw_screen(struct editor *ed) {
  get_console_size(ed->env);
  draw_screen(ed);
}

static int quit(struct env *env) {
  struct editor *ed = env->current;
  struct editor *start = ed;

  do {
    if (ed->dirty) {
      display_message(ed, "Close %s without saving changes (y/n)? ", ed->filename);
      if (!ask()) return 0;
    }
    ed = ed->next;
  } while (ed != start);

  return 1;
}

static void help(struct editor *ed) {
  gotoxy(0, 0);
  clear_screen();
  outstr("Editor Command Summary\r\n");
  outstr("======================\r\n\r\n");
  outstr("<up>         Move one line up (*)         Ctrl+N  New editor\r\n");
  outstr("<down>       Move one line down (*)       Ctrl+O  Open file\r\n");
  outstr("<left>       Move one character left (*)  Ctrl+S  Save file\r\n");
  outstr("<right>      Move one character right (*) Ctrl+W  Close file\r\n");
  outstr("<pgup>       Move one page up (*)         Ctrl+Q  Quit\r\n");
  outstr("<pgdn>       Move one page down (*)       Ctrl+P  Pipe command\r\n");
  outstr("Ctrl+<left>  Move to previous word (*)    Ctrl+A  Select all\r\n");
  outstr("Ctrl+<right> Move to next word (*)        Ctrl+C  Copy selection to clipboard\r\n");
  outstr("<home>       Move to start of line (*)    Ctrl+X  Cut selection to clipboard\r\n");
  outstr("<end>        Move to end of line (*)      Ctrl+V  Paste from clipboard\r\n");
  outstr("Ctrl+<home>  Move to start of file (*)    Ctrl+Z  Undo\r\n");
  outstr("Ctrl+<end>   Move to end of file (*)      Ctrl+R  Redo\r\n");
  outstr("<backspace>  Delete previous character    Ctrl+F  Find text\r\n");
  outstr("<delete>     Delete current character     Ctrl+G  Find next\r\n");
  outstr("Ctrl+<tab>   Next editor                  Ctrl+L  Goto line\r\n");
  outstr("<tab>        Indent selection             F1      Help\r\n");
  outstr("Shift+<tab>  Unindent selection           F2      Select toggle\r\n");
  outstr(" (*) Extends selection, F2 toggles.       F3      Navigate to file\r\n");
  outstr("                                          F4      Copy selection to clipboard\r\n");
  outstr("  Ctrl-Q/S may not work over              F5      Redraw screen\r\n");
  outstr("  serial links, use funcions keys         F9      Save file\r\n");
  outstr("                                          F10     Quit\r\n");
  outstr("Press any key to continue...");
  fflush(stdout);

  getkey();
  draw_screen(ed);
  draw_full_statusline(ed);
}

//
// Editor
//

static void edit(struct editor *ed) {
  int done = 0;
  int key;

  ed->refresh = 1;
  while (!done) {
    if (ed->refresh) {
      draw_screen(ed);
      draw_full_statusline(ed);
      ed->refresh = 0;
      ed->lineupdate = 0;
    } else if (ed->lineupdate) {
      update_line(ed);
      ed->lineupdate = 0;
      draw_statusline(ed);
    } else {
      draw_statusline(ed);
    }

    position_cursor(ed);
    fflush(stdout);
    key = getkey();

    if (key >= ' ' && key <= 0x7F) {
#ifdef LESS
      switch (key) {
        case 'q': done = 1; break;
        case '/': find_text(ed, 0); break;
      }
#else
      insert_char(ed, (unsigned char) key);
#endif
    } else {
      switch (key) {
        case KEY_F1: help(ed); break;
        case KEY_F2: select_toggle(ed); break;
        case KEY_F3: jump_to_editor(ed); ed = ed->env->current; break;
        case KEY_F4: copy_selection(ed); break;
        case KEY_F5: redraw_screen(ed); break;
        case KEY_F9: save_editor(ed); break;
        case KEY_F10: done = 1; break;

#if defined(__linux__) || defined(__rtems__)
        case ctrl('y'): help(ed); break;
        case ctrl('t'): top(ed, 0); break;
        case ctrl('b'): bottom(ed, 0); break;
#endif

        case KEY_UP: up(ed, ed->selecting); break;
        case KEY_DOWN: down(ed, ed->selecting); break;
        case KEY_LEFT: left(ed, ed->selecting); break;
        case KEY_RIGHT: right(ed, ed->selecting); break;
        case KEY_HOME: home(ed, ed->selecting); break;
        case KEY_END: end(ed, ed->selecting); break;
        case KEY_PGUP: pageup(ed, ed->selecting); break;
        case KEY_PGDN: pagedown(ed, ed->selecting); break;

        case KEY_CTRL_RIGHT: wordright(ed, ed->selecting); break;
        case KEY_CTRL_LEFT: wordleft(ed, ed->selecting); break;
        case KEY_CTRL_HOME: top(ed, ed->selecting); break;
        case KEY_CTRL_END: bottom(ed, ed->selecting); break;

#if SHIFT_SELECT
        case KEY_SHIFT_UP: up(ed, 1); break;
        case KEY_SHIFT_DOWN: down(ed, 1); break;
        case KEY_SHIFT_LEFT: left(ed, 1); break;
        case KEY_SHIFT_RIGHT: right(ed, 1); break;
        case KEY_SHIFT_PGUP: pageup(ed, 1); break;
        case KEY_SHIFT_PGDN: pagedown(ed, 1); break;
        case KEY_SHIFT_HOME: home(ed, 1); break;
        case KEY_SHIFT_END: end(ed, 1); break;

        case KEY_SHIFT_CTRL_RIGHT: wordright(ed, 1); break;
        case KEY_SHIFT_CTRL_LEFT: wordleft(ed, 1); break;
        case KEY_SHIFT_CTRL_HOME: top(ed, 1); break;
        case KEY_SHIFT_CTRL_END: bottom(ed, 1); break;
#endif

        case KEY_CTRL_TAB: ed = next_file(ed); break;

        case ctrl('e'): select_toggle(ed); break;
        case ctrl('a'): select_all(ed); break;
        case ctrl('c'): copy_selection(ed);select_toggle(ed); break;
        case ctrl('f'): find_text(ed, 0); break;
        case ctrl('l'): goto_line(ed); break;
        case ctrl('g'): find_text(ed, 1); break;
        case ctrl('q'): done = 1; break;
#ifdef LESS
        case KEY_ESC: done = 1; break;
#else
        case KEY_TAB: indent(ed, (unsigned char*) INDENT); break;
        case KEY_SHIFT_TAB: unindent(ed, (unsigned char*) INDENT); break;

        case KEY_ENTER: newline(ed); break;
        case KEY_BACKSPACE: backspace(ed); break;
        case KEY_DEL: del(ed); break;
        case ctrl('x'): cut_selection(ed); break;
        case ctrl('z'): undo(ed); break;
        case ctrl('r'): redo(ed); break;
        case ctrl('v'): paste_selection(ed); break;
        case ctrl('o'): open_editor(ed); ed = ed->env->current; break;
        case ctrl('n'): new_editor(ed); ed = ed->env->current; break;
        case ctrl('s'): save_editor(ed); break;
        case ctrl('p'): pipe_command(ed); break;
#endif
        case ctrl('w'): ed = close_editor(ed); break;
      }
    }
  }
}

//
// main
//
static int rtems_shell_main_edit(int argc, char *argv[])
{
  struct env env;
  int rc;
  int i;
  sigset_t blocked_sigmask, orig_sigmask;
#if defined(__linux__)
  struct termios tio;
#endif
#if defined(__linux__) || defined(__rtems__)
  struct termios orig_tio;
#endif
#ifdef SANOS
  struct term *term;
#endif

  memset(&env, 0, sizeof(env));
  for (i = 1; i < argc; i++) {
    struct editor *ed = create_editor(&env);
    rc = load_file(ed, argv[i]);
    if (rc < 0 && errno == ENOENT) rc = new_file(ed, argv[i]);
    if (rc < 0) {
      perror(argv[i]);
      return 0;
    }
  }
  if (env.current == NULL) {
    struct editor *ed = create_editor(&env);
    if (isatty(fileno(stdin))) {
      new_file(ed, "");
    } else {
      read_from_stdin(ed);
    }
  }
  env.current = env.current->next;

#ifdef SANOS
  term = gettib()->proc->term;
  if (fdin != term->ttyin) dup2(term->ttyin, fdin);
  if (fdout != term->ttyout) dup2(term->ttyout, fdout);
#elif !defined(__rtems__)
  if (!isatty(fileno(stdin))) {
    if (!freopen("/dev/tty", "r", stdin)) perror("/dev/tty");
  }
#endif

  setvbuf(stdout, NULL, 0, 8192);

#if defined(__linux__) || defined(__rtems__)
  (void) tcgetattr(0, &orig_tio);
#if !defined(__rtems__)
  cfmakeraw(&tio);
  tcsetattr(0, TCSANOW, &tio);
#endif
  if (getenv("TERM") && strcmp(getenv("TERM"), "linux") == 0) {
    linux_console = 1;
  } else {
    outstr(CLRSCR);
    outstr("\033[3 q");  // xterm
    outstr("\033]50;CursorShape=2\a");  // KDE
  }
#endif

  get_console_size(&env);

  sigemptyset(&blocked_sigmask);
  sigaddset(&blocked_sigmask, SIGINT);
  sigaddset(&blocked_sigmask, SIGTSTP);
  sigaddset(&blocked_sigmask, SIGABRT);
  sigprocmask(SIG_BLOCK, &blocked_sigmask, &orig_sigmask);

  for (;;) {
    if (!env.current) break;
    edit(env.current);
    if (quit(&env)) break;
  }

  gotoxy(0, env.lines + 1);
  outstr(RESET_COLOR CLREOL);
#if defined(__linux__) || defined(__rtems__)
  tcsetattr(0, TCSANOW, &orig_tio);
#endif

  while (env.current) delete_editor(env.current);

  if (env.clipboard) free(env.clipboard);
  if (env.search) free(env.search);
  if (env.linebuf) free(env.linebuf);

  setbuf(stdout, NULL);
  sigprocmask(SIG_SETMASK, &orig_sigmask, NULL);

  return 0;
}

rtems_shell_cmd_t rtems_shell_EDIT_Command = {
  .name = "edit",
  .usage = "edit [file ...]",
  .topic = "files",
  .command = rtems_shell_main_edit,
  .alias = NULL,
  .next = NULL
};
