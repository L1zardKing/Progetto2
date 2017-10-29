#pragma once

#include <boost/serialization/string.hpp>

#include <string>

struct Message
{
public:
	std::wstring appName;
	std::wstring processPath;
	std::wstring filePath;
	std::wstring hwnd;
	bool active;
	template <class Archive>
	void serialize(
		Archive& ar,
		unsigned int version
		)
	{
		ar & appName;
	}
};


