#include <spark.h>

class Sandbox : public Spark::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Spark::Application* Spark::CreateApplication()
{
	SPARK_INFO("test {0}", 5);
	return new Sandbox();
}
