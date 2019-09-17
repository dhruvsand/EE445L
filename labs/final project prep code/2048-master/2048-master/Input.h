extern int prevX;
extern int prevY;
extern unsigned int ADCMail;
extern int arrowReady;

// Initializes sliders and buttons
void Input_Init(void);

// Gets position of slider, returning the side of the
// board it is on. 1=left, 2=up, 3=right, 4=down
int getSliderPosition(void);

// initializes Button 1
void Button1_Init(void);

// initializes Button 2
void Button2_Init(void);
