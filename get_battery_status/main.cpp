/*
 * main.cpp
 *
 * Created by Slamtec at 2018-02-07
 * Copyright 2018 (c) Shanghai Slamtec Co., Ltd.
 */
#include <regex>
#include <rpos/robot_platforms/slamware_core_platform.h>

using namespace rpos::robot_platforms;
using namespace rpos::features::system_resource;

std::string ip_address = "";
const char *ip_regex = "\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}";

void ShowHelp(std::string app_name) {
    std::cout << "SLAMWARE console demo." << std::endl << \
        "Usage: " << app_name << " <slamware_address>" << std::endl;
}

bool ParseCommandLine(int argc, const char * argv[]) {
    bool opt_show_help = false;
    for (int pos = 1; pos < argc; ++pos) {
        const char * current = argv[pos];
        if (strcmp(current, "-h") == 0) 
            opt_show_help = true;
		else 
            ip_address = current;       
    }
    std::regex reg(ip_regex);
    if (!opt_show_help && !std::regex_match(ip_address, reg)) 
        opt_show_help = true;
    if (opt_show_help) {
        ShowHelp("get_power_status");
        return false;
    }
    return true;
}

int main(int argc, const char *argv[]) {
	if (!ParseCommandLine(argc , argv)) return 1;
	std::cout << "Connecting SDP @ " << ip_address << "..."<< std::endl;
	try {
		SlamwareCorePlatform sdp = SlamwareCorePlatform::connect(ip_address, 1445);
        std::cout <<"SDK Version: " << sdp.getSDKVersion() << std::endl;
        std::cout <<"SDP Version: " << sdp.getSDPVersion() << std::endl;
		
		PowerStatus status = sdp.getPowerStatus();
		std::cout << "Battery Percentage: " << status.batteryPercentage << std::endl;
		std::cout << "is Charging: " << status.isCharging << std::endl;
		std::cout << "is DC connected: " << status.isDCConnected << std::endl;
		
		if(status.dockingStatus == DockingStatusNotOnDock)
			std::cout << "Docking status: Not on dock" << std::endl;
		else if(status.dockingStatus == DockingStatusOnDock)
			std::cout << "Docking status: On dock" << std::endl;
		else 
			std::cout << "Docking status: Unknown" << std::endl;	
		return 0;		
	}
	catch (const rpos::system::detail::ExceptionBase& e) {		
		std::cout << e.what() << std::endl;
		return 1;	
	}
}