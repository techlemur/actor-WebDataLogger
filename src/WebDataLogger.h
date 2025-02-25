/*
* This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2024 Sam Groveman
* 
* External libraries used:
* ArduinoJSON: https://arduinojson.org/
* 
* Contributors: Sam Groveman
*/

#pragma once
#include <Arduino.h>
#include <Actor.h>
#include <PeriodicTask.h>
#include <SensorManager.h>
#include <TimeInterface.h>
#include <Storage.h>
#include <ArduinoJson.h>
#include <Webhook.h>

/// @brief Logs sensor data locally
class WebDataLogger : public Actor, public PeriodicTask , public Webhook {
	protected:
		/// @brief Holds data logger configuration
		struct {
			/// @brief Url to send data to
			String url;

			/// @brief Enable data logging
			bool enabled;
		} current_config;

		/// @brief Full path to data file
		// String path;

		/// @brief Path to configuration file
		const String config_path = "/settings/act/WebDataLogger.json";

		bool enableLogging(bool enable);

		// bool createDataFile();

	public:
		bool begin();
		String getConfig();
		bool setConfig(String config, bool save);
		void runTask(long elapsed);	
};