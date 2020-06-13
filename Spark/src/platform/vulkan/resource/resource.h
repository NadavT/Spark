#pragma once

#include <string>

namespace Spark
{
	class VulkanResourse
	{
	public:
		std::string getName() const { return m_name; };
	protected:
		explicit VulkanResourse(const std::string& name) :m_name(name) {};
		virtual ~VulkanResourse() = default;
	private:
		std::string m_name;
	};
}

