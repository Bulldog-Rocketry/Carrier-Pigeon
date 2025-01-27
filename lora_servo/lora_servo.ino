
#include <Servo.h>
#include <RH_RF95.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

RH_RF95 driver(24, 26);
RHReliableDatagram manager(driver, CLIENT_ADDRESS);
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];


Servo servo;



void setup() {
  servo.attach(10);
  servo.write(0);
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
