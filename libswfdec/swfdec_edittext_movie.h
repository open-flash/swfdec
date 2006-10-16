/* Swfdec
 * Copyright (C) 2006 Benjamin Otte <otte@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef _SWFDEC_EDIT_TEXT_MOVIE_H_
#define _SWFDEC_EDIT_TEXT_MOVIE_H_

#include <libswfdec/swfdec_movie.h>
#include <libswfdec/swfdec_edittext.h>

G_BEGIN_DECLS


typedef struct _SwfdecEditTextMovie SwfdecEditTextMovie;
typedef struct _SwfdecEditTextMovieClass SwfdecEditTextMovieClass;

#define SWFDEC_TYPE_EDIT_TEXT_MOVIE                    (swfdec_edit_text_movie_get_type())
#define SWFDEC_IS_EDIT_TEXT_MOVIE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SWFDEC_TYPE_EDIT_TEXT_MOVIE))
#define SWFDEC_IS_EDIT_TEXT_MOVIE_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE ((klass), SWFDEC_TYPE_EDIT_TEXT_MOVIE))
#define SWFDEC_EDIT_TEXT_MOVIE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST ((obj), SWFDEC_TYPE_EDIT_TEXT_MOVIE, SwfdecEditTextMovie))
#define SWFDEC_EDIT_TEXT_MOVIE_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST ((klass), SWFDEC_TYPE_EDIT_TEXT_MOVIE, SwfdecEditTextMovieClass))

struct _SwfdecEditTextMovie {
  SwfdecMovie		movie;

  SwfdecEditText *	text;		/* the edit_text object we render */
  char *		str;		/* the string that gets rendered */
  SwfdecParagraph *	paragraph;	/* edit_text parsed to paragraph */
};

struct _SwfdecEditTextMovieClass {
  SwfdecMovieClass	movie_class;
};

GType		swfdec_edit_text_movie_get_type		(void);

void		swfdec_edit_text_movie_set_text		(SwfdecEditTextMovie *	movie,
							 const char *		str);

G_END_DECLS
#endif
