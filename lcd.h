#include <LPC213X.H>

#define RW(x)  (x==1?(IO1SET=1<<23):(IO1CLR=1<<23))
#define DRW(x) (x==1?(IO1DIR|=1<<23):(IO1DIR&=~(1<<23)))
#define RS(x)  (x==1?(IO0SET=1<<10):(IO0CLR=1<<10))
#define DRS(x) (x==1?(IO0DIR|=1<<10):(IO0DIR&=~(1<<10)))
#define EN(x)  (x==1?(IO0SET=1<<11):(IO0CLR=1<<11))
#define DEN(x) (x==1?(IO0DIR|=1<<11):(IO0DIR&=~(1<<11)))
#define D4(x)  (x==1?(IO1SET=1<<21):(IO1CLR=1<<21))
#define DD4(x) (x==1?(IO1DIR|=1<<21):(IO1DIR&=~(1<<21)))
#define D5(x)  (x==1?(IO0SET=1<<15):(IO0CLR=1<<15))
#define DD5(x) (x==1?(IO0DIR|=1<<15):(IO0DIR&=~(1<<15)))
#define D6(x)  (x==1?(IO0SET=1<<16):(IO0CLR=1<<16))
#define DD6(x) (x==1?(IO0DIR|=1<<16):(IO0DIR&=~(1<<16)))
#define D7(x)  (x==1?(IO0SET=1<<17):(IO0CLR=1<<17))
#define DD7(x) (x==1?(IO0DIR|=1<<17):(IO0DIR&=~(1<<17)))

#define D4P    (IO1PIN & (1<<21))
#define D5P    (IO0PIN & (1<<15))
#define D6P    (IO0PIN & (1<<16))
#define D7P    (IO0PIN & (1<<17))
#define BF     (IO0PIN & (1<<17))

void delay_ms(int x);
void busy_check(void);
void lcd_write_data(char x);
void lcd_write_cmd(char x);
void put_data(char x);
void delay_1us(void);
void lcd_init(void);
void output_data(void);
void input_data(void);
void output_ctrl(void);
void lcd_putchar(char x);
void lcd_putsf(const char *x);
void lcd_puts(char *x);
void lcd_gotoxy(char x,char y);
void lcd_clear(void);
char get_data(void);
char lcd_read_data(void);
char lcd_read_cmd(void);

char get_data(void)
{
	char tmp[4],sum,i;
	tmp[0]=!!D4P;
	tmp[1]=!!D5P;
	tmp[2]=!!D6P;
	tmp[3]=!!D7P;
	
	for(i=0,sum=0;i<4;i++)
	{
		sum+=tmp[i]<<i;
	}
	
	return sum;
}

char lcd_read_data(void)
{
	char x;
	input_data();
	RS(1);
	RW(1);
	
	//*********dummy read*********
	EN(1);
	delay_1us();
	EN(0);
	delay_1us();
	
	EN(1);
	delay_1us();
	EN(0);
	delay_1us();
	//****************************
	
	EN(1);
	x=get_data();
	x=x<<4;
	EN(0);
	
	delay_1us();
	
	EN(1);
	x+=get_data();
	EN(0);
	
	output_data();
	
	return x;
}

char lcd_read_cmd(void)
{
	char x;
	input_data();
	RS(0);
	RW(1);
	EN(1);
	x=get_data();
	x=x<<4;
	EN(0);
	
	delay_1us();
	
	EN(1);
	x+=get_data();
	EN(0);
	
	output_data();
	
	return x;
}

void lcd_clear(void)
{
	lcd_write_cmd(0x01);
}

void lcd_gotoxy(char x,char y)
{
	if(y==1)
		lcd_write_cmd(0xC0+x);
	else
		lcd_write_cmd(0x80+x);
}

void lcd_puts(char *x)
{
	while(*x!=0)
	{
		lcd_write_data(*x++);
	}
}

void lcd_putsf(const char *x)
{
	while(*x!=0)
	{
		lcd_write_data(*x++);
	}
}

void lcd_putchar(char x)
{
	lcd_write_data(x);
}

void output_data(void)
{
	DD4(1);
	DD5(1);
	DD6(1);
	DD7(1);
}

void output_ctrl(void)
{
	DRS(1);
	DRW(1);
	DEN(1);
}

void input_data(void)
{
	DD4(0);
	DD5(0);
	DD6(0);
	DD7(0);
}

void delay_1us(void)
{
	char x;
	x=50;
	while(x>0)x--;
}

void delay_ms(int x)
{
	int i;
	if(x<=0)return;
	while(x-->0)
	{
		for(i=0;i<3000;i++);
	}
}

void busy_check(void)
{
	char x;
	input_data();
	RS(0);
	RW(1);
	
	do
	{
		EN(1);
		x=BF;
		EN(0);
		
		delay_1us();
		
		EN(1);
		delay_1us();
		EN(0);
		
	}while(x);
	
	output_data();
}

void put_data(char x)
{
	char i,array[4];
	for(i=0;i<4;i++)
	{
		array[i]=x%2;
		x=x>>1;
	}
	D4(array[0]);
	D5(array[1]);
	D6(array[2]);
	D7(array[3]);
}

void lcd_write_cmd(char x)
{
	busy_check();
	RS(0);
	RW(0);
	put_data(x>>4);
	EN(1);
	delay_1us();
	EN(0);
	
	put_data(x);
	EN(1);
	delay_1us();
	EN(0);
}

void lcd_write_data(char x)
{
	busy_check();
	RS(1);
	RW(0);
	put_data(x>>4);
	EN(1);
	delay_1us();
	EN(0);
	
	put_data(x);
	EN(1);
	delay_1us();
	EN(0);
}

void lcd_init(void)
{
	output_data();
	output_ctrl();
	delay_ms(200);
	RS(0);
	RW(0);
	put_data(0x02);
	EN(1);
	delay_1us();
	EN(0);
	delay_ms(5);
	
	lcd_write_cmd(0x28);
	lcd_write_cmd(0x0C);
	lcd_write_cmd(0x01);
	
	delay_ms(15);
}
