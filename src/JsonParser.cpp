#include "JsonParser.h"
#include <cassert>
#include <stdexcept>

CJsonParser::JsonObject CJsonParser::Parse(Framework::CStream& inputStream)
{
	CJsonParser parser;
	return parser.ParseInternal(inputStream);
}

CJsonParser::JsonObject CJsonParser::ParseInternal(Framework::CStream& inputStream)
{
	bool done = false;
	while(!done)
	{
		auto value = inputStream.Read8();
		if(inputStream.IsEOF()) break;
		switch(m_currentState)
		{
		case STATE_INIT:
			Process_State_Init(value);
			break;
		case STATE_INSIDE_SCOPE:
			Process_State_InsideScope(value);
			break;
		case STATE_READ_KEY:
			Process_State_ReadKey(value);
			break;
		case STATE_WAIT_VALUE:
			Process_State_WaitValue(value);
			break;
		case STATE_READ_VALUE:
			Process_State_ReadValue(value);
			break;
		case STATE_READ_VALUE_STRING:
			Process_State_ReadValueString(value);
			break;
		case STATE_READ_VALUE_NUMBER:
			Process_State_ReadValueNumber(value);
			break;
		case STATE_DONE:
			done = true;
			break;
		default:
			assert(false);
			break;
		}
	}
	if(m_currentState != STATE_DONE)
	{
		throw std::runtime_error("Unexpected end of file.");
	}
	return m_currentObject;
}

void CJsonParser::Process_State_Init(char input)
{
	switch(input)
	{
	case '{':
		m_currentState = STATE_INSIDE_SCOPE;
		break;
	default:
		throw std::runtime_error("Unexpected character.");
		break;
	}
}

void CJsonParser::Process_State_InsideScope(char input)
{
	switch(input)
	{
	case '\"':
		m_currentState = STATE_READ_KEY;
		break;
	case '}':
		//Pop scope, done if nothing else on the stack
		m_currentState = STATE_DONE;
		break;
	case ',':
		//Might be from a string value that was parsed
		break;
	case '\r':
	case '\n':
	case ' ':
		//Whitespace, we don't care
		break;
	default:
		throw std::runtime_error("Unexpected character.");
		break;
	}
}

void CJsonParser::Process_State_ReadKey(char input)
{
	switch(input)
	{
	case '\"':
		m_currentState = STATE_WAIT_VALUE;
		break;
	default:
		m_currentKey += input;
		break;
	}
}

void CJsonParser::Process_State_WaitValue(char input)
{
	switch(input)
	{
	case ':':
		m_currentState = STATE_READ_VALUE;
		break;
	default:
		throw std::runtime_error("Unexpected character.");
		break;
	}
}

void CJsonParser::Process_State_ReadValue(char input)
{
	switch(input)
	{
	case '{':
		assert(false);
		//Push new dictionary
		break;
	case '\"':
		assert(m_currentValue.empty());
		m_currentState = STATE_READ_VALUE_STRING;
		break;
	case ' ':
		//Whitespace meaningless here
		break;
	default:
		if(isdigit(input))
		{
			assert(m_currentValue.empty());
			m_currentValue += input;
			m_currentState = STATE_READ_VALUE_NUMBER;
		}
		else
		{
			throw std::runtime_error("Unexpected character.");
		}
		break;
	}
}

void CJsonParser::Process_State_ReadValueString(char input)
{
	switch(input)
	{
	case '\"':
		m_currentState = STATE_INSIDE_SCOPE;
		m_currentObject.insert(std::make_pair(m_currentKey, m_currentValue));
		m_currentKey.clear();
		m_currentValue.clear();
		break;
	default:
		m_currentValue += input;
		break;
	}
}

void CJsonParser::Process_State_ReadValueNumber(char input)
{
	switch(input)
	{
	case '}':
		m_currentObject.insert(std::make_pair(m_currentKey, m_currentValue));
		m_currentKey.clear();
		m_currentValue.clear();
		//Pop scope, done if nothing else on the stack
		m_currentState = STATE_DONE;
		break;
	default:
		if(isdigit(input))
		{
			m_currentValue += input;
		}
		else
		{
			throw std::runtime_error("Unexpected character.");
		}
	}
}
