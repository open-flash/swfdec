/* Swfdec
 * Copyright (C) 2003-2006 David Schleef <ds@schleef.org>
 *		 2005-2006 Eric Anholt <eric@anholt.net>
 *		 2006-2007 Benjamin Otte <otte@gnome.org>
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
#include <string.h>
#include <math.h>

#include "swfdec_movie.h"
#include "swfdec_bits.h"
#include "swfdec_debug.h"
#include "swfdec_decoder.h"
#include "swfdec_player_internal.h"
#include "swfdec_root_movie.h"
#include "swfdec_sprite.h"
#include "swfdec_sprite_movie.h"
#include "swfdec_swf_decoder.h"

static void
mc_x_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  double d;

  swfdec_movie_update (movie);
  d = SWFDEC_TWIPS_TO_DOUBLE (movie->matrix.x0);
  SWFDEC_AS_VALUE_SET_NUMBER (rval, d);
}

static void
mc_x_set (SwfdecMovie *movie, const SwfdecAsValue *val)
{
  double d;

  d = swfdec_as_value_to_number (SWFDEC_AS_OBJECT (movie)->context, val);
  if (!isfinite (d)) {
    SWFDEC_WARNING ("trying to move %s._x to a non-finite value, ignoring", movie->name);
    return;
  }
  movie->modified = TRUE;
  d = SWFDEC_DOUBLE_TO_TWIPS (d);
  if (d != movie->matrix.x0) {
    movie->matrix.x0 = d;
    swfdec_movie_queue_update (movie, SWFDEC_MOVIE_INVALID_MATRIX);
  }
}

static void
mc_y_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  double d;

  swfdec_movie_update (movie);
  d = SWFDEC_TWIPS_TO_DOUBLE (movie->matrix.y0);
  SWFDEC_AS_VALUE_SET_NUMBER (rval, d);
}

static void
mc_y_set (SwfdecMovie *movie, const SwfdecAsValue *val)
{
  double d;

  d = swfdec_as_value_to_number (SWFDEC_AS_OBJECT (movie)->context, val);
  if (!isfinite (d)) {
    SWFDEC_WARNING ("trying to move %s._y to a non-finite value, ignoring", movie->name);
    return;
  }
  movie->modified = TRUE;
  d = SWFDEC_DOUBLE_TO_TWIPS (d);
  if (d != movie->matrix.y0) {
    movie->matrix.y0 = d;
    swfdec_movie_queue_update (movie, SWFDEC_MOVIE_INVALID_MATRIX);
  }
}

static void
mc_xscale_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  SWFDEC_AS_VALUE_SET_NUMBER (rval, movie->xscale);
}

static void
mc_xscale_set (SwfdecMovie *movie, const SwfdecAsValue *val)
{
  double d;

  d = swfdec_as_value_to_number (SWFDEC_AS_OBJECT (movie)->context, val);
  if (!isfinite (d)) {
    SWFDEC_WARNING ("trying to set xscale to a non-finite value, ignoring");
    return;
  }
  movie->modified = TRUE;
  movie->xscale = d;
  swfdec_movie_queue_update (movie, SWFDEC_MOVIE_INVALID_MATRIX);
}

static void
mc_yscale_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  SWFDEC_AS_VALUE_SET_NUMBER (rval, movie->yscale);
}

static void
mc_yscale_set (SwfdecMovie *movie, const SwfdecAsValue *val)
{
  double d;

  d = swfdec_as_value_to_number (SWFDEC_AS_OBJECT (movie)->context, val);
  if (!isfinite (d)) {
    SWFDEC_WARNING ("trying to set yscale to a non-finite value, ignoring");
    return;
  }
  movie->modified = TRUE;
  movie->yscale = d;
  swfdec_movie_queue_update (movie, SWFDEC_MOVIE_INVALID_MATRIX);
}

static void
mc_currentframe (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  SWFDEC_AS_VALUE_SET_NUMBER (rval, movie->frame + 1);
}

static void
mc_framesloaded (SwfdecMovie *mov, SwfdecAsValue *rval)
{
  /* only root movies can be partially loaded */
  if (SWFDEC_IS_SPRITE_MOVIE (mov)) {
    SwfdecSpriteMovie *movie = SWFDEC_SPRITE_MOVIE (mov);
    if (movie->sprite) {
      SWFDEC_AS_VALUE_SET_NUMBER (rval, movie->sprite->parse_frame);
      return;
    }
  }
  SWFDEC_AS_VALUE_SET_NUMBER (rval, mov->n_frames);
}

static void
mc_name_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  const char *string;

  if (movie->has_name) {
    /* FIXME: make names GC'd */
    string = swfdec_as_context_get_string (SWFDEC_AS_OBJECT (movie)->context, movie->name);
  } else {
    string = SWFDEC_AS_STR_EMPTY;
  }
  SWFDEC_AS_VALUE_SET_STRING (rval, string);
}

static void
mc_name_set (SwfdecMovie *movie, const SwfdecAsValue *val)
{
  const char *str;

  str = swfdec_as_value_to_string (SWFDEC_AS_OBJECT (movie)->context, val);
  g_free (movie->name);
  movie->name = g_strdup (str);
  movie->has_name = TRUE;
}

static void
mc_totalframes (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  SWFDEC_AS_VALUE_SET_NUMBER (rval, movie->n_frames);
}

static void
mc_alpha_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  SWFDEC_AS_VALUE_SET_NUMBER (rval,
      movie->color_transform.aa * 100.0 / 256.0);
}

static void
mc_alpha_set (SwfdecMovie *movie, const SwfdecAsValue *val)
{
  double d;
  int alpha;

  d = swfdec_as_value_to_number (SWFDEC_AS_OBJECT (movie)->context, val);
  if (!isfinite (d)) {
    SWFDEC_WARNING ("trying to set alpha to a non-finite value, ignoring");
    return;
  }
  alpha = d * 256.0 / 100.0;
  if (alpha != movie->color_transform.aa) {
    movie->color_transform.aa = alpha;
    swfdec_movie_invalidate (movie);
  }
}

static void
mc_visible_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  SWFDEC_AS_VALUE_SET_BOOLEAN (rval, movie->visible);
}

static void
mc_visible_set (SwfdecMovie *movie, const SwfdecAsValue *val)
{
  gboolean b;

  b = swfdec_as_value_to_boolean (SWFDEC_AS_OBJECT (movie)->context, val);
  if (b != movie->visible) {
    movie->visible = b;
    swfdec_movie_invalidate (movie);
  }
}

static void
mc_width_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  double d;

  swfdec_movie_update (movie);
  d = SWFDEC_TWIPS_TO_DOUBLE ((SwfdecTwips) (rint (movie->extents.x1 - movie->extents.x0)));
  SWFDEC_AS_VALUE_SET_NUMBER (rval, d);
}

static void
mc_width_set (SwfdecMovie *movie, const SwfdecAsValue *val)
{
  double d, cur;

  /* property was readonly in Flash 4 and before */
  if (SWFDEC_AS_OBJECT (movie)->context->version < 5)
    return;
  d = swfdec_as_value_to_number (SWFDEC_AS_OBJECT (movie)->context, val);
  if (!isfinite (d)) {
    SWFDEC_WARNING ("trying to set height to a non-finite value, ignoring");
    return;
  }
  swfdec_movie_update (movie);
  movie->modified = TRUE;
  cur = SWFDEC_TWIPS_TO_DOUBLE ((SwfdecTwips) (rint (movie->original_extents.x1 - movie->original_extents.x0)));
  if (cur != 0) {
    movie->xscale = 100 * d / cur;
  } else {
    movie->xscale = 0;
    movie->yscale = 0;
  }
  swfdec_movie_queue_update (movie, SWFDEC_MOVIE_INVALID_MATRIX);
}

static void
mc_height_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  double d;

  swfdec_movie_update (movie);
  d = SWFDEC_TWIPS_TO_DOUBLE ((SwfdecTwips) (rint (movie->extents.y1 - movie->extents.y0)));
  SWFDEC_AS_VALUE_SET_NUMBER (rval, d);
}

static void
mc_height_set (SwfdecMovie *movie, const SwfdecAsValue *val)
{
  double d, cur;

  /* property was readonly in Flash 4 and before */
  if (SWFDEC_AS_OBJECT (movie)->context->version < 5)
    return;
  d = swfdec_as_value_to_number (SWFDEC_AS_OBJECT (movie)->context, val);
  if (!isfinite (d)) {
    SWFDEC_WARNING ("trying to set height to a non-finite value, ignoring");
    return;
  }
  swfdec_movie_update (movie);
  movie->modified = TRUE;
  cur = SWFDEC_TWIPS_TO_DOUBLE ((SwfdecTwips) (rint (movie->original_extents.y1 - movie->original_extents.y0)));
  if (cur != 0) {
    movie->yscale = 100 * d / cur;
  } else {
    movie->xscale = 0;
    movie->yscale = 0;
  }
  swfdec_movie_queue_update (movie, SWFDEC_MOVIE_INVALID_MATRIX);
}

static void
mc_rotation_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  SWFDEC_AS_VALUE_SET_NUMBER (rval, movie->rotation);
}

static void
mc_rotation_set (SwfdecMovie *movie, const SwfdecAsValue *val)
{
  double d;

  /* FIXME: Flash 4 handles this differently */
  d = swfdec_as_value_to_number (SWFDEC_AS_OBJECT (movie)->context, val);
  d = fmod (d, 360.0);
  if (d > 180.0)
    d -= 360.0;
  if (d < -180.0)
    d += 360.0;
  if (SWFDEC_AS_OBJECT (movie)->context->version < 5) {
    if (!isfinite (d))
      return;
    SWFDEC_ERROR ("FIXME: implement correct rounding errors here");
  }
  movie->modified = TRUE;
  movie->rotation = d;
  swfdec_movie_queue_update (movie, SWFDEC_MOVIE_INVALID_MATRIX);
}

static void
mc_xmouse_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  double x, y;

  swfdec_movie_get_mouse (movie, &x, &y);
  x = rint (x * SWFDEC_TWIPS_SCALE_FACTOR) / SWFDEC_TWIPS_SCALE_FACTOR;
  SWFDEC_AS_VALUE_SET_NUMBER (rval, x);
}

static void
mc_ymouse_get (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  double x, y;

  swfdec_movie_get_mouse (movie, &x, &y);
  y = rint (y * SWFDEC_TWIPS_SCALE_FACTOR) / SWFDEC_TWIPS_SCALE_FACTOR;
  SWFDEC_AS_VALUE_SET_NUMBER (rval, y);
}

static void
mc_parent (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  if (movie->parent) {
    SWFDEC_AS_VALUE_SET_OBJECT (rval, SWFDEC_AS_OBJECT (movie->parent));
  } else {
    SWFDEC_AS_VALUE_SET_UNDEFINED (rval);
  }
}

static void
mc_root (SwfdecMovie *movie, SwfdecAsValue *rval)
{
  while (movie->parent)
    movie = movie->parent;
  SWFDEC_AS_VALUE_SET_OBJECT (rval, SWFDEC_AS_OBJECT (movie));
}

struct {
  const char *name;
  void (* get) (SwfdecMovie *movie, SwfdecAsValue *ret);
  void (* set) (SwfdecMovie *movie, const SwfdecAsValue *val);
} swfdec_movieclip_props[] = {
  { SWFDEC_AS_STR__x,		mc_x_get,	    mc_x_set },
  { SWFDEC_AS_STR__y,		mc_y_get,	    mc_y_set },
  { SWFDEC_AS_STR__xscale,	mc_xscale_get,	    mc_xscale_set },
  { SWFDEC_AS_STR__yscale,	mc_yscale_get,	    mc_yscale_set },
  { SWFDEC_AS_STR__currentframe,mc_currentframe,    NULL },
  { SWFDEC_AS_STR__totalframes,	mc_totalframes,	    NULL },
  { SWFDEC_AS_STR__alpha,	mc_alpha_get,	    mc_alpha_set },
  { SWFDEC_AS_STR__visible,	mc_visible_get,	    mc_visible_set },
  { SWFDEC_AS_STR__width,	mc_width_get,	    mc_width_set },
  { SWFDEC_AS_STR__height,	mc_height_get,	    mc_height_set },
  { SWFDEC_AS_STR__rotation,	mc_rotation_get,    mc_rotation_set },
  { SWFDEC_AS_STR__target,	NULL,  NULL }, //"_target"
  { SWFDEC_AS_STR__framesloaded,mc_framesloaded,    NULL},
  { SWFDEC_AS_STR__name,	mc_name_get,	    mc_name_set },
  { SWFDEC_AS_STR__droptarget,	NULL,  NULL }, //"_droptarget"
  { SWFDEC_AS_STR__url,		NULL,  NULL }, //"_url"
  { SWFDEC_AS_STR__highquality,	NULL,  NULL }, //"_highquality"
  { SWFDEC_AS_STR__focusrect,	NULL,  NULL }, //"_focusrect"
  { SWFDEC_AS_STR__soundbuftime,NULL,  NULL }, //"_soundbuftime"
  { SWFDEC_AS_STR__quality,	NULL,  NULL }, //"_quality"
  { SWFDEC_AS_STR__xmouse,	mc_xmouse_get,	    NULL },
  { SWFDEC_AS_STR__ymouse,	mc_ymouse_get,	    NULL },
  { SWFDEC_AS_STR__parent,	mc_parent,	    NULL },
  { SWFDEC_AS_STR__root,	mc_root,	    NULL },
};

static inline int
swfdec_movie_get_asprop_index (SwfdecMovie *movie, const char *name)
{
  guint i;

  if (name < SWFDEC_AS_STR__x || name > SWFDEC_AS_STR__root)
    return -1;

  for (i = 0; i < G_N_ELEMENTS (swfdec_movieclip_props); i++) {
    if (swfdec_movieclip_props[i].name == name) {
      if (swfdec_movieclip_props[i].get == NULL) {
	SWFDEC_ERROR ("property %s not implemented", name);
      }
      return i;
    }
  }
  g_assert_not_reached ();
  return -1;
}

gboolean
swfdec_movie_set_asprop (SwfdecMovie *movie, const char *name, const SwfdecAsValue *val)
{
  int i;
  
  i = swfdec_movie_get_asprop_index (movie, name);
  if (i == -1)
    return FALSE;
  if (swfdec_movieclip_props[i].set != NULL) {
    swfdec_movieclip_props[i].set (movie, val);
  }
  return TRUE;
}

gboolean
swfdec_movie_get_asprop (SwfdecMovie *movie, const char *name, SwfdecAsValue *val)
{
  int i;
  
  i = swfdec_movie_get_asprop_index (movie, name);
  if (i == -1)
    return FALSE;
  if (swfdec_movieclip_props[i].get != NULL) {
    swfdec_movieclip_props[i].get (movie, val);
  } else {
    SWFDEC_AS_VALUE_SET_UNDEFINED (val);
  }
  return TRUE;
}

