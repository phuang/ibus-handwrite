/*
 * UI_gtk.c -- provide UI via gtk
 *
 *  Created on: 2010-2-4
 *      Author: cai
 */


#include <gtk/gtk.h>
#include "engine.h"
#include "UI.h"

static gboolean paint_ui(GtkWidget *widget, GdkEventExpose *event,
		gpointer user_data)
{
	GdkGC *gc;

	IBusHandwriteEngine * engine;

	LineStroke cl;
	int i;

	MatchedChar * matched;

	engine = (IBusHandwriteEngine *) (user_data);

	gc = gdk_gc_new(widget->window);

	gdk_draw_rectangle(widget->window, gc,0,0,0,199,199);

	gdk_draw_rectangle(widget->window, gc,0,200,0,399,199);

	puts(__func__);

	//已经录入的笔画

	for (i = 0; i < engine->engine->strokes->len ; i++ )
	{
		printf("drawing %d th line, total %d\n",i,engine->engine->strokes->len);
		cl =  g_array_index(engine->engine->strokes,LineStroke,i);
		gdk_draw_lines(widget->window, gc, cl.points,cl.segments );
	}
	//当下笔画
	if ( engine->currentstroke.segments && engine->currentstroke.points )
		gdk_draw_lines(widget->window, gc, engine->currentstroke.points,
				engine->currentstroke.segments);


	int munber = ibus_handwrite_recog_getmatch(engine->engine,&matched,0);

	//画10个侯选字
	for (i = 0; i < munber ; ++i)
	{

		char drawtext[32]={0};

		sprintf(drawtext,"%d.%s",i,matched[i].chr);

		PangoLayout * layout = gtk_widget_create_pango_layout(widget,drawtext);

		gdk_draw_layout(widget->window, gc, (i % 5) * 40 + 3, 205 + (20 * (i / 5)),	layout);
		g_object_unref(layout);
	}

	g_object_unref(gc);
	return TRUE;
}


static gboolean on_mouse_move(GtkWidget *widget, GdkEventMotion *event,
		gpointer user_data)
{
	IBusHandwriteEngine * engine;

	engine = (IBusHandwriteEngine *) (user_data);

	GdkCursorType ct ;

	ct = event->y < 200 ?  GDK_PENCIL:GDK_CENTER_PTR;

	if( event->state & (GDK_BUTTON2_MASK |GDK_BUTTON3_MASK ))
		ct = GDK_FLEUR;
//	ct = GDK_FLEUR;

	GdkCursor * cursor = gdk_cursor_new(ct);// event->y <200 ?  GDK_PENCIL:GDK_HAND2);

	gdk_window_set_cursor(widget->window, cursor);

	gdk_cursor_unref(cursor);

	if (engine->mouse_state == GDK_BUTTON_PRESS) // 鼠标按下状态
	{
		if ((event->x > 0) && (event->y > 0) && (event->x < 199) && (event->y
				< 199))
		{


			engine->currentstroke.points
					= g_renew(GdkPoint,engine->currentstroke.points,engine->currentstroke.segments +1  );

			engine->currentstroke.points[engine->currentstroke.segments].x
					= event->x;
			engine->currentstroke.points[engine->currentstroke.segments].y
					= event->y;
			engine->currentstroke.segments++;
			printf("move, x= %lf, Y=%lf, segments = %d \n",event->x,event->y,engine->currentstroke.segments);
		}
		gdk_window_invalidate_rect(widget->window, 0, TRUE);
	}
	else if( event->state & (GDK_BUTTON2_MASK |GDK_BUTTON3_MASK ))
	{
//	printf("move start, x = %lf y = %lf \n",event->x_root -engine->lastpoint.x,event->y_root - engine->lastpoint.y);
		gtk_window_move(GTK_WINDOW(widget),event->x_root -engine->lastpoint.x,event->y_root - engine->lastpoint.y);
	}

	return FALSE;
}

static gboolean on_button(GtkWidget* widget, GdkEventButton *event, gpointer user_data)
{
	int i;
	IBusHandwriteEngine * engine;

	engine = (IBusHandwriteEngine *) (user_data);

	switch (event->type)
	{

	case GDK_BUTTON_PRESS:
		if(event->button != 1)
		{
			engine->mouse_state = 0;
			engine->lastpoint.x = event->x;
			engine->lastpoint.y = event->y;
			return TRUE;
		}

		engine->mouse_state = GDK_BUTTON_PRESS;
		if ((event->x > 0) && (event->y > 0) && (event->x < 199) && (event->y
				< 199))
		{

			g_print("mouse clicked\n");

			engine->currentstroke.segments = 1;

			engine->currentstroke.points = g_new(GdkPoint,1);

			engine->currentstroke.points[0].x = event->x;
			engine->currentstroke.points[0].y = event->y;

		}
		else if ((event->x > 0) && (event->y > 0) && (event->x < 199))
		{
			int x = event->x;
			int y = event->y;
			//看鼠标点击的是哪个字，吼吼

			for (i = 9; i >= 0; --i)
			{
				if (((i % 5) * 40 + 3) <= x && ((205 + (20 * (i / 5))) <= y))
				{
					IBUS_HANDWRITE_ENGINE_GET_CLASS(engine)->commit_text(engine, i);
					break;
				}
			}
		}
		break;
	case GDK_BUTTON_RELEASE:
		engine->mouse_state = GDK_BUTTON_RELEASE;

		ibus_handwrite_recog_append_stroke(engine->engine,engine->currentstroke);


		engine->currentstroke.segments = 0;
		g_free(engine->currentstroke.points);

		engine->currentstroke.points = NULL;

		ibus_handwrite_recog_domatch(engine->engine,10);

		g_print("mouse released\n");

		gdk_window_invalidate_rect(event->window, 0, TRUE);

		break;
	default:
		return FALSE;
	}
	return TRUE;
}


void UI_buildui(IBusHandwriteEngine * engine)
{
	GdkPixmap * pxmp;
	GdkGC * gc;
	GdkColor black, white;

	GdkColormap* colormap = gdk_colormap_get_system();

	gdk_color_black(colormap, &black);
	gdk_color_white(colormap, &white);

	g_object_unref(colormap);

	int R = 5;

	if (!engine->drawpanel)
	//建立绘图窗口, 建立空点
	{
		engine->drawpanel = gtk_window_new(GTK_WINDOW_POPUP);
		gtk_window_move((GtkWindow*) engine->drawpanel, 500, 550);
		gtk_widget_add_events(GTK_WIDGET(engine->drawpanel),
				GDK_POINTER_MOTION_MASK | GDK_BUTTON_RELEASE_MASK
						| GDK_BUTTON_PRESS_MASK | GDK_EXPOSURE_MASK);
		g_signal_connect_after(G_OBJECT(engine->drawpanel),"motion_notify_event",G_CALLBACK(on_mouse_move),engine);
		g_signal_connect(G_OBJECT(engine->drawpanel),"expose-event",G_CALLBACK(paint_ui),engine);
		g_signal_connect(G_OBJECT(engine->drawpanel),"button-release-event",G_CALLBACK(on_button),engine);
		g_signal_connect(G_OBJECT(engine->drawpanel),"button-press-event",G_CALLBACK(on_button),engine);

		gtk_window_resize(GTK_WINDOW(engine->drawpanel), 200, 250);

		gtk_widget_show(engine->drawpanel);

		pxmp = gdk_pixmap_new(NULL, 200, 250, 1);
		gc = gdk_gc_new(GDK_DRAWABLE(pxmp));

		gdk_gc_set_foreground(gc, &black);

		gdk_draw_rectangle(GDK_DRAWABLE(pxmp), gc, 1, 0, 0, 200, 250);

		gdk_gc_set_foreground(gc, &white);

		gdk_draw_arc(GDK_DRAWABLE(pxmp), gc, 1, 0, 0, R*2, R*2, 0, 360 * 64);
		gdk_draw_arc(GDK_DRAWABLE(pxmp), gc, 1, 200 - R*2, 0, R*2, R*2, 0, 360
				* 64);
		gdk_draw_arc(GDK_DRAWABLE(pxmp), gc, 1, 200 - R*2, 250 - R*2, R*2, R*2, 0,
				360 * 64);
		gdk_draw_arc(GDK_DRAWABLE(pxmp), gc, 1, 0, 250 - R*2, R*2, R*2, 0, 360
				* 64);
		gdk_draw_rectangle(GDK_DRAWABLE(pxmp), gc, 1, 0, R, 200, 250 - R*2);
		gdk_draw_rectangle(GDK_DRAWABLE(pxmp), gc, 1, R, 0, 200 - R*2, 250);
		gdk_window_shape_combine_mask(engine->drawpanel->window, pxmp, 0, 0);
		g_object_unref(gc);
		g_object_unref(pxmp);
		gtk_window_set_opacity(GTK_WINDOW(engine->drawpanel), 0.62);
		//	engine->GdkPoints = NULL;
	}
	//	gtk_widget_show_all(engine->drawpanel);
}

void UI_show_ui(IBusHandwriteEngine * engine)
{
	GdkCursor* cursor;

	printf("%s \n", __func__);
	if (engine->drawpanel)
	{
		gtk_widget_show(engine->drawpanel);
	}
}

void UI_hide_ui(IBusHandwriteEngine * engine)
{
	if (engine->drawpanel)
	{
		gtk_widget_hide(engine->drawpanel);
	}
}

void UI_cancelui(IBusHandwriteEngine* engine)
{
	// 撤销绘图窗口，销毁点列表
	if (engine->drawpanel)
		gtk_widget_destroy(engine->drawpanel);
	engine->drawpanel = NULL;
}
