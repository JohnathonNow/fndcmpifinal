#ifndef CONTROL_INCLUDE
#define CONTROL_INCLUDE
#define LEFT_ALT 233
#define RIGHT_ALT 234
#define LEFT_SHIFT 225
#define RIGHT_SHIFT 226
void update_keys();
int key_pressed(char key);
int key_down(char key);
int key_released(char key);
#endif
