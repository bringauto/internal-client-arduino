#include <Communication.hpp>
#include <Context.hpp>
#include <EnumMappers.hpp>
#include <InternalProtocol.pb.h>
#include <ProtoSerializer.hpp>
#include <device_management.h>
#include <general_error_codes.h>
#include <ic_error_codes.h>
#include <internal_client.h>
#include <memory_management.h>
#include <pb.h>
#include <pb_common.h>
#include <pb_decode.h>
#include <pb_encode.h>

#include "Arduino.h"
#include "SPI.h"
#include "Ethernet.h"

byte mac[] = { 0x5E, 0xA2, 0xBE, 0xCF, 0xFE, 0x8D };
unsigned int local_port = 62042;

void setup(){

  // LEDs init
  pinMode(LED_D0, OUTPUT);
  pinMode(LED_D1, OUTPUT);
  pinMode(LED_D2, OUTPUT);
  pinMode(LED_D3, OUTPUT);

  Ethernet.begin(mac); 
  
  struct device_identification device { 1, 2, "main", "device", 0};
  void *context {};

  init_connection(&context, "10.170.0.58", 8881, device);
  destroy_connection(&context);
}

void loop(){
    
}
