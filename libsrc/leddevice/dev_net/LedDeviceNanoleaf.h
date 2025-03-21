#ifndef LEDEVICENANOLEAF_H
#define LEDEVICENANOLEAF_H

// LedDevice includes
#include <leddevice/LedDevice.h>
#include "ProviderRestApi.h"
#include "ProviderUdp.h"

// Qt includes
#include <QString>
#include <QNetworkAccessManager>

///
/// Implementation of the LedDevice interface for sending to
/// Nanoleaf devices via network by using the 'external control' protocol.
///
class LedDeviceNanoleaf : public ProviderUdp
{
public:
	///
	/// @brief Constructs LED-device for Nanoleaf LightPanels (aka Aurora) or Canvas
	///
	/// following code shows all configuration options
	/// @code
	/// "device" :
	/// {
	///     "type" : "nanoleaf"
	///     "host" : "hostname or IP",
	///     "token": "Authentication Token",
	/// },
	///@endcode
	///
	/// @param deviceConfig Device's configuration as JSON-Object
	///
	explicit LedDeviceNanoleaf(const QJsonObject& deviceConfig);

	///
	/// @brief Destructor of the LED-device
	///
	~LedDeviceNanoleaf() override;

	///
	/// @brief Constructs the LED-device
	///
	/// @param[in] deviceConfig Device's configuration as JSON-Object
	/// @return LedDevice constructed
	static LedDevice* construct(const QJsonObject& deviceConfig);

	///
	/// @brief Discover Nanoleaf devices available (for configuration).
	///
	/// @param[in] params Parameters used to overwrite discovery default behaviour
	///
	/// @return A JSON structure holding a list of devices found
	///
	QJsonObject discover(const QJsonObject& params) override;

	///
	/// @brief Get the Nanoleaf device's resource properties
	///
	/// Following parameters are required
	/// @code
	/// {
	///     "host"  : "hostname or IP",
	///     "token" : "authentication token",
	///     "filter": "resource to query", root "/" is used, if empty
	/// }
	///@endcode
	///
	/// @param[in] params Parameters to query device
	/// @return A JSON structure holding the device's properties
	///
	QJsonObject getProperties(const QJsonObject& params) override;

	///
	/// @brief Send an update to the Nanoleaf device to identify it.
	///
	/// Following parameters are required
	/// @code
	/// {
	///     "host"  : "hostname or IP",
	///     "token" : "authentication token",
	/// }
	///@endcode
	///
	/// @param[in] params Parameters to address device
	///
	void identify(const QJsonObject& params) override;

	/// @brief Add an API-token to the Nanoleaf device
	///
	/// Following parameters are required
	/// @code
	/// {
	///     "host"  : "hostname or IP",
	/// }
	///@endcode
	///
	/// @param[in] params Parameters to query device
	/// @return A JSON structure holding the authorization keys
	///
	QJsonObject addAuthorization(const QJsonObject& params) override;

protected:

	///
	/// @brief Initialise the Nanoleaf device's configuration and network address details
	///
	/// @param[in] deviceConfig the JSON device configuration
	/// @return True, if success
	///
	bool init(const QJsonObject& deviceConfig) override;

	///
	/// @brief Opens the output device.
	///
	/// @return Zero on success (i.e. device is ready), else negative
	///
	int open() override;

	///
	/// @brief Writes the RGB-Color values to the LEDs.
	///
	/// @param[in] ledValues The RGB-color per LED
	/// @return Zero on success, else negative
	//////
	int write(const std::vector<ColorRgb>& ledValues) override;

	///
	/// @brief Power-/turn on the Nanoleaf device.
	///
	/// @brief Store the device's original state.
	///
	bool powerOn() override;

	///
	/// @brief Power-/turn off the Nanoleaf device.
	///
	/// @return True if success
	///
	bool powerOff() override;

	///
	/// @brief Store the device's original state.
	///
	/// Save the device's state before hyperion color streaming starts allowing to restore state during switchOff().
	///
	/// @return True if success
	///
	bool storeState() override;

	///
	/// @brief Restore the device's original state.
	///
	/// Restore the device's state as before hyperion color streaming started.
	/// This includes the on/off state of the device.
	///
	/// @return True, if success
	///
	bool restoreState() override;

private:

	// Nanoleaf Panel Shapetypes
	enum SHAPETYPES {
		TRIANGLE = 0,
		RHYTM = 1,
		SQUARE = 2,
		CONTROL_SQUARE_PRIMARY = 3,
		CONTROL_SQUARE_PASSIVE = 4,
		POWER_SUPPLY = 5,
		HEXAGON_SHAPES = 7,
		TRIANGE_SHAPES = 8,
		MINI_TRIANGE_SHAPES = 9,
		SHAPES_CONTROLLER = 12,
		ELEMENTS_HEXAGONS = 14,
		ELEMENTS_HEXAGONS_CORNER = 15,
		LINES_CONECTOR = 16,
		LIGHT_LINES = 17,
		LIGHT_LINES_SINGLZONE = 18,
		CONTROLLER_CAP = 19,
        POWER_CONNECTOR = 20,
        NL_4D_LIGHTSTRIP =	29,
        SKYLIGHT_PANEL = 30,
        SKYLIGHT_CONTROLLER_PRIMARY = 31,
        SKYLIGHT_CONTROLLER_PASSIV = 32,
		HD_LIGHT_STRIP = 0
	};

	///
	/// @brief Initialise the access to the REST-API wrapper
	///
	/// @return True, if success
	///
	bool openRestAPI();

	///
	/// @brief Get Nanoleaf device details and configuration
	///
	/// @return True, if Nanoleaf device capabilities fit configuration
	///
	bool initLedsConfiguration();

	///
	/// @brief Change Nanoleaf device to External Control (UDP) mode
	///
	/// @return True, if success
	bool changeToExternalControlMode();
	///
	/// @brief Change Nanoleaf device to External Control (UDP) mode
	///
	/// @param[out] response from device
	///
	/// @return True, if success
	bool changeToExternalControlMode(QJsonDocument& resp);

	///
	/// @brief Discover Nanoleaf devices available (for configuration).
	/// Nanoleaf specific ssdp discovery
	///
	/// @return A JSON structure holding a list of devices found
	///
	QJsonArray discover();

	///
	/// @brief Get number of panels that can be used as LEds.
	///
	/// @return Number of usable LED panels
	///
	int getHwLedCount(const QJsonObject& jsonLayout) const;

	///
	/// @brief Check, if panelshape type has LEDs
	///
	/// @return True, if panel shape type has LEDs
	///
	bool hasLEDs(const SHAPETYPES& panelshapeType) const;

	///REST-API wrapper
	ProviderRestApi* _restApi;
	int	_apiPort;
	QString _authToken;

	bool _topDown;
	bool _leftRight;

	//Nanoleaf device details
	QString _deviceModel;
	QString _deviceFirmwareVersion;
	ushort _extControlVersion;

	/// The number of panels with LEDs
	int _panelLedCount;

	/// Array of the panel ids.
	QVector<int> _panelIds;

	QJsonObject _originalStateProperties;

	bool _isBrightnessOverwrite;
	int _brightness;

	QString _originalColorMode;
	bool _originalIsOn;
	int _originalHue;
	int _originalSat;
	int _originalCt;
	int _originalBri;
	QString _originalEffect;
	bool _originalIsDynEffect {false};

};

#endif // LEDEVICENANOLEAF_H
