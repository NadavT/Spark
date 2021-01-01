#include <spark.h>
#include <spark/core/entry_point.h>

#include "layers/sandbox_2d_layer.h"
#include "layers/sandbox_3d_layer.h"

class Sandbox : public Spark::Application
{
  public:
    Sandbox()
        : m_layer3d()
#ifdef LAYER_2D
        , m_layer2d()
        , m_layer2DEnabled(false)
#endif
    {
        m_layer3d = std::make_unique<Sandbox3DLayer>();
#ifdef LAYER_2D
        m_layer2d = std::make_unique<Sandbox2DLayer>();
#endif
        PushLayer(m_layer3d.get());
    }

#ifdef LAYER_2D
    virtual void generateOverlay()
    {
        Spark::Application::generateOverlay();

        ImGui::SetNextWindowSize(ImVec2(350, 170), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 170), ImGuiCond_Always);
        ImGui::Begin("Sandbox", NULL,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        if (ImGui::Checkbox("2d layer", &m_layer2DEnabled))
        {
            if (m_layer2DEnabled)
            {
                PushLayerBefore(m_layer2d.get(), m_layer3d.get());
            }
            else
            {
                PopLayer(m_layer2d.get());
            }
        }

        ImGui::End();
    }
#endif

    bool handleKeyPressed(Spark::KeyPressedEvent &e)
    {
        return false;
    }

    virtual void handleEvent(Spark::Event &e)
    {
        Application::handleEvent(e);
        Spark::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox::handleKeyPressed));
    }

  private:
    std::unique_ptr<Sandbox3DLayer> m_layer3d;
#ifdef LAYER_2D
    std::unique_ptr<Sandbox2DLayer> m_layer2d;
    bool m_layer2DEnabled;
#endif
};

Spark::Application *Spark::CreateApplication()
{
    return new Sandbox();
}
