
//
//  HTML PAGE
//

const char PAGE_ColorPicker[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>RGB led Color Pick</strong>
<hr>
<form action="" method="post">
 <table>
 
  <tr>
   <td align="left">R:</td>
   <td><input type=range min=0 max=1023 value=0 id=red_led name=red_led step=1  oninput="red_ledUpdate(value)"></td>
   <td><output for=red_led id=PWM_RED>0</output></td>
  </tr>

  <tr>
   <td align="left">G:</td>
   <td><input type=range min=0 max=1023 value=0 id=green_led name=green_led step=1  oninput="green_ledUpdate(value)"></td>
   <td><output for=green_led id=PWM_GREEN>0</output></td>
  </tr>

  <tr>
   <td align="left">B:</td>
   <td><input type=range min=0 max=1023 value=0 id=blue_led name=blue_led step=1  oninput="blue_ledUpdate(value)"></td>
   <td><output for=blue_led id=PWM_BLUE>0</output></td>
  </tr>
 
  <hr>

  <tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Update"></td></tr>
 </table>
</form>



<script>

 function red_ledUpdate(red_pwm) {document.querySelector('#PWM_RED').value = red_pwm;}
 function green_ledUpdate(green_pwm) {document.querySelector('#PWM_GREEN').value = green_pwm;}
 function blue_ledUpdate(blue_pwm) {document.querySelector('#PWM_BLUE').value = blue_pwm;}

 window.onload = function ()
 {
   load("style.css","css", function() 
   {
     load("microajax.js","js", function() 
     {
         setValues("/admin/colorpickervalues");
     });
   });
 }
 function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
</script>
)=====";




// Functions for this Page

void send_colorpicker_configuration_values_html()
{       
  int num = server.args();
	Serial.print("Number of server args =  "); 
	Serial.println(num); 
    
	if (server.args() > 0 )  // Save Settings
	{
		String temp = "";
		for ( uint8_t i = 0; i < server.args(); i++ ) {
        ;
        if (server.argName(i) == "red_led")   config.LED_R = server.arg(i).toInt(); 
        if (server.argName(i) == "green_led") config.LED_G = server.arg(i).toInt(); 
        if (server.argName(i) == "blue_led")  config.LED_B = server.arg(i).toInt(); 
		}
  }
	//WriteConfig();
	server.send ( 200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_ColorPicker )); 
	Serial.println(__FUNCTION__); 
	
}

void send_colorpicker_html()  //Not used, keep as template
{
  
	String values ="";
	//values += "zone_one_enabled|"   +  (String) (config.ZoneOneEnabled ?  "checked" : "") + "|chk\n";
	//values += "zone_two_enabled|"   +  (String) (config.ZoneTwoEnabled ?  "checked" : "") + "|chk\n";
	//values += "zone_three_enabled|" +  (String) (config.ZoneThreeEnabled ?  "checked" : "") + "|chk\n";
	//values += "zone_four_enabled|"  +  (String) (config.ZoneFourEnabled ?  "checked" : "") + "|chk\n";
	//values += "zone_five_enabled|"  +  (String) (config.ZoneFiveEnabled ?  "checked" : "") + "|chk\n";
	//values += "zone_six_enabled|"   +  (String) (config.ZoneSixEnabled ?  "checked" : "") + "|chk\n";
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
}
