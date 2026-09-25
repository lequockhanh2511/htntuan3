#include "main.h"
#include <stdio.h>
#include <string.h>

UART_HandleTypeDef huart1;
DMA_HandleTypeDef  hdma_usart1_tx;

static volatile uint8_t  btn_pressed_flag = 0;
static volatile uint32_t btn_count        = 0;
static volatile uint8_t  uart_tx_busy     = 0;
static uint8_t           tx_buf[64];

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();

  while (1)
  {
    if (btn_pressed_flag)
    {
      btn_pressed_flag = 0;
      btn_count++;

      if (!uart_tx_busy)
      {
        int len = snprintf((char *)tx_buf, sizeof(tx_buf),
                            "%s%s:BTN:%lu\n\r",
                            ID_LOP, ID_NHOM, (unsigned long)btn_count);

        if (len > 0)
        {
          uart_tx_busy = 1;
          if (HAL_UART_Transmit_DMA(&huart1, tx_buf, (uint16_t)len) != HAL_OK)
          {
            uart_tx_busy = 0;
          }
        }
      }
    }
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    uart_tx_busy = 0;
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static uint32_t last_tick = 0;
  uint32_t now = HAL_GetTick();

  if (GPIO_Pin == BTN_PIN)
  {
    if ((now - last_tick) > 50)
    {
      last_tick = now;
      btn_pressed_flag = 1;
    }
  }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  GPIO_InitStruct.Pin  = BTN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BTN_GPIO_PORT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(BTN_EXTI_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(BTN_EXTI_IRQn);
}

static void MX_DMA_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();

  HAL_NVIC_SetPriority(USARTx_TX_DMA_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USARTx_TX_DMA_IRQn);
}

static void MX_USART1_UART_Init(void)
{
  huart1.Instance         = USARTx;
  huart1.Init.BaudRate     = 115200;
  huart1.Init.WordLength   = UART_WORDLENGTH_8B;
  huart1.Init.StopBits     = UART_STOPBITS_1;
  huart1.Init.Parity       = UART_PARITY_NONE;
  huart1.Init.Mode         = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK   | RCC_CLOCKTYPE_SYSCLK |
                                 RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, 2U) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
