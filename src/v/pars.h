typedef enum {
  UNKNOWN,
  AT3COORDS,
  VIBRO,
} task_t;

typedef enum {
  V_COLORS,
  CPK_COLORS,
} colorscheme_t;

typedef enum {
  NO_BOUNDARY,
  CELL,
  SHELL,
  CELL_DISABLED,
} boundary_t;

typedef enum {
  NO_INPUT,
  INPUT_JUMP,
} input_t;

typedef enum {
  NO_ATOM_NUMBERS,
  SHOW_NUMBERS,
  SHOW_TYPES,
} render_atom_numbers_t;

typedef enum {
  DISABLE_BONDS=-1,
  NO_BONDS=0,
  SHOW_BONDS,
  SHOW_LENGTHS,
} render_bonds_t;

typedef enum {
  NO_CENTER,
  CENTER_GEOM,
  CENTER_MASS,
} center_t;

typedef enum {
  MUST_CLEANUP=-1,
  CONTINUE_WORKING=0,
  READY_TO_EXIT,
} exit_t;

typedef struct {
  colorscheme_t  colors;   // colorscheme (v or cpk)
  int     input_files_n;   // number of input files
  char       * fontname;   // font
  char   ** input_files;   // input files
  char     * on_startup;   // command string to run on startup
} initpars;

typedef struct {
  FILE       * f;       // open file for kp_readmore()
  const char * fname;   // file name
} readpars;

typedef struct {
  center_t center; // whether / how to center each molecule upon reading
  int inertia;     // whether to rotate each molecule upon reading wrt axis of inertia
  int bohr;        // whether to use Bohr instead of Å
  boundary_t boundary;
  double cell[9];
  double shell[2];
} geompars;

typedef struct {
  char    input_text[STRLEN];
  char  * com;           // command string for gui:0
  char  * on_exit;       // command string to run on exit
  exit_t  closed;        // closing state
  int     gui;           // if gui is enabled
  input_t input;         // input regime
  int     modkey;        // whether ctrl or shift are pressed
} uipars;

typedef struct {
  double xy0[2];        // translation vector
  double ac3rmx[9];     // rotational matrix
  double scale;         // zoom
  double r;             // atom size scale factor
  render_atom_numbers_t num;
  render_bonds_t bonds;
} rendpars;

typedef struct {
  double rl;            // bond length scale factor
  double bmax;          // max. bond length to show
} bondpars;

typedef struct {
  double vertices[3*8];           // parameters of cell/shell
  double rot_to_lab_basis[3*3];   // "rotation" matrix for PBC
  double rot_to_cell_basis[3*3];  // "rotation" matrix for PBC
  boundary_t boundary;
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
  animpars  anim;
  analpars  anal;

} drawpars;

typedef struct {
  initpars  ip;
  drawpars  dp;
} allpars;

