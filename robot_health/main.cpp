/*
 * main.cpp
 *
 * Created by Slamtec at 2018-02-07
 * Copyright 2018 (c) Shanghai Slamtec Co., Ltd.
 */
#include <regex>
#include <rpos/robot_platforms/slamware_core_platform.h>

using namespace rpos::robot_platforms;
using namespace rpos::features::motion_planner;
using namespace rpos::features::artifact_provider;
using namespace rpos::features::system_resource;

std::string ip_address = "";
const char *ip_regex = "\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}";

void ShowHelp(std::string app_name) {
	std::cout << "SLAMWARE console demo." << std::endl << \
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
	if (!opt_show_help && !std::regex_match(ip_address, reg)) 
		opt_show_help = true;
	if (opt_show_help) {
		ShowHelp("robot_health");
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

		while(true){
			BaseHealthInfo robot_health = sdp.getRobotHealth();
			if(robot_health.hasError)
				std::cout << "Error" << std::endl;
			if(robot_health.hasFatal)
				std::cout << "Fatal" << std::endl;
			if(robot_health.hasWarning)
				std::cout << "Warning" << std::endl;
			if(*robot_health.hasLidarDisconnected)
				std::cout << "LidarDisconnected" << std::endl;
			if(*robot_health.hasSdpDisconnected)
				std::cout << "SdpDisconnected" << std::endl;
			if(*robot_health.hasSystemEmergencyStop)
				std::cout << "SystemEmergencyStop" << std::endl;
			for (auto it = robot_health.errors.begin();it != robot_health.errors.end(); ++ it) {
				std::cout << "Message: " << it->message << std::endl;
				std::cout << "Level: " << it->level << std::endl;
				std::cout << "ErrorType: "<< it->componentErrorType <<std::endl;
				std::cout << "ErrorCode: " << it->errorCode << std::endl;
			}

			int errors_size = robot_health.errors.size();
			if(errors_size > 0){
				std::cout << "Press 'y' to clear errors, press any other key to continue..." << std::endl;
				char is_error_clear;
				std::cin >> is_error_clear;
				if(is_error_clear == 'y' || is_error_clear == 'Y') {
					for (auto it = robot_health.errors.begin();it != robot_health.errors.end(); ++ it) {
						sdp.clearRobotHealth(it->errorCode);
						std::cout << "Error: " << it->message << " cleared!" << std::endl;
					}			
				}				
			}
		}

	}
	catch (const rpos::system::detail::ExceptionBase& e) {		
		std::cout << e.what() << std::endl;
		return 1;	
	}
	return 0;
} 