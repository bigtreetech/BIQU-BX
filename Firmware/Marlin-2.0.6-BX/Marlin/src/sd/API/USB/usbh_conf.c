/**
  ******************************************************************************
  * @file    usbh_conf.c
  * @author  MCD Application Team
  * @brief   This file implements the board support package for the USB host library
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbh_conf.h"
#include "STM32_USB_Host_Library/Core/Inc/usbh_core.h"

HCD_HandleTypeDef hhcd;
 
//初始化HSI48和CRS校准,选择HSI48作为USB时钟源(带CRS自动校准)
void USB_OTG_HSI48_CRS_Init(void)
{
	RCC->CR|=1<<12;				//HSI480N=1,使能HSI48
	while((RCC->CR&(1<<13))==0);//等待HSI48RDY=1,等待HSI48稳定
	RCC->APB1HENR|=1<<1;		//CRSEN=1,使能CRS
	RCC->APB1HRSTR|=1<<1;		//CRSRST=1,复位CRS
	RCC->APB1HRSTR&=~(1<<1);	//CRSRST=0,取消复位 
	CRS->CFGR&=~(3<<28);		//SYNCSRC[1:0]=0,选择USB2 SOF作为SYNC信号
	CRS->CR|=3<<5;				//CEN和AUTIOTRIMEN都为1,使能自动校准以及计数器 
	RCC->D2CCIP2R&=~(3<<20);	//USBSEL[1:0]=0,清零原来的设置.
	RCC->D2CCIP2R|=3<<20;		//USBSEL[1:0]=3,USB时钟源来自hsi48_ck.
}
 
//初始化PCD MSP
//hpcd:PCD结构体指针
//返回值:无
void HAL_HCD_MspInit(HCD_HandleTypeDef * hhcd)
{ 
	if(hhcd->Instance==USB2_OTG_FS)
	{
		// RCC->AHB4ENR|=1<<0;    				//使能PORTA时钟	     
		// RCC->AHB1ENR|=1<<27;    			//使能USB2 OTG时钟	
		// PWR->CR3|=1<<24;					//使能USB VDD3电压检测
		// USB_OTG_HSI48_CRS_Init();			//设置USB时钟来自hsi48_ck,使能CRS	
		// GPIO_Set(GPIOA,3<<11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_NONE);	//PA11/12复用功能输出
		// GPIO_AF_Set(GPIOA,11,10);			//PA11,AF10(USB)
		// GPIO_AF_Set(GPIOA,12,10);			//PA12,AF10(USB)  
		// MY_NVIC_Init(0,3,OTG_FS_IRQn,2);	//优先级设置为抢占0,子优先级3，组2	
	}else if (hhcd->Instance == USB1_OTG_HS)
	{
    GPIO_InitTypeDef GPIO_InitStruct;
		RCC->AHB4ENR|=1<<1;    				//使能PORTA时钟	     
		RCC->AHB1ENR|=1<<25;    			//使能USB2 OTG时钟	
		PWR->CR3|=1<<24;					//使能USB VDD3电压检测
		USB_OTG_HSI48_CRS_Init();			//设置USB时钟来自hsi48_ck,使能CRS	
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = (GPIO_PIN_14 | GPIO_PIN_15);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_OTG1_HS;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_RCC_USB1_OTG_HS_CLK_ENABLE();
    HAL_NVIC_SetPriority(OTG_HS_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
		// GPIO_Set(GPIOB,3<<14,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_NONE);	//PA11/12复用功能输出
		// GPIO_AF_Set(GPIOB,14,10);			//PA11,AF10(USB)
		// GPIO_AF_Set(GPIOB,15,10);			//PA12,AF10(USB)  
		// MY_NVIC_Init(0,3,OTG_HS_IRQn,2);	//优先级设置为抢占0,子优先级3，组2	
		//USB1 OTG本例程没用到,故不做处理
	}
} 

//USB OTG 中断服务函数
//处理所有USB中断
void OTG_HS_IRQHandler(void)
{
	HAL_HCD_IRQHandler(&hhcd);	
}


///////////////////////USBH LL HCD驱动回调函数(HCD->USB Host Library)///////////////////////

//USBH SOF回调函数
//hhcd:HCD结构体指针
//返回值:无
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef * hhcd)
{
	USBH_LL_IncTimer(hhcd->pData);
}

//USBH 连接成功回调函数
//hhcd:HCD结构体指针
//返回值:无
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef * hhcd)
{
	USBH_DbgLog("Connected!\r\n\r\n");	
	USBH_LL_Connect(hhcd->pData);
}

//USBH 断开连接回调函数
//hhcd:HCD结构体指针
//返回值:无
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef * hhcd)
{
	USBH_DbgLog("Disconnected!\r\n\r\n");	
	USBH_LL_Disconnect(hhcd->pData);
}

void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd)
{
	USBH_DbgLog("Prot enabled!\r\n\r\n");	
  USBH_LL_PortEnabled(hhcd->pData);
}

void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd)
{
	USBH_DbgLog("Prot disabled!\r\n\r\n");	
  USBH_LL_PortDisabled(hhcd->pData);
}

//USBH 通知URB变化回调函数
//hhcd:HCD结构体指针
//chnum:端点编号
//urb_state:URB状态
//返回值:无
void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef * hhcd,uint8_t chnum,HCD_URBStateTypeDef urb_state)
{
 	//To be used with OS to sync URB state with the global state machine */
}

/**
  * @brief  USBH_LL_Init
  *         Initialize the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_LL_Init(USBH_HandleTypeDef *phost)
{    
  #ifdef USE_USB_FS
    //设置LL驱动相关参数
    hhcd.Instance = USB1_OTG_HS;			//使用USB2 OTG
    hhcd.Init.Host_channels = 11;			//主机通道数为11个
    hhcd.Init.dma_enable = 0;				//不使用DMA
    hhcd.Init.low_power_enable = 0;			//不使能低功耗模式
    hhcd.Init.phy_itface = HCD_PHY_EMBEDDED;//使用内部PHY
    hhcd.Init.Sof_enable = 0;				//禁止SOF中断
    hhcd.Init.speed = HCD_SPEED_FULL;		//USB全速(12Mbps)
    hhcd.Init.vbus_sensing_enable = 0;		//不使能VBUS检测
    hhcd.Init.lpm_enable = 0;				//使能连接电源管理
  
    hhcd.pData = phost;						//hhcd的pData指向phost
    phost->pData = &hhcd;					//phost的pData指向hhcd 
    
    HAL_HCD_Init(&hhcd);					//初始化LL驱动
  #endif

  #ifdef USE_USB_HS 
    //未实现
  #endif       
	USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(&hhcd));
	return USBH_OK;

  return USBH_OK;
}

/**
  * @brief  USBH_LL_DeInit
  *         De-Initialize the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_LL_DeInit(USBH_HandleTypeDef *phost)
{  
	HAL_HCD_DeInit(phost->pData);

  return USBH_OK;
}

/**
  * @brief  USBH_LL_Start
  *         Start the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_LL_Start(USBH_HandleTypeDef *phost)
{
  HAL_HCD_Start(phost->pData);

  return USBH_OK;
}

/**
  * @brief  USBH_LL_Stop
  *         Stop the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_LL_Stop(USBH_HandleTypeDef *phost)
{
  HAL_HCD_Stop(phost->pData);

  return USBH_OK;
}

/**
  * @brief  USBH_LL_GetSpeed
  *         Return the USB Host Speed from the Low Level Driver.
  * @param  phost: Host handle
  * @retval USBH Speeds
  */
USBH_SpeedTypeDef USBH_LL_GetSpeed(USBH_HandleTypeDef *phost)
{
  USBH_SpeedTypeDef speed = USBH_SPEED_FULL;
  switch (HAL_HCD_GetCurrentSpeed(phost->pData))
  {
    case 0:
      speed = USBH_SPEED_HIGH;
      USBH_DbgLog("USB Host [HS]\r\n");
      break;
    case 1:
      speed = USBH_SPEED_FULL;
      USBH_DbgLog("USB Host [FS]\r\n");
      break;
    case 2:
      speed = USBH_SPEED_LOW;
      USBH_DbgLog("USB Host [LS]\r\n");
      break;
    default:
      speed = USBH_SPEED_FULL;
      USBH_DbgLog("USB Host [FS]\r\n");
      break;
  }

  return speed;
}

/**
  * @brief  USBH_LL_ResetPort
  *         Reset the Host Port of the Low Level Driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_ResetPort(USBH_HandleTypeDef *phost)
{
  HAL_HCD_ResetPort(phost->pData);
  USBH_DbgLog("USB Reset Port\r\n");

  return USBH_OK;
}

/**
  * @brief  USBH_LL_GetLastXferSize
  *         Return the last transferred packet size.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval Packet Size
  */
uint32_t USBH_LL_GetLastXferSize(USBH_HandleTypeDef *phost, uint8_t pipe)
{
  return HAL_HCD_HC_GetXferCount(phost->pData, pipe);
}

/**
  * @brief  USBH_LL_OpenPipe
  *         Open a pipe of the Low Level Driver.
  * @param  phost: Host handle
  * @param  pipe_num: Pipe index
  * @param  epnum: Endpoint Number
  * @param  dev_address: Device USB address
  * @param  speed: Device Speed
  * @param  ep_type: Endpoint Type
  * @param  mps: Endpoint Max Packet Size
  * @retval USBH Status
  */
USBH_StatusTypeDef   USBH_LL_OpenPipe(USBH_HandleTypeDef *phost,
                                      uint8_t pipe_num,
                                      uint8_t epnum,
                                      uint8_t dev_address,
                                      uint8_t speed,
                                      uint8_t ep_type,
                                      uint16_t mps)
{
  HAL_HCD_HC_Init(phost->pData, pipe_num, epnum, dev_address, speed, ep_type, mps);
	return USBH_OK;
}

/**
  * @brief  USBH_LL_ClosePipe
  *         Close a pipe of the Low Level Driver.
  * @param  phost: Host handle
  * @param  pipe_num: Pipe index
  * @retval USBH Status
  */
USBH_StatusTypeDef   USBH_LL_ClosePipe(USBH_HandleTypeDef *phost, uint8_t pipe)
{
  HAL_HCD_HC_Halt(phost->pData, pipe);
	return USBH_OK;
}

/**
  * @brief  USBH_LL_SubmitURB
  *         Submit a new URB to the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  *         This parameter can be a value from 1 to 15
  * @param  direction : Channel number
  *          This parameter can be one of the these values:
  *           0 : Output
  *           1 : Input
  * @param  ep_type : Endpoint Type
  *          This parameter can be one of the these values:
  *            @arg EP_TYPE_CTRL: Control type
  *            @arg EP_TYPE_ISOC: Isochronous type
  *            @arg EP_TYPE_BULK: Bulk type
  *            @arg EP_TYPE_INTR: Interrupt type
  * @param  token : Endpoint Type
  *          This parameter can be one of the these values:
  *            @arg 0: PID_SETUP
  *            @arg 1: PID_DATA
  * @param  pbuff : pointer to URB data
  * @param  length : Length of URB data
  * @param  do_ping : activate do ping protocol (for high speed only)
  *          This parameter can be one of the these values:
  *           0 : do ping inactive
  *           1 : do ping active
  * @retval Status
  */

USBH_StatusTypeDef   USBH_LL_SubmitURB(USBH_HandleTypeDef *phost,
                                       uint8_t pipe,
                                       uint8_t direction,
                                       uint8_t ep_type,
                                       uint8_t token,
                                       uint8_t *pbuff,
                                       uint16_t length,
                                       uint8_t do_ping)
{
  HAL_HCD_HC_SubmitRequest(phost->pData,pipe,direction, ep_type, token, pbuff, length, do_ping);
	return USBH_OK;
}

/**
  * @brief  USBH_LL_GetURBState
  *         Get a URB state from the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  *         This parameter can be a value from 1 to 15
  * @retval URB state
  *          This parameter can be one of the these values:
  *            @arg URB_IDLE
  *            @arg URB_DONE
  *            @arg URB_NOTREADY
  *            @arg URB_NYET
  *            @arg URB_ERROR
  *            @arg URB_STALL
  */
USBH_URBStateTypeDef  USBH_LL_GetURBState(USBH_HandleTypeDef *phost, uint8_t pipe)
{
  return (USBH_URBStateTypeDef) HAL_HCD_HC_GetURBState(phost->pData, pipe);
}

/**
  * @brief  USBH_LL_DriverVBUS
  *         Drive VBUS.
  * @param  phost: Host handle
  * @param  state : VBUS state
  *          This parameter can be one of the these values:
  *           0 : VBUS Active
  *           1 : VBUS Inactive
  * @retval Status
  */

USBH_StatusTypeDef  USBH_LL_DriverVBUS(USBH_HandleTypeDef *phost, uint8_t state)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(phost);
  UNUSED(state);

  return USBH_OK;
}

/**
  * @brief  USBH_LL_SetToggle
  *         Set toggle for a pipe.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @param  pipe_num: Pipe index
  * @param  toggle: toggle (0/1)
  * @retval Status
  */
USBH_StatusTypeDef   USBH_LL_SetToggle(USBH_HandleTypeDef *phost, uint8_t pipe, uint8_t toggle)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(phost);
  UNUSED(pipe);

  if (hhcd.hc[pipe].ep_is_in)
  {
    hhcd.hc[pipe].toggle_in = toggle;
  }
  else
  {
    hhcd.hc[pipe].toggle_out = toggle;
  }

  return USBH_OK;
}

/**
  * @brief  USBH_LL_GetToggle
  *         Return the current toggle of a pipe.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval toggle (0/1)
  */
uint8_t  USBH_LL_GetToggle(USBH_HandleTypeDef *phost, uint8_t pipe)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(phost);
  UNUSED(pipe);

  uint8_t toggle = 0U;
  if (hhcd.hc[pipe].ep_is_in)
  {
    toggle = hhcd.hc[pipe].toggle_in;
  }
  else
  {
    toggle = hhcd.hc[pipe].toggle_out;
  }
  return toggle;
}
/**
  * @brief  USBH_Delay
  *         Delay routine for the USB Host Library
  * @param  Delay: Delay in ms
  * @retval None
  */
void  USBH_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
