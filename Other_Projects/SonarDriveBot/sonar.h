
extern volatile unsigned long sonarValue;

void SonarInit(void);
unsigned long SonarRead(void);
void SonarBackgroundRead(void (*)(unsigned long));
