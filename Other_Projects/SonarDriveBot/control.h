
extern volatile unsigned long potValues[2];
extern volatile int switchValue;

void ControlInit(void);

unsigned long *PotRead(void);

int SwitchRead(void);

void ControlBackgroundRead(void (*callback)(unsigned long *, int));
