### USB Host Custom Class {#host_cust_tutorial}

This example application shows how to communicate with serial RS232 adapter with
[Prolific PL2303](https://www.prolific.com.tw/US/index.aspx)
UART-to-USB bridge chip from a microcontroller. It is a simple demonstration
on how to send data from the USB Host via the USB to serial RS232 adapter to an attached serial terminal. Here, the USB Host sends "Test!" to the
USB to serial RS232 adapter and stores all incoming data from the device into the array `receive_buf`. This example is using the Custom
Class because the PL2303 is not USB CDC ACM compliant.

The following picture shows an exemplary connection of the development board and an PL2303 based Device. The PL2303 Device is
connected to a PC via RS232 to check incoming messages.

![USB host custom class example hardware setup](usbh_cust_pl2303_example_setup.png)

#### Create the "USB Host Custom Class PL2303" Project

In this example, we are using the [MCBSTM32F400](https://www.keil.arm.com/boards/keil-mcbstm32f400-ver-12-3cd831d/projects/)
board with the STM32F407IGHx device. Create a new project in MDK
(Select Device **STMicroelectronics:STM32F4 Series: STM32F407:STM32F407IG:STM32F407IGHx**).
In the **Manage Run-Time Environment** window, select the following components:

- **Board Support:LED (API):LED** (Variant **MCBSTM32F400**)
- **CMSIS:Core**
- **CMSIS:RTOS2 (API):Keil RTX5**
- **CMSIS Driver:USART (API):PL2303**
- **CMSIS Driver:USB Host (API):Full-speed**
- **Device:STM32Cube Framework (API):Classic**
- **USB** (Variant **MDK**)
- **USB:Host: 1**
- **USB:Host:Custom Class**

Click the **Resolve** button and then **OK**.

Before continuing to add the required source code, you need to add a template file called `USBH_PL2303.c`:

- Right-click on **Source Group 1** and select **Add New Item to Group 'Source Group 1'...**.
- Click on **User Code Template** and scroll down on  the left side, until you see the **USB** component template files.
- Choose **USB Host Prolific 2303**
- Click on **Add**.

##### Source Files

- Click on **New (Ctrl + N)** to create a new file.
- Save it (**File -> Save**) as **main.h**.
- Copy the following code into the main.h file and save it again:

```c
/**
  ******************************************************************************
  * @file    Templates/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017-2018 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern uint64_t app_main_stk[];
extern const osThreadAttr_t app_main_attr;

/* Exported macro ------------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */
extern void app_main (void *arg);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
```

- Right-click on **Source Group 1** and select **Add New Item to Group 'Source Group 1'...**.
- Click on **C File (.c)** and enter **main** in the **Name** box.
- Copy the following code into the main.c file:

```c
/**
  ******************************************************************************
  * @file    Templates/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017-2018 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

#ifdef RTE_CMSIS_RTOS2_RTX5
/**
  * Override default HAL_GetTick function
  */
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount ());
  }

  /* If Kernel is not running wait approximately 1 ms then increment
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}
#endif

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();


  /* Add your application code here
     */

#ifdef RTE_CMSIS_RTOS2
  /* Initialize CMSIS-RTOS2 */
  osKernelInitialize ();

  /* Create application main thread */
  osThreadNew(app_main, NULL, &app_main_attr);

  /* Start thread execution */
  osKernelStart();
#endif

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
```

- Right-click on **Source Group 1** and select **Add New Item to Group 'Source Group 1'...**.
- Click on **C File (.c)** and enter **app_main** in the **Name** box.
- Copy the following code into the app_main.c file:

```c
#include "main.h
#include "rl_usb.h

#include "Board_LED.h

#include "Driver_USART.h

/* UART Driver */
extern ARM_DRIVER_USART       Driver_USART9;
#define ptrUART_USB         (&Driver_USART9)

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

static uint8_t  receive_buf[64];

/*------------------------------------------------------
 *        UART Done Callback
 *----------------------------------------------------------------------------*/
void UART_Done (uint32_t event) {
  switch (event) {
    case ARM_USART_EVENT_SEND_COMPLETE:
      // All requested data was transmitted
      LED_On(1);
      break;
    case ARM_USART_EVENT_RECEIVE_COMPLETE:
      // After 64 bytes were received, restart new reception
      ptrUART_USB->Receive (receive_buf, 64);
      LED_On(2);
      break;
  }
}

/*------------------------------------------------------
 *        Application
 *----------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  static bool con_last = false;
         bool con;

  (void)arg;

  LED_Initialize    ();

  USBH_Initialize (0);                        /* Initialize USB Host 0        */

  while (1) {
    con = (USBH_CustomClass_GetStatus(0) == usbOK);
    if (con ^ con_last) {
      if (con) {
        con_last = true;
        LED_On(0);
        osDelay(1000);

        /* Initialize and configure UART <-> USB Bridge */
        ptrUART_USB->Initialize  (UART_Done);
        ptrUART_USB->PowerControl(ARM_POWER_FULL);
        ptrUART_USB->Control     (ARM_USART_MODE_ASYNCHRONOUS |
                                  ARM_USART_DATA_BITS_8       |
                                  ARM_USART_PARITY_NONE       |
                                  ARM_USART_STOP_BITS_1       |
                                  ARM_USART_FLOW_CONTROL_NONE ,
                                  115200                      );
        ptrUART_USB->Control     (ARM_USART_CONTROL_TX, 1);
        ptrUART_USB->Control     (ARM_USART_CONTROL_RX, 1);
        ptrUART_USB->Receive     (receive_buf, 64);
        ptrUART_USB->Send        ("Test!",     5);
      } else {
        con_last = false;
        LED_Off(0);
      }
    } else {
      if (con) {
        // Receive data can be checked by polling as callback will be called
        // only when all requested number of bytes were received
        // (in this case 64 bytes)
        if (ptrUART_USB->GetRxCount() == 5) {
          // If 5 bytes were received
        }
      }
      osDelay(1000);
    }
  }
}
```

Before building the project, you need to edit these configuration files (in Configuration Wizard view):

- Under **Device**, double-click **RTE_Device.h** and:
  - enable **USB OTG Full-speed** and under it:
    - enable **Host [Driver_USBH0]**
    - disable **Host [Driver_USBH0]:Overcurrent Detection Pin**

- Double-click **USB:USBH_Config_0.h** and under **USB Host 0** change:
  - set **Maximum Pipes in system** to **4**

Before building and downloading the project to the target, make sure that the correct debugger is set in the **Options for
Target** dialog (ALT + F7). You may then build and download the example project to the evaluation board using the µVision
commands:

- **Project** --> **Build target (F7)**
- **Flash** --> **Download (F8)**
- **Debug** --> **Start/Stop Debug Session (Ctrl + F5)**
- **Debug** --> **Run (F5)**

After these steps, the project should start executing on your evaluation kit.
In case of errors, refer to the Evaluation Board User's Guide for configuration information.

#### Using the "USB Host Custom Class PL2303" Project

##### Hardware Setup

- Verify all jumper settings on the target hardware.
- Connect an USB to serial  RS232 adapter using the Prolific PL2303 to the development board's **USBFS** connector.
- Connect the serial line to a PC and open a terminal using 115200 baud data rate.
- You should see the message "Test!" in the terminal window. You can also send messages from the PC to the microcontroller
  device. For that, please check the `receive_buf` buffer called by the `->Receive` function.

### USB Host CDC ACM {#host_cdc_tutorial}

This example application shows how to communicate with an **USB CDC ACM Device** from a microcontroller. It is a simple demonstration
on how to send data from the USB Host to the attached USB CDC ACM Device. Here, the USB Host sends "Test!" to the USB CDC ACM Device
and stores all incoming data from the device into the array `receive_buf`.

The following picture shows an exemplary connection of the development board and an USB CDC ACM Device implemented on another
development board. This USB CDC ACM Device is connected to a PC via RS232 to check incoming messages.

![USB device audio example hardware setup](usbh_cdc_example_setup.png)

#### Create the "USB Host CDC" Project

In this example, we are using the [MCBSTM32F400](https://www.keil.arm.com/boards/keil-mcbstm32f400-ver-12-3cd831d/projects/)\
board with the STM32F407IGHx device.
Create a new project in MDK (Select Device **STMicroelectronics:STM32F4 Series: STM32F407:STM32F407IG:STM32F407IGHx**).
In the **Manage Run-Time Environment** window, select the following components:

- **Board Support:LED (API):LED** (Variant **MCBSTM32F400**)
- **CMSIS:Core**
- **CMSIS:RTOS2 (API):Keil RTX5**
- **CMSIS Driver:USART (API):CDC**
- **CMSIS Driver:USB Host (API):Full-speed**
- **Device:STM32Cube Framework (API):Classic**
- **USB** (Variant **MDK**)
- **USB:Host: 1**
- **USB:Host:CDC**

Click the **Resolve** button and then **OK**.

##### Source Files

- Click on **New (Ctrl + N)** to create a new file.
- Save it (**File -> Save**) as **main.h**.
- Copy the following code into the main.h file and save it again:

```c
/**
  ******************************************************************************
  * @file    Templates/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017-2018 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern uint64_t app_main_stk[];
extern const osThreadAttr_t app_main_attr;

/* Exported macro ------------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */
extern void app_main (void *arg);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
```

- Right-click on **Source Group 1** and select **Add New Item to Group 'Source Group 1'...**.
- Click on **C File (.c)** and enter **main** in the **Name** box.
- Copy the following code into the main.c file:

```c
/**
  ******************************************************************************
  * @file    Templates/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017-2018 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

#ifdef RTE_CMSIS_RTOS2_RTX5
/**
  * Override default HAL_GetTick function
  */
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount ());
  }

  /* If Kernel is not running wait approximately 1 ms then increment
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}
#endif

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();


  /* Add your application code here
     */

#ifdef RTE_CMSIS_RTOS2
  /* Initialize CMSIS-RTOS2 */
  osKernelInitialize ();

  /* Create application main thread */
  osThreadNew(app_main, NULL, &app_main_attr);

  /* Start thread execution */
  osKernelStart();
#endif

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
```

- Right-click on **Source Group 1** and select **Add New Item to Group 'Source Group 1'...**.
- Click on **C File (.c)** and enter **app_main** in the **Name** box.
- Copy the following code into the app_main.c file:

```c
#include "main.h
#include "rl_usb.h

#include "Board_LED.h

#include "Driver_USART.h

/* UART Driver */
extern ARM_DRIVER_USART       Driver_USART9;
#define ptrUART_USB         (&Driver_USART9)

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

static uint8_t  receive_buf[64];

/*------------------------------------------------------
 *        UART Done Callback
 *----------------------------------------------------------------------------*/
void UART_Done (uint32_t event) {
  switch (event) {
    case ARM_USART_EVENT_SEND_COMPLETE:
      // All requested data was transmitted
      LED_On(1);
      break;
    case ARM_USART_EVENT_RECEIVE_COMPLETE:
      // After 64 bytes were received, restart new reception
      ptrUART_USB->Receive (receive_buf, 64);
      LED_On(2);
      break;
  }
}

/*------------------------------------------------------
 *        Application
 *----------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  static bool con_last = false;
         bool con;

  (void)arg;

  LED_Initialize    ();

  USBH_Initialize (0);                        /* Initialize USB Host 0        */

  while (1) {
    con = (USBH_CDC_ACM_GetStatus(0) == usbOK);
    if (con ^ con_last) {
      if (con) {
        con_last = true;
        LED_On(0);
        osDelay(1000);

        /* Initialize and configure UART <-> USB Bridge */
        ptrUART_USB->Initialize  (UART_Done);
        ptrUART_USB->PowerControl(ARM_POWER_FULL);
        ptrUART_USB->Control     (ARM_USART_MODE_ASYNCHRONOUS |
                                  ARM_USART_DATA_BITS_8       |
                                  ARM_USART_PARITY_NONE       |
                                  ARM_USART_STOP_BITS_1       |
                                  ARM_USART_FLOW_CONTROL_NONE ,
                                  115200                      );
        ptrUART_USB->Control     (ARM_USART_CONTROL_TX, 1);
        ptrUART_USB->Control     (ARM_USART_CONTROL_RX, 1);
        ptrUART_USB->Receive     (receive_buf, 64);
        ptrUART_USB->Send        ("Test!",     5);
      } else {
        con_last = false;
        LED_Off(0);
      }
    } else {
      if (con) {
        // Receive data can be checked by polling as callback will be called
        // only when all requested number of bytes were received
        // (in this case 64 bytes)
        if (ptrUART_USB->GetRxCount() == 5) {
          // If 5 bytes were received
        }
      }
      osDelay(1000);
    }
  }
}
```

Before building the project, you need to edit these configuration files (in Configuration Wizard view):

- Under **Device**, double-click **RTE_Device.h** and:
  - enable **USB OTG Full-speed** and under it:
    - enable **Host [Driver_USBH0]**
    - disable **Host [Driver_USBH0]:Overcurrent Detection Pin**

- Double-click **USB:USBH_Config_0.h** and under **USB Host 0** change:
  - set **Maximum Pipes in system** to **4**

Before building and downloading the project to the target, make sure that the correct debugger is set in the **Options for
Target** dialog (ALT + F7). You may then build and download the example project to the evaluation board using the µVision
commands:

- **Project** --> **Build target (F7)**
- **Flash** --> **Download (F8)**
- **Debug** --> **Start/Stop Debug Session (Ctrl + F5)**
- **Debug** --> **Run (F5)**

After these steps, the project should start executing on your evaluation kit.
In case of errors, refer to the Evaluation Board User's Guide for configuration information.

#### Using the "USB Host CDC" Project

##### Hardware Setup

- Verify all jumper settings on the target hardware.
- Connect an USB CDC ACM Device (for example another development board with this example project: \ref usbd_example_cdc) to the
  development board's **USBFS** connector.
