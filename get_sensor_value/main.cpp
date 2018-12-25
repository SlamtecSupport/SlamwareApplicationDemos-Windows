/*
 * main.cpp
 *
 * Created by Slamtec at 2018-02-07
 * Copyright 2018 (c) Shanghai Slamtec Co., Ltd.
 */
#include <regex>
#include <rpos/robot_platforms/slamware_core_platform.h>

using namespace rpos::robot_platforms;
using namespace rpos::features::impact_sensor;

std::string ip_address = "";
const char *ip_regex = "\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}";

void ShowHelp(std::string app_name) {
    std::cout << "SLAMWARE console demo." << std::endl << 
        "Usage: " << app_name << " <slamware_address>" << std::endl;
}

bool ParseCommandLine(int argc, const char *argv[]) {
    bool opt_show_help = false;
    for (int pos = 1; pos < argc; ++pos) {
        const char *current = argv[pos];
        if (strcmp(current, "-h") == 0)
            opt_show_help = true;
		else 
            ip_address = current;       
    }
    std::regex reg(ip_regex);
    if(!opt_show_help && !std::regex_match(ip_address, reg)) 
        opt_show_help = true;
    if (opt_show_help) {
        ShowHelp("get_sensor_value");
        return false;
    }
    return true;
}

int main(int argc, const char *argv[]) {
	if (!ParseCommandLine(argc, argv)) return 1;
	std::cout << "Connecting SDP @ " << ip_address << "..."<< std::endl;
	try {
		SlamwareCorePlatform sdp = SlamwareCorePlatform::connect(ip_address, 1445);
        std::cout <<"SDK Version: " << sdp.getSDKVersion() << std::endl;
        std::cout <<"SDP Version: " << sdp.getSDPVersion() << std::endl;

		std::vector<ImpactSensorInfo> sensors;
        bool result = sdp.getSensors(sensors);
		if (result) {
	        for (std::vector<ImpactSensorInfo>::iterator it = sensors.begin(); it != sensors.end(); ++it) {
			    std::cout << "Sensor id : " << it->id << std::endl;
			    if (it->kind == ImpactSensorKindBumper) 
			    	std::cout << "Sensor Kind: Bumper" << std::endl;
			    else if (it->kind == ImpactSensorKindCliff)
			    	std::cout << "Sensor Kind: Cliff" << std::endl;
			    else if (it->kind == ImpactSensorKindSonar)
			    	std::cout << "Sensor Kind: Sonar" << std::endl;		
			    else 
			    	return 1;
			    if (it->type == ImpactSensorTypeAnalog)
			    	std::cout << "Sensor Type: Analog" << std::endl;
			    else if (it->type == ImpactSensorTypeDigital)
			    	std::cout << "Sensor Type: Digital" << std::endl;		
			    else 
				    return 1;
				std::cout << "Sensor Position: ( " <<  it->pose.x() << " , " << it->pose.y() << " , " << \
				    it->pose.z() << " ) ;  Yaw = " << it->pose.yaw() << std::endl;
			    ImpactSensorValue value;
				sdp.getSensorValue(it->id, value);
				std::cout << "Sensor Value : " << value.value<< std::endl; 
			}		
	    }       
		return 0;
	} 
	catch (const rpos::system::detail::ExceptionBase& e) {		
		std::cout << e.what() << std::endl;
		return 1;	
	}
}