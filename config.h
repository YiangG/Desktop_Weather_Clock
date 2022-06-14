//wifi name and passwd
const char *ssid     = "ASUS_50_2G";
const char *password = "agenda_2633";

//openweather map variable and api key
String openWeatherMapApiKey = "483866ef827ce2e7fe1012b3dd90d4c8";
String city = "Philadelphia";
String countryCode = "US";
String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;

