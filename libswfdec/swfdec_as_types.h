/* Swfdec
 * Copyright (C) 2007 Benjamin Otte <otte@gnome.org>
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

#ifndef _SWFDEC_AS_TYPES_H_
#define _SWFDEC_AS_TYPES_H_

#include <glib.h>

#include "swfdec_as_strings.h"

G_BEGIN_DECLS

/* fundamental types */
typedef enum {
  SWFDEC_AS_TYPE_UNDEFINED = 0,
  SWFDEC_AS_TYPE_BOOLEAN,
  SWFDEC_AS_TYPE_INT, /* unimplemented, but reserved if someone wants it */
  SWFDEC_AS_TYPE_NUMBER,
  SWFDEC_AS_TYPE_STRING,
  SWFDEC_AS_TYPE_NULL,
  SWFDEC_AS_TYPE_OBJECT
} SwfdecAsType;

typedef struct _SwfdecAsArray SwfdecAsArray;
typedef struct _SwfdecAsContext SwfdecAsContext;
typedef struct _SwfdecAsFrame SwfdecAsFrame;
typedef struct _SwfdecAsFunction SwfdecAsFunction;
typedef struct _SwfdecAsObject SwfdecAsObject;
typedef struct _SwfdecAsScope SwfdecAsScope;
typedef struct _SwfdecAsStack SwfdecAsStack;
typedef struct _SwfdecAsValue SwfdecAsValue;
typedef void (* SwfdecAsNative) (SwfdecAsObject *thisp, guint argc, SwfdecAsValue *argv, SwfdecAsValue *retval);

/* IMPORTANT: a SwfdecAsValue memset to 0 is a valid undefined value */
struct _SwfdecAsValue {
  SwfdecAsType		type;
  union {
    gboolean		boolean;
    double		number;
    const char *	string;
    SwfdecAsObject *	object;
  } value;
};

#define SWFDEC_IS_AS_VALUE(val) ((val)->type <= SWFDEC_TYPE_AS_OBJECT)

#define SWFDEC_AS_VALUE_IS_UNDEFINED(val) ((val)->type == SWFDEC_AS_TYPE_UNDEFINED)
#define SWFDEC_AS_VALUE_SET_UNDEFINED(val) (val)->type = SWFDEC_AS_TYPE_UNDEFINED

#define SWFDEC_AS_VALUE_IS_BOOLEAN(val) ((val)->type == SWFDEC_AS_TYPE_BOOLEAN)
#define SWFDEC_AS_VALUE_GET_BOOLEAN(val) ((val)->value.boolean)
#define SWFDEC_AS_VALUE_SET_BOOLEAN(val,b) G_STMT_START { \
  SwfdecAsValue *__val = (val); \
  gboolean __tmp = (b); \
  g_assert (__tmp == TRUE || __tmp == FALSE); \
  (__val)->type = SWFDEC_AS_TYPE_BOOLEAN; \
  (__val)->value.boolean = __tmp; \
} G_STMT_END

#define SWFDEC_AS_VALUE_IS_NUMBER(val) ((val)->type == SWFDEC_AS_TYPE_NUMBER)
#define SWFDEC_AS_VALUE_GET_NUMBER(val) ((val)->value.number)
#define SWFDEC_AS_VALUE_SET_NUMBER(val,d) G_STMT_START { \
  SwfdecAsValue *__val = (val); \
  (__val)->type = SWFDEC_AS_TYPE_NUMBER; \
  (__val)->value.number = d; \
} G_STMT_END

#define SWFDEC_AS_VALUE_SET_INT SWFDEC_AS_VALUE_SET_NUMBER

#define SWFDEC_AS_VALUE_IS_STRING(val) ((val)->type == SWFDEC_AS_TYPE_STRING)
#define SWFDEC_AS_VALUE_GET_STRING(val) ((val)->value.string)
#define SWFDEC_AS_VALUE_SET_STRING(val,s) G_STMT_START { \
  SwfdecAsValue *__val = (val); \
  (__val)->type = SWFDEC_AS_TYPE_STRING; \
  (__val)->value.string = s; \
} G_STMT_END

#define SWFDEC_AS_VALUE_IS_NULL(val) ((val)->type == SWFDEC_AS_TYPE_NULL)
#define SWFDEC_AS_VALUE_SET_NULL(val) (val)->type = SWFDEC_AS_TYPE_NULL

#define SWFDEC_AS_VALUE_IS_OBJECT(val) ((val)->type == SWFDEC_AS_TYPE_OBJECT)
#define SWFDEC_AS_VALUE_GET_OBJECT(val) ((val)->value.object)
#define SWFDEC_AS_VALUE_SET_OBJECT(val,o) G_STMT_START { \
  SwfdecAsValue *__val = (val); \
  g_assert (o != NULL); \
  (__val)->type = SWFDEC_AS_TYPE_OBJECT; \
  (__val)->value.object = o; \
} G_STMT_END

/* all existing actions */
typedef enum {
  SWFDEC_AS_ACTION_NEXT_FRAME = 0x04,
  SWFDEC_AS_ACTION_PREVIOUS_FRAME = 0x05,
  SWFDEC_AS_ACTION_PLAY = 0x06,
  SWFDEC_AS_ACTION_STOP = 0x07,
  SWFDEC_AS_ACTION_TOGGLE_QUALITY = 0x08,
  SWFDEC_AS_ACTION_STOP_SOUNDS = 0x09,
  SWFDEC_AS_ACTION_ADD = 0x0A,
  SWFDEC_AS_ACTION_SUBTRACT = 0x0B,
  SWFDEC_AS_ACTION_MULTIPLY = 0x0C,
  SWFDEC_AS_ACTION_DIVIDE = 0x0D,
  SWFDEC_AS_ACTION_EQUALS = 0x0E,
  SWFDEC_AS_ACTION_LESS = 0x0F,
  SWFDEC_AS_ACTION_AND = 0x10,
  SWFDEC_AS_ACTION_OR = 0x11,
  SWFDEC_AS_ACTION_NOT = 0x12,
  SWFDEC_AS_ACTION_STRING_EQUALS = 0x13,
  SWFDEC_AS_ACTION_STRING_LENGTH = 0x14,
  SWFDEC_AS_ACTION_STRING_EXTRACT = 0x15,
  SWFDEC_AS_ACTION_POP = 0x17,
  SWFDEC_AS_ACTION_TO_INTEGER = 0x18,
  SWFDEC_AS_ACTION_GET_VARIABLE = 0x1C,
  SWFDEC_AS_ACTION_SET_VARIABLE = 0x1D,
  SWFDEC_AS_ACTION_SET_TARGET2 = 0x20,
  SWFDEC_AS_ACTION_STRING_ADD = 0x21,
  SWFDEC_AS_ACTION_GET_PROPERTY = 0x22,
  SWFDEC_AS_ACTION_SET_PROPERTY = 0x23,
  SWFDEC_AS_ACTION_CLONE_SPRITE = 0x24,
  SWFDEC_AS_ACTION_REMOVE_SPRITE = 0x25,
  SWFDEC_AS_ACTION_TRACE = 0x26,
  SWFDEC_AS_ACTION_START_DRAG = 0x27,
  SWFDEC_AS_ACTION_END_DRAG = 0x28,
  SWFDEC_AS_ACTION_STRING_LESS = 0x29,
  SWFDEC_AS_ACTION_THROW = 0x2A,
  SWFDEC_AS_ACTION_CAST = 0x2B,
  SWFDEC_AS_ACTION_IMPLEMENTS = 0x2C,
  SWFDEC_AS_ACTION_RANDOM = 0x30,
  SWFDEC_AS_ACTION_MB_STRING_LENGTH = 0x31,
  SWFDEC_AS_ACTION_CHAR_TO_ASCII = 0x32,
  SWFDEC_AS_ACTION_ASCII_TO_CHAR = 0x33,
  SWFDEC_AS_ACTION_GET_TIME = 0x34,
  SWFDEC_AS_ACTION_MB_STRING_EXTRACT = 0x35,
  SWFDEC_AS_ACTION_MB_CHAR_TO_ASCII = 0x36,
  SWFDEC_AS_ACTION_MB_ASCII_TO_CHAR = 0x37,
  SWFDEC_AS_ACTION_DELETE = 0x3A,
  SWFDEC_AS_ACTION_DELETE2 = 0x3B,
  SWFDEC_AS_ACTION_DEFINE_LOCAL = 0x3C,
  SWFDEC_AS_ACTION_CALL_FUNCTION = 0x3D,
  SWFDEC_AS_ACTION_RETURN = 0x3E,
  SWFDEC_AS_ACTION_MODULO = 0x3F,
  SWFDEC_AS_ACTION_NEW_OBJECT = 0x40,
  SWFDEC_AS_ACTION_DEFINE_LOCAL2 = 0x41,
  SWFDEC_AS_ACTION_INIT_ARRAY = 0x42,
  SWFDEC_AS_ACTION_INIT_OBJECT = 0x43,
  SWFDEC_AS_ACTION_TYPE_OF = 0x44,
  SWFDEC_AS_ACTION_TARGET_PATH = 0x45,
  SWFDEC_AS_ACTION_ENUMERATE = 0x46,
  SWFDEC_AS_ACTION_ADD2 = 0x47,
  SWFDEC_AS_ACTION_LESS2 = 0x48,
  SWFDEC_AS_ACTION_EQUALS2 = 0x49,
  SWFDEC_AS_ACTION_TO_NUMBER = 0x4A,
  SWFDEC_AS_ACTION_TO_STRING = 0x4B,
  SWFDEC_AS_ACTION_PUSH_DUPLICATE = 0x4C,
  SWFDEC_AS_ACTION_SWAP = 0x4D,
  SWFDEC_AS_ACTION_GET_MEMBER = 0x4E,
  SWFDEC_AS_ACTION_SET_MEMBER = 0x4F,
  SWFDEC_AS_ACTION_INCREMENT = 0x50,
  SWFDEC_AS_ACTION_DECREMENT = 0x51,
  SWFDEC_AS_ACTION_CALL_METHOD = 0x52,
  SWFDEC_AS_ACTION_NEW_METHOD = 0x53,
  SWFDEC_AS_ACTION_INSTANCE_OF = 0x54,
  SWFDEC_AS_ACTION_ENUMERATE2 = 0x55,
  SWFDEC_AS_ACTION_BIT_AND = 0x60,
  SWFDEC_AS_ACTION_BIT_OR = 0x61,
  SWFDEC_AS_ACTION_BIT_XOR = 0x62,
  SWFDEC_AS_ACTION_BIT_LSHIFT = 0x63,
  SWFDEC_AS_ACTION_BIT_RSHIFT = 0x64,
  SWFDEC_AS_ACTION_BIT_URSHIFT = 0x65,
  SWFDEC_AS_ACTION_STRICT_EQUALS = 0x66,
  SWFDEC_AS_ACTION_GREATER = 0x67,
  SWFDEC_AS_ACTION_STRING_GREATER = 0x68,
  SWFDEC_AS_ACTION_EXTENDS = 0x69,
  SWFDEC_AS_ACTION_GOTO_FRAME = 0x81,
  SWFDEC_AS_ACTION_GET_URL = 0x83,
  SWFDEC_AS_ACTION_STORE_REGISTER = 0x87,
  SWFDEC_AS_ACTION_CONSTANT_POOL = 0x88,
  SWFDEC_AS_ACTION_WAIT_FOR_FRAME = 0x8A,
  SWFDEC_AS_ACTION_SET_TARGET = 0x8B,
  SWFDEC_AS_ACTION_GOTO_LABEL = 0x8C,
  SWFDEC_AS_ACTION_WAIT_FOR_FRAME2 = 0x8D,
  SWFDEC_AS_ACTION_DEFINE_FUNCTION2 = 0x8E,
  SWFDEC_AS_ACTION_TRY = 0x8F,
  SWFDEC_AS_ACTION_WITH = 0x94,
  SWFDEC_AS_ACTION_PUSH = 0x96,
  SWFDEC_AS_ACTION_JUMP = 0x99,
  SWFDEC_AS_ACTION_GET_URL2 = 0x9A,
  SWFDEC_AS_ACTION_DEFINE_FUNCTION = 0x9B,
  SWFDEC_AS_ACTION_IF = 0x9D,
  SWFDEC_AS_ACTION_CALL = 0x9E,
  SWFDEC_AS_ACTION_GOTO_FRAME2 = 0x9F
} SwfdecAsAction;

/* value conversion functions */
gboolean	swfdec_as_value_to_boolean	(SwfdecAsContext *	context,
						 const SwfdecAsValue *	value);
int		swfdec_as_value_to_integer	(SwfdecAsContext *	context,
						 const SwfdecAsValue *	value);
double		swfdec_as_value_to_number	(SwfdecAsContext *	context,
						 const SwfdecAsValue *	value);
SwfdecAsObject *swfdec_as_value_to_object	(SwfdecAsContext *	context,
						 const SwfdecAsValue *	value);
const char *	swfdec_as_value_to_printable	(SwfdecAsContext *	context,
						 const SwfdecAsValue *	value);
const char *	swfdec_as_value_to_string	(SwfdecAsContext *	context,
						 const SwfdecAsValue *	value);

/* special conversion functions */
const char *	swfdec_as_double_to_string	(SwfdecAsContext *	context,
						 double			d);
const char *	swfdec_as_str_concat		(SwfdecAsContext *	cx,
						 const char *		s1,
						 const char *		s2);


G_END_DECLS
#endif
