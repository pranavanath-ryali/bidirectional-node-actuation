#ifndef NODE_SERVO_H
#define NODE_SERVO_H

#define SERVO_GPIO   GPIO_NUM_10
#define SERVO_MIN_US 500
#define SERVO_MAX_US 2400

#define ANGLE_STEP 10
#define MAX_POS    (180 / ANGLE_STEP)
#define CENTER_POS (MAX_POS / 2)

void servo_init(void);
void servo_set_angle(int angle);

#endif
