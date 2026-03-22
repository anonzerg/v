#!/usr/bin/env bash

# my i3 layout:
# alt-h
# xterm &
# alt-v

FONT='-*-terminus-medium-r-normal--24-*-*-*-*-*-*-*'

function repeat(){
  for i in $(seq ${2}); do echo -n ${1}; done
}

ZOOM=$(repeat + 6)

./v mol/C3H6~mCPBA_01x11.qm.out \
  font:${FONT} \
  startcom:${ZOOM}fq
magick -loop 0 mol/C3H6~mCPBA_01x11.qm.out_01_??.xpm fig/C3H6~mCPBA_01x11.qm.out_01.gif
rm mol/C3H6~mCPBA_01x11.qm.out_01_??.xpm

./v mol/C3H6~mCPBA_01x11.qm.out \
  vib:0 z:1,23,24,0,0 frame:3 \
  rot:0.715399,-0.104528,0.690853,0.075191,0.994522,0.072612,-0.694658,0.000000,0.719340 \
  startcom:${ZOOM}ln2mq
magick mol/C3H6~mCPBA_01x11.qm.out_03.xpm fig/C3H6~mCPBA_01x11.qm.out_03.png
rm mol/C3H6~mCPBA_01x11.qm.out_03.xpm

./v mol/S8.qm.out \
  z:1,1,2,0,0 \
  frame:-1 \
  rot:0.426254,0.903023,-0.053442,-0.231825,0.166151,0.958463,0.874394,-0.396160,0.280166 \
  font:${FONT} \
  startcom:${ZOOM}lnmq
magick mol/S8.qm.out_69.xpm fig/S8.qm.out_69.png
rm mol/S8.qm.out_69.xpm

./v mol/C10H16.qm.out \
  frame:-1 \
  rot:-0.224637,0.772861,0.593486,0.311208,0.634056,-0.707900,-0.923411,0.025677,-0.382953 \
  startcom:$(repeat a 6)fq
for i in mol/C10H16.qm.out_72_??.xpm; do
    magick $i -crop 666x+0+0 +repage ${i/.xpm/.gif}
done
magick mol/C10H16.qm.out_72_??.gif -loop 0 fig/C10H16.qm.out_72.gif
rm mol/C10H16.qm.out_72_??.{gif,xpm}


./v mol/1372_D02.340_1.out \
  bonds:0 \
  cell:b10.7 \
  frame:1024 \
  rot:-0.657455,0.753214,0.020521,-0.224092,-0.221460,0.949072,0.719399,0.619373,0.314390 \
  startcom:$(repeat dm 39)q
magick mol/1372_D02.340_1.out_1024.*.xpm -loop 0 fig/1372_D02.340_1.out_1024.gif
rm mol/1372_D02.340_1.out_1024.*.xpm

./v mol/mol0001.xyz mol/mol0002.xyz \
  symtol:1e-2 \
  frame:3\
  rot:0.914278,0.211291,-0.345618,0.166952,0.580820,0.796728,0.369083,-0.786133,0.495755 \
  font:${FONT} \
  startcom:++44t.mq
magick mol/mol0002.xyz_3.xpm fig/mol0002.xyz_3.png
rm mol/mol0002.xyz_3.xpm


./v mol/MOL_3525.xyz \
  cell:8.929542,0.0,0.0,4.197206,8.892922,0.0,0.480945,2.324788,10.016044 \
  font:${FONT} \
  startcom:${ZOOM}$(repeat am 45)q
magick mol/MOL_3525.xyz_1.*.xpm -loop 0 fig/MOL_3525.xyz_1.gif
rm mol/MOL_3525.xyz_1.*.xpm

./v mol/periodic.in \
  bonds:0 \
  colors:v \
  startcom:${ZOOM}$(repeat d 10)sstmq
mv mol/periodic.in_{1,0}.xpm
./v mol/periodic.in \
  bonds:0 \
  colors:cpk \
  startcom:${ZOOM}$(repeat d 10)sstmq
magick -delay 150 -loop 0 mol/periodic.in_{0,1}.xpm fig/periodic.gif
rm mol/periodic.in_{0,1}.xpm
