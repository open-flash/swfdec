
#ifndef _SWFDEC_SOUND_H_
#define _SWFDEC_SOUND_H_

#include <glib-object.h>
#include <swfdec_object.h>
#include "swfdec_types.h"

/* FIXME */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_MAD
#include <mad.h>
#endif

G_BEGIN_DECLS

//typedef struct _SwfdecSound SwfdecSound;
typedef struct _SwfdecSoundClass SwfdecSoundClass;

#define SWFDEC_TYPE_SOUND                    (swfdec_sound_get_type()) 
#define SWFDEC_IS_SOUND(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SWFDEC_TYPE_SOUND))
#define SWFDEC_IS_SOUND_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE ((klass), SWFDEC_TYPE_SOUND))
#define SWFDEC_SOUND(obj)                    (G_TYPE_CHECK_INSTANCE_CAST ((obj), SWFDEC_TYPE_SOUND, SwfdecSound))
#define SWFDEC_SOUND_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST ((klass), SWFDEC_TYPE_SOUND, SwfdecSoundClass))

struct _SwfdecSoundChunk {
  int n_samples;
  int n_left;
  int length;
  unsigned char *data;
};

struct _SwfdecSound {
  SwfdecObject object;

  int format;

  unsigned char *orig_data;
  int orig_len;

#ifdef HAVE_MAD
  struct mad_stream stream;
  struct mad_frame frame;
  struct mad_synth synth;
  unsigned char tmpbuf[1024];
  int tmpbuflen;
#else
  void *mp;
#endif

  int n_samples;

  void *sound_buf;
  int sound_len;
};

struct _SwfdecSoundClass {
  SwfdecObjectClass object_class;

};

GType swfdec_sound_get_type (void);

int tag_func_define_sound (SwfdecDecoder * s);
int tag_func_sound_stream_block (SwfdecDecoder * s);
int tag_func_sound_stream_head (SwfdecDecoder * s);
int tag_func_start_sound (SwfdecDecoder * s);
int tag_func_define_button_sound (SwfdecDecoder * s);
void swfdec_sound_render (SwfdecDecoder * s);

void swfdec_sound_chunk_free (SwfdecSoundChunk *chunk);

int swfdec_sound_mp3_decode_stream (SwfdecDecoder *s, SwfdecSound *sound);

G_END_DECLS

#endif

