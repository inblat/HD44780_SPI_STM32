/*
 * main.c
 *
 *  Created on: 23 февр. 2019 г.
 *      Author: admin
 */
#include "stm32f4xx.h"
#include "main.h"
#include "hd44780.h"

/*const*/ uint8_t chess[8] = {0x06u, 0x06u, 0x19u, 0x19u, 0x06u, 0x06u, 0x19u, 0x19u};

static volatile uint16_t hTimebase_1ms = 0;

// Prototypes
void TB_Wait(uint32_t time);

// Functions
void TB_Wait(uint32_t time)
{
	hTimebase_1ms = time;    // delay = 'time' value * 5ms
	while (hTimebase_1ms != 0){} // wait and do nothing!
}

void lcd_us_delay(volatile uint32_t us)
{
	/* Сonversion to us */
	us *= MCU_FREQ_VALUE;
	/* Wait */
	while (us > 0u)
	{
		us--;
	}
}

int main(void)
{
	uint8_t data = 0;

	clockConfig();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);

//-------------------------------
// DEMO
//-------------------------------
	uint8_t i, j, Flag, progress = 0u;

	/* Initialize LCD */
	lcd_init();
	/* Welcome message */
	lcd_puts("LCDlib by GrAnd\n");	/* USE_FORMATTED_OUTPUT = 1 */

	// Display a numbers
	lcd_ntos(7825u, 2u); // Display only 2 last digits
	lcd_putc('-');
	lcd_ntos(9u, 2u); // Insert a "0"
	lcd_itos(-13);
	lcd_us_delay(300000ul);
	lcd_backspace();	// Correct the year
	lcd_us_delay(300000ul);
	lcd_putc('4');
	lcd_us_delay(300000ul);
	cursor_shift(RIGHT);	// cursor shift
	lcd_ftos(3.1415, 4u);	// display floating number
	lcd_us_delay(300000ul);

	// Scroll the LCD display
	Flag = 0u;
	for (i = 0u; i<2u; i++)
	{
		for (j = 0u; j<16u; j++)
		{
			if (Flag)
			{
				lcd_scroll(LEFT);
			}
			else
			{
				lcd_scroll(RIGHT);
			}

			lcd_us_delay(50000ul);
		}

		Flag = 1u;
	}

	// Load a user-defined symbol into CGRAM memory.
	lcd_loadchar(chess, 6u);
	lcd_goto(LCD_2nd_LINE, 8u);
	// Display the CGRAM content (only the first 6 symbols)
	lcd_putc(0u);
	lcd_putc(1u);
	lcd_putc(2u);
	lcd_putc(3u);
	lcd_putc(4u);
	lcd_putc(5u);
	lcd_putc(6u);
	lcd_us_delay(300000ul);

	// Progress bar
	lcd_clrscr();
	lcd_return();
	lcd_puts("\tLoading...");
	lcd_goto(PROGRESS_BAR_LINE, PROGRESS_BAR_WIDTH); // Переход на первую ячейку после индикатора выполнения
	lcd_puts("[   %]");

	while(1)
	{
		lcd_drawbar(progress); // Display the progress bar
		lcd_goto(PROGRESS_BAR_LINE, PROGRESS_BAR_WIDTH + 1u);
		lcd_ntos((progress*100u)/PROGRESS_BAR_MAX_LOAD, 3u);

		progress++;
		if (progress > PROGRESS_BAR_MAX_LOAD)
		{
			progress = 0u;
		}

		lcd_us_delay(50000ul); // 0.5 seconds

	}
}

void SysTick_Handler(void)
{

	if (hTimebase_1ms != 0)
	{
		hTimebase_1ms --;
	}
}
