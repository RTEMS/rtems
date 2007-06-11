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

static const char	*config_file = CONFIG;

#if !defined(NO_GUI)

static HICON		hIcon;			/* SHTTPD icon handle	*/
HWND			hLog;			/* Log window		*/

/*
 * Dialog box control IDs
 */
#define ID_GROUP	100
#define ID_SAVE		101
#define	ID_STATUS	102
#define	ID_STATIC	103
#define	ID_SETTINGS	104
#define	ID_QUIT		105
#define	ID_TRAYICON	106
#define	ID_TIMER	107
#define	ID_ICON		108
#define	ID_ADVANCED	109
#define	ID_SHOWLOG	110
#define	ID_LOG		111

#define	ID_USER		200
#define	ID_DELTA	1000

static void
run_server(void *param)
{
	struct shttpd_ctx	*ctx = param;

	if (shttpd_listen(ctx, ctx->port) == -1)
		elog(E_FATAL, NULL, "Cannot open socket on port %d", ctx->port);

	while (WaitForSingleObject(ctx->ev[0], 0) != WAIT_OBJECT_0)
		shttpd_poll(ctx, 1000);

	SetEvent(ctx->ev[1]);
	shttpd_fini(ctx);
}

/*
 * Save the configuration back into config file
 */
static void
save_config(HWND hDlg, FILE *fp)
{
	const struct opt	*opt;
	char			text[FILENAME_MAX];
	int			id;

	if (fp == NULL)
		elog(E_FATAL, NULL, "save_config: cannot open %s", config_file);

	for (opt = options; opt->name != NULL; opt++) {
		id = ID_USER + (opt - options);		/* Control ID */

		/* Do not save if the text is the same as default */

		if (opt->flags & OPT_BOOL)
			(void) fprintf(fp, "%s\t%d\n",
			    opt->name, IsDlgButtonChecked(hDlg, id));
		else if (GetDlgItemText(hDlg, id, text, sizeof(text)) != 0 &&
		    (opt->def == NULL || strcmp(text, opt->def) != 0))
			(void) fprintf(fp, "%s\t%s\n", opt->name, text);
	}

	(void) fclose(fp);
}

static void
set_control_values(HWND hDlg, const struct shttpd_ctx *ctx)
{
	const struct opt	*opt;
	const union variant	*v;
	char			buf[FILENAME_MAX];
	int			id;

	for (opt = options; opt->name != NULL; opt++) {
		id = ID_USER + (opt - options);
		v = (union variant *) ((char *) ctx + opt->ofs);
		if (opt->flags & OPT_BOOL) {
			CheckDlgButton(hDlg, id,
			    v->v_int ? BST_CHECKED : BST_UNCHECKED);
		} else if (opt->flags & OPT_INT) {
			my_snprintf(buf, sizeof(buf), "%d", v->v_int);
			SetDlgItemText(hDlg, id, buf);
		} else {
			SetDlgItemText(hDlg, id, v->v_str);
		}
	}

}

static BOOL CALLBACK
DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static struct shttpd_ctx *ctx, **pctx;
	HANDLE		ev;
	const struct opt *opt;
	DWORD tid;
	int		id, up;
	char		text[256];

	switch (msg) {

	case WM_CLOSE:
		KillTimer(hDlg, ID_TIMER);
		DestroyWindow(hDlg);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_SAVE:
			EnableWindow(GetDlgItem(hDlg, ID_SAVE), FALSE);
			save_config(hDlg, fopen(config_file, "w+"));
			ev = ctx->ev[1];
			SetEvent(ctx->ev[0]);
			WaitForSingleObject(ev, INFINITE);
			*pctx = ctx = init_from_argc_argv(config_file, 0, NULL);
			shttpd_listen(ctx, ctx->port);
			_beginthread(run_server, 0, ctx);
			EnableWindow(GetDlgItem(hDlg, ID_SAVE), TRUE);

			break;
		}

		id = ID_USER + ID_DELTA;
		for (opt = options; opt->name != NULL; opt++, id++)
			if (LOWORD(wParam) == id) {
				OPENFILENAME	of;
				BROWSEINFO	bi;
				char		path[FILENAME_MAX] = "";

				memset(&of, 0, sizeof(of));
				of.lStructSize = sizeof(of);
				of.hwndOwner = (HWND) hDlg;
				of.lpstrFile = path;
				of.nMaxFile = sizeof(path);
				of.lpstrInitialDir = ctx->document_root;
				of.Flags = OFN_CREATEPROMPT | OFN_NOCHANGEDIR;
				
				memset(&bi, 0, sizeof(bi));
				bi.hwndOwner = (HWND) hDlg;
				bi.lpszTitle = "Choose WWW root directory:";
				bi.ulFlags = BIF_RETURNONLYFSDIRS;

				if (opt->flags & OPT_DIR)
					SHGetPathFromIDList(
						SHBrowseForFolder(&bi), path);
				else
					GetOpenFileName(&of);

				if (path[0] != '\0')
					SetWindowText(GetDlgItem(hDlg,
						id - ID_DELTA), path);
			}

		break;

	case WM_INITDIALOG:
		pctx = (struct shttpd_ctx **) lParam;
		ctx = *pctx;
		SendMessage(hDlg,WM_SETICON,(WPARAM)ICON_SMALL,(LPARAM)hIcon);
		SendMessage(hDlg,WM_SETICON,(WPARAM)ICON_BIG,(LPARAM)hIcon);
		SetWindowText(hDlg, "SHTTPD settings");
		SetFocus(GetDlgItem(hDlg, ID_SAVE));
		set_control_values(hDlg, ctx);
		break;
	default:
		break;
	}

	return FALSE;
}

static void *
align(void *ptr, DWORD alig)
{
	ULONG ul = (ULONG) ptr;

	ul += alig;
	ul &= ~alig;
	
	return ((void *) ul);
}


static void
add_control(unsigned char **mem, DLGTEMPLATE *dia, WORD type, DWORD id,
	DWORD style, WORD x, WORD y, WORD cx, WORD cy, const char *caption)
{
	DLGITEMTEMPLATE	*tp;
	LPWORD		p;

	dia->cdit++;

	*mem = align(*mem, 3);
	tp = (DLGITEMTEMPLATE *) *mem;

	tp->id			= (WORD)id;
	tp->style		= style;
	tp->dwExtendedStyle	= 0;
	tp->x			= x;
	tp->y			= y;
	tp->cx			= cx;
	tp->cy			= cy;

	p = align(*mem + sizeof(*tp), 1);
	*p++ = 0xffff;
	*p++ = type;

	while (*caption != '\0')
		*p++ = (WCHAR) *caption++;
	*p++ = 0;
	p = align(p, 1);

	*p++ = 0;
	*mem = (unsigned char *) p;
}

static void
show_settings_dialog(struct shttpd_ctx **ctxp)
{
#define	HEIGHT		15
#define	WIDTH		400
#define	LABEL_WIDTH	70

	unsigned char		mem[4096], *p;
	DWORD			style;
	DLGTEMPLATE		*dia = (DLGTEMPLATE *) mem;
	WORD			cl, x, y, width, nelems = 0;
	const struct opt	*opt;
	static int		guard;

	static struct {
		DLGTEMPLATE	template;	/* 18 bytes */
		WORD		menu, class;
		wchar_t		caption[1];
		WORD		fontsiz;
		wchar_t		fontface[7];
	} dialog_header = {{WS_CAPTION | WS_POPUP | WS_SYSMENU | WS_VISIBLE |
		DS_SETFONT | WS_DLGFRAME, WS_EX_TOOLWINDOW,
		0, 200, 200, WIDTH, 0}, 0, 0, L"", 8, L"Tahoma"};

	if (guard == 0)
		guard++;
	else
		return;	

	(void) memset(mem, 0, sizeof(mem));
	(void) memcpy(mem, &dialog_header, sizeof(dialog_header));
	p = mem + sizeof(dialog_header);

	for (opt = options; opt->name != NULL; opt++) {

		style = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
		x = 10 + (WIDTH / 2) * (nelems % 2);
		y = (nelems/2 + 1) * HEIGHT + 5;
		width = WIDTH / 2 - 20 - LABEL_WIDTH;
		if (opt->flags & OPT_INT) {
			style |= ES_NUMBER;
			cl = 0x81;
			style |= WS_BORDER | ES_AUTOHSCROLL;
		} else if (opt->flags & OPT_BOOL) {
			cl = 0x80;
			style |= BS_AUTOCHECKBOX;
		} else if (opt->flags & (OPT_DIR | OPT_FILE)) {
			style |= WS_BORDER | ES_AUTOHSCROLL;
			width -= 20;
			cl = 0x81;
			add_control(&p, dia, 0x80,
				ID_USER + ID_DELTA + (opt - options),
				WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
				(WORD) (x + width + LABEL_WIDTH + 5),
				y, 15, 12, "...");
		} else {
			cl = 0x81;
			style |= WS_BORDER | ES_AUTOHSCROLL;
		}
		add_control(&p, dia, 0x82, ID_STATIC, WS_VISIBLE | WS_CHILD,
			x, y, LABEL_WIDTH, HEIGHT, opt->desc);
		add_control(&p, dia, cl, ID_USER + (opt - options), style,
			(WORD) (x + LABEL_WIDTH), y, width, 12, "");
		nelems++;
	}

	y = (WORD) (((nelems + 1)/2 + 1) * HEIGHT + 5);
	add_control(&p, dia, 0x80, ID_GROUP, WS_CHILD | WS_VISIBLE |
		BS_GROUPBOX, 5, 5, WIDTH - 10, y, "Settings");
	y += 10;
	add_control(&p, dia, 0x80, ID_SAVE,
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
		WIDTH - 70, y, 65, 12, "Save Settings");
#if 0
	add_control(&p, dia, 0x80, ID_ADVANCED,
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
		WIDTH - 190, y, 110, 12, "Show Advanced Settings >>");
#endif
	add_control(&p, dia, 0x82, ID_STATIC,
		WS_CHILD | WS_VISIBLE | WS_DISABLED,
		5, y, 180, 12,"SHTTPD v." VERSION
		"      (http://shttpd.sourceforge.net)");
	
	dia->cy = ((nelems + 1)/2 + 1) * HEIGHT + 30;
	DialogBoxIndirectParam(NULL, dia, NULL, DlgProc, (LPARAM) ctxp);
	guard--;
}

static BOOL CALLBACK
LogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static struct shttpd_ctx	*ctx;
	static HWND	hStatus;
	HWND		hEdit;
	RECT		rect, rect2, rect3, rect4;
	int		len, up, widths[] = {120, 220, 330, 460, -1};
	char		text[256], buf[1024 * 64];

	switch (msg) {

	case WM_CLOSE:
		KillTimer(hDlg, ID_TIMER);
		DestroyWindow(hDlg);
		break;

	case WM_APP:
		hEdit = GetDlgItem(hDlg, ID_LOG);
		len = GetWindowText(hEdit, buf, sizeof(buf));
		if (len > sizeof(buf) * 4 / 5)
			len = sizeof(buf) * 4 / 5;
		my_snprintf(buf + len, sizeof(buf) - len,
		    "%s\r\n", (char *) lParam);
		SetWindowText(hEdit, buf);
		SendMessage(hEdit, WM_VSCROLL, SB_BOTTOM, 0);
		break;

	case WM_TIMER:
		/* Print statistics on a status bar */
		up = current_time - ctx->start_time;
		(void) my_snprintf(text, sizeof(text),
		    " Up: %3d h %2d min %2d sec",
		    up / 3600, up / 60 % 60, up % 60);
		SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM) text);
		(void) my_snprintf(text, sizeof(text),
		    " Requests: %u", ctx->nrequests);
		SendMessage(hStatus, SB_SETTEXT, 1, (LPARAM) text);
		(void) my_snprintf(text, sizeof(text),
		    " Sent: %4.2f Mb", (double) ctx->out / 1048576);
		SendMessage(hStatus, SB_SETTEXT, 2, (LPARAM) text);
		(void) my_snprintf(text, sizeof(text),
		    " Received: %4.2f Mb", (double) ctx->in / 1048576);
		SendMessage(hStatus, SB_SETTEXT, 3, (LPARAM) text);
		break;

	case WM_INITDIALOG:
		ctx = (struct shttpd_ctx *) lParam;
		SendMessage(hDlg,WM_SETICON,(WPARAM)ICON_SMALL,(LPARAM)hIcon);
		SendMessage(hDlg,WM_SETICON,(WPARAM)ICON_BIG,(LPARAM)hIcon);
		hStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE,
			"", hDlg, ID_STATUS);
		SendMessage(hStatus, SB_SETPARTS, 5, (LPARAM) widths);
		SendMessage(hStatus, SB_SETTEXT, 4, (LPARAM) " Running");
		SetWindowText(hDlg, "SHTTPD web server log");
		SetTimer(hDlg, ID_TIMER, 1000, NULL);
		GetWindowRect(GetDesktopWindow(), &rect3);
		GetWindowRect(hDlg, &rect4);
		GetClientRect(hDlg, &rect);
		GetClientRect(hStatus, &rect2);
		SetWindowPos(GetDlgItem(hDlg, ID_LOG), 0,
			0, 0, rect.right, rect.bottom - rect2.bottom, 0);
		SetWindowPos(hDlg, HWND_TOPMOST,
				rect3.right - (rect4.right - rect4.left),
				rect3.bottom - (rect4.bottom - rect4.top) - 30,
				0, 0, SWP_NOSIZE);
		SetFocus(hStatus);
		SendMessage(hDlg, WM_TIMER, 0, 0);
		hLog = hDlg;
		break;
	default:
		break;
	}


	return (FALSE);
}

static void
show_log_window(struct shttpd_ctx *ctx)
{
	unsigned char		mem[4096], *p;
	DWORD			style;
	DLGTEMPLATE		*dia = (DLGTEMPLATE *) mem;
	WORD			cl, x, y, width, nelems = 0;

	static struct {
		DLGTEMPLATE	template;	/* 18 bytes */
		WORD		menu, class;
		wchar_t		caption[1];
		WORD		fontsiz;
		wchar_t		fontface[7];
	} dialog_header = {{WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_SYSMENU |
		DS_SETFONT | WS_DLGFRAME, WS_EX_TOOLWINDOW,
		0, 200, 200, 400, 100}, 0, 0, L"", 8, L"Tahoma"};

	if (hLog != NULL)
		return;	

	(void) memset(mem, 0, sizeof(mem));
	(void) memcpy(mem, &dialog_header, sizeof(dialog_header));
	p = mem + sizeof(dialog_header);

	add_control(&p, dia, 0x81, ID_LOG, WS_CHILD | WS_VISIBLE |
	    WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL |
	    ES_READONLY, 5, 5, WIDTH - 10, 60, "");

	DialogBoxIndirectParam(NULL, dia, NULL, LogProc, (LPARAM) ctx);

	hLog = NULL;
}

static LRESULT CALLBACK
WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static NOTIFYICONDATA	ni;
	static struct shttpd_ctx *ctx;
	DWORD			tid;		/* Thread ID */
	HMENU			hMenu;
	POINT			pt;

	switch (msg) {
	case WM_CREATE:
		ctx = ((CREATESTRUCT *) lParam)->lpCreateParams;
		memset(&ni, 0, sizeof(ni));
		ni.cbSize = sizeof(ni);
		ni.uID = ID_TRAYICON;
		ni.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		ni.hIcon = hIcon;
		ni.hWnd = hWnd;
		my_snprintf(ni.szTip, sizeof(ni.szTip), "SHTTPD web server");
		ni.uCallbackMessage = WM_USER;
		Shell_NotifyIcon(NIM_ADD, &ni);
		ctx->ev[0] = CreateEvent(0, TRUE, FALSE, 0);
		ctx->ev[1] = CreateEvent(0, TRUE, FALSE, 0);
		_beginthread(run_server, 0, ctx);
		break;
	case WM_CLOSE:
		Shell_NotifyIcon(NIM_DELETE, &ni);
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_SETTINGS:
			show_settings_dialog(&ctx);
			break;
		case ID_QUIT:
			SendMessage(hWnd, WM_CLOSE, wParam, lParam);
			PostQuitMessage(0);
			break;
		case ID_SHOWLOG:
			show_log_window(ctx);
			break;
		}
		break;
	case WM_USER:
		switch (lParam) {
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
			hMenu = CreatePopupMenu();
			AppendMenu(hMenu, 0, ID_SETTINGS, "Settings");
			AppendMenu(hMenu, 0, ID_SHOWLOG, "Show Log");
			AppendMenu(hMenu, 0, ID_QUIT, "Exit SHTTPD");
			GetCursorPos(&pt);
			TrackPopupMenu(hMenu, 0, pt.x, pt.y, 0, hWnd, NULL);
			DestroyMenu(hMenu);
			break;
		}
		break;
	}

	return (DefWindowProc(hWnd, msg, wParam, lParam));
}

int WINAPI
WinMain(HINSTANCE h, HINSTANCE prev, char *cmdline, int show)
{
	struct shttpd_ctx	*ctx;
	WNDCLASS		cls;
	HWND			hWnd;
	MSG			msg;

	ctx = init_from_argc_argv(config_file, 0, NULL);
	(void) memset(&cls, 0, sizeof(cls));

	hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON));
	if (hIcon == NULL)
		hIcon = LoadIcon(NULL, IDI_APPLICATION);
	cls.lpfnWndProc = (WNDPROC) WindowProc; 
	cls.hIcon = hIcon;
	cls.lpszClassName = "shttpd v." VERSION; 

	if (!RegisterClass(&cls)) 
		elog(E_FATAL, NULL, "RegisterClass: %d", ERRNO);
	else if ((hWnd = CreateWindow(cls.lpszClassName, "",WS_OVERLAPPEDWINDOW,
	    0, 0, 0, 0, NULL, NULL, NULL, ctx)) == NULL)
		elog(E_FATAL, NULL, "CreateWindow: %d", ERRNO);

	while (GetMessage(&msg, (HWND) NULL, 0, 0)) { 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}

	return (0);
}
#endif /* NO_GUI */

static void
fix_directory_separators(char *path)
{
	for (; *path != '\0'; path++) {
		if (*path == '/')
			*path = '\\';
		if (*path == '\\')
			while (path[1] == '\\' || path[1] == '/') 
				(void) memmove(path + 1,
				    path + 2, strlen(path + 2) + 1);
	}
}

int
my_open(const char *path, int flags, int mode)
{
	char	buf[FILENAME_MAX];
	wchar_t	wbuf[FILENAME_MAX];

	my_strlcpy(buf, path, sizeof(buf));
	fix_directory_separators(buf);
	MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, sizeof(wbuf));

	return (_wopen(wbuf, flags));
}

int
my_stat(const char *path, struct stat *stp)
{
	char	buf[FILENAME_MAX], *p;
	wchar_t	wbuf[FILENAME_MAX];

	my_strlcpy(buf, path, sizeof(buf));
	fix_directory_separators(buf);

	p = buf + strlen(buf) - 1;
	while (p > buf && *p == '\\' && p[-1] != ':')
		*p-- = '\0';

	MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, sizeof(wbuf));

	return (_wstat(wbuf, (struct _stat *) stp));
}

int
my_remove(const char *path)
{
	char	buf[FILENAME_MAX];
	wchar_t	wbuf[FILENAME_MAX];

	my_strlcpy(buf, path, sizeof(buf));
	fix_directory_separators(buf);

	MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, sizeof(wbuf));

	return (_wremove(wbuf));
}

int
my_rename(const char *path1, const char *path2)
{
	char	buf1[FILENAME_MAX];
	char	buf2[FILENAME_MAX];
	wchar_t	wbuf1[FILENAME_MAX];
	wchar_t	wbuf2[FILENAME_MAX];

	my_strlcpy(buf1, path1, sizeof(buf1));
	my_strlcpy(buf2, path2, sizeof(buf2));
	fix_directory_separators(buf1);
	fix_directory_separators(buf2);

	MultiByteToWideChar(CP_UTF8, 0, buf1, -1, wbuf1, sizeof(wbuf1));
	MultiByteToWideChar(CP_UTF8, 0, buf2, -1, wbuf2, sizeof(wbuf2));

	return (_wrename(wbuf1, wbuf2));
}

int
my_mkdir(const char *path, int mode)
{
	char	buf[FILENAME_MAX];
	wchar_t	wbuf[FILENAME_MAX];

	my_strlcpy(buf, path, sizeof(buf));
	fix_directory_separators(buf);

	MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, sizeof(wbuf));

	return (_wmkdir(wbuf));
}

static char *
wide_to_utf8(const wchar_t *str)
{
	char *buf = NULL;
	if (str) {
		int nchar = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
		if (nchar > 0) {
			buf = malloc(nchar);
			if (!buf)
				errno = ENOMEM;
			else if (!WideCharToMultiByte(CP_UTF8, 0, str, -1, buf, nchar, NULL, NULL)) {
				free(buf);
				buf = NULL;
				errno = EINVAL;
			}
		} else
			errno = EINVAL;
	} else
		errno = EINVAL;
	return buf;
}

char *
my_getcwd(char *buffer, int maxlen)
{
	char *result = NULL;
	wchar_t *wbuffer, *wresult;

	if (buffer) {
		/* User-supplied buffer */
		wbuffer = malloc(maxlen * sizeof(wchar_t));
		if (wbuffer == NULL)
			return NULL;
	} else
		/* Dynamically allocated buffer */
		wbuffer = NULL;
	wresult = _wgetcwd(wbuffer, maxlen);
	if (wresult) {
		int err = errno;
		if (buffer) {
			/* User-supplied buffer */
			int n = WideCharToMultiByte(CP_UTF8, 0, wresult, -1, buffer, maxlen, NULL, NULL);
			if (n == 0)
				err = ERANGE;
			free(wbuffer);
			result = buffer;
		} else {
			/* Buffer allocated by _wgetcwd() */
			result = wide_to_utf8(wresult);
			err = errno;
			free(wresult);
		}
		errno = err;
	}
	return result;
}

DIR *
opendir(const char *name)
{
	DIR		*dir = NULL;
	char		path[FILENAME_MAX];
	wchar_t		wpath[FILENAME_MAX];

	if (name == NULL || name[0] == '\0') {
		errno = EINVAL;
	} else if ((dir = malloc(sizeof(*dir))) == NULL) {
		errno = ENOMEM;
	} else {
		my_snprintf(path, sizeof(path), "%s/*", name);
		fix_directory_separators(path);
		MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, sizeof(wpath));
		dir->handle = FindFirstFileW(wpath, &dir->info);

		if (dir->handle != INVALID_HANDLE_VALUE) {
			dir->result.d_name[0] = '\0';
		} else {
			free(dir);
			dir = NULL;
		}
	}

	return (dir);
}

int
closedir(DIR *dir)
{
	int result = -1;

	if (dir != NULL) {
		if (dir->handle != INVALID_HANDLE_VALUE)
			result = FindClose(dir->handle) ? 0 : -1;

		free(dir);
	}

	if (result == -1) 
		errno = EBADF;

	return (result);
}

struct dirent *
readdir(DIR *dir)
{
	struct dirent *result = 0;

	if (dir && dir->handle != INVALID_HANDLE_VALUE) {
		if(!dir->result.d_name ||
		    FindNextFileW(dir->handle, &dir->info)) {
			result = &dir->result;

			WideCharToMultiByte(CP_UTF8, 0, dir->info.cFileName,
			    -1, result->d_name,
			    sizeof(result->d_name), NULL, NULL);
		}
	} else {
		errno = EBADF;
	}

	return (result);
}

int
set_non_blocking_mode(int fd)
{
	unsigned long	on = 1;

	return (ioctlsocket(fd, FIONBIO, &on));
}

void
set_close_on_exec(int fd)
{
	fd = 0;	/* Do nothing. There is no FD_CLOEXEC on Windows */
}

#if !defined(NO_CGI)

struct threadparam {
	SOCKET	s;
	HANDLE	hPipe;
	big_int_t content_len;
};

/*
 * Thread function that reads POST data from the socket pair
 * and writes it to the CGI process.
 */
static void//DWORD WINAPI
stdoutput(void *arg)
{
	struct threadparam	*tp = arg;
	int			n, sent, stop = 0;
	big_int_t		total = 0;
	DWORD k;
	char			buf[BUFSIZ];
	size_t			max_recv;

	max_recv = min(sizeof(buf), tp->content_len - total);
	while (!stop && max_recv > 0 && (n = recv(tp->s, buf, max_recv, 0)) > 0) {
		for (sent = 0; !stop && sent < n; sent += k)
			if (!WriteFile(tp->hPipe, buf + sent, n - sent, &k, 0))
				stop++;
		total += n;
		max_recv = min(sizeof(buf), tp->content_len - total);
	}
	
	CloseHandle(tp->hPipe);	/* Suppose we have POSTed everything */
	free(tp);
}

/*
 * Thread function that reads CGI output and pushes it to the socket pair.
 */
static void
stdinput(void *arg)
{
	struct threadparam	*tp = arg;
	static			int ntotal;
	int			k, stop = 0;
	DWORD n, sent;
	char			buf[BUFSIZ];

	while (!stop && ReadFile(tp->hPipe, buf, sizeof(buf), &n, NULL)) {
		ntotal += n;
		for (sent = 0; !stop && sent < n; sent += k)
			if ((k = send(tp->s, buf + sent, n - sent, 0)) <= 0)
				stop++;
	}
	CloseHandle(tp->hPipe);
	
	/*
	 * Windows is a piece of crap. When this thread closes its end
	 * of the socket pair, the other end (get_cgi() function) may loose
	 * some data. I presume, this happens if get_cgi() is not fast enough,
	 * and the data written by this end does not "push-ed" to the other
	 * end socket buffer. So after closesocket() the remaining data is
	 * gone. If I put shutdown() before closesocket(), that seems to
	 * fix the problem, but I am not sure this is the right fix.
	 * XXX (submitted by James Marshall) we do not do shutdown() on UNIX.
	 * If fork() is called from user callback, shutdown() messes up things.
	 */
	shutdown(tp->s, 2);

	closesocket(tp->s);
	free(tp);

	_endthread();
}

static void
spawn_stdio_thread(int sock, HANDLE hPipe, void (*func)(void *),
		big_int_t content_len)
{
	struct threadparam	*tp;
	DWORD			tid;

	tp = malloc(sizeof(*tp));
	assert(tp != NULL);

	tp->s		= sock;
	tp->hPipe	= hPipe;
	tp->content_len = content_len;
	_beginthread(func, 0, tp);
}

int
spawn_process(struct conn *c, const char *prog, char *envblk,
		char *envp[], int sock, const char *dir)
{
	HANDLE			a[2], b[2], h[2], me;
	DWORD			flags;
	char			*p, cmdline[FILENAME_MAX], line[FILENAME_MAX];
	FILE			*fp;
	STARTUPINFOA	si;
	PROCESS_INFORMATION	pi;

	me = GetCurrentProcess();
	flags = DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS;

	/* FIXME add error checking code here */
	CreatePipe(&a[0], &a[1], NULL, 0);
	CreatePipe(&b[0], &b[1], NULL, 0);
	DuplicateHandle(me, a[0], me, &h[0], 0, TRUE, flags);
	DuplicateHandle(me, b[1], me, &h[1], 0, TRUE, flags);
	
	(void) memset(&si, 0, sizeof(si));
	(void) memset(&pi, 0, sizeof(pi));

	/* XXX redirect CGI errors to the error log file */
	si.cb		= sizeof(si);
	si.dwFlags	= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow	= SW_HIDE;
	si.hStdOutput	= si.hStdError = h[1];
	si.hStdInput	= h[0];

	/* If CGI file is a script, try to read the interpreter line */
	if (c->ctx->cgi_interpreter == NULL) {
		if ((fp = fopen(prog, "r")) != NULL) {
			(void) fgets(line, sizeof(line), fp);
			if (memcmp(line, "#!", 2) != 0)
				line[2] = '\0';
			/* Trim whitespaces from interpreter name */
			for (p = &line[strlen(line) - 1]; p > line &&
			    isspace(*p); p--)
				*p = '\0';
			(void) fclose(fp);
		}
		(void) my_snprintf(cmdline, sizeof(cmdline), "%s%s%s",
		    line + 2, line[2] == '\0' ? "" : " ", prog);
	} else {
		(void) my_snprintf(cmdline, sizeof(cmdline), "%s %s",
		    c->ctx->cgi_interpreter, prog);
	}

	(void) my_snprintf(line, sizeof(line), "%s", dir);
	fix_directory_separators(line);
	fix_directory_separators(cmdline);

	/*
	 * Spawn reader & writer threads before we create CGI process.
	 * Otherwise CGI process may die too quickly, loosing the data
	 */
	spawn_stdio_thread(sock, b[0], stdinput, 0);
	spawn_stdio_thread(sock, a[1], stdoutput, c->rem.content_len);

	if (CreateProcessA(NULL, cmdline, NULL, NULL, TRUE,
	    CREATE_NEW_PROCESS_GROUP, envblk, line, &si, &pi) == 0) {
		elog(E_LOG, c,"redirect: CreateProcess(%s): %d",cmdline,ERRNO);
		return (-1);
	} else {
		CloseHandle(h[0]);
		CloseHandle(h[1]);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	return (0);
}

#endif /* !NO_CGI */
