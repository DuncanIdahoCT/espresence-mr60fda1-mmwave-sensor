#include "esphome.h"

using namespace esphome;

class UARTSensor : public Component, public UARTDevice {
  public:
  UARTSensor(UARTComponent *parent) : UARTDevice(parent) {}
  Sensor* presence_sensor = new Sensor();
  Sensor* motion_sensor = new Sensor();
  Sensor* activity_sensor = new Sensor();
  Sensor* fall_sensor = new Sensor();
  Sensor* fall_det_sw = new Sensor();
  Sensor* fall_det_sens = new Sensor();

  void setup() {
  }

  std::vector<int> bytes;

void loop() override
{
    while (available())
    {
        bytes.push_back(read());

        //End of Frame is 0x54 0x43
        if(bytes[bytes.size()-2] == 0x54 && bytes[bytes.size()-1] == 0x43)
        {            
			processPacket();
            bytes.clear();        
        }
    }
}

  void processPacket() {            

    std::string str = "";
	
	// As described in the Seeed Studio protocol tables:

    // Presence information
	if((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x80) && (bytes[3] == 0x01) && (bytes[4] == 0x00) && (bytes[5] == 0x01))
    {        
      
		int presence_sensor_value = int(bytes[6]);

		//str = "Bytes: " + hexStr(bytes[0]) + ", " + hexStr(bytes[1]) + ", " + hexStr(bytes[2]) + ", " + hexStr(bytes[3]) + ", " + hexStr(bytes[4]) + ", " + hexStr(bytes[5]) + ", " + hexStr(bytes[6]) + ", " + hexStr(bytes[7]) + ", " + hexStr(bytes[8]) + ", " + hexStr(bytes[9]);
		//ESP_LOGI("Presence Frame", str.c_str());

		presence_sensor->publish_state(presence_sensor_value);
		return;
	}

    // Motion information
    if((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x80) && (bytes[3] == 0x02) && (bytes[4] == 0x00) && (bytes[5] == 0x01))
    {        
      
		if((bytes[6] == 0x02))
		{
		int motion_sensor_value = 1;
		motion_sensor->publish_state(motion_sensor_value);
		}

		if((bytes[6] == 0x01))
		{
		int motion_sensor_value = 0;            

		motion_sensor->publish_state(motion_sensor_value);
		}

		//str = "Bytes: " + hexStr(bytes[0]) + ", " + hexStr(bytes[1]) + ", " + hexStr(bytes[2]) + ", " + hexStr(bytes[3]) + ", " + hexStr(bytes[4]) + ", " + hexStr(bytes[5]) + ", " + hexStr(bytes[6]) + ", " + hexStr(bytes[7]) + ", " + hexStr(bytes[8]) + ", " + hexStr(bytes[9]);
		//ESP_LOGI("Motion Frame", str.c_str());

		return;
	}

    // Body dynamic parameters
    if((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x80) && (bytes[3] == 0x03) && (bytes[4] == 0x00) && (bytes[5] == 0x01))
    {        
      
		int activity_sensor_value = int(bytes[6]);

		//str = "Bytes: " + hexStr(bytes[0]) + ", " + hexStr(bytes[1]) + ", " + hexStr(bytes[2]) + ", " + hexStr(bytes[3]) + ", " + hexStr(bytes[4]) + ", " + hexStr(bytes[5]) + ", " + hexStr(bytes[6]) + ", " + hexStr(bytes[7]) + ", " + hexStr(bytes[8]) + ", " + hexStr(bytes[9]);
		//ESP_LOGI("Activity Level Frame", str.c_str());

		//str = esphome::to_string(activity_sensor_value);
		//ESP_LOGI("Activity Level", str.c_str());
            
		activity_sensor->publish_state(activity_sensor_value);
		return;
	}

    // Fall detection reporting
    if((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x83) && (bytes[3] == 0x01) && (bytes[4] == 0x00) && (bytes[5] == 0x01))
    {        
      
		int fall_sensor_value = int(bytes[6]);

		//str = "Bytes: " + hexStr(bytes[0]) + ", " + hexStr(bytes[1]) + ", " + hexStr(bytes[2]) + ", " + hexStr(bytes[3]) + ", " + hexStr(bytes[4]) + ", " + hexStr(bytes[5]) + ", " + hexStr(bytes[6]) + ", " + hexStr(bytes[7]) + ", " + hexStr(bytes[8]) + ", " + hexStr(bytes[9]);
		//ESP_LOGI("Fall Detection Frame", str.c_str());

		//str = esphome::to_string(fall_sensor_value);
		//ESP_LOGI("Fall Detection", str.c_str());
            
		fall_sensor->publish_state(fall_sensor_value);
		return;
	}

    // Fall sensitivity change reporting
    if((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x83) && (bytes[3] == 0x0D) && (bytes[4] == 0x00) && (bytes[5] == 0x01))
    {        
      
		//str = hexStr(bytes[6]);
		//ESP_LOGI("Fall Sensitivity", str.c_str());

		//str = "Bytes: " + hexStr(bytes[0]) + ", " + hexStr(bytes[1]) + ", " + hexStr(bytes[2]) + ", " + hexStr(bytes[3]) + ", " + hexStr(bytes[4]) + ", " + hexStr(bytes[5]) + ", " + hexStr(bytes[6]) + ", " + hexStr(bytes[7]) + ", " + hexStr(bytes[8]) + ", " + hexStr(bytes[9]);
		//ESP_LOGI("Fall Sensitivity Frame", str.c_str());

		int fall_det_sens_state = int(bytes[6]);
		
		//str = esphome::to_string(fall_det_sens_state);
		//ESP_LOGI("Fall Detection Sensitivity State", str.c_str());
		
		fall_det_sens->publish_state(fall_det_sens_state);

		return;
	}

    // Fall detection function switch status
    if((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x83) && (bytes[3] == 0x00) && (bytes[4] == 0x00) && (bytes[5] == 0x01))
    {        
      
		//str = hexStr(bytes[6]);
		//ESP_LOGI("Fall Detection Switch State", str.c_str());

		//str = "Bytes: " + hexStr(bytes[0]) + ", " + hexStr(bytes[1]) + ", " + hexStr(bytes[2]) + ", " + hexStr(bytes[3]) + ", " + hexStr(bytes[4]) + ", " + hexStr(bytes[5]) + ", " + hexStr(bytes[6]) + ", " + hexStr(bytes[7]) + ", " + hexStr(bytes[8]) + ", " + hexStr(bytes[9]);
		//ESP_LOGI("Fall Detection Switch State Frame", str.c_str());

		int fall_det_sw_state = int(bytes[6]);
		
		//str = esphome::to_string(fall_det_sw_state);
		//ESP_LOGI("Fall Detection Switch State", str.c_str());
		
		fall_det_sw->publish_state(fall_det_sw_state);

		return;
	}

  }

	char hexmap[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

	std::string hexStr(int data)
	{
		std::string s = "    ";
		s[0] = '0';
		s[1] = 'x';
		s[2] = hexmap[(data & 0xF0) >> 4];
		s[3] = hexmap[data & 0x0F];
	
		//s = "0x" + s;  
		return s;
	}

};
