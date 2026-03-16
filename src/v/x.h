#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/xpm.h>
#include <X11/keysymdef.h>

#define NCOLORS 110

typedef struct {
  Display * dis;
  Window    win;
  GC        gc_white, gc_black, gc_red, gc_dot[2], gcc[NCOLORS];
  Pixmap    px;
  Drawable  canv;
  XFontStruct * fontInfo;
  int       W, H, size;
} draw_world_t;
