typedef enum {
  UNKNOWN,
  AT3COORDS,
  VIBRO,
} task_t;

typedef enum {
  V_COLORS,
  CPK_COLORS,
} colorscheme_t;

typedef struct {
  int               gui;   // if gui is enabled
  char       * fontname;   // font
  int     input_files_n;   // number of input files
  char   ** input_files;   // input files
  colorscheme_t  colors;   // colorscheme (v or cpk)
} initpars;

typedef struct {
  FILE       * f;       // open file for kp_readmore()
  const char * fname;   // file name
} readpars;

typedef struct {
  int center;   // 0: nothing; 1: center each molecule upon reading; 2: center wrt center of mass
  int inertia;  // 0: nothing; 1: rotate each molecule upon reading wrt axis of inertia
  int bohr;     // 0: Å        1: Bohr
} geompars;


typedef struct {
  int    closed;        // 1: time to go
  char * com;           // command string for gui:0
  char * on_exit;       // command string to run on exit
  int input;            // 0=no input regime, 1=jump, ...
  char input_text[STRLEN];
  int    modkey;        // whether ctrl or shift are pressed
} uipars;

typedef struct {
  double xy0[2];        // translation vector
  double ac3rmx[9];     // rotational matrix
  double scale;         // zoom
  double r;             // atom size scale factor
  int    num;           // 0: do not show; 1: show numbers;  -1: show atom types
  int    bonds;         // 0: do not show; 1: show bonds;     2: show bond+lengths; -1: never show
} rendpars;

typedef struct {
  double rl;            // bond length scale factor
  double bmax;          // max. bond length to show
} bondpars;

typedef struct {
  double vertices[3*8];           // parameters of cell/shell
  double rot_to_lab_basis[3*3];   // "rotation" matrix for PBC
  double rot_to_cell_basis[3*3];  // "rotation" matrix for PBC
  int    vert;          // 0: nothing;     1: show cell;      2: show shell
} cellpars;

typedef struct {
  int    t;             // counter for mode animation
  int    dir;           // 0: nothing;     1: play forwards; -1: play backwards
  unsigned int dt;      // animation timeout
} animpars;

typedef struct {
  double symtol;        // tolerance for symmetry determination
  int    intcoord[5];   // internal coordinate to show
} analpars;

typedef struct {

  task_t task;          // data type

  int    N;             // number of structures / modes
  int    n;             // current structure / mode

  readpars  read;
  geompars  geom;
  uipars    ui;
  rendpars  rend;
  bondpars  bond;
  cellpars  cell;
  animpars  anim;
  analpars  anal;

} drawpars;

typedef struct {
  initpars  ip;
  drawpars  dp;
} allpars;

