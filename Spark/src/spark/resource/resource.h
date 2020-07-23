#pragma once

#include <string>

namespace Spark
{
	class Resource
	{
	public:
		std::string getName() const { return m_name; };
	protected:
		explicit Resource(const std::string& name) :m_name(name) {};
		virtual ~Resource() = default;
	private:
		std::string m_name;
	};
}
