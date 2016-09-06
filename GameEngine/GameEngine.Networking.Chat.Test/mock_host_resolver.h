#pragma once
#include <gmock/gmock.h>
#include "host_resolver.h"

namespace GameEngine
{
namespace Networking
{
	class MockHostResolver : public HostResolver
	{
	public:
	    MOCK_METHOD3(ResolveHost, void(const std::string& host, const std::string& service, OnHostResolvedCallback& callback));
	};
}
}