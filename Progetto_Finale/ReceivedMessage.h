#pragma once

#include <boost/serialization/string.hpp>

#include <string>

struct ReceivedMessage
{
public:
	std::wstring controlKey;

	template <class Archive>
	void serialize(
		Archive& ar,
		unsigned int version
		)
	{
		ar & controlKey;
	}
};


