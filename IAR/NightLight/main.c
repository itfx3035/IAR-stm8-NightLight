#include "iostm8s103f3.h" 

unsigned int adc_ain2;
unsigned int min_level, max_level;
unsigned char low, high;

void SomeDelay()
{
  for (unsigned long delay_count=0; delay_count<300000; delay_count++);
};  


void adc_init(){

   CLK_PCKENR2 |= 0x08;	    // turn on ADC cycling

   ADC_CR1_bit.SPSEL = 0x07;     // Fosc/18
   ADC_CR1_bit.CONT = 0;	 // single conversion
   ADC_CSR_bit.EOCIE = 0;	 // interrupt off
   ADC_CSR_bit.AWDIE = 0;	 // watchdog off
   ADC_CR2 = 0;
   ADC_CR2_bit.ALIGN = 1;	 

   ADC_CR1_bit.ADON = 1;
}

void adc_conv(){
 //set ADC channel
 ADC_CSR_bit.CH = 2; //PC4 port

 for(unsigned long int delay = 0; delay < 0xfff; delay++); // adc need some time to start

 //start conversion
 ADC_CR1_bit.ADON = 1;			  

 //wait for finish flag
 while(!ADC_CSR_bit.EOC);	  

 //clear finish flag
 ADC_CSR_bit.EOC = 0;			   

 //get result
 low = ADC_DRL;  // exact sequence! don't read twice!
 high = ADC_DRH;
 adc_ain2 = high << 8;
 adc_ain2 |= low;
}


int main( void )
{
  adc_ain2 = 20;
  min_level = 60;
  max_level = 70;
  
  // set PD6 port to output
  PD_DDR_bit.DDR6 = 1; 
  PD_CR1_bit.C16 = 1;
  
  // set initial state - off
  PD_ODR_bit.ODR6 = 0; 
  
  // set ADC
  adc_init();
  
  while (1) // infinite loop
  {
    adc_conv();
    
    if (adc_ain2>max_level) // light level is high on photodiode
    {
      PD_ODR_bit.ODR6 = 0; // turn off the light
    }
    
    if (adc_ain2<min_level) // light level is low on photodiode
    {
      PD_ODR_bit.ODR6 = 1; // turn on the light
    }
    
    SomeDelay();  //delay   
    
  }
  return 0;
}
