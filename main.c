#include <xc.h>
#include "matrix_keypad.h"
#include "adc.h"
#include "clcd.h"
#include "eeprom.h"
#include "i2c.h"
#include "uart.h"
#include "ds1307.h"


char GEAR[][3] = {"ON", "GN", "GR", "G1", "G2", "G3", "G4", "G5", "C "};
int key, GEAR_COUNT = 0, flag = 0, event = 0, event_count = 0,disp_flag=0,menu_flag=0,clear_flag=0,view_count=0,s_flag=0,delay1=0,T_flag=0;
unsigned short sp_val;
unsigned char speed = 0;
unsigned char addr = 0x00,buffer[11];
//char time[] = {"00:00:00"};
char view_array[10][15];
unsigned long int delay;
int hour=0,minute=0,sec=0,present_event = 0;


void init_config();
void dashboard();
void eeprom_store();
void main_menu();
void main_menu_inside();
void view_log();
void display();
void clear_log();
void download_log();
void set_time();


unsigned char clock_reg[3];
unsigned char calender_reg[4];
unsigned char time[9];
unsigned char date[11];



static void get_time(void)
{
	clock_reg[0] = read_ds1307(HOUR_ADDR);
	clock_reg[1] = read_ds1307(MIN_ADDR);
	clock_reg[2] = read_ds1307(SEC_ADDR);

	if (clock_reg[0] & 0x40)
	{
        // In 12-hour mode, the upper nibble contains a single bit for AM/PM
        // Extract and convert the hour tens digit and units digit
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);//Extract tens place (0 or 1 for 12-hour format)
		time[1] = '0' + (clock_reg[0] & 0x0F);//Extract units place
	}
	else
	{
        
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
		time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	time[2] = ':';
	time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
	time[4] = '0' + (clock_reg[1] & 0x0F);
	time[5] = ':';
	time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
	time[7] = '0' + (clock_reg[2] & 0x0F);
	time[8] = '\0';
}


void init_config()
{
    init_clcd();
    init_matrix_keypad();
    init_adc();
    init_i2c();
    init_uart();
    init_ds1307();
}

void dashboard()
{
    get_time();  // Retrieve the current time from the RTC
    sp_val = read_adc(CHANNEL4);
    speed = (unsigned char)(sp_val / 10);
    if (speed > 99)  //speed greater than 99 be at that speed itself
    {
        speed = 99;
    }
    
    clcd_putch((speed / 10) + '0', LINE2(14));//printing speed
    clcd_putch((speed % 10) + '0', LINE2(15));
    
    clcd_print("TIME", LINE1(2));//printing these in line 1
    clcd_print("EV", LINE1(10));
    clcd_print("SP", LINE1(14));
    if (flag)
    {
        if(key == 2)//key 2 is detected increment geat count to 1
        {
            GEAR_COUNT = 1;
            flag = 0;//set flag as 0
        }
        if(key == 1)
        {
            GEAR_COUNT=0;
            flag=0;
        }
    }
    
    if (key == 1)//if key 1 is pressed and flag is cleared
    {
        if(flag==0)
        {
            GEAR_COUNT++;
            event=1;
        
            if (GEAR_COUNT > 7)
            {
                GEAR_COUNT = 7;//if gear is greater than 7 be at gear
                event = 0;
            }
        }
    }
    if (key == 2)
    {
        if(GEAR_COUNT==0)
        {
            GEAR_COUNT=0;
        }
        else
        {
            GEAR_COUNT--;
            event = 1;
            if (GEAR_COUNT <= 1 )//if gear count is less than 1 be at there itself
            {
                GEAR_COUNT = 1;
                event = 0;
            }
        }
    }
    if (key == 3)//if key 3 is set set event at 1
    {
        event = 1;
        if (GEAR_COUNT == 8)
            event = 0;
        GEAR_COUNT = 8;
        flag = 1;
    }
    clcd_print(time, LINE2(0));
    clcd_print(GEAR[GEAR_COUNT], LINE2(10));//printing gear 
    s_flag=0;
}

void eeprom_store()//function to write data to eeprom
{
    
    if (addr == 0x64)
    {
            addr = 0x00;
          //  event_count = 1;
    }
        
        write_external_eeprom(addr++, time[0]);
        write_external_eeprom(addr++, time[1]);
        write_external_eeprom(addr++, time[3]);
        write_external_eeprom(addr++, time[4]);
        write_external_eeprom(addr++, time[6]);
        write_external_eeprom(addr++, time[7]);
        write_external_eeprom(addr++, GEAR[GEAR_COUNT][0]);
        write_external_eeprom(addr++, GEAR[GEAR_COUNT][1]);
        write_external_eeprom(addr++, (speed / 10) + '0');
        write_external_eeprom(addr++, (speed % 10) + '0');
        
        event = 0;
 }


void main_menu()//function to display main menu when key 11 is pressed
{
    if(key==1)
        disp_flag--;
    
    if(key==2)
        disp_flag++;
    
    if(disp_flag==0)
    {
        clcd_print("*VIEW LOG       ",LINE1(0));
        clcd_print(" DOWNLOAD LOG   ",LINE2(0));
    }
    if(disp_flag==1)
    {
        clcd_print(" VIEW LOG       ",LINE1(0));
        clcd_print("*DOWNLOAD LOG   ",LINE2(0));
    }
    if(disp_flag==2)
    {
        clcd_print(" DOWNLOAD LOG   ",LINE1(0));
        clcd_print("*SET TIME       ",LINE2(0));
    }
    if(disp_flag==3)
    {
        clcd_print(" SET TIME       ",LINE1(0));
        clcd_print("*CLEAR LOG      ",LINE2(0));
    }
    if(disp_flag>=3)
        disp_flag=3;
    
    if(disp_flag<0)
        disp_flag=0;
    
}

void display() //display function is to add colon and space in view log contents
{
    unsigned char addr1 = 0x00;
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<14;j++)
        {
            if(j == 2 || j == 5)
            {
                view_array[i][j]=':';//taken an array to store
            }
            else if(j == 8 || j == 11)
            {
                view_array[i][j] = ' ';
            }
            else
                view_array[i][j] = read_external_eeprom(addr1++);
        }
        view_array[i][14]='\0';//null character
    }
}

void view_log()//function to view log the logged in events
{
    static int var = 0;
    clcd_print("----VIEW LOG----",LINE1(0));
    if(addr == 0x00 && event_count == 0)
    {
        clcd_print("NO LOGS FOUND   ",LINE2(0));
        for( delay=0;delay<300000;delay++);
        menu_flag=1; 
        return;
        
    }
    display();
    if(key == 1)//if key 1 is detecetd and pc greater then 0 decerment present event
    {
        if(present_event > 0)
        {
            present_event--;
        }
        else
        {
            present_event = 0;//if not make the present event as zero
        }
    }
    else if(key == 2)
    {
        if(present_event >= event_count-1)
            present_event = event_count - 1;
        
        else
            present_event++;//increment the present event
        
        if(present_event == 10)//if present event is equal to 10 stay at the 9th event itself
           present_event = 9;
        
    }
     if(event_count > 10)
            var = event_count % 10;//moding logic for events logged
     
     else
            var = 0;
     
     clcd_putch(present_event + '0',LINE2(0));//printing the counts
     clcd_print(view_array[(present_event+var)%10],LINE2(2)); //printing the events logged in
}

void clear_log()//function to clear the logs that have been logged
{
    if(event_count == 0)
    {
        clcd_print("NO LOGS         ",LINE1(0));//printing when no events are logged in
        clcd_print("TO CLEAR        ",LINE2(2));
        for(delay=0;delay<300000;delay++);
        menu_flag=1;
        return;
    }
    clcd_print("CLEARING LOGS   ",LINE1(0));
    clcd_print("Just a minute...",LINE2(0));
    for(delay=0;delay<300000;delay++);
    addr = 0x00;
    event_count = 0;
    menu_flag=1;
    return;
}
            


void download_log()//this is to download the logs that are logged
{
    int temp = 0;
    int str=0;
    if(event_count<=10)
    {
        temp = event_count;
    }
    else if(event_count>10)
    {
        int str=(event_count%10);
        temp=event_count-str;
    }
    int varr=0;
    display();
    if(event_count>10)
    {
        varr=event_count % 10;
    }
    else
    {
        varr=0;
    }
    if(event_count==0)
    {
        clcd_print("NO LOGS         ",LINE1(0));//this is to rint when no events are logged in
        clcd_print("TO DOWNLOAD     ",LINE2(2));
        for(delay=0;delay<300000;delay++);
        menu_flag=1;//come back to main menu
        return;
    }
    else
    {
        for(int i=0;i<temp;i++)
        {
            puts(view_array[(i+varr)%10]);
            putch('\n');
            putch('\r');
        }
        clcd_print("downloading...  ",LINE1(0));//download successfulll
        clcd_print("successfull!!   ",LINE2(2));
        for(delay=0;delay<300000;delay++);
        menu_flag=1;
        return;
    }
}


    
void set_time()//this function is used to set time using RTC
{
   
    clcd_print("HH:MM:SS        ",LINE1(0));//initially print this message
    if(s_flag==0)
    {
        hour=(time[0]-48)*10;
        hour=hour+(time[1]-48);
        minute=(time[3]-48)*10;
        minute=minute+(time[4]-48);
        sec=(time[6]-48)*10;
        sec=sec+(time[7]-48);
        s_flag=1;
    }
    if(key==2)
    {
        T_flag++;//increment the time flag
    }
    if(T_flag==0)
    {
        delay1++;
        if(delay1>0 && delay1<=500)
        {
            clcd_print(time,LINE2(0));//printing the time in line2 of clcd
        }
        else if(delay1>500 && delay1<=1000)
        {
            clcd_print("  ",LINE2(0)); //this is for blinking
        }
        else
            delay1=0;
    }
    if(T_flag==1)
    {
        delay1++;
        if(delay1>0 && delay1<=500)
        {
            clcd_print(time,LINE2(0));
        }
        else if(delay1>500 && delay1<=1000)
        {
            clcd_print("  ",LINE2(3)); 
        }
        else
            delay1=0;
    }
    if(T_flag==2)
    {
        delay1++;
        if(delay1>0 && delay1<=500)
        {
            clcd_print(time,LINE2(0));
        }
        else if(delay1>500 && delay1<=1000)
        {
            clcd_print("  ",LINE2(6)); 
        }
        else
            delay1=0;
    }
    if(T_flag>=3)
        T_flag=0;
    if(key==1 && T_flag==0)
    {
        hour++;
        if(hour==24)
            hour = 0;
        if(hour<24)
        {
            time[0]=hour/10+48;
            time[1]=hour%10+48;
        }
    }
    if(key==1 && T_flag==1)//if key is 1 and flag is set
    {
        minute++;
        if(minute==60)
            minute=0;
        if(minute<60)
        {
            time[3]=minute/10+48;
            time[4]=minute%10+48;
        }
    }
    if(key==1 && T_flag==2)//if key is 1 and flag is 2
    {
        sec++;
        if(sec==60)
            sec=0;
        if(sec<60)
        {
            time[6]=sec/10+48;
            time[7]=sec%10+48;
        }
    }
    if(key == MK_SW11)//this is logic is complete for wrting hour add,sec add,min addr to rtc
    {
        int temp=hour/10;
        temp=temp<<4;
        int temp1=hour%10;
        hour=temp|temp1;
        write_ds1307(HOUR_ADDR,hour);//writing hour
        int temp2=minute/10;
        temp2=temp2<<4;
        int temp3=minute%10;
        minute=temp2|temp3;
        write_ds1307(MIN_ADDR,minute);//writing minute
        int temp4=sec/10;
        temp4=temp4<<4;
        int temp5=sec%10;
        sec=temp4|temp5;
        write_ds1307(SEC_ADDR,sec);//writing sec
        T_flag=0;
        menu_flag=0;
        CLEAR_DISP_SCREEN;
    }
}



void main_menu_inside()
{
    if(disp_flag==0)
    {
        view_log();//calling view log
    }
    if(disp_flag==1)
    {
        download_log();//calling download log
    }
    if(disp_flag==2)
    {
        if(key == MK_SW11 && s_flag==0)
            key=ALL_RELEASED;
        set_time();//calling set time
    }
    if(disp_flag==3)
    {
        clear_log();//calling clear log
    }
}


void main() 
{
    init_config();//calling init configuration
    while(1)
    {
        key=read_switches(STATE_CHANGE);//reading switches
        if(key == MK_SW11)
        {
            menu_flag++;//menu flag increment
            clear_flag=1;
            if(menu_flag>=2)
            {
                menu_flag=2;
            }
        }
        if(key == MK_SW12)
        {
            menu_flag--;//decerement menu flag
            clear_flag=1;
            if(menu_flag<0)
            {
                menu_flag=0;
            }
            present_event=0;//present event set to 0
            T_flag=0;
            s_flag=0;
        }
        if(clear_flag)
        {
            CLEAR_DISP_SCREEN;
            clear_flag=0;
        }
        if(menu_flag==1)
        {
            main_menu();//if menu flag is 1 go to main menu
        }
        if(menu_flag==0)
        {
            disp_flag=0;
            dashboard();//if menu flag is 0 go to dashboard
        }
        if(menu_flag==2)
        {
            main_menu_inside();//if menu flag is 2 go to sub menu
        }
        if(event)
        {
            event_count++;
            eeprom_store();//if event is set go to the eeprom store
        }
    }
    return;
}

//---------------------------------------------------------------------------------------------------------------------



