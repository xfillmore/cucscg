#include "bmbObjects.h"

/* helper function */
/* takes args: current angle pos, current height pos */
void cvertex(int th, double h)
{
    glColor3f(47.0  / rgbmax, 161.0 / rgbmax, 64.0  / rgbmax);
    glVertex3d(cosd(th), h, sind(th));
}