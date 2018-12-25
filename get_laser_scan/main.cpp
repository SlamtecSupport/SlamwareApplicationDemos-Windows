/*
 * main.cpp
 *
 * Created by Slamtec at 2018-02-07
 * Copyright 2018 (c) Shanghai Slamtec Co., Ltd.
 */
#include <regex>
#include <rpos/robot_platforms/slamware_core_platform.h>
#include <boost/thread/thread.hpp>

using namespace rpos::robot_platforms;
using namespace rpos::features::system_resource;

std::string ip_address = "";
const char *ip_regex = "\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}";
SlamwareCorePlatform sdp;


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
    if (!opt_show_help && !std::regex_match(ip_address, reg)) 
        opt_show_help = true;
    if (opt_show_help) {
        ShowHelp("get_laser_scan");
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
        while(true){
		rpos::features::system_resource::LaserScan laser_scan = sdp.getLaserScan();
	    std::vector<rpos::core::LaserPoint> laser_points =laser_scan.getLaserPoints();		
		for (std::vector<rpos::core::LaserPoint>::iterator it = laser_points.begin(); it!= laser_points.end(); ++it) 
			std::cout << "Angle: " << it->angle() << "; Distance: " << 
			it->distance() << "; is Valid: " << it->valid() << std::endl;		
        }
		return 0;
	}
    catch(const rpos::system::detail::ExceptionBase& e){
        std::cout << e.what() << std::endl; 
        return 1;
    }

}