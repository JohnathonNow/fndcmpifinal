/* control.c
 * Author:	John Westhoff
 * Date:	12/08/2015
 * Course:	CSE20211
 *
 * Provides a library for wrapping the gfx_events for button presses
 * in an easy to use interface that allows for edge-detection on events.
 */
#include <stdio.h>
#include <string.h>
#include "gfx_john.h"
static char keys_cur[255] = {'\0'};
static char keys_prev[255] = {'\0'};
/* update_keys
 * Updates the key states.
 */
void update_keys()
{
	unsigned char e;
	memcpy(keys_prev,keys_cur,255);
	while (gfx_event_waiting())
	{
		e = gfx_wait();
		if (e>0&&e<255)
		{
			keys_cur[e] = !keys_cur[e];
		}
	}
}
/* key_down
 * Arguments:
 * 	key		-	the key to check
 * Returns:
 * 	1 if the key is pressed, 0 otherwise
 * Tells whether key is being held down
 */
int key_down(unsigned char key)
{
	if (key<0||key>255)
	{
		return 0;
	}
	return keys_cur[key];
}
/* key_pressed
 * Arguments:
 * 	key		-	the key to check
 * Returns:
 * 	1 if the key was just pressed, 0 otherwise
 * Tells whether key was just pressed or not
 */
int key_pressed(unsigned char key)
{
	if (key<0||key>255)
	{
		return 0;
	}
	return keys_cur[key]&&!keys_prev[key];
}
/* key_released
 * Arguments:
 * 	key		-	the key to check
 * Returns:
 * 	1 if the key was just released, 0 otherwise
 * Tells whether key was just released or not
 */
int key_released(unsigned char key)
{
	if (key<0||key>255)
	{
		return 0;
	}
	return keys_prev[key]&&!keys_cur[key];
}
