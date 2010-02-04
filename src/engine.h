/* vim:set et sts=4: */
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <ibus.h>
#ifndef WITH_X11
#include <gtk/gtk.h>
#else
#include <X11/Xlib.h>
#endif
#include "handrecog.h"

#define IBUS_TYPE_HANDWRITE_ENGINE	\
	(ibus_handwrite_engine_get_type ())

GType ibus_handwrite_engine_get_type(void);

#define IBUS_HANDWRITE_ENGINE_GET_CLASS(obj)	((IBusHandwriteEngineClass*)(IBUS_ENGINE_GET_CLASS(obj)))


typedef struct _IBusHandwriteEngine IBusHandwriteEngine;
typedef struct _IBusHandwriteEngineClass IBusHandwriteEngineClass;

struct _IBusHandwriteEngine
{
	IBusEngine parent;

	/* members */
#ifdef WITH_X11
	Window drawpanel;
#else
	GtkWidget * drawpanel;
#endif
	GdkPoint lastpoint;
	guint mouse_state;
	IbusHandwriteRecog * engine;
	LineStroke currentstroke;
	gboolean	needclear;

};

struct _IBusHandwriteEngineClass
{
	IBusEngineClass parent;
};


typedef struct _RESULTCHAR RESULTCHAR;

extern char modelfile[1024] ; //= "data/handwriting-zh_CN.model";
extern char tablefile[1024] ; //= "data/table.txt";
#endif
