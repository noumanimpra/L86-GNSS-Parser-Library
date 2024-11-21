#include "l86_gnss_parser.h"

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
static S_GPS_L86_DATA gnss_data;
/* USER CODE END PV */

int main()
{
    UsrGpsL86Init(huart2);

    while (1)
    {
        Usr_GpsL86GetValues(&gnss_data);
    }
}