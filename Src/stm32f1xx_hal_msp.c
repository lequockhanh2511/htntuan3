#include "main.h"

void HAL_MspInit(void)
{
  __HAL_RCC_AFIO_CLK_ENABLE();
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (huart->Instance != USARTx)
  {
    return;
  }

  USARTx_GPIO_CLK_ENABLE();
  USARTx_CLK_ENABLE();

  GPIO_InitStruct.Pin   = USARTx_TX_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin  = USARTx_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStruct);

  hdma_usart1_tx.Instance                 = USARTx_TX_DMA_CHANNEL;
  hdma_usart1_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_usart1_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_usart1_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_usart1_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_usart1_tx.Init.Mode                = DMA_NORMAL;
  hdma_usart1_tx.Init.Priority            = DMA_PRIORITY_MEDIUM;

  if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(huart, hdmatx, hdma_usart1_tx);

  HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  if (huart->Instance != USARTx)
  {
    return;
  }

  __HAL_RCC_USART1_CLK_DISABLE();

  HAL_GPIO_DeInit(USARTx_GPIO_PORT, USARTx_TX_PIN);
  HAL_GPIO_DeInit(USARTx_GPIO_PORT, USARTx_RX_PIN);

  HAL_DMA_DeInit(huart->hdmatx);

  HAL_NVIC_DisableIRQ(USART1_IRQn);
}
