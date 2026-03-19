#include "v.h"
#include "vecn.h"
#include "matrix.h"

#define EPS 1e-15

static int lazysscanf(char * const s, const char * const template, char ** ret){
  // if the string `s` begins with the template,
  // assign *ret to the part of the string after the template,
  // otherwise do nothing
  int n = strlen(template);
  int r = !strncmp(template, s, n);
  if(r){
    if(strlen(s)==n){
      PRINT_WARN("option %s needs an argument\n", template)
    }
    else{
      *ret = s+n;
    }
  }
  return r;
}

static int sscan_rot(const char * arg, double ac3rmx[9]){
  double rot[9];
  int count = sscanf(arg, "rot:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", rot, rot+1, rot+2, rot+3, rot+4, rot+5, rot+6, rot+7, rot+8);
  if(count <= 0){
    return 0;
  }
  else if(count == 9){
    veccp(9, ac3rmx, rot);  // we don't check if it is unitary
  }
  else{
    PRINT_WARN("option `rot:` takes exactly 9 comma-separated arguments\n")
  }
  return 1;
}

static int sscan_cell(const char * arg, geompars * geom){
  double cell[9];
  int count = sscanf(arg, "cell:b%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", cell, cell+1, cell+2, cell+3, cell+4, cell+5, cell+6, cell+7, cell+8);
  if(count > 0){
    if(count==9 || count==3 || count==1){
      vecscal(count, cell, BA);
    }
    else{
      PRINT_WARN("option `cell:b` takes exactly 1, 3 or 9 comma-separated arguments\n")
      return 1;
    }
  }
  else{
    count = sscanf(arg, "cell:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", cell, cell+1, cell+2, cell+3, cell+4, cell+5, cell+6, cell+7, cell+8);
    if(count <= 0){
      return 0;
    }
    else if(count!=3 && count!=9 && count!=1){
      PRINT_WARN("option `cell:` takes exactly 1, 3, or 9 comma-separated arguments\n")
      return 1;
    }
  }

  double s=0.0;
  for(int i=0; i<count; i++){
    s += fabs(cell[i]);
  }
  if(s<EPS){
    geom->boundary = CELL_DISABLED;
    return 1;
  }

  if(count==1){
    geom->cell[0] = cell[0];
    geom->cell[4] = cell[0];
    geom->cell[8] = cell[0];
  }
  else if(count==3){
    geom->cell[0] = cell[0];
    geom->cell[4] = cell[1];
    geom->cell[8] = cell[2];
  }
  else{
    veccp(9, geom->cell, cell);
  }
  geom->boundary = CELL;
  return 1;
}

static int sscan_shell(const char * arg, geompars * geom){
  double shell[2];
  int count = sscanf(arg, "shell:b%lf,%lf", shell, shell+1);
  if(count > 0){
    if(count==2)
      vecscal(count, shell, BA);
  }
  else{
    count = sscanf(arg, "shell:%lf,%lf", shell, shell+1);
  }
  if(count <= 0)
    return 0;
  if(count == 2){
    geom->shell[0] = shell[0];
    geom->shell[1] = shell[1];
    geom->boundary = SHELL;
  }
  return 1;
}

static int cli_parse_arg(char * arg, allpars * ap){
  drawpars * dp = &ap->dp;
  initpars * ip = &ap->ip;
  int vib = -1, bonds = -2, frame = 0;
  double tf = 0, bmax = 0;
  char * ts = NULL;

  int cli =
       sscanf (arg, "vib:%d",     &vib)
    || sscanf (arg, "bonds:%d",   &bonds)
    || sscanf (arg, "dt:%lf",     &tf)
    || sscanf (arg, "bmax:%lf",   &bmax)
    || sscanf (arg, "frame:%d",   &frame)
    || sscanf (arg, "symtol:%lf", &dp->anal.symtol)
    || sscanf (arg, "gui:%d",     &ip->gui)
    || sscanf (arg, "bohr:%d",    &dp->geom.bohr)
    || sscanf (arg, "center:%d",  &dp->geom.center)
    || sscanf (arg, "inertia:%d", &dp->geom.inertia)
    || sscanf (arg, "z:%d,%d,%d,%d,%d", dp->anal.intcoord, dp->anal.intcoord+1, dp->anal.intcoord+2, dp->anal.intcoord+3, dp->anal.intcoord+4)
    || lazysscanf(arg, "font:",    &ip->fontname)
    || lazysscanf(arg, "com:",     &dp->ui.com)
    || lazysscanf(arg, "exitcom:", &dp->ui.on_exit)
    || lazysscanf(arg, "colors:",  &ts)
    || sscan_rot  (arg, dp->rend.ac3rmx)
    || sscan_cell (arg, &dp->geom)
    || sscan_shell(arg, &dp->geom)
    ;

  if(vib==0){
    dp->task = AT3COORDS;
  }
  else if(vib==1){
    dp->task = VIBRO;
  }

  if(bonds==0){
    dp->rend.bonds = -1;
  }

  if(tf>0.0){
    dp->anim.dt = ceil(tf*S_TO_MS);
  }

  if(bmax>0.0){
    dp->bond.bmax = bmax;
  }

  if(frame){
    dp->n = frame-1;
  }

  if(ts){
    const char * const colorscheme_names[] = {[V_COLORS] = "v", [CPK_COLORS] = "cpk"};
    int ncs = sizeof(colorscheme_names)/sizeof(colorscheme_names[0]);
    for(int i=0; i<ncs; i++){
      int l0 = strlen(colorscheme_names[i]);
      int l1 = strlen(ts);
      if((l0==l1) && (!strncmp(ts, colorscheme_names[i], l0))){
        ip->colors = i;
        break;
      }
      if(i==ncs-1){
        PRINT_WARN("unknown colorscheme %s. defaulting to %s\n", ts, colorscheme_names[V_COLORS]);
      }
    }
  }

  if(!cli){
    ip->input_files[ip->input_files_n++] = arg;
  }
  return cli;
}

static allpars allpars_init(void){
  allpars ap = {};  // everything not set below is 0 / 0.0 / NULL / '\0'

  ap.ip.gui    = 1;
  ap.ip.colors = V_COLORS;

  ap.dp.task        = UNKNOWN;
  ap.dp.anim.dt     = DEFAULT_TIMEOUT;
  ap.dp.anal.symtol = DEFAULT_SYMTOL;
  ap.dp.bond.rl     = 1.0;
  ap.dp.geom.center = 1;

  ap.dp.rend.r     = 1.0;
  ap.dp.rend.scale = 1.0;
  ap.dp.rend.bonds = 1;
  mx_id(3, ap.dp.rend.ac3rmx);

  return ap;
}

allpars cli_parse(int argc, char ** argv){
  allpars ap = allpars_init();
  ap.ip.input_files = malloc(argc*sizeof(char*));
  for(int i=1; i<argc; i++){
    cli_parse_arg(argv[i], &ap);
  }

  if(!ap.ip.gui){
    if(ap.dp.task == VIBRO){
      PRINT_WARN("normal modes are not supported in the headless regime\n");
    }
    ap.dp.task = AT3COORDS;
  }
  return ap;
}
