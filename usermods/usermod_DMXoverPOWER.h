#pragma once

#include "wled.h"


//class name. Use something descriptive and leave the ": public Usermod" part :)
class DMXoverPOWER : public Usermod {

  private:

    // Private class members. You can declare variables and functions only accessible to your usermod here
    bool enabled = false;
    unsigned long lastTime;
    bool initDone;

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


    // any private methods should go here (non-inline method should be defined out of class)
    void publishMqtt(const char* state, bool retain = false); // example for publishing MQTT message


  public:

    // non WLED related methods, may be used for data exchange between usermods (non-inline methods should be defined out of class)

    /**
     * Enable/Disable the usermod
     */
    inline void enable(bool enable) { enabled = enable; }

    /**
     * Get usermod enabled/disabled state
     */
    inline bool isEnabled() { return enabled; }

    // in such case add the following to another usermod:
    //  in private vars:
    //   #ifdef USERMOD_EXAMPLE
    //   DMXoverPOWER* UM;
    //   #endif
    //  in setup()
    //   #ifdef USERMOD_EXAMPLE
    //   UM = (DMXoverPOWER*) usermods.lookup(USERMOD_ID_EXAMPLE);
    //   #endif
    //  somewhere in loop() or other member method
    //   #ifdef USERMOD_EXAMPLE
    //   if (UM != nullptr) isExampleEnabled = UM->isEnabled();
    //   if (!isExampleEnabled) UM->enable(true);
    //   #endif


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

      // do your magic here
      if (millis() - lastTime > 1000) {
        //Serial.println("I'm alive!");
        lastTime = millis();
      }
    }


    /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     */
    void addToJsonInfo(JsonObject& root)
    {
      // if "u" object does not exist yet wee need to create it
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");
    }


    /*
     * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void addToJsonState(JsonObject& root)
    {
      if (!initDone || !enabled) return;  // prevent crash on boot applyPreset()

      JsonObject usermod = root[FPSTR(_name)];
      if (usermod.isNull()) usermod = root.createNestedObject(FPSTR(_name));
    }


    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void readFromJsonState(JsonObject& root)
    {
      if (!initDone) return;  // prevent crash on boot applyPreset()

      JsonObject usermod = root[FPSTR(_name)];
      if (!usermod.isNull()) {
        // expect JSON usermod data in usermod name object: {"ExampleUsermod:{"user0":10}"}
        userVar0 = usermod["user0"] | userVar0; //if "user0" key exists in JSON, update, else keep old value
      }
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
      /*top[FPSTR(_yamar_startup_dmx_value)] = Yamar_Startup_DMXvalue;
      top[FPSTR(_yamar_startup_delay)] = Yamar_Startup_Delay;
      */

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
      /*Yamar_Startup_DMXvalue = top[FPSTR(_yamar_startup_dmx_value)] | Yamar_Startup_DMXvalue;
      Yamar_Startup_DMXvalue = (uint8_t) min(255,max(0,(int)Yamar_Startup_DMXvalue)); // bounds checking
      Yamar_Startup_Delay = top[FPSTR(_yamar_startup_delay)] | Yamar_Startup_Delay;
      Yamar_Startup_Delay = (uint16_t) min(25000,max(5000,(int)Yamar_Startup_Delay)); // bounds checking
      */
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


    /*
     * handleOverlayDraw() is called just before every show() (LED strip update frame) after effects have set the colors.
     * Use this to blank out some LEDs or set them to a different color regardless of the set effect mode.
     * Commonly used for custom clocks (Cronixie, 7 segment)
     */
    void handleOverlayDraw()
    {
      //strip.setPixelColor(0, RGBW32(0,0,0,0)) // set the first pixel to black
    }


    /**
     * handleButton() can be used to override default button behaviour. Returning true
     * will prevent button working in a default way.
     * Replicating button.cpp
     */
    bool handleButton(uint8_t b) {
      yield();
      // ignore certain button types as they may have other consequences
      if (!enabled
       || buttonType[b] == BTN_TYPE_NONE
       || buttonType[b] == BTN_TYPE_RESERVED
       || buttonType[b] == BTN_TYPE_PIR_SENSOR
       || buttonType[b] == BTN_TYPE_ANALOG
       || buttonType[b] == BTN_TYPE_ANALOG_INVERTED) {
        return false;
      }

      bool handled = false;
      // do your button handling here
      return handled;
    }
  

#ifndef WLED_DISABLE_MQTT
    /**
     * handling of MQTT message
     * topic only contains stripped topic (part after /wled/MAC)
     */
    bool onMqttMessage(char* topic, char* payload) {
      // check if we received a command
      //if (strlen(topic) == 8 && strncmp_P(topic, PSTR("/command"), 8) == 0) {
      //  String action = payload;
      //  if (action == "on") {
      //    enabled = true;
      //    return true;
      //  } else if (action == "off") {
      //    enabled = false;
      //    return true;
      //  } else if (action == "toggle") {
      //    enabled = !enabled;
      //    return true;
      //  }
      //}
      return false;
    }

    /**
     * onMqttConnect() is called when MQTT connection is established
     */
    void onMqttConnect(bool sessionPresent) {
      // do any MQTT related initialisation here
      //publishMqtt("I am alive!");
    }
#endif


    /**
     * onStateChanged() is used to detect WLED state change
     * @mode parameter is CALL_MODE_... parameter used for notifications
     */
    void onStateChange(uint8_t mode) {
      // do something if WLED state changed (color, brightness, effect, preset, etc)
    }


    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId()
    {
      return USERMOD_ID_EXAMPLE;
    }

   //More methods can be added in the future, this example will then be extended.
   //Your usermod will remain compatible as it does not need to implement all methods from the Usermod base class!
};


// add more strings here to reduce flash memory usage
const char DMXoverPOWER::_name[]    PROGMEM = "DMX Over Power Lines";
const char DMXoverPOWER::_enabled[] PROGMEM = "enabled";
const char DMXoverPOWER::_yamar_startup_dmx_value[] PROGMEM = "Yamar startup DMX value";
const char DMXoverPOWER::_yamar_startup_delay[] PROGMEM = "Yamar startup delay";



// implementation of non-inline member methods

void DMXoverPOWER::publishMqtt(const char* state, bool retain)
{
#ifndef WLED_DISABLE_MQTT
  //Check if MQTT Connected, otherwise it will crash the 8266
  if (WLED_MQTT_CONNECTED) {
    char subuf[64];
    strcpy(subuf, mqttDeviceTopic);
    strcat_P(subuf, PSTR("/example"));
    mqtt->publish(subuf, 0, retain, state);
  }
#endif
}
