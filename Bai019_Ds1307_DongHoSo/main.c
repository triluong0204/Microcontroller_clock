#include "main.h"
#include "..\lib\Soft_I2c.h"
#include "..\lib\Lcd4.h"
#include "..\lib\Rtc_Ds1307.h"
#include "Port.h"

#define LED P2_1
#define LED1 P3_7

unsigned char *code Days[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

void delay_ms1(unsigned int time)
{
	unsigned int x, y;
	for (x = 0; x < time; x++)
		for (y = 0; y < 125; y++)
		{
		}
}

void delay_ms_CL(int ms)
{
	while (ms--)
	{
		TMOD = 0x01;
		TH0 = 0xfc;
		TL0 = 0x18;
		TR0 = 1;
		while (!TF0)
			;
		TF0 = 0;
		TR0 = 0;
	}
}

void timer_delay() /* Timer0 delay function */
{
	TH0 = 0xFC; /* Load higher 8-bit in TH0 */
	TL0 = 0x74; /* Load lower 8-bit in TL0 */
	TR0 = 1;	/* Start timer0 */
	while (TF0 == 0)
		;	 /* Wait until timer0 flag set */
	TR0 = 0; /* Stop timer0 */
	TF0 = 0; /* Clear timer0 flag */
}

unsigned int count;
unsigned int mode_clock;
unsigned int temp;
unsigned int Hour_setup;
unsigned int Minute_setup;
unsigned int demc;
unsigned int hi;

void Delay_ngao(unsigned int Time)
{
	while (Time--)
		;
}
//-----------------
void print_LCD(unsigned char Hour,
			   unsigned char Minute,
			   unsigned char Second,
			   unsigned char Day,
			   unsigned char Date,
			   unsigned char Month,
			   unsigned char Year)
{
	Lcd_Chr(1, 5, Hour / 10 + 0x30);
	Lcd_Chr_Cp(Hour % 10 + 0x30);
	Lcd_Chr_Cp(':');
	Lcd_Chr_Cp(Minute / 10 + 0x30);
	Lcd_Chr_Cp(Minute % 10 + 0x30);
	Lcd_Chr_Cp(':');
	Lcd_Chr_Cp(Second / 10 + 0x30);
	Lcd_Chr_Cp(Second % 10 + 0x30);
	Lcd_Out(2, 1, "                ");
	Lcd_Out(2, 2, Days[Day - 1]);
	Lcd_Chr_Cp(' ');
	Lcd_Chr_Cp(Date / 10 + 0x30);
	Lcd_Chr_Cp(Date % 10 + 0x30);
	Lcd_Chr_Cp('/');
	Lcd_Chr_Cp(Month / 10 + 0x30);
	Lcd_Chr_Cp(Month % 10 + 0x30);
	Lcd_Out_Cp("/20");
	Lcd_Chr_Cp(Year / 10 + 0x30);
	Lcd_Chr_Cp(Year % 10 + 0x30);
}

void Baothuc(unsigned char Hour_setup,
			 unsigned char Minute_setup,
			 unsigned char Hour,
			 unsigned char Minute)
{
	if (Hour_setup == Hour)
		if (Minute_setup == Minute)
			LED1 = 1;
		else
			LED1 = 0;
}

void Up(void) interrupt 0 //Khai báo trình phục vụ ngắt ngoài 1
{
	mode_clock++;
}

void main()
{
	unsigned char Hour, Minute, Second, Mode, Day, Date, Month, Year, old_second;
	unsigned char Hou_CL, Minute_CL, Second_CL;
	bit am_pm;
	BTN_MODE = 1;
	EA = 1;
	IT0 = 1;
	EX0 = 1;
	Soft_I2c_Init();
	Ds1307_Init();
	Lcd_Init();
	mode_clock = 0;
	count = 0;
	temp = 0;
	Hou_CL = 0;
	Minute_CL = 0;
	Second_CL = 0;
	TMOD = 0x01;
	//-------------------
	//Ds1307_Write_Time(10, 10, 10, 12, 0);
	//Ds1307_Init();
	// nạp vào xong phải khởi tạo lại

	while (1)
	{
		//LED = 1;
		// doc va hien thi gio
		am_pm = Ds1307_Read_Time(&Hour, &Minute, &Second, &Mode);
		Ds1307_Read_Date(&Day, &Date, &Month, &Year);
		if (old_second != Second)
		{
			old_second = Second;
			if (Mode == 12)
			{
				if (am_pm == 1)
					Lcd_Out(1, 14, "PM");
				else
					Lcd_Out(1, 14, "AM");
			}
			else
				Lcd_Cmd(_LCD_CLEAR);
			print_LCD(Hour, Minute, Second, Day, Date, Month, Year);
		}
		LED = 0;

		//Lcd_Cmd(15);
		while (mode_clock > 0)
		{
			LED = 1;
			switch (mode_clock)
			{
			case 1:
				Lcd_Out(1, 11, "  ");
				timer_delay();

				Lcd_Chr(1, 11, Second / 10 + 0x30);
				Lcd_Chr_Cp(Second % 10 + 0x30);
				timer_delay();
				if (BTN_MOVE == 0)
				{
					//Lcd_Cmd(15);
					delay_ms1(15);
					do
					{
					} while (BTN_MOVE == 0);
					count = 1;
					while (mode_clock == 1)
					{
						if (BTN_MOVE == 0)
						{
							delay_ms1(15);
							if (BTN_MOVE == 0)
								count = count + 1;
							do
							{
							} while (BTN_MOVE == 0);
						}
						switch (count)
						{
						case 2:
							while (count == 2 && mode_clock == 1)
							{
								char old_hour = Hour;
								if (BTN_MOVE == 0)
								{
									delay_ms1(15);
									if (BTN_MOVE == 0)
										count = count + 1;
									do
									{
									} while (BTN_MOVE == 0);
								}
								else if (BTN_DOWN == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_DOWN == 0);
									if ((Hour == 0) && (Mode == 12))
										Hour = 12;
									if ((Hour == 0) && (Mode == 24))
										Hour = 24;
									Hour--;
								}
								else if (BTN_UP == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_UP == 0);
									if ((Hour == 11) && (Mode == 12))
										Hour = -1;
									if ((Hour == 23) && (Mode == 24))
										Hour = -1;
									Hour++;
								}
								else
								{
									Lcd_Out(1, 5, "  ");
									timer_delay();

									Lcd_Chr(1, 5, Hour / 10 + 0x30);
									Lcd_Chr_Cp(Hour % 10 + 0x30);
									timer_delay();
								}
								if (old_hour != Hour)
								{
									old_hour = Hour;
									print_LCD(Hour, Minute, Second, Day, Date, Month, Year);
								}
								if (count != 2)
									break;
							}
							break;
						case 1:
							while (count == 1 && mode_clock == 1)
							{
								char old_minute = Minute;
								if (BTN_MOVE == 0)
								{
									delay_ms1(15);
									if (BTN_MOVE == 0)
										count = count + 1;
									do
									{
									} while (BTN_MOVE == 0);
								}
								else if (BTN_DOWN == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_DOWN == 0);
									if (Minute == 0)
										Minute = 60;
									Minute--;
								}
								else if (BTN_UP == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_UP == 0);
									if (Minute == 59)
										Minute = -1;
									Minute++;
								}
								else
								{
									Lcd_Out(1, 8, "  ");
									timer_delay();

									Lcd_Chr(1, 8, Minute / 10 + 0x30);
									Lcd_Chr_Cp(Minute % 10 + 0x30);
									timer_delay();
								}
								if (old_minute != Minute)
								{
									old_minute = Minute;
									print_LCD(Hour, Minute, Second, Day, Date, Month, Year);
								}
								if (count != 1)
									break;
							}
							break;
						// case 3:
						// 	Lcd_Out(1, 11, "__");
						// 	Delay_ngao(30000);

						// 	Lcd_Chr(1, 11, Second / 10 + 0x30);
						// 	Lcd_Chr_Cp(Second % 10 + 0x30);
						// 	Delay_ngao(30000);
						// 	while (count == 3 && mode_clock == 1)
						// 	{
						// 		char old_sec = Second;
						// 		if (BTN_MOVE == 0)
						// 		{
						// 			delay_ms1(15);
						// 			if (BTN_MOVE == 0)
						// 				count = count + 1;
						// 			do
						// 			{
						// 			} while (BTN_MOVE == 0);
						// 		}
						// 		else if (BTN_DOWN == 0)
						// 		{
						// 			delay_ms1(15);
						// 			do
						// 			{
						// 			} while (BTN_DOWN == 0);
						// 			if (Second == 0)
						// 				Second = 60;
						// 			Second--;
						// 		}
						// 		else if (BTN_UP == 0)
						// 		{
						// 			delay_ms1(15);
						// 			do
						// 			{
						// 			} while (BTN_UP == 0);
						// 			if (Second == 59)
						// 				Second = -1;
						// 			Second++;
						// 		}
						// 		if (old_sec != Second)
						// 		{
						// 			old_sec = Second;
						// 			print_LCD(Hour, Minute, Second, Day, Date, Month, Year);
						// 		}
						// 		if (count != 3)
						// 			break;
						// 	}
						// 	break;
						case 3:
							while (count == 3 && mode_clock == 1)
							{
								char old_day = Day;
								if (BTN_MOVE == 0)
								{
									delay_ms1(15);
									if (BTN_MOVE == 0)
										count = count + 1;
									do
									{
									} while (BTN_MOVE == 0);
								}
								else if (BTN_DOWN == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_DOWN == 0);
									if (Day == 1)
										Day = 8;
									Day--;
								}
								else if (BTN_UP == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_UP == 0);
									if (Day == 7)
										Day = 0;
									Day++;
								}
								else
								{
									Lcd_Out(2, 2, "   ");
									timer_delay();

									Lcd_Out(2, 2, Days[Day - 1]);
									Lcd_Chr_Cp(' ');
									timer_delay();
								}
								if (old_day != Day)
								{
									old_day = Day;
									print_LCD(Hour, Minute, Second, Day, Date, Month, Year);
								}
								if (count != 3)
									break;
							}
							break;
						case 4:
							while (count == 4 && mode_clock == 1)
							{
								char old_date = Date;
								if (BTN_MOVE == 0)
								{
									delay_ms1(15);
									if (BTN_MOVE == 0)
										count = count + 1;
									do
									{
									} while (BTN_MOVE == 0);
								}
								else if (BTN_DOWN == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_DOWN == 0);
									if (Date == 1)
										Date = 32;
									Date--;
								}
								else if (BTN_UP == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_UP == 0);
									if (Date == 31)
										Date = 0;
									Date++;
								}
								else
								{
									Lcd_Out(2, 6, "  ");
									timer_delay();

									Lcd_Chr(2, 6, Date / 10 + 0x30);
									Lcd_Chr_Cp(Date % 10 + 0x30);
									timer_delay();
								}
								if (old_date != Date)
								{
									old_date = Date;
									print_LCD(Hour, Minute, Second, Day, Date, Month, Year);
								}
								if (count != 4)
									break;
							}
							break;
						case 5:
							while (count == 5 && mode_clock == 1)
							{
								char old_month = Month;
								if (BTN_MOVE == 0)
								{
									delay_ms1(15);
									if (BTN_MOVE == 0)
										count = count + 1;
									do
									{
									} while (BTN_MOVE == 0);
								}
								else if (BTN_DOWN == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_DOWN == 0);
									if (Month == 1)
										Month = 13;
									Month--;
								}
								else if (BTN_UP == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_UP == 0);
									if (Month == 12)
										Month = 0;
									Month++;
								}
								else
								{
									Lcd_Out(2, 9, "  ");
									timer_delay();

									Lcd_Chr(2, 9, Month / 10 + 0x30);
									Lcd_Chr_Cp(Month % 10 + 0x30);
									timer_delay();
								}
								if (old_month != Month)
								{
									old_month = Month;
									print_LCD(Hour, Minute, Second, Day, Date, Month, Year);
								}
								if (count != 5)
									break;
							}
							break;
						case 6:
							while (count == 6 && mode_clock == 1)
							{
								char old_year = Year;
								if (BTN_MOVE == 0)
								{
									delay_ms1(15);
									if (BTN_MOVE == 0)
										count = count + 1;
									do
									{
									} while (BTN_MOVE == 0);
								}
								else if (BTN_DOWN == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_DOWN == 0);
									Year--;
								}
								else if (BTN_UP == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_UP == 0);
									Year++;
								}
								else
								{
									Lcd_Out(2, 12, "    ");
									timer_delay();
									Lcd_Out(2, 12, "20");
									Lcd_Chr(2, 14, Year / 10 + 0x30);
									//Lcd_Chr_Cp(Year / 10 + 0x30);
									Lcd_Chr_Cp(Year % 10 + 0x30);
									timer_delay();
								}
								if (old_year != Year)
								{
									old_year = Year;
									print_LCD(Hour, Minute, Second, Day, Date, Month, Year);
								}
								if (count != 6)
									break;
							}
							break;
						default:
							break; // Error handling
						}
						if (count >= 7)
						{
							mode_clock++;
							// Ds1307_Write_Time(Hour, Minute, Second, Mode, am_pm);
							// Ds1307_Write_Date(Day, Date, Month, Year);
							// Ds1307_Init();
							break;
						}
					}
					if (mode_clock != 1)
						break;
				}
				break;
				//while(mode_clock == 1){}
			case 2:
				Lcd_Cmd(1);
				Lcd_Out(1, 1, "CAI DAT BAO THUC");
				delay_ms1(15);
				if (BTN_MOVE == 0)
				{
					// Lcd_Cmd(1);
					// Lcd_Out(1, 1, "CAI DAT BAO THUC");
					// Lcd_Chr(2, 6, 0 / 10 + 0x30);
					// Lcd_Chr_Cp(0 % 10 + 0x30);
					// Lcd_Chr_Cp(':');
					// Lcd_Chr_Cp(0 / 10 + 0x30);
					// Lcd_Chr_Cp(0 % 10 + 0x30);
					delay_ms1(15);
					temp = 1;
					do
					{
					} while (BTN_MOVE == 0);
					Hour_setup = 0;
					Minute_setup = 0;
					while (mode_clock == 2)
					{
						if (BTN_MOVE == 0)
						{
							delay_ms1(15);
							if (BTN_MOVE == 0)
								temp = temp + 1;
							do
							{
							} while (BTN_MOVE == 0);
						}

						switch (temp)
						{
						case 1:
							Lcd_Out(2, 6, "  ");
							Lcd_Chr_Cp(':');
							Lcd_Chr_Cp(Minute_setup / 10 + 0x30);
							Lcd_Chr_Cp(Minute_setup % 10 + 0x30);
							Delay_ngao(30000);

							Lcd_Chr(2, 6, Hour_setup / 10 + 0x30);
							Lcd_Chr_Cp(Hour_setup % 10 + 0x30);
							Lcd_Chr_Cp(':');
							Lcd_Chr_Cp(Minute_setup / 10 + 0x30);
							Lcd_Chr_Cp(Minute_setup % 10 + 0x30);
							Delay_ngao(30000);
							Lcd_Out(1, 1, "CAI DAT BAO THUC");
							while (temp == 1 && mode_clock == 2)
							{
								char old_Hour_setup = Hour_setup;
								if (BTN_MOVE == 0)
								{
									delay_ms1(15);
									if (BTN_MOVE == 0)
										temp = temp + 1;
									do
									{
									} while (BTN_MOVE == 0);
								}
								else if (BTN_DOWN == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_DOWN == 0);
									if ((Hour_setup == 0) && (Mode == 12))
										Hour_setup = 12;
									if ((Hour_setup == 0) && (Mode == 24))
										Hour_setup = 24;
									Hour_setup--;
								}
								else if (BTN_UP == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_UP == 0);
									if ((Hour_setup == 11) && (Mode == 12))
										Hour_setup = -1;
									if ((Hour_setup == 23) && (Mode == 24))
										Hour_setup = -1;
									Hour_setup++;
								}
								else
								{
									Lcd_Out(2, 6, "  ");
									Lcd_Chr_Cp(':');
									Lcd_Chr_Cp(Minute_setup / 10 + 0x30);
									Lcd_Chr_Cp(Minute_setup % 10 + 0x30);
									timer_delay();

									Lcd_Chr(2, 6, Hour_setup / 10 + 0x30);
									Lcd_Chr_Cp(Hour_setup % 10 + 0x30);
									Lcd_Chr_Cp(':');
									Lcd_Chr_Cp(Minute_setup / 10 + 0x30);
									Lcd_Chr_Cp(Minute_setup % 10 + 0x30);
									timer_delay();
								}
								if (old_Hour_setup != Hour_setup)
								{
									old_Hour_setup = Hour_setup;
									Lcd_Chr(2, 6, Hour_setup / 10 + 0x30);
									Lcd_Chr_Cp(Hour_setup % 10 + 0x30);
									Lcd_Chr_Cp(':');
									Lcd_Chr_Cp(Minute_setup / 10 + 0x30);
									Lcd_Chr_Cp(Minute_setup % 10 + 0x30);
								}
							}
							break;
						case 2:
							Lcd_Chr(2, 6, Hour_setup / 10 + 0x30);
							Lcd_Chr_Cp(Hour_setup % 10 + 0x30);
							Lcd_Chr_Cp(':');
							Lcd_Out(2, 9, "  ");
							Delay_ngao(30000);

							Lcd_Chr(2, 6, Hour_setup / 10 + 0x30);
							Lcd_Chr_Cp(Hour_setup % 10 + 0x30);
							Lcd_Chr_Cp(':');
							Lcd_Chr_Cp(Minute_setup / 10 + 0x30);
							Lcd_Chr_Cp(Minute_setup % 10 + 0x30);
							Delay_ngao(30000);
							Lcd_Out(1, 1, "CAI DAT BAO THUC");
							while (temp == 2 && mode_clock == 2)
							{
								char old_Minute_setup = Minute_setup;
								if (BTN_MOVE == 0)
								{
									delay_ms1(15);
									if (BTN_MOVE == 0)
										temp = temp + 1;
									do
									{
									} while (BTN_MOVE == 0);
								}
								else if (BTN_DOWN == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_DOWN == 0);
									if (Minute_setup == 0)
										Minute_setup = 60;
									Minute_setup--;
								}
								else if (BTN_UP == 0)
								{
									delay_ms1(15);
									do
									{
									} while (BTN_UP == 0);
									if (Minute_setup == 59)
										Minute_setup = -1;
									Minute_setup++;
								}
								else
								{
									Lcd_Chr(2, 6, Hour_setup / 10 + 0x30);
									Lcd_Chr_Cp(Hour_setup % 10 + 0x30);
									Lcd_Chr_Cp(':');
									Lcd_Out(2, 9, "  ");
									timer_delay();

									Lcd_Chr(2, 6, Hour_setup / 10 + 0x30);
									Lcd_Chr_Cp(Hour_setup % 10 + 0x30);
									Lcd_Chr_Cp(':');
									Lcd_Chr(2, 9, Minute_setup / 10 + 0x30);
									Lcd_Chr_Cp(Minute_setup % 10 + 0x30);
									// Lcd_Chr_Cp(Minute_setup / 10 + 0x30);
									// Lcd_Chr_Cp(Minute_setup % 10 + 0x30);
									timer_delay();
								}
								if (old_Minute_setup != Minute_setup)
								{
									old_Minute_setup = Minute_setup;
									Lcd_Chr(2, 6, Hour_setup / 10 + 0x30);
									Lcd_Chr_Cp(Hour_setup % 10 + 0x30);
									Lcd_Chr_Cp(':');
									Lcd_Chr_Cp(Minute_setup / 10 + 0x30);
									Lcd_Chr_Cp(Minute_setup % 10 + 0x30);
								}
								if (temp != 2)
								{
									Lcd_Cmd(1);
									break;
								}
							}
							break;
						default:
							break;
						}
						if (temp >= 3)
						{
							mode_clock++;
							break;
						}
					}
					if (mode_clock != 2)
						break;
				}
				break;
			case 3:
				demc = 1;
				Lcd_Cmd(1);
				Lcd_Out(1, 2, "DONG HO BAM GIO");
				Lcd_Chr(2, 5, Hou_CL / 10 + 0x30);
				Lcd_Chr_Cp(Hou_CL % 10 + 0x30);
				Lcd_Chr_Cp(':');
				Lcd_Chr_Cp(Minute_CL / 10 + 0x30);
				Lcd_Chr_Cp(Minute_CL % 10 + 0x30);
				Lcd_Chr_Cp(':');
				Lcd_Chr_Cp(Second_CL / 10 + 0x30);
				Lcd_Chr_Cp(Second_CL % 10 + 0x30);
				delay_ms1(15);
				while (mode_clock == 3)
				{
					char old_Hou_CL = Hou_CL;
					if (BTN_MOVE == 0)
					{
						delay_ms1(15);
						if (BTN_MOVE == 0)
							demc = demc + 1;
						do
						{
						} while (BTN_MOVE == 0);
						while (demc % 2 == 0 && mode_clock == 3)
						{
							int j = 0;
							Second_CL++;
							for (hi = 0; hi < 35; hi++)
							{
								// delay_ms_CL(10);
								// delay_ms_CL(10);
								// delay_ms_CL(10);
								for (j = 0; j < 25; j++)
								{
									timer_delay();
								}
								Lcd_Chr(2, 5, Hou_CL / 10 + 0x30);
								Lcd_Chr_Cp(Hou_CL % 10 + 0x30);
								Lcd_Chr_Cp(':');
								Lcd_Chr_Cp(Minute_CL / 10 + 0x30);
								Lcd_Chr_Cp(Minute_CL % 10 + 0x30);
								Lcd_Chr_Cp(':');
								Lcd_Chr_Cp(Second_CL / 10 + 0x30);
								Lcd_Chr_Cp(Second_CL % 10 + 0x30);
							}
							if (BTN_MOVE == 0)
							{
								delay_ms1(15);
								demc = demc + 1;
								do
								{
								} while (BTN_MOVE == 0);
							}
							else if (BTN_DOWN == 0)
							{
								delay_ms1(15);
								do
								{
								} while (BTN_DOWN == 0);
								Hou_CL = 0;
								Minute_CL = 0;
								Second_CL = 0;
								Lcd_Chr(2, 5, Hou_CL / 10 + 0x30);
								Lcd_Chr_Cp(Hou_CL % 10 + 0x30);
								Lcd_Chr_Cp(':');
								Lcd_Chr_Cp(Minute_CL / 10 + 0x30);
								Lcd_Chr_Cp(Minute_CL % 10 + 0x30);
								Lcd_Chr_Cp(':');
								Lcd_Chr_Cp(Second_CL / 10 + 0x30);
								Lcd_Chr_Cp(Second_CL % 10 + 0x30);
								demc++;
							}
							if (Second_CL == 59)
								Minute_CL++;
							if (Minute_CL == 59)
								Hou_CL++;
							if (Second_CL > 59)
								Second_CL = 0;
							if (Minute_CL > 59)
								Minute_CL = 0;
						}
					}
					if (BTN_DOWN == 0)
					{
						delay_ms1(15);
						do
						{
						} while (BTN_DOWN == 0);
						Hou_CL = 0;
						Minute_CL = 0;
						Second_CL = 0;
						Lcd_Chr(2, 5, Hou_CL / 10 + 0x30);
						Lcd_Chr_Cp(Hou_CL % 10 + 0x30);
						Lcd_Chr_Cp(':');
						Lcd_Chr_Cp(Minute_CL / 10 + 0x30);
						Lcd_Chr_Cp(Minute_CL % 10 + 0x30);
						Lcd_Chr_Cp(':');
						Lcd_Chr_Cp(Second_CL / 10 + 0x30);
						Lcd_Chr_Cp(Second_CL % 10 + 0x30);
					}
				}
				break;
			default:
				//LED = 1;
				//Delay(50000);
				break; // Error handling
			}
			if (mode_clock >= 4)
			{
				Ds1307_Write_Time(Hour, Minute, Second, Mode, am_pm);
				Ds1307_Write_Date(Day, Date, Month, Year);
				Ds1307_Init();
				mode_clock = 0;
				break;
			}
		}
		//==================
		if (BTN_MOVE == 0)
		{
			LED = 0;
			delay_ms1(15);
			if (BTN_MOVE == 0)
			{
				if (Mode == 12)
				{
					if (am_pm == 1)
						Hour = Hour + 12;
					Mode = 24;
				}
				else
				{
					if (Hour >= 12)
						Hour = Hour - 12;
					Mode = 12;
				}
				Ds1307_Write_Time(Hour, Minute, Second, Mode, am_pm);
				Ds1307_Init();
			}
			do
			{
			} while (BTN_MOVE == 0);
		}

		if ((Hour_setup == Hour) && (Minute_setup == Minute))
		{
			LED1 = 1;
			Delay_ngao(10000000);
			Hour_setup = 24;
			Minute_setup = 24;
		}
		else
			LED1 = 0;
		//Baothuc(Hour_setup, Minute_setup, Hour, Minute_setup);
	}
}