#include "mol.h"

#define DEFAULT_TIMEOUT  20000
#define DEFAULT_SYMTOL   1e-3
#define NKP  256
#define BONDS_MAX 32
#define POINTER_SPEED 2.0
#define STRLEN 256
#define MAX_LINES 5

#include "pars.h"

typedef void (* ptf )();

typedef enum {
  UNKNOWN_FORMAT,
  XYZ,
  IN,
  OUT,
} format_t;

typedef struct {
  int      flag;    // whether bonds are up-to-date. 0: no, 1: yes
  double   rl;      // the last used bond length scale factor
  int    * a;       // lists of bonded atoms
  double * r;       // distances to the bonded atoms
} bondstr;

typedef struct {
  int      n;            // number of atoms
  int    * q;            // atom charges
  double * r;            // atom coordinates (rotated)
  const char * fname;    // file name

  int rotated;           // is `r` up-to-date
  double * r0;           // atom coordinates (original)

  int nf[2];             // number of molecule in file, file size
  styp   sym;            // point group
  bondstr bonds;
  cellpars cell;
} atcoord;

typedef struct {
  double * freq;  // frequencies (cm-1)
  double * ints;  // intensities
  double * disp;  // displacements
  double * mass;  // masses
  int      n;     // number of modes
} vibr_t;

typedef struct {
  int n, Nmem;
  atcoord ** m;
  vibr_t * vib;
} object;


// load.c
object * acs_from_var(int n, mol * m, vibr_t vib, allpars * ap);
void acs_readmore  (readpars read, int b, geompars geom, object * acs);
object * read_files(allpars * ap);
// scale.c
double ac3_scale(atcoord * ac);
double acs_scale(object * acs);
// mode_read.c
vibr_t * make_vibr_t(int n_modes, int n_atoms);
vibr_t * mode_read(FILE * f, int na);
// ac3_read*.c
void mol2cell(atcoord * m);
int read_cart_atom(FILE * f, int n, mol * m);
atcoord * atcoord_fill(mol * m0, const render_bonds_t b, const geompars geom, const double cell[9]);
atcoord * ac3_read(readpars read, const render_bonds_t b, const geompars geom, format_t * format);
mol * ac3_read_in (FILE * f);
mol * ac3_read_out(FILE * f);
mol * ac3_read_xyz(FILE * f, int * cell_found, double * cell);

// man.c
void printman(FILE * f, char * exename);
// cli.c
allpars cli_parse(int argc, char ** argv);

// loop.c
void wait_for_configure(object * ent, drawpars * dp);
void main_loop(object * ent, drawpars * dp, ptf kp[NKP]);

// redraw.c
void exp_redraw(object * ent, drawpars * dp);

// ac3_draw.c
void ac3_draw     (atcoord * ac, rendpars * rend);
// ac3_print.c
void ac3_print    (atcoord * ac, rendpars * rend);
void ac3_print_xyz(atcoord * ac, rendpars * rend);
void ac3_print2fig(atcoord * ac, rendpars * rend);
// bonds.c
void bonds_fill(bondpars bond, atcoord * ac);

// get_atpar.c
double getradius(int q);
double getmaxradius(int n, const int * q);
const char * getname(int q);
int get_element(const char * s);

// x.c
void close_x      (void);
void init_x       (const char * const capt, const colorscheme_t colorscheme);
void init_font    (char * fontname);
void textincorner (const char * const lines[MAX_LINES], const int red[MAX_LINES]);
void setcaption   (const char * const capt);
void draw_vertices(const double v[8*3], rendpars * rend);
void draw_shell   (const double r[2], rendpars * rend);
int  savepic      (char * s);
void clear_canv();
void fill_canv();
// xinput.c
int process_x_input(char input_text[STRLEN], unsigned int keycode);

// tools.c
void obj_free(object * ent);
void pg(atcoord * a, double symtol);

// headless.c
void run_commands(FILE * f, char * command, drawpars * dp, object * ent);
int headless(drawpars * dp, object * ent);

// main.c
int main (int argc, char * argv[]);

// api.c
void PRINTOUT(FILE * f, char * format, ...);
object * READ_FILES(allpars * ap);
int SHOULD_PRINT_MAN(int argc);
