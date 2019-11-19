/* ###################################################################
**     Filename    : main.c
**     Processor   : S32K11x
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
/* MODULE main */


/* Including necessary module. Cpu.h contains other modules needed for compiling.*/
#include "Cpu.h"

  volatile int exit_code = 0;

/* User includes (#include below this line is not maintained by Processor Expert) */

/*!
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
*/

  volatile long adcRawValue;

void ADC_ISR(void) {
	//Read ADC Result
	ADC_DRV_GetChanResult(INST_ADCONV1, 0U, (uint16_t *)&adcRawValue);

	//Update PWM Channel duty cycle with 4 times the ADC Output (8x4096 = 32768)
	FTM_DRV_UpdatePwmChannel(INST_FLEXTIMER_PWM1, 0UL, FTM_PWM_UPDATE_IN_DUTY_CYCLE, adcRawValue * 8,0UL, true);

	//Reset PDB Counter
	PDB_DRV_SoftTriggerCmd(INST_PDB1);
}


int main(void)
{
  /* Write your local variable definition here */
	//FTM State struct
	ftm_state_t ftmState;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */

    //Configure clock
    CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT, g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0u, CLOCK_MANAGER_POLICY_AGREEMENT);

    //Init pins driver
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);

    //Init PWM using FTM (FlexTimer Module)
    FTM_DRV_Init(INST_FLEXTIMER_PWM1, &flexTimer_pwm1_InitConfig, &ftmState);
    FTM_DRV_InitPwm(INST_FLEXTIMER_PWM1, &flexTimer_pwm1_PwmConfig);
    FTM_DRV_UpdatePwmChannel(INST_FLEXTIMER_PWM1, 0UL, FTM_PWM_UPDATE_IN_DUTY_CYCLE, 0UL,0UL, true);


    //Configure ADC
    ADC_DRV_ConfigConverter(INST_ADCONV1, &adConv1_ConvConfig0);
    ADC_DRV_ConfigChan(INST_ADCONV1, 0UL, &adConv1_ChnConfig0);

    //Configure PDB (programmable delay block) for ADC Pre-triggering
    PDB_DRV_Init(INST_PDB1, &pdb1_InitConfig0);
    PDB_DRV_Enable(INST_PDB1);
    PDB_DRV_ConfigAdcPreTrigger(INST_PDB1, 0UL, &pdb1_AdcTrigInitConfig0);
    //32500 corresponds to 1 second delay (calculated from clock speed)
    //32500/20=1625. ADC Freq of 20hz
    PDB_DRV_SetTimerModulusValue(INST_PDB1, 1625);
    PDB_DRV_SetAdcPreTriggerDelayValue(INST_PDB1, 0UL, 0UL, 1625);
    PDB_DRV_LoadValuesCmd(INST_PDB1);
    PDB_DRV_SoftTriggerCmd(INST_PDB1);


    //Install and Enable ADC Interrupt
    INT_SYS_InstallHandler(ADC0_IRQn, &ADC_ISR, (isr_t *)0);
    INT_SYS_EnableIRQ(ADC0_IRQn);

    for (;;) {

    }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.1 [05.21]
**     for the Freescale S32K series of microcontrollers.
**
** ###################################################################
*/
