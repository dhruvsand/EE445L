// ----------------DAC_Init------------------
// Initialize 6-bit DAC, called once, PC0-PC5
// Input: none
// Output: none
void DAC_Init(void);


// -----------------DAC_Out------------------
// output to DAC
// Input: 6-bit data, 0 to 63 
// Output: none
void DAC_Out(unsigned long data);
