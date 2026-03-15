#include "v.h"
#include "x.h"
#include "vec2.h"

extern Display * dis;
extern int       screen;
extern Window    win;
extern GC        gc_white, gc_black, gc_red, gc_dot[2], gcc[NCOLORS];
extern Pixmap    px;
extern Drawable  canv;
extern XFontStruct * fontInfo;

extern int W,H;

void close_x(void) {
  XFreeGC(dis, gc_red);
  XFreeGC(dis, gc_black);
  XFreeGC(dis, gc_white);
  XFreeGC(dis, gc_dot[0]);
  XFreeGC(dis, gc_dot[1]);
  for(int i=0; i<NCOLORS; i++){
    XFreeGC(dis, gcc[i]);
  }
  XDestroyWindow (dis, win);
  XFreePixmap    (dis, px);
  if(fontInfo){
    XFreeFont    (dis, fontInfo);
  }
  XCloseDisplay  (dis);
};

static void setcolors(colorscheme_t colorscheme){

  static const unsigned int palette_v[NCOLORS][3]={
    #include "palette_v.h"
  };

  static const unsigned int palette_cpk[NCOLORS][3]={
    #include "palette_cpk.h"
  };

  const unsigned int (*palette)[3];
  switch (colorscheme){
    case CPK_COLORS:
      palette = palette_cpk;
      break;
    case V_COLORS:
    default:
      palette = palette_v;
      break;
  }

  Colormap colmap = DefaultColormap(dis, DefaultScreen(dis));
  for(int i=0; i<NCOLORS; i++){
    int j = (palette[i][0] || palette[i][1] || palette[i][2]) ? i : 0;
    XColor c = {.red = palette[j][0], .green = palette[j][1], .blue = palette[j][2]};
    gcc[i] = XCreateGC (dis, win, 0, 0);
    XAllocColor(dis, colmap, &c);
    XSetForeground(dis, gcc[i], c.pixel);
  }
  return;
}

void init_x(const char * const capt, const colorscheme_t colorscheme){

  dis    = XOpenDisplay((char *)0);
  screen = DefaultScreen(dis);
  W = DisplayWidth  (dis, screen);
  H = DisplayHeight (dis, screen);

  unsigned long bp = BlackPixel (dis, screen);
  unsigned long wp = WhitePixel (dis, screen);
  unsigned long red_pixel = 0xff0000;

  win = XCreateSimpleWindow(dis, DefaultRootWindow(dis),
      0, 0, W, H, 0, bp, wp);

  XSetStandardProperties(dis, win, capt, "icon", None, NULL, 0, NULL);
  XSelectInput   (dis, win, ExposureMask|KeyPressMask|StructureNotifyMask   |ButtonPressMask|ButtonReleaseMask|PointerMotionMask	  );

  gc_white = XCreateGC (dis, win, 0, 0);
  XSetBackground (dis, gc_white, wp);
  XSetForeground (dis, gc_white, wp);

  gc_black = XCreateGC (dis, win, 0, 0);
  XSetBackground (dis, gc_black, wp);
  XSetForeground (dis, gc_black, bp);
  XSetLineAttributes(dis, gc_black, 2, 0, 0, 0);

  gc_red = XCreateGC (dis, win, 0, 0);
  XSetBackground (dis, gc_red, wp);
  XSetForeground (dis, gc_red, red_pixel);

  gc_dot[0] = XCreateGC (dis, win, 0, 0);
  XSetBackground (dis, gc_black, wp);
  XSetForeground (dis, gc_black, bp);
  XSetLineAttributes(dis, gc_dot[0], 2, 1, 0, 0);

  gc_dot[1] = XCreateGC (dis, win, 0, 0);
  XSetBackground (dis, gc_black, wp);
  XSetForeground (dis, gc_black, bp);
  XSetLineAttributes(dis, gc_dot[1], 0, 1, 0, 0);

  setcolors(colorscheme);

  XClearWindow   (dis, win);
  XMapRaised     (dis, win);

  px = XCreatePixmap(dis, win, W, H, DefaultDepth(dis, 0));
  return;
};

static void autosize_font(char * fontname){
  const int screen_sizes[] = {1200, 1080, 960, 900, 840, 768};
  const int font_sizes[]   = {  24,   20,  18,  16,  15,  14}; //  font_size='ceil'(screen_size) / 60
  int font_size = 13;
  int screen_size = MIN(W, H);
  for(int i=0; i<sizeof(screen_sizes)/sizeof(screen_sizes[0]); i++){
    if(screen_size>screen_sizes[i]){
      font_size = font_sizes[i];
      break;
    }
  }
  sprintf(fontname, "*x%d", font_size);
  return;
}

void init_font(char * fontname){
  if(!fontname){
    styp s;
    fontname = s;
    autosize_font(fontname);
  }
  fontInfo = XLoadQueryFont(dis, fontname);
  if(fontInfo){
    XSetFont (dis, gc_black, fontInfo->fid);
  }
  else{
    PRINT_WARN("cannot load font '%s'\n", fontname);
  }
  return;
}

void textincorner(const char * const text1, const char * const text2){
  int voffset = fontInfo ? (fontInfo->ascent + fontInfo->descent + 5) : 20;
  XDrawString(dis, win, gc_black, 10, voffset, text1, strlen(text1));
  if(text2){
    XDrawString(dis, win, gc_black, 10, voffset*2, text2, strlen(text2));
  }
  return;
}

void textincorner2(const char * const text1){
  int voffset = fontInfo ? (fontInfo->ascent + fontInfo->descent + 5) : 20;
  XDrawString(dis, win, gc_red, 10, voffset*3, text1, strlen(text1));
  return;
}

void setcaption(const char * const capt){
  XStoreName(dis, win, capt);
  return;
}

void draw_edge(double vi[3], double vj[3], double scale, double xy0[2]){
  int iw = (vi[2]>0.0 || vj[2]>0.0) ? 0 : 1;
  double pi[2], pj[2];
  r2sum(pi, xy0, vi);
  r2sum(pj, xy0, vj);
  XDrawLine(dis, win, gc_dot[iw],
      W/2+scale*pi[0], H/2-scale*pi[1],
      W/2+scale*pj[0], H/2-scale*pj[1]);
  return;
}

void drawvertices(double * v, double scale, double xy0[2]){
  double d = MIN(H, W)*scale;
#define LINE(i,j) draw_edge(v+(i)*3, v+(j)*3, d, xy0)
  for(int i=0; i<8; i+=2){
    LINE(i,i+1); // || z-axis
  }
  for(int j=0; j<2; j++){
    for(int i=0; i<2; i++){
      LINE(i*4+j, i*4+2+j);  // || y-axis
      LINE(i*2+j, i*2+4+j);  // || x-axis
    }
  }
#undef LINE
  return;
}

void drawshell(double rmin, double rmax, double scale, double xy0[2]){
  double d = MIN(H,W)*scale;
  double r[] = {rmax*d, rmin*d};
  int x = W/2+d*xy0[0];
  int y = H/2-d*xy0[1];
  for(int i=0; i<2; i++){
    XDrawArc(dis, win, gc_dot[i], x-r[i], y-r[i], 2*r[i], 2*r[i], 0, 360*64);
  }
  return;
}

int savepic(char * s){
  XpmAttributes a;
  a.valuemask = (0 | XpmSize) ;
  a.width     = W;
  a.height    = H;
  XCopyArea (dis, win, px, gc_white, 0, 0, W, H, 0, 0);   /* with text */
  return XpmWriteFileFromPixmap(dis, s, px, 0, &a);
}

