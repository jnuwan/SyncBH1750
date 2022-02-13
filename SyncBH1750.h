#ifndef SyncBH1750_h
#define SyncBH1750_h

#include "Arduino.h"
#include "Wire.h"

/** The default I2C address for BH1750FVI */
#define BH1750_ADDR_MSB (0x5C)
#define BH1750_ADDR_LSB (0x23)

class SyncBH1750 {    

  public:

    /** Power state modes for BH1750FVI.
     *  Ref. Datasheet Rev.D (Nov 2011) P5 [Instruction set Architecture]
    */
    enum bh1750State {
			BH1750_POWER_DOWN = 0x00,//< Power off
			BH1750_POWER_ON = 0x01,//< Power on
			BH1750_RESET = 0x07//< Reset
    };

		/** Sensor sensitivity adjusting for BH1750FVI.
     *  Ref. Datasheet Rev.D (Nov 2011) P11 [Adjust measurement result for influence of optical window]
    */
		enum bh1750MtregLimit {
			BH1750_MTREG_MIN = 31, //the datashet specifies 31 as minimum value
			//but you can go even lower (depending on your specific chip)
			//BH1750_MTREG_LOW=5 works with my chip and enhances the range
			//from 121.556,8 Lux to 753.652,5 Lux.
			BH1750_MTREG_DEFAULT = 69,
			BH1750_MTREG_MAX = 254
		};

		/** Sensor resolution mode adjusting for BH1750FVI.
     *  Ref. Datasheet Rev.D (Nov 2011) P5 [Instruction set Architecture]
    */
		enum bh1750ResolutionMode {
      BH1750_CONT_HIGH_RES = 0x10,//< 1lx, measure time 120ms
      BH1750_CONT_HIGH_RES2 = 0x11,//< 0.5lx, measure time 120ms
      BH1750_CONT_LOW_RES = 0x13,//< 4lx, measure time 16ms
      BH1750_ONETIME_HIGH_RES = 0x20,//< 1lx, measure time 120ms
      BH1750_ONETIME_HIGH_RES2 = 0x21,//< 0.5lx, measure time 120ms
      BH1750_ONETIME_LOW_RES = 0x23//< 4lx, measure time 16ms
    };

		SyncBH1750();
    void begin(bh1750ResolutionMode mode);
		void configureMode(bh1750ResolutionMode mode);
    void sleep(bool state); 
    void reset();
    float getLightIntensity();

  private:

		void writeMtreg(bh1750MtregLimit mtreg);
		void writeWithoutRegister(uint8_t I2CAddress, uint8_t data);
		uint8_t _bh1750I2CAddress;
		bh1750MtregLimit _bh1750MtregLimit;
		bh1750ResolutionMode _bh1750ResolutionMode;

};

#endif