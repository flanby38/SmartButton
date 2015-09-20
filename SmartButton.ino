/* 
  Name: Arduino Smat Button 
  Date: Sept. 16, 2015

  Copyright (c) 2015 S. Carriere. All rights reserved.
  Copyright (c) 2015 R. Wynn. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Latest version: 1.1.3  - 2015-07-20
  Changed the loading of the Javascript and CCS Files, so that they will successively loaded and that only one request goes to the ESP.

  -----------------------------------------------------------------------------------------------
  History

  Version: 1.1.2  - 2015-07-17
  Added URLDECODE for some input-fields (SSID, PASSWORD...)

  Version  1.1.1 - 2015-07-12
  First initial version to the public

  
  */


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <Button.h>

#include "helpers.h"
#include "global.h"
/*
Include the HTML, STYLE and Script "Pages"
*/
#include "Page_Root.h"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPSettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"

#include "Page_Ifttt.h"

#include "Page_ColorPicker.h"


#define ACCESS_POINT_NAME  "ESP"				
#define ACCESS_POINT_PASSWORD  "12345678" 
#define AdminTimeOut 600  // Defines the Time in Seconds, when the Admin-Mode will be diabled

int ledState = LOW; 
const int BUILTIN_BUTTON = 0;

const int BUILTIN_RED_LED = 0; 
const int BUILTIN_BLUE_LED = 2; 

const int buttonPin = 4;

Button button(buttonPin);

void setup ( void ) {
	EEPROM.begin(512);
	Serial.begin(115200);
	delay(500);
	Serial.println("Starting ESP8266");


  
	if (!ReadConfig())
	{
		// DEFAULT CONFIG
		config.ssid = "MYSSID";
		config.password = "MYPASSWORD";
		config.dhcp = true;
		config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 111;
		config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
		config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
		config.ntpServerName = "129.6.15.28";
		config.Update_Time_Via_NTP_Every =  0;
		config.timezone = -10;
		config.daylight = true;
		config.DeviceName = "Not Named";
		config.AutoTurnOff = false;
		config.AutoTurnOn = false;
		config.TurnOffHour = 0;
		config.TurnOffMinute = 0;
		config.TurnOnHour = 0;
		config.TurnOnMinute = 0;
		WriteConfig();
		Serial.println("General config applied");
	}

 	if (AdminEnabled)
	{
		WiFi.mode(WIFI_AP_STA);
		WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
	}
	else
	{
		WiFi.mode(WIFI_STA);
	}

	ConfigureWifi();
	

	server.on ( "/ifttt.html", send_ifttt_configuration_html  );
	server.on ( "/admin/iftttvalues", send_ifttt_values_html );
	
	server.on ( "/favicon.ico",   []() { Serial.println("favicon.ico"); server.send ( 200, "text/html", "" );   }  );

	server.on ( "/admin.html", []() { Serial.println("admin.html"); server.send ( 200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_AdminMainPage ));   }  );
	server.on ( "/config.html", send_network_configuration_html );
	server.on ( "/info.html", []() { Serial.println("info.html"); server.send ( 200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_Information ));   }  );
	server.on ( "/ntp.html", send_NTP_configuration_html  );
	server.on ( "/general.html", send_general_html  );
	server.on ( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/plain", reinterpret_cast<const __FlashStringHelper *>(PAGE_Style_css ));  } );
	server.on ( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "text/plain", reinterpret_cast<const __FlashStringHelper *>(PAGE_microajax_js ));  } );
	server.on ( "/admin/values", send_network_configuration_values_html );
	server.on ( "/admin/connectionstate", send_connection_state_values_html );
	server.on ( "/admin/infovalues", send_information_values_html );
	server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
	server.on ( "/admin/generalvalues", send_general_configuration_values_html);
	server.on ( "/admin/devicename",     send_devicename_value_html);
  
  server.on ( "/colorpicker.html",       send_colorpicker_configuration_values_html);
  server.on ( "/admin/colorpickervalues",send_colorpicker_html);
  
 	server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 400, "text/html", "Page not Found" );   }  );
	
	server.begin();
	Serial.println( "HTTP server started" );
	tkSecond.attach(1,Second_Tick);
	UDPNTPClient.begin(2390);  // Port for NTP receive

  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED, ledState);   // Turn the LED on (Note that LOW is the voltage level

//    button.on_release(on_release);                      // on NBP (if not set nothing will be done)
    button.on_press(on_press);                          // on NBsP (if not set nothing will be done)
//    button.on_long_press(on_long_press);                // on LBsP (if not set, on_press will be used for on_long_press events)
//    button.on_long_release(on_long_release);            // on LBP (if not set, on_release will be used for on_long_release events)
//    button.on_bounced_release(on_bounced_release);      // on BBP (junk, if not set nothing will be done)
//    button.on_bounced_press(on_bounced_press);      
}

void on_press() {
    Serial.print(String("button pressed immediately: ") + button.gap() + String(" milliseconds\n"));
callIfttt();
}

void callIfttt(void) {
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 443;

  String host = get_ifttt_trigger_host();
  char paramChar[host.length()+1];
  host.toCharArray(paramChar, host.length()+1);
  
  if (!client.connect(paramChar, httpPort)) {
    Serial.println("connection failed: " + (String)paramChar);
    return;
  }
  
  // We now create a URI for the request
  String url = get_ifttt_trigger_url();
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
}

void loop ( void ) {
  server.handleClient();

  button.init();

	if (AdminEnabled)
	{
		if (AdminTimeOutCounter > AdminTimeOut)
		{
			 AdminEnabled = false;
			 Serial.println("Admin Mode disabled!");
			 WiFi.mode(WIFI_STA);
		}
	}

	if (config.Update_Time_Via_NTP_Every  > 0 )
	{
		if (cNTP_Update > 5 && firstStart)
		{
			NTPRefresh();
			cNTP_Update =0;
			firstStart = false;
		}
		else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
		{

			NTPRefresh();
			cNTP_Update =0;
		}
	}


	if (Refresh)  // Called every one second-------------------------------------------------------
  {
		Refresh = false;
    
    if (ledState == LOW)
      ledState = HIGH;  // Note that this switches the LED *off*
    else
      ledState = LOW;   // Note that this switches the LED *on*
    digitalWrite(BUILTIN_LED, ledState);

  }

} //--------------End of Loop---------------------



