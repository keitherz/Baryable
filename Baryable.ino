/***
Baryable Arduino Code
baryable.ino v0.1r01
***/

#include <LiquidCrystal.h>

#define PIN_COIN	7
#define PIN_OUT		13

#define COUNT_NAMES	2

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

struct s_namesTable
{
	String rfid;
	String name;
	unsigned int balance;
} namesTable[COUNT_NAMES];

String serialBuf;
unsigned int coinsBuf = 0;
unsigned long lastActTime = millis();
bool isCoin = false;
bool coinUpdate = false;
bool standbyUpdate = false;

void initAccounts(void);

void displayStandby(void);
void displayCoins(const unsigned int coins);
void displayAccount(String name, const unsigned int coins);

void setup()
{
	initAccounts();

	pinMode(PIN_COIN, INPUT);
	pinMode(PIN_OUT, OUTPUT);

	Serial.begin(9600);
	lcd.begin(16,2);

	displayStandby();
}

void loop()
{
	unsigned char bufI;
	unsigned char namesI;

	while( Serial.available() > 0 )
	{
		serialBuf.concat((char) Serial.read());
		delay(50);
	}

	for( bufI = 0; bufI < serialBuf.length(); bufI++ )
	{
		if( (serialBuf.charAt(bufI) == 0x02) && (serialBuf.charAt(bufI+13) == 0x03) )
		{
			serialBuf = serialBuf.substring(bufI+1, bufI+13);
			Serial.println(serialBuf);

			for( namesI = 0; namesI < COUNT_NAMES; namesI++ )
			{
				if( serialBuf.equals(namesTable[namesI].rfid) )
				{
					namesTable[namesI].balance += coinsBuf;
					coinsBuf = 0;
					lastActTime = millis();
					standbyUpdate = true;

					displayAccount(namesTable[namesI].name, namesTable[namesI].balance);
				}
			}
			serialBuf = "";
			break;
		}
	}

	if( digitalRead(PIN_COIN) == HIGH )
	{
		coinsBuf++;
		isCoin = true;
		coinUpdate = true;
		lastActTime = millis();
		digitalWrite(PIN_OUT, HIGH);

		delay(20);
		while(digitalRead(PIN_COIN) == HIGH) continue;
	}

	if( coinUpdate == true )
	{
		coinUpdate = false;
		digitalWrite(PIN_OUT, LOW);
		displayCoins(coinsBuf);
	}

	if( standbyUpdate == true && ((millis() - lastActTime) > 10*1000UL) )
	{
		standbyUpdate = false;
		displayStandby();
	}
}

void initAccounts(void)
{
	namesTable[0].rfid = "70004BA521BF";
	namesTable[0].name = "F. BINOS";
	namesTable[0].balance = 15;

	namesTable[1].rfid = "70004B871DA1";
	namesTable[1].name = "K. BEJA";
	namesTable[1].balance = 20;
}

void displayStandby(void)
{
	lcd.clear();
	lcd.setCursor(1, 0);
	lcd.print("PLEASE INSERT  ");
	lcd.setCursor(5, 1);
	lcd.print("COIN");
}

void displayCoins(const unsigned int coins)
{
	lcd.clear();
	lcd.setCursor(4, 0);
	lcd.print("PESO: ");
	lcd.setCursor(11, 0);
	lcd.print(coins, 10);
}

void displayAccount(String name, const unsigned int coins)
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("ID: " + name);
	lcd.setCursor(0, 1);
	lcd.print("BALANCE: ");
	lcd.setCursor(9, 1);
	lcd.print(coins, 10);
}
