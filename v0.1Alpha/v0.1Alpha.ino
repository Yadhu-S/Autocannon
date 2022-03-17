#define PinESC1 9 // ESC must use PWM
#define PinESC2 6

#define powerButtonPin 7
#define bldcbldcMotorTriggerPin 5
#define mainTriggerPin 4
#define crankMotorPin 2


#define arduinoEnablePin 10
#define bldcEnablePin 8

#define initialArmingSpeed 10 // percentage (0-180) 
#define voltageReadPin A2
#define minBatteryVoltage 10.4 // Going beyond 9.6 will damange LiPo battery(3S)
#define maxBatteryVoltage 12.6

#include <Servo.h>
#include <Arduino.h>
#include <U8x8lib.h>

U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

Servo ESC;
Servo ESC1;

float batteryVoltage;
int powerButtonPressedAt;

void arm() {
	setSpeed(initialArmingSpeed); // Sets speed variable
	delay(1000);
}

void setSpeed(int speed) {
	int angle = map(speed, 0, 100, 0, 180); // Sets servo positions to different speeds
	ESC.write(angle);
	ESC1.write(angle);
}

float getBatteryVoltage(){	
	return ((analogRead(voltageReadPin)* 5.98) / 1024.0)/ (7420.0/(29700.0+7420.0));
}

float getBatteryPercentage(float batteryVoltage){
	return ((batteryVoltage - minBatteryVoltage)/(maxBatteryVoltage - minBatteryVoltage) * 100); 
}

void setup() {
	pinMode(powerButtonPin, INPUT_PULLUP);
	pinMode(arduinoEnablePin,OUTPUT);
	u8x8.begin();
	u8x8.setPowerSave(0);
	u8x8.setFont(u8x8_font_chroma48medium8_u);

	while (getBatteryVoltage() <= minBatteryVoltage ){
		u8x8.clear(); u8x8.print(F("LOW BATT!"));
		u8x8.setCursor(0,1);
		u8x8.print(getBatteryVoltage());
		u8x8.print(" VOLTS");
		u8x8.setCursor(0,2);
		u8x8.print(F("RECHARGE PLS :D"));
		delay(3000);
		digitalWrite(arduinoEnablePin,LOW);
	}

	digitalWrite(arduinoEnablePin,HIGH);
	pinMode(bldcEnablePin , OUTPUT);
	digitalWrite(bldcEnablePin , HIGH);

	u8x8.setCursor(0,0);
	u8x8.print(F("ESC INIT"));

	//Serial.begin(9600);
	//Serial.print("ESC INIT");

	ESC.attach(PinESC1); // Adds ESC to D3 pin.
	ESC1.attach(PinESC2);
	arm();

	pinMode(bldcbldcMotorTriggerPin, INPUT_PULLUP);
	pinMode(mainTriggerPin, INPUT_PULLUP);
	pinMode(crankMotorPin, OUTPUT);

	u8x8.clear();
	u8x8.print(F("ARMED"));

}

void loop() {
	batteryVoltage = getBatteryVoltage();
	while (batteryVoltage <= minBatteryVoltage ){
		u8x8.clear();
		u8x8.setCursor(0,0);
		u8x8.print(F("DISARMED"));
		u8x8.setCursor(0,1);
		u8x8.print(batteryVoltage);
		u8x8.setCursor(0,2);
		u8x8.print(F("BATT LOW!"));
		u8x8.setCursor(0,3);
		u8x8.print(F("RECHARGE PLS :D"));
		digitalWrite(bldcEnablePin , LOW);
		batteryVoltage = getBatteryVoltage();
		delay(3000);
		digitalWrite(arduinoEnablePin,LOW);
	}
	u8x8.setCursor(0,1);
	u8x8.print(batteryVoltage);
	//u8x8.setCursor(0,5);
	u8x8.print(F("V"));
	u8x8.setCursor(0,2);
	u8x8.print(getBatteryPercentage(batteryVoltage));
	u8x8.print(F("%"));
	// //Serial.println(speed);

	if (digitalRead(bldcbldcMotorTriggerPin) == LOW) {
		setSpeed(83);
		//Serial.println("Motor_spin");
	} else {
		setSpeed(0);
	}

	if (digitalRead(mainTriggerPin)==LOW) {
		digitalWrite(crankMotorPin, HIGH);
		//Serial.println("Main trigger");
	} else {
		digitalWrite(crankMotorPin, LOW);
	}

	if (digitalRead(powerButtonPin)==LOW){
		// TODO
		u8x8.setCursor(0,0);
		u8x8.print("POWER");
		delay(100);
		u8x8.clear();
	}
	//Serial.println("IDLE");
}
