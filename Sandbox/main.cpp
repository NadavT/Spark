#include <spark.h>

class SandboxLayer : public Spark::Layer
{
public:
	SandboxLayer()
		:Spark::Layer("Sandbox layer")
	{

	}

	virtual void OnAttach()
	{
		SPARK_INFO("Attached {0}", getName());
	}

	virtual void OnDetach()
	{
		SPARK_INFO("Detached {0}", getName());
	}

	virtual void OnUpdate(Spark::Time& diffTime)
	{
		
	}

	virtual void OnEvent(Spark::Event& e)
	{

	}

};

class Sandbox : public Spark::Application
{
public:
	Sandbox()
		: layer()
	{
		PushLayer(&layer);
	}

	~Sandbox()
	{
		PopLayer(&layer);
	}

private:
	SandboxLayer layer;
};

Spark::Application* Spark::CreateApplication()
{
	return new Sandbox();
}
