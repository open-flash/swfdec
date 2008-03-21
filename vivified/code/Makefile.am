SUBDIRS = test

noinst_LTLIBRARIES = libvivified-compiler.la

libvivified_compiler_la_CFLAGS = $(GLOBAL_CFLAGS) $(SWFDEC_CFLAGS)
libvivified_compiler_la_LDFLAGS = $(SWFDEC_LIBS)

libvivified_compiler_la_SOURCES = \
	vivi_code_assignment.c \
	vivi_code_binary.c \
	vivi_code_block.c \
	vivi_code_break.c \
	vivi_code_comment.c \
	vivi_code_constant.c \
	vivi_code_continue.c \
	vivi_code_get.c \
	vivi_code_get_url.c \
	vivi_code_goto.c \
	vivi_code_if.c \
	vivi_code_label.c \
	vivi_code_loop.c \
	vivi_code_printer.c \
	vivi_code_return.c \
	vivi_code_statement.c \
	vivi_code_text_printer.c \
	vivi_code_token.c \
	vivi_code_trace.c \
	vivi_code_unary.c \
	vivi_code_value.c \
	vivi_code_value_statement.c \
	vivi_decompiler_block.c \
	vivi_decompiler_state.c \
	vivi_decompiler.c

noinst_HEADERS = \
	vivi_code_assignment.h \
	vivi_code_binary.c \
	vivi_code_block.h \
	vivi_code_break.h \
	vivi_code_comment.h \
	vivi_code_constant.h \
	vivi_code_continue.h \
	vivi_code_get.h \
	vivi_code_get_url.h \
	vivi_code_goto.h \
	vivi_code_if.h \
	vivi_code_label.h \
	vivi_code_loop.h \
	vivi_code_printer.h \
	vivi_code_return.h \
	vivi_code_statement.h \
	vivi_code_text_printer.h \
	vivi_code_token.h \
	vivi_code_trace.h \
	vivi_code_unary.h \
	vivi_code_value.h \
	vivi_code_value_statement.h \
	vivi_decompiler.h \
	vivi_decompiler_block.h \
	vivi_decompiler_state.h
	vivified-compiler.h


noinst_PROGRAMS = vivi-decompile

vivi_decompile_SOURCES = \
	decompiler.c

vivi_decompile_CFLAGS =  $(GLOBAL_CFLAGS) $(SWFDEC_CFLAGS)
vivi_decompile_LDFLAGS = $(SWFDEC_LIBS)
vivi_decompile_LDADD = libvivified-compiler.la