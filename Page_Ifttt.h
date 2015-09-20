#ifndef PAGE_IFTTT_H
#define PAGE_IFTTT_H

//
//   The EXAMPLE PAGE
//
const char PAGE_ifttt[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

  <a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>IFTTT Configuration</strong>

  <hr>
Set IFTTT key and event values:<br>

   
  <form action="" method="get">
    <table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
      <tr><td align="right">Key:</td><td><input type="text" id="iftttKey" name="iftttKey" value=""></td></tr>
      <tr><td align="right">Event:</td><td><input type="text" id="iftttEvent" name="iftttEvent" value=""></td></tr>
      <tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Update"></td></tr>    
    </table>
  </form>

  <hr>
  <div>Generated Url:</div>
  <div id="mydynamicdata"> -- None -- </div>
   <script>                

    function GetIftttValues()
    {
      setValues("/admin/iftttvalues");
    }

		window.onload = function ()
		{
			load("style.css","css", function() 
			{
				load("microajax.js","js", function() 
				{
					  setValues("/admin/iftttvalues");  //-- this function calls the function on the ESP  
            //setTimeout(GetIftttValues,3000);
				});
			});
		}
		function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



   </script>

)=====";
#endif

String get_ifttt_trigger_url() {
  return "/trigger/" + (String) config.iftttEvent + "/with/key/" + (String) config.iftttKey;
}

String get_ifttt_trigger_host() {
  return "https://maker.ifttt.com";
}


void send_ifttt_values_html()
{        
    String values ="";
    values += "mydynamicdata|" + get_ifttt_trigger_host() + get_ifttt_trigger_url() + "|div\n";   // Build a string, like this:  ID|VALUE|TYPE
    values += "iftttKey|" + (String) config.iftttKey + "|input\n";
    values += "iftttEvent|" +  (String) config.iftttEvent + "|input\n";

    server.send ( 200, "text/plain", values);   
}

void send_ifttt_configuration_html()
{        
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            if (server.argName(i) == "iftttKey") { config.iftttKey =    urldecode(server.arg(i));}
            if (server.argName(i) == "iftttEvent") { config.iftttEvent =    urldecode(server.arg(i));}
        }
        WriteConfig();
    }
    server.send ( 200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_ifttt)); 
    Serial.println(__FUNCTION__); 
}
