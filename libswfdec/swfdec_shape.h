
#ifndef _SWFDEC_SHAPE_H_
#define _SWFDEC_SHAPE_H_

#include <glib-object.h>
#include <swfdec_types.h>
#include <swfdec_object.h>
#include <color.h>
#include <swfdec_bits.h>
#include "swfdec_transform.h"

G_BEGIN_DECLS

//typedef struct _SwfdecShape SwfdecShape;
typedef struct _SwfdecShapeClass SwfdecShapeClass;
typedef struct _SwfdecShapePoint SwfdecShapePoint;

#define SWFDEC_TYPE_SHAPE                    (swfdec_shape_get_type()) 
#define SWFDEC_IS_SHAPE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SWFDEC_TYPE_SHAPE))
#define SWFDEC_IS_SHAPE_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE ((klass), SWFDEC_TYPE_SHAPE))
#define SWFDEC_SHAPE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST ((obj), SWFDEC_TYPE_SHAPE, SwfdecShape))
#define SWFDEC_SHAPE_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST ((klass), SWFDEC_TYPE_SHAPE, SwfdecShapeClass))

#define SWFDEC_SHAPE_POINT_SPECIAL (-0x8000)
#define SWFDEC_SHAPE_POINT_MOVETO 0
#define SWFDEC_SHAPE_POINT_LINETO 1

struct _SwfdecShapePoint {
  gint16 control_x;
  gint16 control_y;
  gint16 to_x;
  gint16 to_y;
};

struct _SwfdecShapeVec
{
  int type;
  int index;
  unsigned int color;
  double width;

  GArray *path;
  int array_len;

  int fill_type;
  int fill_id;
  SwfdecTransform fill_transform;

  SwfdecGradient *grad;
};

struct _SwfdecShape {
  SwfdecObject object;

  GPtrArray *lines;
  GPtrArray *fills;
  GPtrArray *fills2;

  /* used while defining */
  int fills_offset;
  int lines_offset;
  int n_fill_bits;
  int n_line_bits;
  int rgba;
};

struct _SwfdecShapeClass {
  SwfdecObjectClass object_class;

};

GType swfdec_shape_get_type (void);

void swfdec_shape_free (SwfdecObject * object);
void _swfdec_shape_free (SwfdecShape * shape);
int tag_func_define_shape (SwfdecDecoder * s);
SwfdecShapeVec *swf_shape_vec_new (void);
int tag_define_shape (SwfdecDecoder * s);
int tag_define_shape_3 (SwfdecDecoder * s);
void swf_shape_add_styles (SwfdecDecoder * s, SwfdecShape * shape,
    SwfdecBits * bits);
void swf_shape_get_recs (SwfdecDecoder * s, SwfdecBits * bits, SwfdecShape * shape);
int tag_define_shape_2 (SwfdecDecoder * s);
int tag_func_define_button_2 (SwfdecDecoder * s);
int tag_func_define_sprite (SwfdecDecoder * s);
void dump_layers (SwfdecDecoder * s);

void swfdec_shape_compose (SwfdecDecoder * s, SwfdecLayerVec * layervec,
    SwfdecShapeVec * shapevec, SwfdecTransform *trans);
void swfdec_shape_compose_gradient (SwfdecDecoder * s,
    SwfdecLayerVec * layervec, SwfdecShapeVec * shapevec,
    SwfdecTransform *trans, SwfdecSpriteSegment * seg);
unsigned char *swfdec_gradient_to_palette (SwfdecGradient * grad,
    SwfdecColorTransform *color_transform);
SwfdecLayer * swfdec_shape_prerender (SwfdecDecoder * s,
    SwfdecSpriteSegment * seg, SwfdecObject * obj, SwfdecLayer * oldlayer);

G_END_DECLS

#endif

