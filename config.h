//wifi name and passwd
const char *ssid     = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWARD";

//openweather map variable and api key
String openWeatherMapApiKey = "YOUR_OPENWEATHERMAP_API_KEY";
String city = "YOUR_CITY";
String countryCode = "YOUR_CITY_COUNTRY_CODE";
String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;

