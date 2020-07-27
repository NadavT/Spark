#pragma once

#include <string>

namespace Spark
{
	enum class ResourceType
	{
		texture,
		internal
	};

	class Resource
	{
	public:
		virtual ~Resource() = default;

		std::string getName() const { return m_name; };
		ResourceType getType() const { return m_type; };
	protected:
		Resource(const std::string& name, ResourceType type) :m_name(name), m_type(type) {};
	private:
		std::string m_name;
		ResourceType m_type;
	};
}
