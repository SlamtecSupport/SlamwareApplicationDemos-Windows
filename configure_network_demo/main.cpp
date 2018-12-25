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
bool opt_AP_mode = false;
bool opt_STA_mode = false;
bool opt_disable_wifi = false;
bool opt_disable_dhcp = false;
bool opt_enable_dhcp = false;
const char *ip_regex = "\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}";

void ShowHelp(std::string app_name) {
	std::cout << "SLAMWARE console demo." << std::endl << "Usage: " << std::endl << 
		app_name << " [OPTS] <SDP IP Address>" << std::endl << 
		"slamware_address    The ip address string of the SLAMWARE SDP." << std::endl << 
		"AP                  Configurate network in AP mode." << std::endl << 
		"STA                 Configurate network in STA mode." << std::endl << 
		"--disable-dhcp      disable dhcp." << std::endl << 
		"--enable-dhcp       enable dhcp." << std::endl << 
		"--disable-wifi      disable wifi" << std::endl << 
		"-h                  Show this message" << std::endl;
}


bool ParseCommandLine(int argc, const char *argv[]) {
    bool opt_show_help = false;
    for (int pos = 1; pos < argc; ++pos) {
        const char *current = argv[pos];
        if (strcmp(current, "-h") == 0)
            opt_show_help = true;
		else if (strcmp(current, "AP") == 0)
		    opt_AP_mode = true;	
        else if (strcmp(current, "STA") == 0)
		    opt_STA_mode = true;
		else if (strcmp(current, "--disable-wifi") == 0)
		   opt_disable_wifi = true;
		else if (strcmp(current, "--disable-dhcp") == 0)
		   opt_disable_dhcp = true;
		else if (strcmp(current, "--enable-dhcp") == 0)
		   opt_enable_dhcp = true;
        else
            ip_address = current;
    }
    std::regex reg(ip_regex);
    if (!opt_show_help && !std::regex_match(ip_address, reg)) 
        opt_show_help = true;
    if (opt_show_help) {
        ShowHelp("configure_network_demo");
        return false;
    }
    return true;
}

int main(int argc, const char * argv[]) {
	if (!ParseCommandLine(argc, argv)) return 1;
	std::cout << "Connecting SDP @ " << ip_address << "..."<< std::endl;
	try {
		SlamwareCorePlatform sdp = SlamwareCorePlatform::connect(ip_address, 1445);
        std::cout <<"SDK Version: " << sdp.getSDKVersion() << std::endl;
        std::cout <<"SDP Version: " << sdp.getSDPVersion() << std::endl;
        std::map<std::string, std::string> options;
		bool result = false;
		if (opt_AP_mode == true) {
			std::cout << "Configure network in AP mode" << std::endl;
    		std::cout << "ssid: test" << std::endl;
			std::cout << "password: 12345678" << std::endl;
			std::cout << "IP: 192.168.11.101" << std::endl;
			std::cout << "channel: 6" << std::endl;

			options["ssid"] = "test";      
			options["password"] = "12345678"; // password length should surpass 8 
			options["ip"] = "192.168.11.101"; // do not use address from 192.168.11.1 to 192.168.11.100 (reserved for internal usage)
			options["channel"] = "6";
			result = sdp.configurateNetwork(NetworkMode::NetworkModeAP, options);
		} 
		else if (opt_STA_mode == true) {
			std::cout << "Configure network in station mode" << std::endl;
            std::cout << "Please enter Wifi Name: " << std::endl;
			std::string ssid;
			std::cin >> ssid;
			std::cout << "Please enter Wifi password: " << std::endl;
			std::string password;
			std::cin >> password;

            options.clear();
			options["ssid"] = ssid;
			options["password"] = password;
			result = sdp.configurateNetwork(NetworkMode::NetworkModeStation, options);	
            
		}
	    else if (opt_enable_dhcp == true) {
			std::cout << "enable DHCP" << std::endl;
            options.clear();
			result = sdp.configurateNetwork(NetworkModeDHCPEnabled, options);
		}
		else if (opt_disable_dhcp == true) {
			std::cout << "disable DHCP" << std::endl;
            options.clear();
			result = sdp.configurateNetwork(NetworkModeDHCPEnabled, options);
		}
		else if (opt_disable_wifi == true) {
			std::cout << "disable wifi" << std::endl;
            options.clear();
			result = sdp.configurateNetwork(NetworkMode::NetworkModeWifiDisabled, options);			
		} 
		else 
			return 1;
		if(result == true)
			std::cout << "Configuring network, please use the new setting to connect the next time" << std::endl;
		else
			std::cout << "Configure failed!" << std::endl;
		return 0;		
	} 
	catch (const rpos::system::detail::ExceptionBase &e) {		
		std::cout << e.what() << std::endl;
		return 1;	
	}
}