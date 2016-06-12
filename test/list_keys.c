/****************************************************************************
 * Copyright (c) 2016 Free Software Foundation, Inc.                        *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/
/*
 * $Id: list_keys.c,v 1.4 2016/06/04 22:39:47 tom Exp $
 *
 * Author: Thomas E Dickey
 *
 * List function keys for one or more terminals.
 */

#define USE_TINFO
#include <test.priv.h>

#if NCURSES_XNAMES
#if HAVE_TERM_ENTRY_H
#include <term_entry.h>
#else
#undef NCURSES_XNAMES
#define NCURSES_XNAMES 0
#endif
#endif

#if HAVE_TIGETSTR
#if defined(HAVE_CURSES_DATA_BOOLNAMES) || defined(DECL_CURSES_DATA_BOOLNAMES)

static bool f_opt = FALSE;
static bool t_opt = FALSE;
static bool x_opt = FALSE;

static const char *
full_name(const char *name)
{
    const char *result = name;
    int n;
    for (n = 0; strnames[n] != 0; ++n) {
	if (!strcmp(name, strnames[n])) {
	    result = strfnames[n];
	    break;
	}
    }
    return result;
}

static int
show_key(const char *name, bool show)
{
    int width = 0;
    char buffer[10];
    char *value = tigetstr(name);

    if (show && t_opt)
	fputc('"', stdout);

    if (value != 0 && value != (char *) -1) {
	while (*value != 0) {
	    int ch = UChar(*value++);
	    switch (ch) {
	    case '\177':
		strcpy(buffer, "^?");
		break;
	    case '\033':
		strcpy(buffer, "\\E");
		break;
	    case '\b':
		strcpy(buffer, "\\b");
		break;
	    case '\f':
		strcpy(buffer, "\\f");
		break;
	    case '\n':
		strcpy(buffer, "\\n");
		break;
	    case '\r':
		strcpy(buffer, "\\r");
		break;
	    case ' ':
		strcpy(buffer, "\\s");
		break;
	    case '\t':
		strcpy(buffer, "\\t");
		break;
	    case '^':
		strcpy(buffer, "\\^");
		break;
	    case ':':
		strcpy(buffer, "\\072");
		break;
	    case '\\':
		strcpy(buffer, "\\\\");
		break;
	    default:
		if (t_opt && ch == '"') {
		    strcpy(buffer, "\"\"");
		} else if (isgraph(ch)) {
		    sprintf(buffer, "%c", ch);
		} else if (ch < 32) {
		    sprintf(buffer, "^%c", ch + '@');
		} else {
		    sprintf(buffer, "\\%03o", ch);
		}
		break;
	    }
	    width += (int) strlen(buffer);
	    if (show)
		fputs(buffer, stdout);
	}
    }

    if (show && t_opt)
	fputc('"', stdout);

    return width;
}

static bool
valid_key(const char *name, TERMINAL ** terms, int count)
{
    bool result = FALSE;
    if (*name == 'k') {
	int k;
	for (k = 0; k < count; ++k) {
	    set_curterm(terms[k]);
	    if (show_key(name, FALSE)) {
		result = TRUE;
		break;
	    }
	}
    }
    return result;
}

static void
list_keys(TERMINAL ** terms, int count)
{
    int j, k;
    int widths0 = 0;
    int widths1 = 0;
    int check;
    int total = 0;
    const char *name = f_opt ? "strfname" : "strname";
    const char **list;

    for (total = 0; strnames[total]; ++total) {
	;
    }
#if NCURSES_XNAMES
    if (x_opt) {
	TERMTYPE *term;
	for (k = 0; k < count; ++k) {
	    set_curterm(terms[k]);
	    term = &(cur_term->type);
	    total += NUM_STRINGS(term) - STRCOUNT;
	}
    }
#endif
    list = typeCalloc(const char *, total + 1);
    for (j = 0; strnames[j]; ++j) {
	list[j] = strnames[j];
    }
#if NCURSES_XNAMES
    if (x_opt) {
	TERMTYPE *term;
	int m, n;
	for (k = 0; k < count; ++k) {
	    set_curterm(terms[k]);
	    term = &(cur_term->type);
	    for (n = STRCOUNT; n < NUM_STRINGS(term); ++n) {
		bool found = FALSE;
		const char *estr = ExtStrname(term, (int) n, strnames);
		for (m = STRCOUNT; m < j; ++m) {
		    if (!strcmp(estr, list[m])) {
			found = TRUE;
			break;
		    }
		}
		if (!found) {
		    list[j++] = estr;
		}
	    }
	}
    }
#endif

    widths0 = (int) strlen(name);
    for (k = 0; k < count; ++k) {
	set_curterm(terms[k]);
	check = (int) strlen(name);
	if (widths1 < check)
	    widths1 = check;
    }
    for (j = 0; list[j] != 0; ++j) {
	if (valid_key(list[j], terms, count)) {
	    const char *label = f_opt ? full_name(list[j]) : list[j];
	    check = (int) strlen(label);
	    if (widths0 < check)
		widths0 = check;
	    for (k = 0; k < count; ++k) {
		set_curterm(terms[k]);
		check = show_key(list[j], FALSE);
		if (widths1 < check)
		    widths1 = check;
	    }
	}
    }

    if (t_opt) {
	printf("\"%s\"", name);
    } else {
	printf("%-*s", widths0, name);
    }
    for (k = 0; k < count; ++k) {
	set_curterm(terms[k]);
	if (t_opt) {
	    printf(",\"%s\"", termname());
	} else if (k + 1 >= count) {
	    printf(" %s", termname());
	} else {
	    printf(" %-*s", widths1, termname());
	}
    }
    printf("\n");

    for (j = 0; list[j] != 0; ++j) {
	if (valid_key(list[j], terms, count)) {
	    const char *label = f_opt ? full_name(list[j]) : list[j];
	    if (t_opt) {
		printf("\"%s\"", label);
	    } else {
		printf("%-*s", widths0, label);
	    }
	    for (k = 0; k < count; ++k) {
		printf(t_opt ? "," : " ");
		set_curterm(terms[k]);
		check = show_key(list[j], TRUE);
		if (!t_opt) {
		    if (k + 1 < count) {
			printf("%*s", widths1 + 1 - check, " ");
		    }
		}
	    }
	    printf("\n");
	}
    }
}

static void
usage(void)
{
    static const char *msg[] =
    {
	"Usage: list_keys [options] [terminal [terminal2 [...]]]",
	"",
	"Print capabilities for terminal special keys.",
	"",
	"Options:",
	" -f       print full names",
	" -t       print result as CSV table",
#ifdef NCURSES_VERSION
	" -x       print extended capabilities",
#endif
    };
    unsigned n;
    for (n = 0; n < SIZEOF(msg); ++n) {
	fprintf(stderr, "%s\n", msg[n]);
    }
    ExitProgram(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int n;
    TERMINAL **terms = typeCalloc(TERMINAL *, argc);

    while ((n = getopt(argc, argv, "ftx")) != -1) {
	switch (n) {
	case 'f':
	    f_opt = TRUE;
	    break;
	case 't':
	    t_opt = TRUE;
	    break;
#ifdef NCURSES_VERSION
	case 'x':
	    x_opt = TRUE;
	    break;
#endif
	default:
	    usage();
	    break;
	}
    }

#if HAVE_USE_EXTENDED_NAMES
    use_extended_names(x_opt);
#endif

    for (n = optind; n < argc; ++n) {
	setupterm((NCURSES_CONST char *) argv[n], 1, (int *) 0);
	terms[n - optind] = cur_term;
    }
    list_keys(terms, argc - optind);

    ExitProgram(EXIT_SUCCESS);
}

#else
int
main(int argc GCC_UNUSED, char *argv[]GCC_UNUSED)
{
    printf("This program requires the terminfo arrays\n");
    ExitProgram(EXIT_FAILURE);
}
#endif
#else /* !HAVE_TIGETSTR */
int
main(int argc GCC_UNUSED, char *argv[]GCC_UNUSED)
{
    printf("This program requires the terminfo functions such as tigetstr\n");
    ExitProgram(EXIT_FAILURE);
}
#endif /* HAVE_TIGETSTR */