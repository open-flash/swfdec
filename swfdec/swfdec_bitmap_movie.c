/* Swfdec
 * Copyright (C) 2008 Benjamin Otte <otte@gnome.org>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "swfdec_bitmap_movie.h"
#include "swfdec_debug.h"
#include "swfdec_player_internal.h"

G_DEFINE_TYPE (SwfdecBitmapMovie, swfdec_bitmap_movie, SWFDEC_TYPE_MOVIE)

static void
swfdec_bitmap_movie_update_extents (SwfdecMovie *movie,
    SwfdecRect *extents)
{
  SwfdecBitmapMovie *bitmap = SWFDEC_BITMAP_MOVIE (movie);
  SwfdecRect rect = { 0, 0, 0, 0 };

  if (bitmap->bitmap->surface == NULL)
    return;

  rect.x1 = cairo_image_surface_get_width (bitmap->bitmap->surface) * SWFDEC_TWIPS_SCALE_FACTOR;
  rect.y1 = cairo_image_surface_get_height (bitmap->bitmap->surface) * SWFDEC_TWIPS_SCALE_FACTOR;

  swfdec_rect_union (extents, extents, &rect);
}

static void
swfdec_bitmap_movie_render (SwfdecMovie *movie, cairo_t *cr, 
    const SwfdecColorTransform *trans)
{
  SwfdecBitmapMovie *bitmap = SWFDEC_BITMAP_MOVIE (movie);

  if (bitmap->bitmap->surface == NULL)
    return;

  cairo_scale (cr, SWFDEC_TWIPS_SCALE_FACTOR, SWFDEC_TWIPS_SCALE_FACTOR);
  if (swfdec_color_transform_is_mask (trans)) {
    SWFDEC_FIXME ("does attachBitmap mask?");
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_rectangle (cr, 0, 0, 
	cairo_image_surface_get_width (bitmap->bitmap->surface),
	cairo_image_surface_get_height (bitmap->bitmap->surface));
    cairo_fill (cr);
  } else if (swfdec_color_transform_is_identity (trans)) {
    cairo_set_source_surface (cr, bitmap->bitmap->surface, 0, 0);
    cairo_paint (cr);
  } else if (swfdec_color_transform_is_alpha (trans)) {
    cairo_set_source_surface (cr, bitmap->bitmap->surface, 0, 0);
    cairo_paint_with_alpha (cr, trans->aa / 255.0);
  } else {
    SWFDEC_FIXME ("properly color-transform bitmap");
    cairo_set_source_surface (cr, bitmap->bitmap->surface, 0, 0);
    cairo_paint (cr);
  }
}

static void
swfdec_bitmap_movie_invalidate (SwfdecMovie *movie, const cairo_matrix_t *matrix, gboolean last)
{
  SwfdecBitmapMovie *bitmap = SWFDEC_BITMAP_MOVIE (movie);
  SwfdecRect rect = { 0, 0, 0, 0 };

  if (bitmap->bitmap->surface == NULL)
    return;

  rect.x1 = cairo_image_surface_get_width (bitmap->bitmap->surface);
  rect.y1 = cairo_image_surface_get_height (bitmap->bitmap->surface);

  swfdec_rect_transform (&rect, &rect, matrix);
  swfdec_player_invalidate (SWFDEC_PLAYER (swfdec_gc_object_get_context (movie)), &rect);
}

static SwfdecMovie *
swfdec_bitmap_movie_contains (SwfdecMovie *movie, double x, double y, 
    gboolean events)
{
  return movie;
}

static void
swfdec_bitmap_movie_mark (SwfdecGcObject *object)
{
  SwfdecBitmapMovie *bitmap = SWFDEC_BITMAP_MOVIE (object);

  swfdec_gc_object_mark (bitmap->bitmap);

  SWFDEC_GC_OBJECT_CLASS (swfdec_bitmap_movie_parent_class)->mark (object);
}

static void
swfdec_bitmap_movie_class_init (SwfdecBitmapMovieClass * g_class)
{
  SwfdecGcObjectClass *gc_class = SWFDEC_GC_OBJECT_CLASS (g_class);
  SwfdecMovieClass *movie_class = SWFDEC_MOVIE_CLASS (g_class);

  gc_class->mark = swfdec_bitmap_movie_mark;

  movie_class->update_extents = swfdec_bitmap_movie_update_extents;
  movie_class->render = swfdec_bitmap_movie_render;
  movie_class->invalidate = swfdec_bitmap_movie_invalidate;
  movie_class->contains = swfdec_bitmap_movie_contains;
}

static void
swfdec_bitmap_movie_init (SwfdecBitmapMovie * bitmap_movie)
{
}

