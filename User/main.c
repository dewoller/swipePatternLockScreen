/*
 *
 */


#include "defines.h"
#include "tm_stm32f4_usb_vcp.h"
#include "tm_stm32f4_disco.h"
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "tm_stm32f4_ili9341_ltdc.h"
#include "tm_stm32f4_fonts.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_stmpe811.h"
#include <stdio.h>
#include <math.h>
#include "main.h"


int main(void ) {
    TM_STMPE811_TouchData touchData;
    int nsegment = 0;

    char str[300];
    point_t lastPoint;
    point_t thisPoint;
    int nUpEvents = 0;
    point_t segments[ MAXSEGMENT ];
    button_t buttons[ NBUTTON ];

    SystemInit();
    TM_ILI9341_Init();
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Portrait_2);
    TM_STMPE811_Init();
    TM_USB_VCP_Init();	

    touchData.orientation = TM_STMPE811_Orientation_Portrait_2;
    displayInitialScreen(buttons);
    lastPoint.x=0;
    lastPoint.y=0;


    while (1) {
        if (TM_STMPE811_ReadTouch(&touchData) == TM_STMPE811_State_Pressed) {
            nUpEvents=0;
            if (DEBUG) {sprintf(str, "Pressed: point %d %d %d\n\r", touchData.x, touchData.y, nsegment); sendStringViaUSB(str);}
            thisPoint.x = touchData.x;
            thisPoint.y = touchData.y;
            if (DEBUG) {sprintf(str, "distance %f\n\r", dist2(thisPoint, lastPoint)); sendStringViaUSB(str);}
            if (
                    (nsegment < MAXSEGMENT) && 
                    ((nsegment == 0) || 
                     (dist2(thisPoint, lastPoint) > (MINDISTANCE * MINDISTANCE)))
               ) {
                segments[nsegment++] = thisPoint;
                lastPoint = thisPoint;
                findHits(segments, nsegment, buttons);
                if (DEBUG) {sprintf(str, "added segment: point %d %d %d\n\r", touchData.x, touchData.y, nsegment); sendStringViaUSB(str);}
            }
        } else { //event==up
           if(nUpEvents++ > MINUPEVENTS) { 
               if (DEBUG) {sprintf(str, "no touch data: \n\r" ); sendStringViaUSB(str);}
                nUpEvents=0;

            if (nsegment > 0) {  // we have actually received some data
                //send(buttons);
                if (DEBUG) {sprintf(str, "up: point %d %d %d\n\r", touchData.x, touchData.y, nsegment); sendStringViaUSB(str);}
                sendHits( buttons );
                lastPoint.x=0;
                lastPoint.y=0;
                nsegment=0;
                displayInitialScreen(buttons);
            }
           }
        }
    }
} 		

int compareButtonHit(const void *a,const void *b) {
    button_t *x = (button_t *) a;
    button_t *y = (button_t *) b;
    //sprintf(str, "compare: point %d %c  %d %c \n\r", x->hit, x->ch, y->hit, y->ch); sendStringViaUSB(str);
    //sprintf(str, "compare: point %d %d \n\r", x->hit, y->hit); sendStringViaUSB(str);
    
    return x->hit - y->hit;
}

void sendHits(button_t *buttons ) {
    char rv[ NBUTTON +3 ];
    int pos=0;
    int i;
    //sprintf(str, "prebuttonhitSort"); sendStringViaUSB(str);
    qsort (buttons, NBUTTON, sizeof(struct button_t), compareButtonHit);
    //sprintf(str, "postbuttonhitSort"); sendStringViaUSB(str);
    for (i = 0; i<NBUTTON; i++ ) {
        if (buttons[ i ].hit > 0) {
            rv[pos++] = buttons[ i ].ch;
        }
    }
    rv[pos++] = '\n';
    rv[pos++] = '\0';
    sendStringViaUSB( rv );
}


void displayInitialScreen(button_t *buttons) {
    int i;
    clearButtons( buttons);
    TM_ILI9341_Fill(BACKGROUNDCOLOR);
    for( i = 0; i < NBUTTON; i++ ) {
      	TM_ILI9341_Putc(buttons[i].center.x-5, buttons[i].center.y-9, buttons[i].ch, &TM_Font_11x18, BUTTONCOLOR, BACKGROUNDCOLOR);
        //TM_ILI9341_DrawFilledCircle(buttons[i].center.x, buttons[i].center.y, BUTTONRADIUS1, color);
        TM_ILI9341_DrawCircle(buttons[i].center.x, buttons[i].center.y, BUTTONRADIUS2, BUTTONCOLOR);
//        TM_ILI9341_DrawCircle(buttons[i].center.x, buttons[i].center.y, BUTTONRADIUS1, color);
//        TM_ILI9341_DrawCircle(buttons[i].center.x, buttons[i].center.y, BUTTONRADIUS2, color);
    }
}

void clearButtons( button_t *buttons) {
    int i,j,pos;
	    for (i = 0; i < NROW; i++) { 
        for (j = 0; j < NCOL; j++) { 
            pos = (int) ((i * NCOL ) + j);
            buttons[ pos ].center.x = (int) ((j * COLSPACING) + (COLSPACING / 2) + (MARGIN * j));
            buttons[ pos ].center.y = (int) ((i * ROWSPACING) + (ROWSPACING / 2) + (MARGIN * i));
            if (pos>=9) {
                buttons[pos].ch = (char) (pos + (int) 'X' - 9);
            } else {
                buttons[pos].ch = (char) (pos + (int) '1');
            }
            buttons[ pos ].hit = 0;
        }
    }
}

void findHits(point_t *segments, int nsegment, button_t *buttons) {
    int nhit = 0;
    int j;
    char str[300];
    if (nsegment < 2) { return; }
    if (DEBUG) {sprintf(str, "drawing line %d %d %d %d\n\r",
            segments[nsegment-2 ].x, 
            segments[nsegment-2 ].y,  
            segments[nsegment-1].x, 
            segments[nsegment-1].y 
            );sendStringViaUSB(str);}

    TM_ILI9341_DrawRectangle(
            segments[nsegment-2 ].x, 
            segments[nsegment-2 ].y, 
            segments[nsegment-1].x, 
            segments[nsegment-1].y, 
            LINECOLOR);
    for (j = 0; j < NBUTTON; j++ ) {
        if (buttons[j].hit > nhit ) {
            nhit = buttons[j].hit ;
        }
    }

    for (j = 0; j < NBUTTON; j++ ) {
        if (buttons[j].hit ==0 ) {
            if (distToSegmentSquared(segments[nsegment-2], 
                        segments[nsegment-1 ], 
                        buttons[ j ].center) <= (BUTTONRADIUS2 * BUTTONRADIUS2)) { 
                buttons[j].hit = ++nhit;
                if (DEBUG) {sprintf(str, "NEW hits %d\n\r", nhit);sendStringViaUSB(str);}
                TM_ILI9341_DrawFilledCircle(buttons[j].center.x, 
                        buttons[j].center.y, 
                        BUTTONRADIUS2,     
                        HITBUTTONCOLOR);
            }
        }
    }  
}

double sqr(double x) { return x * x; }
double dist2(point_t v, point_t w) { return sqr(v.x - w.x) + sqr(v.y - w.y); }
double distToSegmentSquared(point_t v, point_t w, point_t p) {
  point_t p1;
  double t;
  double l2 = dist2(v, w);
  if (l2 == 0) return dist2(p, v);
  t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;
  if (t < 0) return dist2(p, v);
  if (t > 1) return dist2(p, w);
  p1.x= v.x + t * (w.x - v.x);
  p1.y = v.y + t * (w.y - v.y);
  return dist2(p, p1);  
                    
}

void  sendStringViaUSB( 	char *str ) {
	
    int i;
    int string_len;

    string_len = strlen(str);

    for(i = 0; i < string_len; i++) {
					TM_USB_VCP_Putc(str[i]);
    }

}	
