#include "WebDataLogger.h"

bool WebDataLogger::begin() {
	// Set description
	Description.actionQuantity = 0;
	Description.type = "datalogger";
	Description.name = "Web Data Logger";
	bool result = false;
	if (!checkConfig(config_path)) {
		// Set defaults
		current_config = { .name = "WebDataLogger", .enabled = false };
		task_config = { .taskName = "WebDataLogger", .taskPeriod = 10000 };
		path = "/data/" + current_config.name;
		result = saveConfig(config_path, getConfig());
	} else {
		// Load settings
		result = setConfig(Storage::readFile(config_path), false);
	}
	return result;
}

/// @brief Enables the web data logger
/// @param enable True to enable, false to disable 
/// @return True on success
bool WebDataLogger::enableLogging(bool enable) {
	current_config.enabled = enable;
	return enableTask(enable);
}

/// @brief Sets the configuration for this device
/// @param config A JSON string of the configuration settings
/// @param save If the configuration should be saved to a file
/// @return True on success
bool WebDataLogger::setConfig(String config, bool save) {
	// Allocate the JSON document
  	JsonDocument doc;
	// Deserialize file contents
	DeserializationError error = deserializeJson(doc, config);
	// Test if parsing succeeds.
	if (error) {
		Logger.print(F("Deserialization failed: "));
		Logger.println(error.f_str());
		return false;
	}
	// Assign loaded values
	current_config.name = doc["name"].as<String>();
	current_config.enabled = doc["enabled"].as<bool>();
	task_config.taskPeriod = doc["samplingPeriod"].as<long>();
	task_config.url = doc["url"].as<String>();
	task_config.taskName = doc["taskName"].as<std::string>();
	path = "/data/" + current_config.name;
	enableLogging(current_config.enabled);
	if (save) {
		return saveConfig(config_path, getConfig());
	}
	return true;
}

/// @brief Logs current data from all sensors
/// @param elapsed The time in ms since this task was last called
void WebDataLogger::runTask(long elapsed) {
	if (taskPeriodTriggered(elapsed)) {
		String data = TimeInterface::getFormattedTime("%m-%d-%Y %T");
		// Allocate the JSON document
		JsonDocument doc;
		// Deserialize sensor info
		DeserializationError error = deserializeJson(doc, SensorManager::getLastMeasurement());
		// Test if parsing succeeds.
		if (error) {
			Logger.print(F("Deserialization failed: "));
			Logger.println(error.f_str());
			return;
		}

		// Prepare parameters for GET request
        std::map<String, String> params;
        params["timestamp"] = data;  // Add timestamp as a parameter

        // Extract sensor data and add to parameters
        int count = 1;
        for (const auto& m : doc["measurements"].as<JsonArray>()) {
            params["sensor" + String(count)] = m["value"].as<String>();
            count++;
        }

        // Send data via GET request using Webhook
        Webhook myWebhook(doc["url"]);  // Replace with your actual endpoint
        String response = myWebhook.getRequest(params);

        // Log the response
        Logger.println("Server Response: " + response);


		// for (const auto& m : doc["measurements"].as<JsonArray>()) {
		// 	data += "," + m["value"].as<String>();
		// }

		
		// data += '\n';
		// if (Storage::freeSpace() > data.length()) {
		// 	Storage::appendToFile(path, data);
		// }
	}
}


/// @brief Gets the current config
/// @return A JSON string of the config
String WebDataLogger::getConfig() {
	// Allocate the JSON document
	JsonDocument doc;
	// Assign current values
	doc["name"] = current_config.name;
	doc["enabled"] = current_config.enabled;
	doc["samplingPeriod"] = task_config.taskPeriod;
	doc["url"] = task_config.url;
	doc["taskName"] = task_config.taskName;

	// Create string to hold output
	String output;
	// Serialize to string
	serializeJson(doc, output);
	return output;
}