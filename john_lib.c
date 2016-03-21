/* john_lib.c
 * Author:	John Westhoff
 * Date:	12/08/2015
 * Course:	CSE20211
 *
 * This is just several functions that I thought could be useful
 * outside the program too, and so I put them in their own file
 * for code-reuse's sake.
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
/* num2str
 * arguments:
 *      n               -       the number to convert to a string
 * Returns:
 *      A c string representation of the number n
 * Source:
 *      Piazza post by Prof. Ramzi Baulaun
 * Converts a number into a string.
 */
char *num2str(int n)
{
  static char a[10], *p = a;
  snprintf(p, 10, "%d", n);
  return p;
}
/* collision_square
 * arguments:
 *      x1			-	the first x coordinate
 *		y1			-	the first y coordinate
 *		x2			-	the second x coordinate
 *		y2			-	the second y coordinate
 *		s			-	the size of the square (half the side)
 * Returns:
 *      Whether (x1,y1) and (x2,y2) fit within a box with side 2s or not
 * Just a short collision box check.
 */
int collision_square(double x1, double y1, double x2, double y2, double s)
{
	return (fabs(x1-x2)<s) && (fabs(y1-y2)<s);
}
/* collision_square
 * arguments:
 *      x1			-	the first x coordinate
 *		y1			-	the first y coordinate
 *		x2			-	the second x coordinate
 *		y2			-	the second y coordinate
 * Returns:
 *      The angle in radians from (x1,y1) to (x2,y2)
 * Gets the angle between two points.
 */
double angle_to(double x1, double y1, double x2, double y2)
{
	return atan2(y2-y1,x2-x1);
}
