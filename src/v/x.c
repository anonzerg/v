#include "v.h"
#include "x.h"
#include "vec2.h"

extern draw_world_t world;

void close_x(void) {
  XFreeGC(world.dis, world.gc_red);
  XFreeGC(world.dis, world.gc_black);
  XFreeGC(world.dis, world.gc_white);
  XFreeGC(world.dis, world.gc_dot[0]);
  XFreeGC(world.dis, world.gc_dot[1]);
  for(int i=0; i<NCOLORS; i++){
    XFreeGC(world.dis, world.gcc[i]);
  }
  XDestroyWindow (world.dis, world.win);
  XFreePixmap    (world.dis, world.px);
  if(world.fontInfo){
    XFreeFont    (world.dis, world.fontInfo);
  }
  XCloseDisplay  (world.dis);
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

  Colormap colmap = DefaultColormap(world.dis, DefaultScreen(world.dis));
  for(int i=0; i<NCOLORS; i++){
    int j = (palette[i][0] || palette[i][1] || palette[i][2]) ? i : 0;
    XColor c = {.red = palette[j][0], .green = palette[j][1], .blue = palette[j][2]};
    world.gcc[i] = XCreateGC (world.dis, world.win, 0, 0);
    XAllocColor(world.dis, colmap, &c);
    XSetForeground(world.dis, world.gcc[i], c.pixel);
  }
  return;
}

void init_x(const char * const capt, const colorscheme_t colorscheme){

  Display * dis = world.dis = XOpenDisplay((char *)0);
  int screen = DefaultScreen(dis);
  world.W = DisplayWidth  (dis, screen);
  world.H = DisplayHeight (dis, screen);

  unsigned long bp = BlackPixel (dis, screen);
  unsigned long wp = WhitePixel (dis, screen);
  unsigned long red_pixel = 0xff0000;

  world.win = XCreateSimpleWindow(dis, DefaultRootWindow(dis),
      0, 0, world.W, world.H, 0, bp, wp);

  XSetStandardProperties(dis, world.win, capt, "icon", None, NULL, 0, NULL);
  XSelectInput   (dis, world.win, ExposureMask|KeyPressMask|StructureNotifyMask   |ButtonPressMask|ButtonReleaseMask|PointerMotionMask	  );

  world.gc_white = XCreateGC (dis, world.win, 0, 0);
  XSetBackground (dis, world.gc_white, wp);
  XSetForeground (dis, world.gc_white, wp);

  world.gc_black = XCreateGC (dis, world.win, 0, 0);
  XSetBackground (dis, world.gc_black, wp);
  XSetForeground (dis, world.gc_black, bp);
  XSetLineAttributes(dis, world.gc_black, 2, 0, 0, 0);

  world.gc_red = XCreateGC (dis, world.win, 0, 0);
  XSetBackground (dis, world.gc_red, wp);
  XSetForeground (dis, world.gc_red, red_pixel);

  world.gc_dot[0] = XCreateGC (dis, world.win, 0, 0);
  XSetBackground (dis, world.gc_black, wp);
  XSetForeground (dis, world.gc_black, bp);
  XSetLineAttributes(dis, world.gc_dot[0], 2, 1, 0, 0);

  world.gc_dot[1] = XCreateGC (dis, world.win, 0, 0);
  XSetBackground (dis, world.gc_black, wp);
  XSetForeground (dis, world.gc_black, bp);
  XSetLineAttributes(dis, world.gc_dot[1], 0, 1, 0, 0);

  setcolors(colorscheme);

  XClearWindow   (dis, world.win);
  XMapRaised     (dis, world.win);

  world.px = XCreatePixmap(dis, world.win, world.W, world.H, DefaultDepth(dis, 0));
  world.canv = world.px;
  return;
};

static void autosize_font(char * fontname){
  const int screen_sizes[] = {1200, 1080, 960, 900, 840, 768};
  const int font_sizes[]   = {  24,   20,  18,  16,  15,  14}; //  font_size='ceil'(screen_size) / 60
  int font_size = 13;
  int screen_size = MIN(world.W, world.H);
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
  world.fontInfo = XLoadQueryFont(world.dis, fontname);
  if(world.fontInfo){
    XSetFont (world.dis, world.gc_black, world.fontInfo->fid);
    XSetFont (world.dis, world.gc_red,   world.fontInfo->fid);
  }
  else{
    PRINT_WARN("cannot load font '%s'\n", fontname);
  }
  return;
}

void textincorner(const char * const lines[MAX_LINES], const int red[MAX_LINES]){
  int voffset = world.fontInfo ? (world.fontInfo->ascent + world.fontInfo->descent + 5) : 20;
  for(int i=0; i<MAX_LINES; i++){
    if(lines[i]){
      XDrawString(world.dis, world.canv, red[i] ? world.gc_red:world.gc_black, 10, voffset*(i+1), lines[i], strlen(lines[i]));
    }
  }
  return;
}

void setcaption(const char * const capt){
  XStoreName(world.dis, world.win, capt);
  return;
}

void draw_edge(double vi[3], double vj[3], double scale, double xy0[2]){
  int iw = (vi[2]>0.0 || vj[2]>0.0) ? 0 : 1;
  double pi[2], pj[2];
  r2sum(pi, xy0, vi);
  r2sum(pj, xy0, vj);
  XDrawLine(world.dis, world.canv, world.gc_dot[iw],
      world.W/2+scale*pi[0], world.H/2-scale*pi[1],
      world.W/2+scale*pj[0], world.H/2-scale*pj[1]);
  return;
}

void drawvertices(double * v, double scale, double xy0[2]){
  double d = MIN(world.H, world.W)*scale;
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
  double d = MIN(world.H,world.W)*scale;
  double r[] = {rmax*d, rmin*d};
  int x = world.W/2+d*xy0[0];
  int y = world.H/2-d*xy0[1];
  for(int i=0; i<2; i++){
    XDrawArc(world.dis, world.canv, world.gc_dot[i], x-r[i], y-r[i], 2*r[i], 2*r[i], 0, 360*64);
  }
  return;
}

int savepic(char * s){
  XpmAttributes a;
  a.valuemask = (0 | XpmSize) ;
  a.width     = world.W;
  a.height    = world.H;
  return XpmWriteFileFromPixmap(world.dis, s, world.px, 0, &a);
}

void clear_canv(){ // TODO other canvases?
  if(world.canv == world.px){
    XFillRectangle(world.dis, world.canv, world.gc_white, 0, 0, world.W, world.H);\
  }
  return;
}

void fill_canv(){
  if(world.canv == world.px){ // TODO other canvases?
    XCopyArea(world.dis, world.canv, world.win, world.gc_white, 0, 0, world.W, world.H, 0, 0);
  }
  return;
}
