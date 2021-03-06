/*
 ******************************************************************************
 *  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include <stdatomic.h>
#include "core_hal_stm32f2xx.h"
#include "core_hal.h"
#include "stm32f2xx.h"
#include <string.h>
#include "hw_config.h"
#include "stm32_it.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "dcd_flash.h"

/* Private typedef ----------------------------------------------------------*/

/* Private define -----------------------------------------------------------*/

/* Private macro ------------------------------------------------------------*/

/* Private variables --------------------------------------------------------*/
/**
 * Start of interrupt vector table.
 */
extern char link_interrupt_vectors_location;
extern char link_ram_interrupt_vectors_location;
extern char link_ram_interrupt_vectors_location_end;

/* USB Interrupt Handlers from usb_hal.c */
#ifdef USE_USB_OTG_FS
    extern void OTG_FS_WKUP_irq(void);
    extern void OTG_FS_irq(void);
#elif defined USE_USB_OTG_HS
    extern void OTG_HS_EP1_OUT_irq(void);
    extern void OTG_HS_EP1_IN_irq(void);
    extern void OTG_HS_WKUP_irq(void);
    extern void OTG_HS_irq(void);
#endif

#if 0
IDX[x] = added IRQ handler
00 [ ] _estack
01 [x] Reset_Handler - in startup_stm32f2xx.s file
02 [x] NMI_Handler
03 [x] HardFault_Handler
04 [x] MemManage_Handler
05 [x] BusFault_Handler
06 [x] UsageFault_Handler
07 [ ] 0
08 [ ] 0
09 [ ] 0
10 [ ] 0
11 [x] SVC_Handler
12 [x] DebugMon_Handler
13 [ ] 0
14 [x] PendSV_Handler
15 [x] SysTick_Handler
                                         // External Interrupts ----------------
16 [ ] WWDG_IRQHandler                   // Window WatchDog
17 [ ] PVD_IRQHandler                    // PVD through EXTI Line detection
18 [ ] TAMP_STAMP_IRQHandler             // Tamper and TimeStamps through the EXTI line
19 [ ] RTC_WKUP_IRQHandler               // RTC Wakeup through the EXTI line
20 [ ] FLASH_IRQHandler                  // FLASH
21 [ ] RCC_IRQHandler                    // RCC
22 [x] EXTI0_IRQHandler                  // EXTI Line0
23 [x] EXTI1_IRQHandler                  // EXTI Line1
24 [x] EXTI2_IRQHandler                  // EXTI Line2
25 [x] EXTI3_IRQHandler                  // EXTI Line3
26 [x] EXTI4_IRQHandler                  // EXTI Line4
27 [ ] DMA1_Stream0_IRQHandler           // DMA1 Stream 0
28 [ ] DMA1_Stream1_IRQHandler           // DMA1 Stream 1
29 [ ] DMA1_Stream2_IRQHandler           // DMA1 Stream 2
30 [ ] DMA1_Stream3_IRQHandler           // DMA1 Stream 3
31 [ ] DMA1_Stream4_IRQHandler           // DMA1 Stream 4
32 [ ] DMA1_Stream5_IRQHandler           // DMA1 Stream 5
33 [ ] DMA1_Stream6_IRQHandler           // DMA1 Stream 6
34 [x] ADC_IRQHandler                    // ADC1, ADC2 and ADC3s
35 [ ] CAN1_TX_IRQHandler                // CAN1 TX
36 [ ] CAN1_RX0_IRQHandler               // CAN1 RX0
37 [ ] CAN1_RX1_IRQHandler               // CAN1 RX1
38 [ ] CAN1_SCE_IRQHandler               // CAN1 SCE
39 [x] EXTI9_5_IRQHandler                // External Line[9:5]s
40 [x] TIM1_BRK_TIM9_IRQHandler          // TIM1 Break and TIM9
41 [x] TIM1_UP_TIM10_IRQHandler          // TIM1 Update and TIM10
42 [x] TIM1_TRG_COM_TIM11_IRQHandler     // TIM1 Trigger and Commutation and TIM11
43 [x] TIM1_CC_IRQHandler                // TIM1 Capture Compare
44 [x] TIM2_IRQHandler                   // TIM2
45 [x] TIM3_IRQHandler                   // TIM3
46 [x] TIM4_IRQHandler                   // TIM4
47 [x] I2C1_EV_IRQHandler                // I2C1 Event
48 [x] I2C1_ER_IRQHandler                // I2C1 Error
49 [ ] I2C2_EV_IRQHandler                // I2C2 Event
50 [ ] I2C2_ER_IRQHandler                // I2C2 Error
51 [ ] SPI1_IRQHandler                   // SPI1
52 [ ] SPI2_IRQHandler                   // SPI2
53 [x] USART1_IRQHandler                 // USART1
54 [x] USART2_IRQHandler                 // USART2
55 [x] USART3_IRQHandler                 // USART3
56 [x] EXTI15_10_IRQHandler              // External Line[15:10]s
57 [x] RTC_Alarm_IRQHandler              // RTC Alarm (A and B) through EXTI Line
58 [x] OTG_FS_WKUP_IRQHandler            // USB OTG FS Wakeup through EXTI line
59 [x] TIM8_BRK_TIM12_IRQHandler         // TIM8 Break and TIM12
60 [x] TIM8_UP_TIM13_IRQHandler          // TIM8 Update and TIM13
61 [x] TIM8_TRG_COM_TIM14_IRQHandler     // TIM8 Trigger and Commutation and TIM14
62 [x] TIM8_CC_IRQHandler                // TIM8 Capture Compare
63 [x] DMA1_Stream7_IRQHandler           // DMA1 Stream7
64 [ ] FSMC_IRQHandler                   // FSMC
65 [ ] SDIO_IRQHandler                   // SDIO
66 [x] TIM5_IRQHandler                   // TIM5
67 [ ] SPI3_IRQHandler                   // SPI3
68 [x] UART4_IRQHandler                  // UART4
69 [x] UART5_IRQHandler                  // UART5
70 [x] TIM6_DAC_IRQHandler               // TIM6 and DAC1&2 underrun errors
71 [x] TIM7_IRQHandler                   // TIM7
72 [ ] DMA2_Stream0_IRQHandler           // DMA2 Stream 0
73 [ ] DMA2_Stream1_IRQHandler           // DMA2 Stream 1
74 [ ] DMA2_Stream2_IRQHandler           // DMA2 Stream 2
75 [ ] DMA2_Stream3_IRQHandler           // DMA2 Stream 3
76 [ ] DMA2_Stream4_IRQHandler           // DMA2 Stream 4
77 [ ] ETH_IRQHandler                    // Ethernet
78 [ ] ETH_WKUP_IRQHandler               // Ethernet Wakeup through EXTI line
79 [x] CAN2_TX_IRQHandler                // CAN2 TX
80 [x] CAN2_RX0_IRQHandler               // CAN2 RX0
81 [x] CAN2_RX1_IRQHandler               // CAN2 RX1
82 [x] CAN2_SCE_IRQHandler               // CAN2 SCE
83 [x] OTG_FS_IRQHandler                 // USB OTG FS
84 [x] DMA2_Stream5_IRQHandler           // DMA2 Stream 5
85 [ ] DMA2_Stream6_IRQHandler           // DMA2 Stream 6
86 [ ] DMA2_Stream7_IRQHandler           // DMA2 Stream 7
87 [ ] USART6_IRQHandler                 // USART6
88 [x] I2C3_EV_IRQHandler                // I2C3 event
89 [x] I2C3_ER_IRQHandler                // I2C3 error
90 [x] OTG_HS_EP1_OUT_IRQHandler         // USB OTG HS End Point 1 Out
91 [x] OTG_HS_EP1_IN_IRQHandler          // USB OTG HS End Point 1 In
92 [x] OTG_HS_WKUP_IRQHandler            // USB OTG HS Wakeup through EXTI
93 [x] OTG_HS_IRQHandler                 // USB OTG HS
94 [ ] DCMI_IRQHandler                   // DCMI
95 [ ] CRYP_IRQHandler                   // CRYP crypto
96 [ ] HASH_RNG_IRQHandler               // Hash and Rng
#endif

// Reset_Handler defined in startup_stm32f2xx.s (index 1)
const unsigned NMI_Handler_Idx                      = 2;
const unsigned HardFault_Handler_Idx                = 3;
const unsigned MemManage_Handler_Idx                = 4;
const unsigned BusFault_Handler_Idx                 = 5;
const unsigned UsageFault_Handler_Idx               = 6;
const unsigned SVC_Handler_Idx                      = 11;
const unsigned DebugMon_Handler_Idx                 = 12;
const unsigned PendSV_Handler_Idx                   = 14;
const unsigned SysTick_Handler_Idx                  = 15;
const unsigned EXTI0_IRQHandler_Idx                 = 22;
const unsigned EXTI1_IRQHandler_Idx                 = 23;
const unsigned EXTI2_IRQHandler_Idx                 = 24;
const unsigned EXTI3_IRQHandler_Idx                 = 25;
const unsigned EXTI4_IRQHandler_Idx                 = 26;
const unsigned ADC_IRQHandler_Idx                   = 34;
const unsigned EXTI9_5_IRQHandler_Idx               = 39;
const unsigned TIM1_BRK_TIM9_IRQHandler_Idx         = 40;
const unsigned TIM1_UP_TIM10_IRQHandler_Idx         = 41;
const unsigned TIM1_TRG_COM_TIM11_IRQHandler_Idx    = 42;
const unsigned TIM1_CC_IRQHandler_Idx               = 43;
const unsigned TIM2_IRQHandler_Idx                  = 44;
const unsigned TIM3_IRQHandler_Idx                  = 45;
const unsigned TIM4_IRQHandler_Idx                  = 46;
const unsigned I2C1_EV_IRQHandler_Idx               = 47;
const unsigned I2C1_ER_IRQHandler_Idx               = 48;
const unsigned USART1_IRQHandler_Idx                = 53;
const unsigned USART2_IRQHandler_Idx                = 54;
const unsigned USART3_IRQHandler_Idx                = 55;
const unsigned EXTI15_10_IRQHandler_Idx             = 56;
const unsigned RTC_Alarm_IRQHandler_Idx             = 57;
const unsigned OTG_FS_WKUP_IRQHandler_Idx           = 58;
const unsigned TIM8_BRK_TIM12_IRQHandler_Idx        = 59;
const unsigned TIM8_UP_TIM13_IRQHandler_Idx         = 60;
const unsigned TIM8_TRG_COM_TIM14_IRQHandler_Idx    = 61;
const unsigned TIM8_CC_IRQHandler_Idx               = 62;
const unsigned DMA1_Stream7_IRQHandler_Idx          = 63;
const unsigned TIM5_IRQHandler_Idx                  = 66;
const unsigned UART4_IRQHandler_Idx                 = 68;
const unsigned UART5_IRQHandler_Idx                 = 69;
const unsigned TIM6_DAC_IRQHandler_Idx              = 70;
const unsigned TIM7_IRQHandler_Idx                  = 71;
const unsigned CAN2_TX_IRQHandler_Idx               = 79;
const unsigned CAN2_RX0_IRQHandler_Idx              = 80;
const unsigned CAN2_RX1_IRQHandler_Idx              = 81;
const unsigned CAN2_SCE_IRQHandler_Idx              = 82;
const unsigned OTG_FS_IRQHandler_Idx                = 83;
const unsigned DMA2_Stream5_IRQHandler_Idx          = 84;
const unsigned I2C3_EV_IRQHandler_Idx               = 88;
const unsigned I2C3_ER_IRQHandler_Idx               = 89;
const unsigned OTG_HS_EP1_OUT_IRQHandler_Idx        = 90;
const unsigned OTG_HS_EP1_IN_IRQHandler_Idx         = 91;
const unsigned OTG_HS_WKUP_IRQHandler_Idx           = 92;
const unsigned OTG_HS_IRQHandler_Idx                = 93;

/* Extern variables ---------------------------------------------------------*/
/**
 * Updated by HAL_1Ms_Tick()
 */
extern volatile uint32_t TimingDelay;

/* Private function prototypes ----------------------------------------------*/

void HAL_Core_Config_systick_configuration(void) {
    SysTick_Configuration();

    dcd_migrate_data();
}

/**
 * Called by HAL_Core_Config() to allow the HAL implementation to override
 * the interrupt table if required.
 */
void HAL_Core_Setup_override_interrupts(void)
{
    memcpy(&link_ram_interrupt_vectors_location, &link_interrupt_vectors_location, &link_ram_interrupt_vectors_location_end-&link_ram_interrupt_vectors_location);
    uint32_t* isrs                          = (uint32_t*)&link_ram_interrupt_vectors_location;
    isrs[NMI_Handler_Idx]                   = (uint32_t)NMI_Handler;
    isrs[HardFault_Handler_Idx]             = (uint32_t)HardFault_Handler;
    isrs[MemManage_Handler_Idx]             = (uint32_t)MemManage_Handler;
    isrs[BusFault_Handler_Idx]              = (uint32_t)BusFault_Handler;
    isrs[UsageFault_Handler_Idx]            = (uint32_t)UsageFault_Handler;
    isrs[SVC_Handler_Idx]                   = (uint32_t)SVC_Handler;
    isrs[DebugMon_Handler_Idx]              = (uint32_t)DebugMon_Handler;
    isrs[PendSV_Handler_Idx]                = (uint32_t)PendSV_Handler;
    isrs[SysTick_Handler_Idx]               = (uint32_t)SysTickOverride;
    isrs[EXTI0_IRQHandler_Idx]              = (uint32_t)EXTI0_IRQHandler;
    isrs[EXTI1_IRQHandler_Idx]              = (uint32_t)EXTI1_IRQHandler;
    isrs[EXTI2_IRQHandler_Idx]              = (uint32_t)EXTI2_IRQHandler;
    isrs[EXTI3_IRQHandler_Idx]              = (uint32_t)EXTI3_IRQHandler;
    isrs[EXTI4_IRQHandler_Idx]              = (uint32_t)EXTI4_IRQHandler;
    isrs[ADC_IRQHandler_Idx]                = (uint32_t)ADC_irq;
    isrs[EXTI9_5_IRQHandler_Idx]            = (uint32_t)EXTI9_5_IRQHandler;
    isrs[TIM1_BRK_TIM9_IRQHandler_Idx]      = (uint32_t)TIM1_BRK_TIM9_irq;
    isrs[TIM1_UP_TIM10_IRQHandler_Idx]      = (uint32_t)TIM1_UP_TIM10_irq;
    isrs[TIM1_TRG_COM_TIM11_IRQHandler_Idx] = (uint32_t)TIM1_TRG_COM_TIM11_irq;
    isrs[TIM1_CC_IRQHandler_Idx]            = (uint32_t)TIM1_CC_irq;
    isrs[TIM2_IRQHandler_Idx]               = (uint32_t)TIM2_irq;
    isrs[TIM3_IRQHandler_Idx]               = (uint32_t)TIM3_irq;
    isrs[TIM4_IRQHandler_Idx]               = (uint32_t)TIM4_irq;
    isrs[USART1_IRQHandler_Idx]             = (uint32_t)HAL_USART1_Handler;
    isrs[USART2_IRQHandler_Idx]             = (uint32_t)HAL_USART2_Handler;
    isrs[USART3_IRQHandler_Idx]             = (uint32_t)HAL_USART3_Handler;
#ifdef USE_USB_OTG_FS
    isrs[OTG_FS_WKUP_IRQHandler_Idx]        = (uint32_t)OTG_FS_WKUP_irq;
#endif
    isrs[TIM8_BRK_TIM12_IRQHandler_Idx]     = (uint32_t)TIM8_BRK_TIM12_irq;
    isrs[TIM8_UP_TIM13_IRQHandler_Idx]      = (uint32_t)TIM8_UP_TIM13_irq;
    isrs[TIM8_TRG_COM_TIM14_IRQHandler_Idx] = (uint32_t)TIM8_TRG_COM_TIM14_irq;
    isrs[TIM8_CC_IRQHandler_Idx]            = (uint32_t)TIM8_CC_irq;
    isrs[TIM5_IRQHandler_Idx]               = (uint32_t)TIM5_irq;
    isrs[UART4_IRQHandler_Idx]              = (uint32_t)HAL_USART4_Handler;
    isrs[UART5_IRQHandler_Idx]              = (uint32_t)HAL_USART5_Handler;
    isrs[TIM6_DAC_IRQHandler_Idx]           = (uint32_t)TIM6_DAC_irq;
    isrs[TIM7_IRQHandler_Idx]               = (uint32_t)TIM7_override;  // WICED uses this for a JTAG watchdog handler
    isrs[CAN2_TX_IRQHandler_Idx]            = (uint32_t)CAN2_TX_irq;
    isrs[CAN2_RX0_IRQHandler_Idx]           = (uint32_t)CAN2_RX0_irq;
    isrs[CAN2_RX1_IRQHandler_Idx]           = (uint32_t)CAN2_RX1_irq;
    isrs[CAN2_SCE_IRQHandler_Idx]           = (uint32_t)CAN2_SCE_irq;
#ifdef USE_USB_OTG_FS
    isrs[OTG_FS_IRQHandler_Idx]             = (uint32_t)OTG_FS_irq;
#elif defined USE_USB_OTG_HS
    isrs[OTG_HS_EP1_OUT_IRQHandler_Idx]     = (uint32_t)OTG_HS_EP1_OUT_irq;
    isrs[OTG_HS_EP1_IN_IRQHandler_Idx]      = (uint32_t)OTG_HS_EP1_IN_irq;
    isrs[OTG_HS_WKUP_IRQHandler_Idx]        = (uint32_t)OTG_HS_WKUP_irq;
    isrs[OTG_HS_IRQHandler_Idx]             = (uint32_t)OTG_HS_irq;
#endif
    isrs[EXTI15_10_IRQHandler_Idx]          = (uint32_t)EXTI15_10_IRQHandler;
    isrs[I2C1_EV_IRQHandler_Idx]            = (uint32_t)I2C1_EV_irq;
    isrs[I2C1_ER_IRQHandler_Idx]            = (uint32_t)I2C1_ER_irq;
    isrs[I2C3_EV_IRQHandler_Idx]            = (uint32_t)I2C3_EV_irq;
    isrs[I2C3_ER_IRQHandler_Idx]            = (uint32_t)I2C3_ER_irq;
    isrs[DMA1_Stream7_IRQHandler_Idx]       = (uint32_t)DMA1_Stream7_irq;
    isrs[DMA2_Stream5_IRQHandler_Idx]       = (uint32_t)DMA2_Stream5_irq;
    isrs[RTC_Alarm_IRQHandler_Idx]          = (uint32_t)RTC_Alarm_irq;
    SCB->VTOR = (unsigned long)isrs;
}

static TaskHandle_t  app_thread_handle;
#define APPLICATION_STACK_SIZE 6144

/**
 * The mutex to ensure only one thread manipulates the heap at a given time.
 */
xSemaphoreHandle malloc_mutex = 0;

static void init_malloc_mutex(void)
{
    malloc_mutex = xSemaphoreCreateRecursiveMutex();
}

void __malloc_lock(void* ptr)
{
    if (malloc_mutex)
        while (!xSemaphoreTakeRecursive(malloc_mutex, 0xFFFFFFFF)) {}
}

void __malloc_unlock(void* ptr)
{
    if (malloc_mutex)
        xSemaphoreGiveRecursive(malloc_mutex);
}

/**
 * Called from startup_stm32f2xx.s at boot, main entry point.
 */
int main(void)
{
    init_malloc_mutex();
    xTaskCreate( application_start, "app_thread", APPLICATION_STACK_SIZE/sizeof( portSTACK_TYPE ), NULL, 2, &app_thread_handle);

    vTaskStartScheduler();


    uint32_t* isrs                          = (uint32_t*)&link_ram_interrupt_vectors_location;
    isrs[SysTick_Handler_Idx]               = (uint32_t)SysTick_Handler;

    /* we should never get here */
    while (1);

    return 0;
}


void vApplicationTickHook()
{
    SysTickOverride();
}

/**
 * Called at the beginning of app_setup_and_loop() from main.cpp to
 * pre-initialize any low level hardware before the main loop runs.
 */
void HAL_Core_Init(void)
{
}

void HAL_1Ms_Tick()
{
    if (TimingDelay != 0x00)
    {
        __sync_sub_and_fetch(&TimingDelay, 1);
    }
}

/**
 * Called by HAL_Core_Setup() to perform any post-setup config after the
 * watchdog has been disabled.
 */
void HAL_Core_Setup_finalize(void)
{
}

/******************************************************************************/
/*            Cortex-M Processor Exceptions Handlers                          */
/******************************************************************************/

void NMI_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

/*******************************************************************************
 * Function Name  : EXTI0_IRQHandler
 * Description    : This function handles EXTI0 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line0);

        HAL_EXTI_Handler(0);
    }
}

/*******************************************************************************
 * Function Name  : EXTI1_IRQHandler
 * Description    : This function handles EXTI1 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line1);

        HAL_EXTI_Handler(1);
    }
}

/*******************************************************************************
 * Function Name  : EXTI2_IRQHandler
 * Description    : This function handles EXTI2 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI2_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line2) != RESET)//BUTTON1_EXTI_LINE
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line2);//BUTTON1_EXTI_LINE

        HAL_EXTI_Handler(2);
    }
}

/*******************************************************************************
 * Function Name  : EXTI3_IRQHandler
 * Description    : This function handles EXTI3 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI3_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line3);

        HAL_EXTI_Handler(3);
    }
}

/*******************************************************************************
 * Function Name  : EXTI4_IRQHandler
 * Description    : This function handles EXTI4 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI4_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line4);

        HAL_EXTI_Handler(4);
    }
}

/*******************************************************************************
 * Function Name  : EXTI9_5_IRQHandler
 * Description    : This function handles EXTI9_5 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line5);

        HAL_EXTI_Handler(5);
    }

    if (EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line6);

        HAL_EXTI_Handler(6);
    }

    if (EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        Handle_Mode_Button_EXTI_irq();
    }

    if (EXTI_GetITStatus(EXTI_Line8) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line8);

        HAL_EXTI_Handler(8);
    }

    if (EXTI_GetITStatus(EXTI_Line9) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line9);

        HAL_EXTI_Handler(9);
    }
}

/*******************************************************************************
 * Function Name  : EXTI15_10_IRQHandler
 * Description    : This function handles EXTI15_10 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line10) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line10);

        HAL_EXTI_Handler(10);
    }

    if (EXTI_GetITStatus(EXTI_Line11) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line11);

        HAL_EXTI_Handler(11);
    }

    if (EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line12);

        HAL_EXTI_Handler(12);
    }

    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line13);

        HAL_EXTI_Handler(13);
    }

    if (EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line14);

        HAL_EXTI_Handler(14);
    }

    if (EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        /* Clear the EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line15);

        HAL_EXTI_Handler(15);
    }
}

/*** * * *
 * Typically without the following function definitions
 * these interrupts would be un-handled and default to Default_Handler
 * in the startup_stm32f2xx.S file; but now we've explicitly defined
 * them for debugging purposes to break on entry and let us know
 * which interrupt we are not currently handling.
 */
void WWDG_IRQHandler(void)          {__ASM("bkpt 0");}
void PVD_IRQHandler(void)           {__ASM("bkpt 0");}
void TAMP_STAMP_IRQHandler(void)    {__ASM("bkpt 0");}
void RTC_WKUP_IRQHandler(void)      {__ASM("bkpt 0");}
void FLASH_IRQHandler(void)         {__ASM("bkpt 0");}
void RCC_IRQHandler(void)           {__ASM("bkpt 0");}
void DMA1_Stream0_IRQHandler(void)  {__ASM("bkpt 0");}
void DMA1_Stream1_IRQHandler(void)  {__ASM("bkpt 0");}
void DMA1_Stream2_IRQHandler(void)  {__ASM("bkpt 0");}
void DMA1_Stream3_IRQHandler(void)  {__ASM("bkpt 0");}
void DMA1_Stream4_IRQHandler(void)  {__ASM("bkpt 0");}
void DMA1_Stream5_IRQHandler(void)  {__ASM("bkpt 0");}
void DMA1_Stream6_IRQHandler(void)  {__ASM("bkpt 0");}
void CAN1_TX_IRQHandler(void)       {__ASM("bkpt 0");}
void CAN1_RX0_IRQHandler(void)      {__ASM("bkpt 0");}
void CAN1_RX1_IRQHandler(void)      {__ASM("bkpt 0");}
void CAN1_SCE_IRQHandler(void)      {__ASM("bkpt 0");}
void I2C2_EV_IRQHandler(void)       {__ASM("bkpt 0");}
void I2C2_ER_IRQHandler(void)       {__ASM("bkpt 0");}
void SPI1_IRQHandler(void)          {__ASM("bkpt 0");}
void SPI2_IRQHandler(void)          {__ASM("bkpt 0");}
void RTC_Alarm_IRQHandler(void)     {__ASM("bkpt 0");}
void FSMC_IRQHandler(void)          {__ASM("bkpt 0");}
void SDIO_IRQHandler(void)          {__ASM("bkpt 0");}
void SPI3_IRQHandler(void)          {__ASM("bkpt 0");}
void DMA2_Stream0_IRQHandler(void)  {__ASM("bkpt 0");}
void DMA2_Stream1_IRQHandler(void)  {__ASM("bkpt 0");}
void DMA2_Stream2_IRQHandler(void)  {__ASM("bkpt 0");}
void DMA2_Stream3_IRQHandler(void)  {__ASM("bkpt 0");}
void DMA2_Stream4_IRQHandler(void)  {__ASM("bkpt 0");}
void ETH_IRQHandler(void)           {__ASM("bkpt 0");}
void ETH_WKUP_IRQHandler(void)      {__ASM("bkpt 0");}
void DMA2_Stream6_IRQHandler(void)  {__ASM("bkpt 0");}
void DMA2_Stream7_IRQHandler(void)  {__ASM("bkpt 0");}
void USART6_IRQHandler(void)        {__ASM("bkpt 0");}
void DCMI_IRQHandler(void)          {__ASM("bkpt 0");}
void CRYP_IRQHandler(void)          {__ASM("bkpt 0");}
void HASH_RNG_IRQHandler(void)      {__ASM("bkpt 0");}
