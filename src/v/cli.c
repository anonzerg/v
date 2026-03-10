#include "v.h"
#include "vecn.h"
#include "matrix.h"
#include "vec3.h"

static int sscan_rot(const char * arg, double rot[9]){
  return sscanf(arg, "rot:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", rot,  rot+1, rot+2, rot+3, rot+4, rot+5, rot+6, rot+7, rot+8);
}

static int sscan_cell(const char * arg, double cell[9]){
  int count = sscanf(arg, "cell:b%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", cell,  cell+1, cell+2, cell+3, cell+4, cell+5, cell+6, cell+7, cell+8);
  if(count==9 || count==3){
    vecscal(count, cell, BA);
    return count;
  }
  count = sscanf(arg, "cell:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", cell,  cell+1, cell+2, cell+3, cell+4, cell+5, cell+6, cell+7, cell+8);
  return count;
}

static int sscan_shell(const char * arg, double shell[2]){
  int count = sscanf (arg, "shell:b%lf,%lf", shell, shell+1);
  if(count==2){
    vecscal(count, shell, BA);
    return count;
  }
  count = sscanf (arg, "shell:%lf,%lf", shell, shell+1);
  return count;
}

static void getcell(double cell[9], drawpars * dp, int cell_count){

  double a[3]={};
  double b[3]={};
  double c[3]={};
  if(cell_count==3){
    a[0] = cell[0];
    b[1] = cell[1];
    c[2] = cell[2];
  }
  else if(cell_count==9){
    r3cp(a, cell+0);
    r3cp(b, cell+3);
    r3cp(c, cell+6);
  }

  r3sums3(dp->vertices+ 0, a, -0.5, b, -0.5, c, -0.5);
  r3sums3(dp->vertices+ 3, a, +0.5, b, -0.5, c, -0.5);
  r3sums3(dp->vertices+ 6, a, -0.5, b, +0.5, c, -0.5);
  r3sums3(dp->vertices+ 9, a, -0.5, b, -0.5, c, +0.5);
  r3sums3(dp->vertices+12, a, +0.5, b, +0.5, c, -0.5);
  r3sums3(dp->vertices+15, a, +0.5, b, -0.5, c, +0.5);
  r3sums3(dp->vertices+18, a, -0.5, b, +0.5, c, +0.5);
  r3sums3(dp->vertices+21, a, +0.5, b, +0.5, c, +0.5);

  double rot_to_lab_basis[9] = {a[0], b[0], c[0],
                                a[1], b[1], c[1],
                                a[2], b[2], c[2]};
  veccp(9, dp->rot_to_lab_basis, rot_to_lab_basis);
  mx_id(3, dp->rot_to_cell_basis);
  mx_inv(3, 3, dp->rot_to_cell_basis, rot_to_lab_basis, 1e-15);

  dp->vert = 1;
  return;
}

static void getshell(double shell[2], drawpars * dp){
  dp->vertices[0] = shell[0];
  dp->vertices[1] = shell[1];
  dp->vert = 2;
  return;
}

static int cli_parse_arg(char * arg, drawpars * dp){
  int vib   = -1;
  int bonds = 1;
  int frame = 1;
  double rot  [9]={0};
  double cell [9]={0};
  double shell[2]={0};
  double tf = 0.0;
  double bmax = 0.0;
  char ts[256] = "";

  int a0  = sscanf (arg, "vib:%d", &vib);
  int a1  = sscanf (arg, "dt:%lf", &tf);
  int a2  = sscanf (arg, "symtol:%lf", &(dp->symtol));
  int a3  = sscanf (arg, "bonds:%d", &bonds);
  int a4  = sscanf (arg, "z:%d,%d,%d,%d,%d", dp->z, dp->z+1, dp->z+2, dp->z+3, dp->z+4);
  int a5  = sscanf (arg, "font:%255s", dp->fontname);
  int a6  = sscanf (arg, "gui:%d", &(dp->gui));
  int a7  = sscanf (arg, "bohr:%d", &(dp->bohr));
  int a8  = sscanf (arg, "bmax:%lf", &bmax);
  int a9  = sscanf (arg, "frame:%d", &frame);
  int a10 = sscanf (arg, "center:%d", &(dp->center));
  int a11 = sscanf (arg, "inertia:%d", &(dp->inertia));
  int a12 = sscanf (arg, "com:%255s", dp->com);
  int a13 = sscanf (arg, "exitcom:%255s", dp->on_exit);
  int a14 = sscanf (arg, "colors:%255s", ts);
  int rot_count   = sscan_rot  (arg, rot);
  int cell_count  = sscan_cell (arg, cell);
  int shell_count = sscan_shell(arg, shell);

  int cli = a0||a1||a2||a3||a4||a5||a6||a7||a8||a9||a10||a11||a12||a13||a14 || rot_count||cell_count||shell_count;

  if(vib==0){
    dp->task = AT3COORDS;
  }
  else if(vib==1){
    dp->task = VIBRO;
  }

  if(!bonds){
    dp->b = -1;
  }

  if(tf>0.0){
    dp->dt = ceil(tf*1e6);
  }

  if(bmax>0.0){
    dp->bmax = bmax;
  }

  if(ts[0]){
    const char * const colorscheme_names[] = {[V_COLORS] = "v", [CPK_COLORS] = "cpk"};
    int ncs = sizeof(colorscheme_names)/sizeof(colorscheme_names[0]);
    for(int i=0; i<ncs; i++){
      int l0 = strlen(colorscheme_names[i]);
      int l1 = strlen(ts);
      if((l0==l1) && (!strncmp(ts, colorscheme_names[i], l0))){
        dp->colors = i;
        break;
      }
      if(i==ncs-1){
        PRINT_WARN("unknown colorscheme %s. defaulting to %s\n", ts, colorscheme_names[V_COLORS]);
      }
    }
  }

  if(rot_count==9){
    veccp(9, dp->ac3rmx, rot); // we don't check if the matrix is unitary
  }
  if(cell_count==3 || cell_count==9){
    getcell(cell, dp, cell_count);
  }
  if(shell_count==2){
    getshell(shell, dp);
  }
  dp->n = frame-1;

  if(!cli){
    dp->input_files[dp->input_files_n++] = arg;
  }
  return cli;
}

static drawpars dp_init(void){
  drawpars dp = {};
  dp.task = UNKNOWN;
  dp.gui  = 1;
  dp.input = 0;
  memset(dp.input_text, 0, STRLEN);
  dp.dt   = DEFAULT_TIMEOUT;
  memset(dp.fontname, 0, STRLEN);
  dp.n   = 0;
  dp.fbw = 0;
  dp.num = 0;
  dp.t   = 0;
  dp.rl  = 1.0;
  dp.r   = 1.0;
  dp.xy0[0] = dp.xy0[1] = 0.0;
  mx_id(3, dp.ac3rmx);
  // from command-line
  dp.inertia = 0;
  dp.center = 1;
  dp.b = 1;
  dp.bmax = 0.0;
  dp.symtol = DEFAULT_SYMTOL;
  dp.vert = -1;
  dp.z[0] = dp.z[1] = dp.z[2] = dp.z[3] = dp.z[4] = 0;
  vecset(3*8, dp.vertices, 0.0);
  memset(dp.com, 0, STRLEN);
  memset(dp.on_exit, 0, STRLEN);
  dp.input_files_n = 0;
  dp.input_files = NULL;
  dp.colors = V_COLORS;
  // from data read
  dp.scale = 1.0;
  dp.N = 0;
  dp.f = NULL;
  dp.fname = NULL;
  dp.bohr = 0;
  // runtime
  dp.closed = 0;
  return dp;
}

drawpars cli_parse(int argc, char ** argv){
  drawpars dp = dp_init();
  dp.input_files = malloc(argc*sizeof(char*));
  for(int i=1; i<argc; i++){
    cli_parse_arg(argv[i], &dp);
  }

  if(!dp.gui){
    if(dp.task == VIBRO){
      PRINT_WARN("normal modes are not supported in the headless regime\n");
    }
    dp.task = AT3COORDS;
  }

  return dp;
}
