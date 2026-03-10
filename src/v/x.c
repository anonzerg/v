#include "v.h"
#include "x.h"

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

  XColor c[NCOLORS] = {};

  if(colorscheme==V_COLORS){
    for(int i=0; i<NCOLORS; i++){
      c[i] = (XColor){.red = 0x9999, .green = 0x9999, .blue = 0x9999};
    }
    c[  1] = (XColor){.red = 0xBFFF, .green = 0xBFFF, .blue = 0xBFFF}; /* H  */
    c[  2] = (XColor){.red = 0xAAAA, .green = 0xFFFF, .blue = 0xFFFF}; /* He */
    c[  6] = (XColor){.red = 0x5FFF, .green = 0x5FFF, .blue = 0x5FFF}; /* C  */
    c[  5] = (XColor){.red = 0xFFFF, .green = 0xDDDD, .blue = 0xFFFF}; /* P  */
    c[  7] = (XColor){.red = 0x1FFF, .green = 0x1FFF, .blue = 0xBFFF}; /* N  */
    c[  8] = (XColor){.red = 0xBFFF, .green = 0x1FFF, .blue = 0x1FFF}; /* O  */
    c[  9] = (XColor){.red = 0xF500, .green = 0xFFFF, .blue = 0x8500}; /* F  */
    c[ 10] = (XColor){.red = 0xAAAA, .green = 0xFFFF, .blue = 0xFFFF}; /* Ne */
    c[ 14] = (XColor){.red = 0x5FFF, .green = 0x5FFF, .blue = 0x5FFF}; /* Si */
    c[ 15] = (XColor){.red = 0xFFFF, .green = 0xCCCC, .blue = 0x9999}; /* P  */
    c[ 16] = (XColor){.red = 0xFFFF, .green = 0xEEEE, .blue = 0x1111}; /* S  */
    c[ 17] = (XColor){.red = 0xCCCC, .green = 0xFFFF, .blue = 0x9999}; /* Cl */
    c[ 18] = (XColor){.red = 0xAAAA, .green = 0xFFFF, .blue = 0xFFFF}; /* Ar */
    c[ 28] = (XColor){.red = 0x9999, .green = 0x5555, .blue = 0xFFFF}; /* Ni */
    c[ 29] = (XColor){.red = 0xBBBB, .green = 0x7777, .blue = 0x3333}; /* Cu */
    c[ 34] = (XColor){.red = 0xFFFF, .green = 0xAAAA, .blue = 0x1111}; /* Se */
    c[ 35] = (XColor){.red = 0xAAAA, .green = 0x4444, .blue = 0x0000}; /* Br */
    c[ 36] = (XColor){.red = 0xAAAA, .green = 0xFFFF, .blue = 0xFFFF}; /* Kr */
    c[ 46] = (XColor){.red = 0x0000, .green = 0x6666, .blue = 0x7777}; /* Pd */
    c[ 47] = (XColor){.red = 0xAAAA, .green = 0xAAAA, .blue = 0xAAAA}; /* Ag */
    c[ 53] = (XColor){.red = 0xAAAA, .green = 0x0000, .blue = 0xFFFF}; /* I  */
    c[ 54] = (XColor){.red = 0xAAAA, .green = 0xFFFF, .blue = 0xFFFF}; /* Xe */
    c[ 78] = (XColor){.red = 0x3333, .green = 0x9999, .blue = 0xDDDD}; /* Pt */
    c[ 79] = (XColor){.red = 0xFFFF, .green = 0xCCCC, .blue = 0x0000}; /* Au */
  }
  else if(colorscheme==CPK_COLORS){
    c[  0] = (XColor){.red = 0x9999, .green = 0x9999, .blue = 0x9999};
    c[  1] = (XColor){.red = 0xBFFF, .green = 0xBFFF, .blue = 0xBFFF}; /* H  */
    c[  2] = (XColor){.red = 0xD9D9, .green = 0xFFFF, .blue = 0xFFFF}; /* He */
    c[  3] = (XColor){.red = 0xCCCC, .green = 0x8080, .blue = 0xFFFF}; /* Li */
    c[  4] = (XColor){.red = 0xC2C2, .green = 0xFFFF, .blue = 0x0000}; /* Be */
    c[  5] = (XColor){.red = 0xFFFF, .green = 0xB5B5, .blue = 0xB5B5}; /* B */
    c[  6] = (XColor){.red = 0x5FFF, .green = 0x5FFF, .blue = 0x5FFF}; /* C  */
    c[  7] = (XColor){.red = 0x1FFF, .green = 0x1FFF, .blue = 0xBFFF}; /* N  */
    c[  8] = (XColor){.red = 0xBFFF, .green = 0x1FFF, .blue = 0x1FFF}; /* O  */
    c[  9] = (XColor){.red = 0xF500, .green = 0xFFFF, .blue = 0x8500}; /* F  */
    c[ 10] = (XColor){.red = 0xAAAA, .green = 0xFFFF, .blue = 0xFFFF}; /* Ne */
    c[ 11] = (XColor){.red = 0xABAB, .green = 0x5C5C, .blue = 0xF2F2}; /* Na */
    c[ 12] = (XColor){.red = 0x8A8A, .green = 0xFFFF, .blue = 0x0000}; /* Mg */
    c[ 13] = (XColor){.red = 0xBFBF, .green = 0xA6A6, .blue = 0xA6A6}; /* Al */
    c[ 14] = (XColor){.red = 0x5FFF, .green = 0x5FFF, .blue = 0x5FFF}; /* Si */
    c[ 15] = (XColor){.red = 0xFFFF, .green = 0xCCCC, .blue = 0x9999}; /* P  */
    c[ 16] = (XColor){.red = 0xFFFF, .green = 0xEEEE, .blue = 0x1111}; /* S  */
    c[ 17] = (XColor){.red = 0xCCCC, .green = 0xFFFF, .blue = 0x9999}; /* Cl */
    c[ 18] = (XColor){.red = 0xAAAA, .green = 0xFFFF, .blue = 0xFFFF}; /* Ar */
    c[ 19] = (XColor){.red = 0x8F8F, .green = 0x4040, .blue = 0xD4D4}; /* K */
    c[ 20] = (XColor){.red = 0x3D3D, .green = 0xFFFF, .blue = 0x0000}; /* Ca */
    c[ 21] = (XColor){.red = 0xE6E6, .green = 0xE6E6, .blue = 0xE6E6}; /* Sc */
    c[ 22] = (XColor){.red = 0xBFBF, .green = 0xC2C2, .blue = 0xC7C7}; /* Ti */
    c[ 23] = (XColor){.red = 0xA6A6, .green = 0xA6A6, .blue = 0xABAB}; /* V */
    c[ 24] = (XColor){.red = 0x8A8A, .green = 0x9999, .blue = 0xC7C7}; /* Cr */
    c[ 25] = (XColor){.red = 0x9C9C, .green = 0x7A7A, .blue = 0xC7C7}; /* Mn */
    c[ 26] = (XColor){.red = 0xDDDD, .green = 0x6666, .blue = 0x3333}; /* Fe */
    c[ 27] = (XColor){.red = 0xEEEE, .green = 0x8888, .blue = 0x9999}; /* Co */
    c[ 28] = (XColor){.red = 0x5050, .green = 0xD0D0, .blue = 0x5050}; /* Ni */
    c[ 29] = (XColor){.red = 0xBBBB, .green = 0x7777, .blue = 0x3333}; /* Cu */
    c[ 30] = (XColor){.red = 0x7D7D, .green = 0x8080, .blue = 0xB0B0}; /* Zn */
    c[ 31] = (XColor){.red = 0xC2C2, .green = 0x8F8F, .blue = 0x8F8F}; /* Ga */
    c[ 32] = (XColor){.red = 0x6666, .green = 0x8F8F, .blue = 0x8F8F}; /* Ge */
    c[ 33] = (XColor){.red = 0xBDBD, .green = 0x8080, .blue = 0xE3E3}; /* As */
    c[ 34] = (XColor){.red = 0xFFFF, .green = 0xAAAA, .blue = 0x1111}; /* Se */
    c[ 35] = (XColor){.red = 0xAAAA, .green = 0x4444, .blue = 0x0000}; /* Br */
    c[ 36] = (XColor){.red = 0xAAAA, .green = 0xFFFF, .blue = 0xFFFF}; /* Kr */
    c[ 37] = (XColor){.red = 0x7070, .green = 0x2E2E, .blue = 0xB0B0}; /* Rb */
    c[ 38] = (XColor){.red = 0x0000, .green = 0xFFFF, .blue = 0x0000}; /* Sr */
    c[ 39] = (XColor){.red = 0x9494, .green = 0xFFFF, .blue = 0xFFFF}; /* Y */
    c[ 40] = (XColor){.red = 0x9494, .green = 0xE0E0, .blue = 0xE0E0}; /* Zr */
    c[ 41] = (XColor){.red = 0x7373, .green = 0xC2C2, .blue = 0xC9C9}; /* Nb */
    c[ 42] = (XColor){.red = 0x5454, .green = 0xB5B5, .blue = 0xB5B5}; /* Mo */
    c[ 43] = (XColor){.red = 0x3B3B, .green = 0x9E9E, .blue = 0x9E9E}; /* Tc */
    c[ 44] = (XColor){.red = 0x2424, .green = 0x8F8F, .blue = 0x8F8F}; /* Ru */
    c[ 45] = (XColor){.red = 0x0A0A, .green = 0x7D7D, .blue = 0x8C8C}; /* Rh */
    c[ 46] = (XColor){.red = 0x0000, .green = 0x6969, .blue = 0x8585}; /* Pd */
    c[ 47] = (XColor){.red = 0xC0C0, .green = 0xC0C0, .blue = 0xC0C0}; /* Ag */
    c[ 48] = (XColor){.red = 0xFFFF, .green = 0xD9D9, .blue = 0x8F8F}; /* Cd */
    c[ 49] = (XColor){.red = 0xA6A6, .green = 0x7575, .blue = 0x7373}; /* In */
    c[ 50] = (XColor){.red = 0x6666, .green = 0x8080, .blue = 0x8080}; /* Sn */
    c[ 51] = (XColor){.red = 0x9E9E, .green = 0x6363, .blue = 0xB5B5}; /* Sb */
    c[ 52] = (XColor){.red = 0xD4D4, .green = 0x7A7A, .blue = 0x0000}; /* Te */
    c[ 53] = (XColor){.red = 0x9494, .green = 0x0000, .blue = 0x9494}; /* I */
    c[ 54] = (XColor){.red = 0x4242, .green = 0x9E9E, .blue = 0xB0B0}; /* Xe */
    c[ 55] = (XColor){.red = 0x5757, .green = 0x1717, .blue = 0x8F8F}; /* Cs */
    c[ 56] = (XColor){.red = 0x0000, .green = 0xC9C9, .blue = 0x0000}; /* Ba */
    c[ 57] = (XColor){.red = 0x7070, .green = 0xD4D4, .blue = 0xFFFF}; /* La */
    c[ 58] = (XColor){.red = 0xFFFF, .green = 0xFFFF, .blue = 0xC7C7}; /* Ce */
    c[ 59] = (XColor){.red = 0xD9D9, .green = 0xFFFF, .blue = 0xC7C7}; /* Pr */
    c[ 60] = (XColor){.red = 0xC7C7, .green = 0xFFFF, .blue = 0xC7C7}; /* Nd */
    c[ 61] = (XColor){.red = 0xA3A3, .green = 0xFFFF, .blue = 0xC7C7}; /* Pm */
    c[ 62] = (XColor){.red = 0x8F8F, .green = 0xFFFF, .blue = 0xC7C7}; /* Sm */
    c[ 63] = (XColor){.red = 0x6161, .green = 0xFFFF, .blue = 0xC7C7}; /* Eu */
    c[ 64] = (XColor){.red = 0x4545, .green = 0xFFFF, .blue = 0xC7C7}; /* Gd */
    c[ 65] = (XColor){.red = 0x3030, .green = 0xFFFF, .blue = 0xC7C7}; /* Tb */
    c[ 66] = (XColor){.red = 0x1F1F, .green = 0xFFFF, .blue = 0xC7C7}; /* Dy */
    c[ 67] = (XColor){.red = 0x0000, .green = 0xFFFF, .blue = 0x9C9C}; /* Ho */
    c[ 68] = (XColor){.red = 0x0000, .green = 0xE6E6, .blue = 0x7575}; /* Er */
    c[ 69] = (XColor){.red = 0x0000, .green = 0xD4D4, .blue = 0x5252}; /* Tm */
    c[ 70] = (XColor){.red = 0x0000, .green = 0xBFBF, .blue = 0x3838}; /* Yb */
    c[ 71] = (XColor){.red = 0x0000, .green = 0xABAB, .blue = 0x2424}; /* Lu */
    c[ 72] = (XColor){.red = 0x4D4D, .green = 0xC2C2, .blue = 0xFFFF}; /* Hf */
    c[ 73] = (XColor){.red = 0x4D4D, .green = 0xA6A6, .blue = 0xFFFF}; /* Ta */
    c[ 74] = (XColor){.red = 0x2121, .green = 0x9494, .blue = 0xD6D6}; /* W */
    c[ 75] = (XColor){.red = 0x2626, .green = 0x7D7D, .blue = 0xABAB}; /* Re */
    c[ 76] = (XColor){.red = 0x2626, .green = 0x6666, .blue = 0x9696}; /* Os */
    c[ 77] = (XColor){.red = 0x1717, .green = 0x5454, .blue = 0x8787}; /* Ir */
    c[ 78] = (XColor){.red = 0xD0D0, .green = 0xD0D0, .blue = 0xE0E0}; /* Pt */
    c[ 79] = (XColor){.red = 0xFFFF, .green = 0xD1D1, .blue = 0x2323}; /* Au */
    c[ 80] = (XColor){.red = 0xB8B8, .green = 0xB8B8, .blue = 0xD0D0}; /* Hg */
    c[ 81] = (XColor){.red = 0xA6A6, .green = 0x5454, .blue = 0x4D4D}; /* Tl */
    c[ 82] = (XColor){.red = 0x5757, .green = 0x5959, .blue = 0x6161}; /* Pb */
    c[ 83] = (XColor){.red = 0x9E9E, .green = 0x4F4F, .blue = 0xB5B5}; /* Bi */
    c[ 84] = (XColor){.red = 0xABAB, .green = 0x5C5C, .blue = 0x0000}; /* Po */
    c[ 85] = (XColor){.red = 0x7575, .green = 0x4F4F, .blue = 0x4545}; /* At */
    c[ 86] = (XColor){.red = 0x4242, .green = 0x8282, .blue = 0x9696}; /* Rn */
    c[ 87] = (XColor){.red = 0x4242, .green = 0x0000, .blue = 0x6666}; /* Fr */
    c[ 88] = (XColor){.red = 0x0000, .green = 0x7D7D, .blue = 0x0000}; /* Ra */
    c[ 89] = (XColor){.red = 0x7070, .green = 0xABAB, .blue = 0xFAFA}; /* Ac */
    c[ 90] = (XColor){.red = 0x0000, .green = 0xBABA, .blue = 0xFFFF}; /* Th */
    c[ 91] = (XColor){.red = 0x0000, .green = 0xA1A1, .blue = 0xFFFF}; /* Pa */
    c[ 92] = (XColor){.red = 0x0000, .green = 0x8F8F, .blue = 0xFFFF}; /* U */
    c[ 93] = (XColor){.red = 0x0000, .green = 0x8080, .blue = 0xFFFF}; /* Np */
    c[ 94] = (XColor){.red = 0x0000, .green = 0x6B6B, .blue = 0xFFFF}; /* Pu */
    c[ 95] = (XColor){.red = 0x5454, .green = 0x5C5C, .blue = 0xF2F2}; /* Am */
    c[ 96] = (XColor){.red = 0x7878, .green = 0x5C5C, .blue = 0xE3E3}; /* Cm */
    c[ 97] = (XColor){.red = 0x8A8A, .green = 0x4F4F, .blue = 0xE3E3}; /* Bk */
    c[ 98] = (XColor){.red = 0xA1A1, .green = 0x3636, .blue = 0xD4D4}; /* Cf */
    c[ 99] = (XColor){.red = 0xB3B3, .green = 0x1F1F, .blue = 0xD4D4}; /* Es */
    c[100] = (XColor){.red = 0xB3B3, .green = 0x1F1F, .blue = 0xBABA}; /* Fm */
    c[101] = (XColor){.red = 0xB3B3, .green = 0x0D0D, .blue = 0xA6A6}; /* Md */
    c[102] = (XColor){.red = 0xBDBD, .green = 0x0D0D, .blue = 0x8787}; /* No */
    c[103] = (XColor){.red = 0xC7C7, .green = 0x0000, .blue = 0x6666}; /* Lr */
    c[104] = (XColor){.red = 0xCCCC, .green = 0x0000, .blue = 0x5959}; /* Rf */
    c[105] = (XColor){.red = 0xD1D1, .green = 0x0000, .blue = 0x4F4F}; /* Db */
    c[106] = (XColor){.red = 0xD9D9, .green = 0x0000, .blue = 0x4545}; /* Sg */
    c[107] = (XColor){.red = 0xE0E0, .green = 0x0000, .blue = 0x3838}; /* Bh */
    c[108] = (XColor){.red = 0xE6E6, .green = 0x0000, .blue = 0x2E2E}; /* Hs */
  }

  Colormap colmap = DefaultColormap(dis, DefaultScreen(dis));
  for(int i=0; i<NCOLORS; i++){
    gcc[i] = XCreateGC (dis, win, 0, 0);
    XAllocColor(dis, colmap, c+i);
    XSetForeground (dis, gcc[i], c[i].pixel);
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
  if(!fontname[0]){
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
}

void drawvertices(double * v, double scale, double xy0[2]){
  double d = MIN(H, W)*scale;
  int iw;
#define LINE(I,J) \
  iw=( v[(I)*3+2]>0.0 || v[(J)*3+2]>0.0) ? 0 : 1; \
  XDrawLine(dis, win, gc_dot[iw],\
      W/2+d*(xy0[0]+v[(I)*3]), H/2-d*(xy0[1]+v[(I)*3+1]),\
      W/2+d*(xy0[0]+v[(J)*3]), H/2-d*(xy0[1]+v[(J)*3+1]));
  LINE(0,1);
  LINE(0,2);
  LINE(0,3);
  LINE(1,4);
  LINE(1,5);
  LINE(2,4);
  LINE(2,6);
  LINE(3,5);
  LINE(3,6);
  LINE(4,7);
  LINE(5,7);
  LINE(6,7);
#undef LINE
  return;
}

void drawshell(double rmin, double rmax, double scale, double xy0[2]){
  double d = MIN(H,W)*scale;
  rmax *= d;
  rmin *= d;
  int x = W/2+d*xy0[0];
  int y = H/2-d*xy0[1];
  XDrawArc(dis, win, gc_dot[0], x-rmax, y-rmax, 2*rmax, 2*rmax, 0, 360*64);
  XDrawArc(dis, win, gc_dot[1], x-rmin, y-rmin, 2*rmin, 2*rmin, 0, 360*64);
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

