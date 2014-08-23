#ifndef TM_ILI9341_BUTTON_H
#define TM_ILI9341_BUTTON_H

#include <stdint.h>

#define MAXSEGMENT  300
#define MARGIN (5)

#define NCOL  3
#define WIDTH  240 
#define COLSPACING ((WIDTH-MARGIN*2)/NCOL)

#define NROW  4
#define HEIGHT  320
#define ROWSPACING ((HEIGHT-MARGIN*2)/NROW)

#define BUTTONRADIUS1  3
#define BUTTONRADIUS2  30
#define NBUTTON  (NROW*NCOL)
#define MINDISTANCE  (10)
#define MINUPEVENTS  50
#define BACKGROUNDCOLOR  ILI9341_COLOR_BLACK
#define HITBUTTONCOLOR  ILI9341_COLOR_BLUE
#define BUTTONCOLOR ILI9341_COLOR_WHITE
#define LINECOLOR ILI9341_COLOR_WHITE 
#define DEBUG 0

typedef struct point_t {
    int16_t x;
    int16_t y;
} point_t ;

typedef enum { false, true } bool;


typedef struct button_t{
    point_t center ;
    char ch;
    int hit;  // the order it was hit in.  0 if not yet hit;
}  button_t; 

extern void displayInitialScreen(button_t *buttons);

extern void clearButtons( button_t *buttons);

extern void findHits(point_t *segments, int nsegment, button_t *buttons);
int compareButtonHit(const void *a,const void *b);
void sendHits(button_t *buttons );

extern double sqr(double x);
extern double dist2(point_t v, point_t w);
extern double distToSegmentSquared(point_t v, point_t w, point_t p);
 
extern void  sendStringViaUSB( 	char *str ) ;

                    

#endif
