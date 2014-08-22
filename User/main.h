#ifndef TM_ILI9341_BUTTON_H
#define TM_ILI9341_BUTTON_H

#include <stdint.h>

#define MAXSEG  100 
#define WIDTH  320 
#define HEIGHT  240
#define NROW  3
#define NCOL  3
#define BUTTONRADIUS1  3
#define BUTTONRADIUS2 40 
#define ROWSPACING (HEIGHT/NROW)
#define COLSPACING (HEIGHT/NROW)
#define NBUTTON  (NROW*NCOL)
#define MINDISTANCE  (10)
#define MINUPEVENTS  50
#define HITBUTTONCOLOR  ILI9341_COLOR_BROWN
#define BUTTONCOLOR ILI9341_COLOR_ORANGE
#define LINECOLOR ILI9341_COLOR_BLACK 

typedef struct point_t {
    int16_t x;
    int16_t y;
} point_t ;

typedef enum { false, true } bool;


typedef struct button_t{
    point_t center ;
    int hit;  // the order it was hit in.  0 if not yet hit;
}  button_t; 

extern void displayInitialScreen(button_t *buttons);

extern void clearButtons( button_t *buttons);

extern void findHits(point_t *segments, int nsegment, button_t *buttons);

extern double sqr(double x);
extern double dist2(point_t v, point_t w);
extern double distToSegmentSquared(point_t v, point_t w, point_t p);
 
extern void  sendStringViaUSB( 	char *str ) ;

                    

#endif
