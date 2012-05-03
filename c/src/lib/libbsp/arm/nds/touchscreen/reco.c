#include <nds.h>
#include "reco.h"

PA_StylusPosition PA_StylusPos[20000];

PA_RecoValues PA_Reco;

char PA_RecoShape[16];

u8 PA_UseGraffiti = true;

u8 PA_CustomReco = 0; // number of custom shapes
PA_FormType PA_CustomShape[200];

PA_RecoInfos PA_RecoInfo;

PA_FormType PA_Graffiti[PA_RECOTESTS] = {
  {' ', "AAAAAAAAAAAAAAA"},
  {'a', "FGGGGGFB;:;;;;;"},
  {'b', "JJJJJIGC>:8=<62"},
  {'c', "01234689;<>?@BC"},
  {'d', "=995KJIFB?=;853"},
  {'e', "1236;>@136;=?@A"},
  {'f', "121111399998779"},
  {'g', "235689;=@CFIJEA"},
  {'h', "8999999;HFEA><:"},
  {'i', "999999999999999"},
  {'j', "999999998653100"},
  {'k', "654320NIDCA?==="},
  {'l', "999999988<AA@@@"},
  {'m', "HGGEA<;EFDA=;99"},
  {'n', "HHHIE<;;;<GIHHH"},
  {'o', "4689;<?BDFHKMO0"},
  {'p', "<KJIIIHGEB>;853"},
  {'q', "4679<?CDFHKMJBA"},
  {'r', "999IIIIGC?:53<="},
  {'s', "0235:>?>>;73100"},
  {'t', "AAAA@@@;8999999"},
  {'u', "899:;<>ACEGHHIH"},
  {'v', ";;:::::?FGGHHGF"},
  {'w', "::;=BA<<@CDEFGH"},
  {'x', ";;;;;;;;;;;;;;;"},
  {'y', ">;=AFF899974OIF"},
  {'z', ">A@@>843348?@AA"},
  {'0', "35789;=AEGHIJKM"},
  {'1', "CDEFFE988889999"},
  {'2', "GDB@>:6546>AAAA"},
  {'3', "CA>833A@=953210"},
  {'4', "55558>@A@>97778"},
  {'5', "00037:@@=;7310O"},
  {'6', "346779;>@CFKN02"},
  {'7', "AAAAAA@>9655556"},
  {'8', "37;=;71MIECCGM1"},
  {'9', "8<@CGL28<<;6311"},
  {PA_BACKSPACE, "000000000000000"},
  {PA_ENTER, "555555555555555"},
  //	{'?', "FCA@><8655799::"},
  {'.', "LLLLLLLLLLLLLLL"}
};



void PA_AddStylusPos(u8 x, u8 y){
  if (!((x == PA_StylusPos[PA_Reco.nvalues-1].x) && (y == PA_StylusPos[PA_Reco.nvalues-1].y))){
    PA_StylusPos[PA_Reco.nvalues].x = x;
    PA_StylusPos[PA_Reco.nvalues].y = y;
    PA_Reco.nvalues++;
  }
}


void PA_StylusLine(u8 x1, u8 y1, u8 x2, u8 y2){
  int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs = dx;
  sdx = 1;
  if (dx < 0) {
    dxabs = -dx;
    sdx = -1;
  }
  dyabs = dy;
  sdy = 1;
  if (dy < 0) {
    dyabs = -dy;
    sdy = -1;
  }

  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;

  PA_AddStylusPos(px, py);


  if (dxabs>=dyabs) {
    for(i=0;i<dxabs;i++)  {
      y+=dyabs;
      if (y>=dxabs)  {
        y-=dxabs;
        py+=sdy;
      }
      px+=sdx;
      PA_AddStylusPos(px, py);
    }
  }
  else {
    for(i=0;i<dyabs;i++) {
      x+=dxabs;
      if (x>=dyabs)  {
        x-=dyabs;
        px+=sdx;
      }
      py+=sdy;
      PA_AddStylusPos(px, py);
    }
  }
}


#define PA_ShapeAddPoint(i, value){					\
    points[i] = PA_StylusPos[value];					\
    if (points[i].x < PA_RecoInfo.minX) PA_RecoInfo.minX = points[i].x;	\
    else if (points[i].x > PA_RecoInfo.maxX) PA_RecoInfo.maxX = points[i].x; \
    if (points[i].y < PA_RecoInfo.minY) PA_RecoInfo.minY = points[i].y;	\
    else if (points[i].y > PA_RecoInfo.maxY) PA_RecoInfo.maxY = points[i].y;	}




char PA_AnalyzeShape(void){
  s32 i;
  //  for (i = 0; i < 32; i++) PA_OutputSimpleText(1, 0, i, "             ");

  PA_StylusPosition points[17];
  for (i = 0; i < 16; i++){
    PA_ShapeAddPoint(i, (PA_Reco.nvalues*i)>>4)
      }
  PA_ShapeAddPoint(16, PA_Reco.nvalues-1)
    //points[16] = PA_StylusPos[PA_Reco.nvalues-1];
    PA_RecoInfo.endX = PA_StylusPos[PA_Reco.nvalues-1].x; // last values
  PA_RecoInfo.endY = PA_StylusPos[PA_Reco.nvalues-1].y; // last values
  PA_RecoInfo.Length = PA_Reco.nvalues; // Total length
  PA_RecoInfo.Angle = PA_GetAngle(points[0].x, points[0].y, points[16].x, points[16].y);

  //Better values
  if (PA_RecoInfo.minX > 1) PA_RecoInfo.minX-=2;
  if (PA_RecoInfo.maxX <254) PA_RecoInfo.maxX+=2;
  if (PA_RecoInfo.minY > 1) PA_RecoInfo.minY-=2;
  if (PA_RecoInfo.maxY <190) PA_RecoInfo.maxY+=2;

  u16 angles[15];
  for (i = 0; i < 15; i++) angles[i] = PA_GetAngle(points[i+2].x, points[i+2].y, points[i].x, points[i].y);

  for (i = 0; i < 15; i++) PA_RecoShape[i] = '0' + (((angles[i]+16)&511)>>4);
  PA_RecoShape[15] = 0;
  //  printk("%s\n", PA_RecoShape);

  u8 letter = 0; // 0 par défaut
  s32 diff = 65000; // Diff max par défaut

  u8 j;

  if(PA_UseGraffiti){
    for (j = 0; j < PA_RECOTESTS; j++){
      s32 tempvalue = 0;
      s32 tempdiff = 0;
      for (i = 0; i < 15; i++) {
	tempvalue = (PA_RecoShape[i]-PA_Graffiti[j].code[i])&31;
	tempvalue -= (tempvalue >> 4)<<5; // normalise
	if (tempvalue < 0) tempvalue = -tempvalue;
	tempdiff += tempvalue; // value
      }

      if (tempdiff < diff){ // Nouvelle lettre !
	diff = tempdiff;
	letter = PA_Graffiti[j].letter;
      }
    }
  }
  if (PA_CustomReco > 0){
    for (j = 0; j < PA_CustomReco; j++){
      s32 tempvalue = 0;
      s32 tempdiff = 0;
      for (i = 0; i < 15; i++) {
	tempvalue = (PA_RecoShape[i]-PA_CustomShape[j].code[i])&31;
	tempvalue -= (tempvalue >> 4)<<5; // normalise
	if (tempvalue < 0) tempvalue = -tempvalue;
	tempdiff += tempvalue; // value
      }

      if (tempdiff < diff){ // Nouvelle lettre !
	diff = tempdiff;
	letter = PA_CustomShape[j].letter;
      }
    }

  }
  PA_RecoInfo.Difference = diff; // Difference to perfect shape
  PA_RecoInfo.Shape = letter;

  return letter;
}

int old_down = 0;

char PA_CheckLetter(int down, int x, int y){
  if(!old_down && down){
    PA_Reco.nvalues = 0; // Start over again
    PA_Reco.oldn = 0;
    PA_Reco.veryold = 0;

    PA_RecoInfo.startX = PA_StylusPos[PA_Reco.nvalues].x = x; // start values
    PA_RecoInfo.startY = PA_StylusPos[PA_Reco.nvalues].y = y;
    PA_RecoInfo.minX = PA_RecoInfo.maxX = PA_RecoInfo.startX;
    PA_RecoInfo.minY = PA_RecoInfo.maxY = PA_RecoInfo.startY;
    PA_Reco.nvalues++;
  }
  else if(old_down && down) {
    PA_StylusLine(PA_StylusPos[PA_Reco.nvalues-1].x, PA_StylusPos[PA_Reco.nvalues-1].y, x, y);

  }


  if(old_down && !down){ // Start analyzing...
    PA_Reco.nvalues = PA_Reco.veryold;
    old_down = down;
    return PA_AnalyzeShape();
  }
  PA_Reco.veryold = PA_Reco.oldn;
  PA_Reco.oldn = PA_Reco.nvalues;
  old_down = down;
  return 0;
}


