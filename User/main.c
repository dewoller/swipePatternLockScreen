/*
 *
 */


#include "defines.h"
#include "tm_stm32f4_usb_vcp.h"
#include "tm_stm32f4_disco.h"
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "tm_stm32f4_ili9341.h"
#include "tm_stm32f4_fonts.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_stmpe811.h"
#include <stdio.h>
#include <math.h>
#include "main.h"


int main(void ) {
    TM_STMPE811_TouchData touchData;
    int i;
    int j;
    int nsegment = 0;

    char str[300];
    point_t lastPoint;
    point_t thisPoint;
    int nUpEvents = 0;
    point_t segments[ MAXSEG ];
    button_t buttons[ NBUTTON ];

    SystemInit();
    TM_ILI9341_Init();
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Portrait_2);
    TM_STMPE811_Init();
    TM_USB_VCP_Init();	

    touchData.orientation = TM_STMPE811_Orientation_Portrait_2;
    for (i = 0; i < NROW; i++) { 
        for (j = 0; j < NCOL; j++) { 
            buttons[ (i * NCOL ) + j ].center.x = (j * COLSPACING) + (COLSPACING / 2) + (MARGIN * j);
            buttons[ (i * NCOL ) + j ].center.y = (i * ROWSPACING) + (ROWSPACING / 2) + (MARGIN * i);
            //sprintf(str, "buttons %d %d %d %d %d\n\r", i,j,(i * NCOL ) + j,buttons[ (i * NCOL ) + j ].center.x,buttons[ (i * NCOL ) + j ].center.y) ; sendStringViaUSB(str);
        }
    }
    displayInitialScreen(buttons);
    lastPoint.x=0;
    lastPoint.y=0;


    while (1) {
        if (TM_STMPE811_ReadTouch(&touchData) == TM_STMPE811_State_Pressed) {
            nUpEvents=0;
            sprintf(str, "Pressed: point %d %d %d\n\r", touchData.x, touchData.y, nsegment); sendStringViaUSB(str);
            thisPoint.x = touchData.x;
            thisPoint.y = touchData.y;
            sprintf(str, "distance %f\n\r", dist2(thisPoint, lastPoint)); sendStringViaUSB(str);
            if ((nsegment == 0) || (dist2(thisPoint, lastPoint) > (MINDISTANCE * MINDISTANCE))) {
                segments[nsegment++] = thisPoint;
                lastPoint = thisPoint;
                findHits(segments, nsegment, buttons);
                sprintf(str, "added segment: point %d %d %d\n\r", touchData.x, touchData.y, nsegment); sendStringViaUSB(str);
            }
        } else { //event==up
sprintf(str, "no touch data: \n\r" ); sendStringViaUSB(str);

            if ((nUpEvents++ > MINUPEVENTS ) && (nsegment > 0)) {
                nUpEvents=0;
                //send(buttons);
                sprintf(str, "up: point %d %d %d\n\r", touchData.x, touchData.y, nsegment); sendStringViaUSB(str);
                lastPoint.x=0;
                lastPoint.y=0;
                nsegment=0;
                displayInitialScreen(buttons);
            }
        }
    }
} 		



void displayInitialScreen(button_t *buttons) {
    int i;
    char str[3];
    clearButtons( buttons);
    TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
    for( i = 0; i < NBUTTON; i++ ) {
        sprintf(str, "%d",i+1);
      	TM_ILI9341_Puts(buttons[i].center.x-5, buttons[i].center.y-9, str, &TM_Font_11x18, BUTTONCOLOR, ILI9341_COLOR_WHITE);
        //TM_ILI9341_DrawFilledCircle(buttons[i].center.x, buttons[i].center.y, BUTTONRADIUS1, color);
        TM_ILI9341_DrawCircle(buttons[i].center.x, buttons[i].center.y, BUTTONRADIUS2, BUTTONCOLOR);
    }
}

void clearButtons( button_t *buttons) {
	int j;
    for (j = 0; j < NBUTTON; j++ ) {
        buttons[ j ].hit = 0;
    }
}

void findHits(point_t *segments, int nsegment, button_t *buttons) {
    int nhit = 0;
    int j;
    char str[300];
    if (nsegment < 2) { return; }
    sprintf(str, "drawing line %d %d %d %d\n\r",
            segments[nsegment-2 ].x, 
            segments[nsegment-2 ].y,  
            segments[nsegment-1].x, 
            segments[nsegment-1].y 
            );sendStringViaUSB(str);

    TM_ILI9341_DrawLine(
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
                sprintf(str, "NEW hits %d\n\r", nhit);sendStringViaUSB(str);
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
