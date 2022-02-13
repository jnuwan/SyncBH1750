
#include "SyncBH1750.h"
#include "Arduino.h"

/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  BH1750 RELATED FUNCTIONS
 *	This library scale down for power saving mode using one time measurement.
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
SyncBH1750::SyncBH1750(){
	
}

/*!
 *	@brief Initialize and start the BH1750FVI
 *			This will take care of setting the address, mode and powering up the device
 *	@return void
 */
void SyncBH1750::begin(bh1750ResolutionMode mode){
	Wire.begin();
  Wire.beginTransmission(BH1750_ADDR_LSB);
  uint8_t error = Wire.endTransmission();
	if(error == 0){
    _bh1750I2CAddress = BH1750_ADDR_LSB;
  }
  else{
    Wire.beginTransmission(BH1750_ADDR_MSB);
    error = Wire.endTransmission();
    if(error == 0){
      _bh1750I2CAddress = BH1750_ADDR_MSB;
    }
  }
	writeWithoutRegister(_bh1750I2CAddress, BH1750_POWER_ON);// Turn it On 
	writeMtreg(BH1750_MTREG_DEFAULT);// Set measurement time as default

	/** Ref. Datasheet Rev.D (Nov 2011) P4 [Measurement Procedure] */
	configureMode(mode);///< Set resolution mode base on application.
}

/*!
 *	@brief Configure BH1750FVI Measurement Time register (MTreg) value
 *	@param MTreg a value between 32 and 254. Default: 69
 *	@return void
 */
void SyncBH1750::writeMtreg(bh1750MtregLimit mtreg){
	if (mtreg < BH1750_MTREG_MIN) {
		mtreg = BH1750_MTREG_MIN;
	}
	if(mtreg > BH1750_MTREG_MAX){
		mtreg = BH1750_MTREG_MAX;
	}
	_bh1750MtregLimit = mtreg;
  // Change sensitivity measurement time
  // We send two bytes: 3 Bits und 5 Bits, with a prefix.
  // High bit: 01000_MT[7,6,5]
  // Low bit:  011_MT[4,3,2,1,0]
  uint8_t hiByte = _bh1750MtregLimit >> 5;
  hiByte |= 0b01000000;
  writeWithoutRegister(_bh1750I2CAddress, hiByte);
  uint8_t loByte = _bh1750MtregLimit & 0b00011111;
  loByte |= 0b01100000;
	writeWithoutRegister(_bh1750I2CAddress, loByte);
}

/*!
 *	@brief Configure BH1750FVI with specified mode
 *	@param mode Measurement mode
 *	@return void
 */
void SyncBH1750::configureMode(bh1750ResolutionMode mode){
	writeWithoutRegister(_bh1750I2CAddress, mode);
	_bh1750ResolutionMode = mode;
	delay(10);
}

/*!
 *	@brief Activate sleep mode
 *	@param true: device goes to sleep
 *				false: device wake up
 *	@return void
 */
void SyncBH1750::sleep(bool state){
	if(state){
		writeWithoutRegister(_bh1750I2CAddress, BH1750_POWER_DOWN);
	}
  else{
		writeWithoutRegister(_bh1750I2CAddress, BH1750_POWER_ON);
		delay(120);
	}
}

/*!
 *	@brief Reset Data register value. Reset command is not acceptable in Power Down mode.
 *	@param void
 *	@return void
*/
void SyncBH1750::reset(){
  sleep(false);
	delay(10);
  writeWithoutRegister(_bh1750I2CAddress, BH1750_RESET);
}

/*!
 *	@brief Get the measured light intensity from the device.
 *	@param void
 *	@return The light intensity measured by the device in 'lux'
*/
float SyncBH1750::getLightIntensity(){
  float level = -1.0;
	if(_bh1750I2CAddress == BH1750_ADDR_LSB || _bh1750I2CAddress == BH1750_ADDR_MSB){
		Wire.beginTransmission(_bh1750I2CAddress);
		Wire.requestFrom(_bh1750I2CAddress, 2);
		uint16_t value = Wire.read();
		value <<= 8;
		value |= Wire.read();
		level = value;
	}

	/** Ref. Datasheet Rev.D (Nov 2011) P11 [Adjust measurement result for influence of optical window]	*/
	if (level != -1.0) {
    if (_bh1750MtregLimit != BH1750_MTREG_DEFAULT) {
      level *= (float)((byte)BH1750_MTREG_DEFAULT/(float)_bh1750MtregLimit);
    }
    if (_bh1750ResolutionMode == BH1750_CONT_HIGH_RES2 || _bh1750ResolutionMode == BH1750_ONETIME_HIGH_RES2) {
      level /= 2;
    }
    level /= 1.2;// Convert raw value to lux
	}
	return level;
}

/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  BASIC FUNCTIONS
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
/*!
 *	@brief Writes an 8 bit value over I2C
 *	@param I2CAddress -Slave device address
 *		data -Data
 *	@return value from selected register (one byte)
 */
 void SyncBH1750::writeWithoutRegister(uint8_t I2CAddress, uint8_t data){
	Wire.beginTransmission(I2CAddress);
	Wire.write(data);
	Wire.endTransmission();
}
