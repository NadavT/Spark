#pragma once

#include "resource.h"

#include <memory>

namespace Spark
{
	class Texture : public Resource
	{
	public:
		explicit Texture(const std::string& name);
		virtual ~Texture() = default;
	};

	//std::unique_ptr<Texture> createTexture(const std::string& name, const std::string& path);
}
