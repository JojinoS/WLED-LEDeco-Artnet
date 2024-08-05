#pragma once

#include "wled.h"


//class name. Use something descriptive and leave the ": public Usermod" part :)
class DMXoverPOWER : public Usermod {

  private:

    // Private class members. You can declare variables and functions only accessible to your usermod here
    bool enabled = false;

    int Yamar_Startup_DMXvalue = 20;
    unsigned long Yamar_Startup_Delay = 10000;
    int Yamar_TXmode = 1;
    int Yamar_TXlevel = 0;
    int Yamar_ECC;
    int Yamar_FREQ_SEL = 5;

    // string that are used multiple time (this will save some flash memory)
    static const char _name[];
    static const char _enabled[];
    static const char _yamar_startup_dmx_value[];
    static const char _yamar_startup_delay[];


  public:

    // methods called by WLED (can be inlined as they are called only once but if you call them explicitly define them out of class)

    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * readFromConfig() is called prior to setup()
     * You can use it to initialize variables, sensors or similar.
     */
    void setup() {
      // do your set-up here

    }


    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() {
      //Serial.println("Connected to WiFi!");
    }


    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     * 
     * Tips:
     * 1. You can use "if (WLED_CONNECTED)" to check for a successful network connection.
     *    Additionally, "if (WLED_MQTT_CONNECTED)" is available to check for a connection to an MQTT broker.
     * 
     * 2. Try to avoid using the delay() function. NEVER use delays longer than 10 milliseconds.
     *    Instead, use a timer check as shown here.
     */
    void loop() {
      // if usermod is disabled or called during strip updating just exit
      // NOTE: on very long strips strip.isUpdating() may always return true so update accordingly
      if (!enabled || strip.isUpdating()) return;

    }

    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     * If you want to force saving the current state, use serializeConfig() in your loop().
     * 
     * CAUTION: serializeConfig() will initiate a filesystem write operation.
     * It might cause the LEDs to stutter and will cause flash wear if called too often.
     * Use it sparingly and always in the loop, never in network callbacks!
     * 
     * addToConfig() will make your settings editable through the Usermod Settings page automatically.
     *
     * Usermod Settings Overview:
     * - Numeric values are treated as floats in the browser.
     *   - If the numeric value entered into the browser contains a decimal point, it will be parsed as a C float
     *     before being returned to the Usermod.  The float data type has only 6-7 decimal digits of precision, and
     *     doubles are not supported, numbers will be rounded to the nearest float value when being parsed.
     *     The range accepted by the input field is +/- 1.175494351e-38 to +/- 3.402823466e+38.
     *   - If the numeric value entered into the browser doesn't contain a decimal point, it will be parsed as a
     *     C int32_t (range: -2147483648 to 2147483647) before being returned to the usermod.
     *     Overflows or underflows are truncated to the max/min value for an int32_t, and again truncated to the type
     *     used in the Usermod when reading the value from ArduinoJson.
     * - Pin values can be treated differently from an integer value by using the key name "pin"
     *   - "pin" can contain a single or array of integer values
     *   - On the Usermod Settings page there is simple checking for pin conflicts and warnings for special pins
     *     - Red color indicates a conflict.  Yellow color indicates a pin with a warning (e.g. an input-only pin)
     *   - Tip: use int8_t to store the pin value in the Usermod, so a -1 value (pin not set) can be used
     *
     * See usermod_v2_auto_save.h for an example that saves Flash space by reusing ArduinoJson key name strings
     * 
     * If you need a dedicated settings page with custom layout for your Usermod, that takes a lot more work.  
     * You will have to add the setting to the HTML, xml.cpp and set.cpp manually.
     * See the WLED Soundreactive fork (code and wiki) for reference.  https://github.com/atuline/WLED
     * 
     * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
     */
    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR(_enabled)] = enabled;
      top["Yamar Startup DMXvalue"] = Yamar_Startup_DMXvalue;
      top["Yamar Startup delay"] = Yamar_Startup_Delay;
      top["Yamar TX mode"] = Yamar_TXmode;
      top["Yamar TX level"] = Yamar_TXlevel;
      top["Yamar ECC"] = Yamar_ECC;
      top["Yamar Freq. selector"] = Yamar_FREQ_SEL;
    }


    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens immediately after boot, or after saving on the Usermod Settings page)
     * 
     * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
     * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
     * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
     * 
     * Return true in case the config values returned from Usermod Settings were complete, or false if you'd like WLED to save your defaults to disk (so any missing values are editable in Usermod Settings)
     * 
     * getJsonValue() returns false if the value is missing, or copies the value into the variable provided and returns true if the value is present
     * The configComplete variable is true only if the "exampleUsermod" object and all values are present.  If any values are missing, WLED will know to call addToConfig() to save them
     * 
     * This function is guaranteed to be called on boot, but could also be called every time settings are updated
     */
    bool readFromConfig(JsonObject& root)
    {

      JsonObject top = root[FPSTR(_name)];

      bool configComplete = !top.isNull();

      enabled = top[FPSTR(_enabled)] | enabled;
      configComplete &= getJsonValue(top["Yamar Startup DMXvalue"], Yamar_Startup_DMXvalue);
      configComplete &= getJsonValue(top["Yamar Startup delay"], Yamar_Startup_Delay);
      configComplete &= getJsonValue(top["Yamar TX mode"], Yamar_TXmode, 1);
      configComplete &= getJsonValue(top["Yamar TX level"], Yamar_TXlevel);
      configComplete &= getJsonValue(top["Yamar ECC"], Yamar_ECC);
      configComplete &= getJsonValue(top["Yamar Freq. selector"], Yamar_FREQ_SEL);

      return configComplete;
    }
    /*
     * appendConfigData() is called when user enters usermod settings page
     * it may add additional metadata for certain entry fields (adding drop down is possible)
     * be careful not to add too much as oappend() buffer is limited to 3k
     */
    void appendConfigData()
    {
      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F(":Yamar Startup delay")); oappend(SET_F("',1,'milliseconds');"));
      oappend(SET_F("txm=addDropdown('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F("','Yamar TX mode');"));
      oappend(SET_F("addOption(txm,'UART',0);"));
      oappend(SET_F("addOption(txm,'DMX',1);"));
      oappend(SET_F("addOption(txm,'SPI',10);"));
      oappend(SET_F("addOption(txm,'I2C',11);"));
      oappend(SET_F("txl=addDropdown('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F("','Yamar TX level');"));
      oappend(SET_F("addOption(txl,'1Vpp',0);"));
      oappend(SET_F("addOption(txl,'2Vpp',1);"));
      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F(":Yamar ECC")); oappend(SET_F("',1,'Future implementation');"));
      oappend(SET_F("fs=addDropdown('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F("','Yamar Freq. selector');"));
      oappend(SET_F("addOption(fs,'5 Mhz',0);"));
      oappend(SET_F("addOption(fs,'6 Mhz',1);"));
      oappend(SET_F("addOption(fs,'7 Mhz',2);"));
      oappend(SET_F("addOption(fs,'8 Mhz',3);"));
      oappend(SET_F("addOption(fs,'9 Mhz',4);"));
      oappend(SET_F("addOption(fs,'10 Mhz',5);"));
      oappend(SET_F("addOption(fs,'11.4 Mhz',6);"));
      oappend(SET_F("addOption(fs,'12 Mhz',7);"));
      oappend(SET_F("addOption(fs,'13 Mhz',8);"));
      oappend(SET_F("addOption(fs,'14 Mhz',9);"));
      oappend(SET_F("addOption(fs,'14.6 Mhz',10);"));
      oappend(SET_F("addOption(fs,'15 Mhz',11);"));
      oappend(SET_F("addOption(fs,'16 Mhz',12);"));
      oappend(SET_F("addOption(fs,'17 Mhz',13);"));
      oappend(SET_F("addOption(fs,'18 Mhz',14);"));
      oappend(SET_F("addOption(fs,'19 Mhz',15);"));
      oappend(SET_F("addOption(fs,'20 Mhz',17);"));
    }
};

// add more strings here to reduce flash memory usage
const char DMXoverPOWER::_name[]    PROGMEM = "DMX Over Power Lines";
const char DMXoverPOWER::_enabled[] PROGMEM = "enabled";
const char DMXoverPOWER::_yamar_startup_dmx_value[] PROGMEM = "Yamar startup DMX value";
const char DMXoverPOWER::_yamar_startup_delay[] PROGMEM = "Yamar startup delay";