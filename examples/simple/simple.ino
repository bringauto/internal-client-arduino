#include <Communication.hpp>
#include <device_management.h>
#include <general_error_codes.h>
#include <internal_client.h>
#include <memory_management.h>

#include "SPI.h"
#include "Ethernet.h"

byte mac[] = { 0x5E, 0xA2, 0xBE, 0xCF, 0xFE, 0x8D };
void *context {};
device_identification device;
unsigned long previousMillis = 0;
const int interval = 1000; // interval in milliseconds how often send statuses
int counter = 0;
bool connected = false;

void init_device_id(device_identification &device) {
  device.module = 1;
  device.device_type = 2;
  device.priority = 0;
  const char device_role[] = "main";
  const char device_name[] = "device";

  if(allocate(&device.device_role, sizeof(device_role)) == OK) {
    memcpy(device.device_role.data, device_role, device.device_role.size_in_bytes);
  }

  if(allocate(&device.device_name, sizeof(device_name)) == OK) {
    memcpy(device.device_name.data, device_name, device.device_name.size_in_bytes);
  }
}

void destroy_device_id(device_identification &device) {
  deallocate(&device.device_role);
  deallocate(&device.device_name);
}

void setup(){
  Serial.begin(9600);
  Ethernet.begin(mac); 

  init_device_id(device);

  // init_connection with Module gateway's ip address and port
  int rc = init_connection(&context, "10.170.0.54", 8888, device);

  if(rc == OK){
    Serial.println("connection ok");
    connected = true;
  }
  else{
    Serial.print("connection not ok, rc: ");
    Serial.println(rc);
  }
}

void loop(){
  if(connected) {
    if(counter < 5) {
      unsigned long currentMillis = millis();

      if(currentMillis - previousMillis >= interval){
        previousMillis = currentMillis;

        buffer status_buffer;
        String status = "Some status data";

        if(allocate(&status_buffer, status.length()) == NOT_OK) {
          Serial.println("Allocation failed");
        }
        memcpy(status_buffer.data, status.c_str(), status_buffer.size_in_bytes);

        send_status(&context, status_buffer, 30);
        counter++;

        buffer command_buffer;
        get_command(&context, &command_buffer);

        // Now in command_buffer.data are command data from Internal server

        deallocate(&command_buffer);
      }
    }
    else if(counter == 5) {
      // 5 statuses sent - ending connection
      destroy_device_id(device);
      destroy_connection(&context);
      connected = false;
      counter++;
    }
    else {
      // Do nothing
    }
  }
}
