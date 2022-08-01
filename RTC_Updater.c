////////////////////////////////////////////////////////////////////////////////
//  Project Name: RTC Updating Module
//  MCU           : PIC 18F452
//  Crystal       : 20MHz
//  Author        : Mohd. Ohidul islam
//  Version       : 1.01
//  Date          : 01.08.2022
////////////////////////////////////////////////////////////////////////////////


//  LCD Configuration

sbit LCD_D7 at RB0_bit;
sbit LCD_D6 at RB1_bit;
sbit LCD_D5 at RB2_bit;
sbit LCD_D4 at RB3_bit;
sbit LCD_EN at RB4_bit;
sbit LCD_RS at RB5_bit;

sbit LCD_D7_Direction at TRISB0_bit;
sbit LCD_D6_Direction at TRISB1_bit;
sbit LCD_D5_Direction at TRISB2_bit;
sbit LCD_D4_Direction at TRISB3_bit;
sbit LCD_EN_Direction at TRISB4_bit;
sbit LCD_RS_Direction at TRISB5_bit;

sbit LED_R1 at RC0_bit;
sbit LED_R2 at RC1_bit;
sbit LED_G1 at RD0_bit;
sbit LED_G2 at RD1_bit;

char sec_dis[5], *TIME="00-00-00", *DATE="00/00/2000";

char UART_RTC[]={0x55,0xAA,0x00,0x51,0x51,0x51,0xFF,0x51,0x51,0x51} ;

unsigned short sec,min,hour,day,month,year,BCD_Sec,BCD_Min,BCD_Hour,BCD_Day,BCD_Month,BCD_Year,i,present_sec=0;
short Button_1_lock=0,Button_2_lock=0,Button_3_lock=0,l,setting=0;

unsigned long int count=0, Time_Count;


unsigned short ds3231_read(unsigned short address) // ds3231 RTC module read

                                                {

                                                   unsigned short read_data;

                                                   I2C1_Start();             // Start I2C signal

                                                   I2C1_Wr(0xD0);                // Device address + W   Device Address 1101 000+ '0' for Wright

                                                   I2C1_Wr(address);              // Data address

                                                   I2C1_Repeated_Start();

                                                   I2C1_Wr(0xD1);                 // Device address + R

                                                   read_data=I2C1_Rd(0);         // no acknowladge

                                                   I2C1_Stop();                  // Stop I2C signal

                                                   return(read_data);            // return read data

                                                   }

unsigned short ds3231_write(unsigned short address,unsigned short RTC_Value) // ds3231 RTC module read

                                                {

                                                   unsigned short read_data;

                                                   I2C1_Start();             // Start I2C signal

                                                   I2C1_Wr(0xD0);                // Device address + W   Device Address 1101 000+ '0' for Wright

                                                   I2C1_Wr(address);              // Data address

                                                   I2C1_Repeated_Start();

                                                   I2C1_Wr(RTC_Value);                 // Device address + Write

                                                  // read_data=I2C1_Rd(0);         // no acknowladge

                                                   I2C1_Stop();                  // Stop I2C signal

                                                   return(read_data);            // return read data

                                                   }





void Interrupt(){
              if (TMR0IF_bit){
                TMR0IF_bit = 0;
                TMR0H         = 0x3C;
                TMR0L         = 0xB0;
                //Enter your code here
                 
              if(PORTA.F1==1)
                 {
                 Time_Count++;

                 }
                 else
                     Time_Count=0;

              }
}


void Update_UART_RTC_DATA()
                           {
                            UART_RTC[0]= 0x55;
                            UART_RTC[1]= 0xAA;
                            UART_RTC[2]= 0x00;
                            
                            UART_RTC[3]= hour;
                            UART_RTC[4]= min;
                            UART_RTC[5]= sec;
                           
                            UART_RTC[6]= 0xFF;
                           
                            UART_RTC[7]= day;
                            UART_RTC[8]= month;
                            UART_RTC[9]= year;
                           
                           }


void InitTimer0(){
  T0CON         = 0x88;
  TMR0H         = 0x3C;
  TMR0L         = 0xB0;
  GIE_bit         = 1;
  TMR0IE_bit         = 1;
}



void main()
            {
             ADCON1=0x07;
             TRISA=0x1E;
             TRISB = 0x00;
             PORTB=0x80;
             TRISC.F5=0;
             TRISC.F0=0;
             TRISC.F1=0;
             TRISD=0x00;
             PORTD=0x00;

             InitTimer0();

             Lcd_Init();                                // Initialize Lcd module
             Lcd_Cmd(_LCD_CLEAR);                         // Clear Lcd display
             Lcd_Cmd(_LCD_CURSOR_OFF);                   // Lcd Cursor off
             Lcd_Out(1,1,"  RTC UPDATING" );
             Lcd_Out(2,1,"     MODULE");


             I2C1_Init(100000);                          // Initialize I2C signal at 100KHz
             I2C1_Start();                               // Start I2C signal

              UART1_Init(9600);               // Initialize UART module at 9600 bps
              Delay_ms(1000);                  // Wait for UART module to stabilize

              UART1_Write_Text("RTC Updating Module");
              UART1_Write(10);
              UART1_Write(13);


               
              LED_R1=1;
              LED_R2=1;

              Delay_ms(2000);
              Lcd_Cmd(_LCD_CLEAR);                         // Clear Lcd display
              Lcd_Out(1,1,"Date:" );
              Lcd_Out(2,1,"Time:");

              //PORTB=0x80;
              
         while(1)
               {

               if(PORTA.F4==0)
                              Button_3_lock=0;
                              
                if(Button_3_lock==0&&PORTA.F4==1)
                                                 {
                              Button_3_lock=1;
                              Update_UART_RTC_DATA();
                              
                              for(i=0;i<=9;i++)
                                      {
                                      UART1_Write(UART_RTC[i]);
                                      }
                              }
                              
               if(setting==0)
                     {

                      BCD_Sec=ds3231_read(0);    // Read Second
                      sec=Bcd2Dec(BCD_Sec);        // Convert BCD data to Decimal








                      if(present_sec!=sec)
                          {
                          //************* Convert First  ************


                          BCD_Min=ds3231_read(1);    // Read Minutes
                          BCD_Hour=ds3231_read(2);   // Read Hour

                          BCD_Day=ds3231_read(4);       // Read date of the month
                          BCD_Month=ds3231_read(5);     // Read Month
                          BCD_Year=ds3231_read(6);      // Read Year

                          min=Bcd2Dec(BCD_Min);        // Convert BCD data to Decimal
                          hour=Bcd2Dec(BCD_Hour);      // Convert BCD data to Decimal

                          day=Bcd2Dec(BCD_Day);        // Convert BCD data to Decimal
                          month=Bcd2Dec(BCD_Month);    // Convert BCD data to Decimal
                          year=Bcd2Dec(BCD_Year);      // Convert BCD data to Decimal


                           if(hour>=12)
                                 {
                                 Lcd_Out(2,15,"PM");
                                 if(hour>12)
                                            hour=hour-12;
                                 }
                                 else
                                     Lcd_Out(2,15,"AM");
                                     
                                     
                          //--------  Print Data In LCD -------------- //

                          TIME[0]=(hour/10)+48;
                          TIME[1]=(hour%10)+48;
                          TIME[3]=(min/10)+48;
                          TIME[4]=(min%10)+48;
                          TIME[6]=(sec/10)+48;
                          TIME[7]=(sec%10)+48;

                          DATE[0]=(day/10)+48;
                          DATE[1]=(day%10)+48;
                          DATE[3]=(month/10)+48;
                          DATE[4]=(month%10)+48;
                          DATE[8]=(year/10)+48;
                          DATE[9]=(year%10)+48;

                          Lcd_Out(2,6,TIME);
                          Lcd_Out(1,6,DATE);

                          Update_UART_RTC_DATA();


                          present_sec=sec;
                          }

                      delay_ms(100);

                      }// END Setting==0;




               if(setting==1)       // Time Setting
                                         {



                                         }

                if(setting==2)       // Date Setting
                                         {



                                         }
 /////////////////----  SETTING----------------------------------------------------
 
 
                    if( Time_Count>300)       // 3 Sec; Since 10ms Interrupt
                           {
                           Time_Count=0;
                           
                           setting++;
                           
                           if(setting>2)       // RESET
                                        {
                                        setting=0;
                                        Lcd_Cmd(_LCD_CLEAR);                         // Clear Lcd display
                                         Lcd_Out(1,1,"Date:" );
                                         Lcd_Out(2,1,"Time:");
                                        }
                                        
                                        
                                        
                            if(setting==1)       // Time Setting
                                         {
                                        Lcd_Cmd(_LCD_CLEAR);                         // Clear Lcd display
                                       Lcd_Out(1,1," TIME SETTING " );
                                       Lcd_Out(2,6,TIME);
                                           }


                            if(setting==2)       // DATE Setting
                                         {
                                        Lcd_Cmd(_LCD_CLEAR);                         // Clear Lcd display
                                        Lcd_Out(1,1," DATE SETTING " );
                                        Lcd_Out(2,6,DATE);
                                           }






                           }    // END   if( Time_Count>300)       // 3 Sec; Since 10ms Interrupt
               
               
               
               
               
                     
               }  // END While



}