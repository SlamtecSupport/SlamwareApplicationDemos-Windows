/*
 * main.cpp
 *
 * Created by Slamtec at 2018-02-07
 * Copyright 2018 (c) Shanghai Slamtec Co., Ltd.
 */
#include <regex>
#include <rpos/robot_platforms/slamware_core_platform.h>

using namespace rpos::robot_platforms;

std::string ip_address = "";
const int max_go_home_times = 3;
const char *ip_regex = "\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}";

void ShowHelp(std::string app_name){
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
        ShowHelp("go_home_to_charge");
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
		int go_home_count = 1;
		rpos::actions::MoveAction action;
		do {
			action = sdp.goHome();
			action.waitUntilDone();
			if(action.getStatus() == rpos::core::ActionStatusFinished)
				break;
			std::cout << "go home to charge times count: "  << go_home_count << std::endl;
			go_home_count++;
		}
		while(go_home_count <= max_go_home_times);
		return 0;
	}
	catch (const rpos::system::detail::ExceptionBase& e) {		
		std::cout << e.what() << std::endl;
		return 1;	
	}
}