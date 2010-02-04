/*
 * UI_gtk.c -- provide UI via gtk
 *
 *  Created on: 2010-2-4
 *      Author: cai
 */

#include <stdio.h>
#include <poll.h>
#include <X11/Xlib.h>

#include "engine.h"
#include "UI.h"

static Display * xdisplay;


static gboolean paint_ui(XEvent * ev , IBusHandwriteEngine * engine)
{
	GC gc;

	LineStroke cl;

	int i;

	MatchedChar * matched;
	Drawable gw ;

	gw = ev->xexpose.window ;

	XGCValues gv = {0};

	gc = XCreateGC(xdisplay,gw,0,&gv);

//	XSetWindowBackground(xdisplay,0,888888);
	XSetForeground(xdisplay,gc,5263);

	XRectangle rg[2] = { {0,0,199,199} , {200,0,399,199} };

	XFillRectangles(xdisplay,gw,gc,rg,2);


	//已经录入的笔画
#if 0
	for (i = 0; i < engine->engine->strokes->len ; i++ )
	{
		printf("drawing %d th line, total %d\n",i,engine->engine->strokes->len);
		cl =  g_array_index(engine->engine->strokes,LineStroke,i);
//		gdk_draw_lines(widget->window, gc, cl.points,cl.segments );
	}
#endif


	//当下笔画
	if (engine->currentstroke.points)
		XDrawLines(xdisplay,gw,gc,(XPoint*)engine->currentstroke.points,	engine->currentstroke.segments,CoordModeOrigin);


//	puts(__func__);
//	return 0;

	int munber = 0 ;// ibus_handwrite_recog_getmatch(engine->engine,&matched,0);



	//画10个侯选字
	for (i = 0; i < munber ; ++i)
	{

		char drawtext[32]={0};

		sprintf(drawtext,"%d.%s",i,matched[i].chr);

//		PangoLayout * layout = gtk_widget_create_pango_layout(widget,drawtext);

//		gdk_draw_layout(widget->window, gc, (i % 5) * 40 + 3, 205 + (20 * (i / 5)),	layout);
//		g_object_unref(layout);
	}

	XFreeGC(xdisplay,gc);

//	g_object_unref(gc);
	return TRUE;
}

//
//static gboolean on_mouse_move(GtkWidget *widget, GdkEventMotion *event,
//		gpointer user_data)
//{
//	IBusHandwriteEngine * engine;
//
//	engine = (IBusHandwriteEngine *) (user_data);
//
//	if (engine->mouse_state == GDK_BUTTON_PRESS) // 鼠标按下状态
//	{
//		if ((event->x > 0) && (event->y > 0) && (event->x < 199) && (event->y
//				< 199))
//		{
//			engine->currentstroke.points
//					= g_renew(GdkPoint,engine->currentstroke.points,engine->currentstroke.segments +1  );
//
//			engine->currentstroke.points[engine->currentstroke.segments].x
//					= event->x;
//			engine->currentstroke.points[engine->currentstroke.segments].y
//					= event->y;
//			engine->currentstroke.segments++;
//			printf("move, x= %lf, Y=%lf, segments = %d \n",event->x,event->y,engine->currentstroke.segments);
//		}
//	}
//	else
//	{
////	printf("move start, x = %lf y = %lf \n",event->x_root -engine->lastpoint.x,event->y_root - engine->lastpoint.y);
//		gtk_window_move(GTK_WINDOW(widget),event->x_root -engine->lastpoint.x,event->y_root - engine->lastpoint.y);
//	}
//
//	gdk_window_invalidate_rect(widget->window, 0, TRUE);
//
//	return FALSE;
//}
//
//static gboolean on_button(GtkWidget* widget, GdkEventButton *event, gpointer user_data)
//{
//	IBusHandwriteEngine * engine;
//
//	engine = (IBusHandwriteEngine *) (user_data);
//
//	switch (event->type)
//	{
//
//	case GDK_BUTTON_PRESS:
//		if(event->button != 1)
//		{
//			engine->mouse_state = 0;
//			engine->lastpoint.x = event->x;
//			engine->lastpoint.y = event->y;
//			return TRUE;
//		}
//
//		if ((event->x > 0) && (event->y > 0) && (event->x < 199) && (event->y
//				< 199))
//		{
//
//			engine->mouse_state = GDK_BUTTON_PRESS;
//
//			g_print("mouse clicked\n");
//
//			engine->currentstroke.segments = 1;
//
//			engine->currentstroke.points = g_new(GdkPoint,1);
//
//			engine->currentstroke.points[0].x = event->x;
//			engine->currentstroke.points[0].y = event->y;
//
//		}
//		break;
//	case GDK_BUTTON_RELEASE:
//		engine->mouse_state = GDK_BUTTON_RELEASE;
//
//		ibus_handwrite_recog_append_stroke(engine->engine,engine->currentstroke);
//
//		ibus_handwrite_recog_domatch(engine->engine,10);
//
//		g_print("mouse released\n");
//
//		gdk_window_invalidate_rect(event->window, 0, TRUE);
//
//		break;
//	default:
//		return FALSE;
//	}
//	return TRUE;
//}



static gboolean gsfunc(gpointer data)
{
	XEvent ev;
	printf("***********************wait ev %s \n",__func__);
	XNextEvent(xdisplay,&ev);
	printf("***********************got %d ev %s  \n",ev.type,__func__);
	switch(ev.type)
	{
	case Expose:
		paint_ui(&ev,(IBusHandwriteEngine*)data);
		break;
	default:
		return TRUE;
	}
	return TRUE;
}

void UI_buildui(IBusHandwriteEngine * engine)
{
	if(!xdisplay)
		xdisplay = XOpenDisplay(NULL);

	GSource * gs;

	if (!engine->drawpanel)
	//建立绘图窗口, 建立空点
	{

		GIOChannel * gio = g_io_channel_unix_new(ConnectionNumber(xdisplay));

		gs = g_io_create_watch(gio,G_IO_IN);

		g_source_set_callback(gs,gsfunc,engine,0);

		int width = 200, height = 250;

		XSetWindowAttributes wa;
		wa.event_mask = ExposureMask | KeyPressMask;

		engine->drawpanel = XCreateWindow (xdisplay,  RootWindow(xdisplay,0), 0, 0, width, height,
		                                             0, CopyFromParent,
		                                             InputOutput, CopyFromParent,
		                                             CWEventMask, &wa);

//		engine->drawpanel = gtk_window_new(GTK_WINDOW_POPUP);

		g_source_attach(gs,NULL);
		g_source_unref(gs);
		g_io_channel_unref(gio);
				//	engine->GdkPoints = NULL;
	}
	//	gtk_widget_show_all(engine->dg_main_context_preparerawpanel);
}

void UI_show_ui(IBusHandwriteEngine * engine)
{
	printf("%s \n", __func__);
	if (engine->drawpanel)
	{
		XMapWindow(xdisplay,(Window)engine->drawpanel);
		XFlush(xdisplay);
	}
}

void UI_hide_ui(IBusHandwriteEngine * engine)
{
	if (engine->drawpanel)
	{
//		XUnmapWindow(xdisplay,(Window)engine->drawpanel);
	}
}

void UI_cancelui(IBusHandwriteEngine* engine)
{
	// 撤销绘图窗口，销毁点列表
	if (engine->drawpanel)
		XDestroyWindow(xdisplay,engine->drawpanel);
	engine->drawpanel = 0;
}

void UI_update(IBusHandwriteEngine* engine)
{
	//Xe

}
