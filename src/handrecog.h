/*
 * handrecog.c
 *
 *  Created on: 2010-2-4
 *      Author: cai
 */

#ifndef HANDRECOG_C_
#define HANDRECOG_C_

#include <glib.h>
#include <glib-object.h>

#ifndef WITH_X11
#include <gtk/gtk.h>
#else
typedef struct _GdkPoint
{
  gint x;
  gint y;
}GdkPoint;
#endif

typedef struct _LineStroke LineStroke;
typedef struct _MatchedChar MatchedChar;
typedef struct _IbusHandwriteRecog IbusHandwriteRecog;
typedef struct _IbusHandwriteRecogClass IbusHandwriteRecogClass;

struct _LineStroke
{
	int		segments; //包含有的段数目
	GdkPoint* points; //包含的用来构成笔画的点
};

struct _MatchedChar{
	char	chr[8];
};


struct _IbusHandwriteRecog{
	GObject parent;
	GArray  * matched;
	GArray  * strokes;
};

struct _IbusHandwriteRecogClass{
	GObjectClass parent;

	void (*dispose)(GObject*);
	guint singal[2];

	/* signals */
    void (* destroy)        (IbusHandwriteRecog*object);

    /*numbers*/

	/*
	 *  if way = IBUS_HANDWRITE_RECOG_TABLE_FROM_FILE, then pass filename
	 *  if way = IBUS_HANDWRITE_RECOG_TABLE_FROM_MEMORY, then pass start point and length
	 *  if way = IBUS_HANDWRITE_RECOG_TABLE_FROM_FD, then pass file descriptor
	 *
	 *  return 0 at success
	 *  return -1 at failed
	 *  return 1 , IbusHandwriteRecog will try another way, ie, wrapper
	 */
	int (*load_table)(IbusHandwriteRecog*, int way, ...);
	void (*change_stroke)(IbusHandwriteRecog*);
	gboolean (*domatch)(IbusHandwriteRecog*,int want);
};

GType ibus_handwrite_recog_get_type(void);
GType ibus_handwrite_recog_zinnia_get_type(void);

#define G_TYPE_IBUS_HANDWRITE_RECOG (ibus_handwrite_recog_get_type())
#define IBUS_HANDWRITE_RECOG_GET_CLASS(obj) G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_IBUS_HANDWRITE_RECOG, IbusHandwriteRecogClass)
#define IBUS_HANDWRITE_RECOG(obj) \
		G_TYPE_CHECK_INSTANCE_CAST(obj,G_TYPE_IBUS_HANDWRITE_RECOG,IbusHandwriteRecog)
enum{
	IBUS_HANDWRITE_RECOG_TABLE_FROM_FILENAME,
	IBUS_HANDWRITE_RECOG_TABLE_FROM_FILE,
	IBUS_HANDWRITE_RECOG_TABLE_FROM_FD,
	IBUS_HANDWRITE_RECOG_TABLE_FROM_MEMORY
};

typedef enum{
	IBUS_HANDWRITE_RECOG_ENGINE_ZINNIA,
	IBUS_HANDWRITE_RECOG_ENGINE_CAICAI // ? fixme
}ENGINEYTPE;



IbusHandwriteRecog* ibus_handwrite_recog_new( ENGINEYTPE   engine );

/*
 *  if way = IBUS_HANDWRITE_RECOG_TABLE_FROM_FILE, then pass filename
 *  if way = IBUS_HANDWRITE_RECOG_TABLE_FROM_MEMORY, then pass start point and length
 *  if way = IBUS_HANDWRITE_RECOG_TABLE_FROM_FD, then pass file descriptor
 */

int ibus_handwrite_recog_load_table(IbusHandwriteRecog*, int way, ...);
void ibus_handwrite_recog_clear_stroke(IbusHandwriteRecog*obj);
void ibus_handwrite_recog_append_stroke(IbusHandwriteRecog*obj,LineStroke stroke);
void ibus_handwrite_recog_remove_stroke(IbusHandwriteRecog*obj,int number);
guint ibus_handwrite_recog_getmatch(IbusHandwriteRecog*obj,MatchedChar ** matchedchars, int munbers_skip);
gboolean ibus_handwrite_recog_domatch(IbusHandwriteRecog*,int want);


#endif /* HANDRECOG_C_ */
