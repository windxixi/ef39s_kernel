#ifndef _APDS9900_H_
#define _APDS9900_H_

#define APDS9900_TYPE_PROXIMITY		1
#define APDS9900_TYPE_LIGHT			2
#define APDS9900_TYPE_ALL           (APDS9900_TYPE_PROXIMITY | APDS9900_TYPE_LIGHT)

typedef struct {
        int x;
        int y;
        int z;
} axes_t;

typedef enum {
        E_ACTIVE_NONE,
        E_ACTIVE_PROXIMITY = APDS9900_TYPE_PROXIMITY,
        E_ACTIVE_LIGHT = APDS9900_TYPE_LIGHT,
        E_ACTIVE_ALL,
} active_e;

int apds9900_control_enable(int type, bool enable);
//int apds9900_control_suspend(void);
//int apds9900_control_resume(void);
int apds9900_ps_measure(axes_t *val);
int apds9900_ls_measure(axes_t *val);

#endif // _APDS9900_H_
