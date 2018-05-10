#pragma once

#include "Stream.h"
#include <map>

class CJsonParser
{
public:
	typedef std::string JsonValue;
	typedef std::map<std::string, JsonValue> JsonObject;

	static JsonObject Parse(Framework::CStream&);

private:

	enum STATE
	{
		STATE_INIT,
		STATE_INSIDE_SCOPE,
		STATE_READ_KEY,
		STATE_WAIT_VALUE,
		STATE_READ_VALUE,
		STATE_READ_VALUE_STRING,
		STATE_READ_VALUE_NUMBER,
		STATE_DONE,
	};

	JsonObject ParseInternal(Framework::CStream&);

	void Process_State_Init(char);
	void Process_State_InsideScope(char);
	void Process_State_ReadKey(char);
	void Process_State_WaitValue(char);
	void Process_State_ReadValue(char);
	void Process_State_ReadValueString(char);
	void Process_State_ReadValueNumber(char);

	STATE m_currentState = STATE_INIT;

	std::string m_currentKey;
	std::string m_currentValue;
	
	JsonObject m_currentObject;
};
