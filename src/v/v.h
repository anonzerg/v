#include "mol.h"

#define DEFAULT_TIMEOUT  20000
#define DEFAULT_SYMTOL   1e-3
#define NKP  256
#define BONDS_MAX 32
#define POINTER_SPEED 2.0
#define STRLEN 256
#define BIGSTRLEN 4096

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
  int    * q;            // charges of atoms
  double * r;            // coordinates of atoms
  const char * fname;    // file name

  int nf[2];             // number of molecule in file, file size
  styp   sym;            // point group
  bondstr bonds;
} atcoord;

typedef struct {
  double * freq;  // frequencies (cm-1)
  double * ints;  // intensities
  double * disp;  // displacements
  double * mass;  // masses
  double * r0;    // atom configuration at the central point
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
vibr_t * mode_read(FILE * f, int na);
// ac3_read*.c
int read_cart_atom(FILE * f, int n, mol * m);
atcoord * atcoord_fill(mol * m, int b, geompars geom);
atcoord * ac3_read(readpars read, int b, geompars geom, format_t * format);
mol * ac3_read_in (FILE * f);
mol * ac3_read_out(FILE * f);
mol * ac3_read_xyz(FILE * f);

// man.c
void printman(FILE * f, char * exename);
// cli.c
allpars cli_parse(int argc, char ** argv);

// loop.c
void main_loop(object * ent, drawpars * dp, ptf kp[NKP]);

// ac3_draw.c
void ac3_draw      (atcoord * ac, rendpars rend);
// ac3_print.c
void ac3_print    (atcoord * ac, rendpars rend);
void ac3_print_xyz(atcoord * ac, rendpars rend);
void ac3_print2fig(atcoord * ac, rendpars rend, double * v);
// bonds.c
void bonds_fill(bondpars bond, atcoord * ac);

// get_atpar.c
double getradius(int q);
double getmaxradius(int n, int * q);
const char * getname(int q);
int get_element(char * s);

// x.c
void close_x      (void);
void init_x       (const char * const capt, const colorscheme_t colorscheme);
void init_font    (char * fontname);
void textincorner (const char * const text1, const char * const text2);
void textincorner2(const char * const text1);
void setcaption   (const char * const capt);
void drawvertices (double * v, double scale, double xy0[2]);
void drawshell    (double rmin, double rmax, double scale, double * xy0);
int  savepic      (char * s);
// xinput.c
int process_x_input(char input_text[STRLEN], unsigned int keycode);

// tools.c
void obj_free(object * ent);
void newmol_prep(object * acs, drawpars * dp);
void ac3_text(atcoord * ac, drawpars * dp);
void vibro_text(vibr_t * ms, drawpars * dp);
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
