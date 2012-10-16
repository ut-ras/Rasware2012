
extern volatile unsigned long sonarValues[2];


void SonarInit(void);

unsigned long *SonarRead(void);

void SonarBackgroundRead(void (*callback)(unsigned long *));
