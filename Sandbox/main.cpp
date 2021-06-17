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
        , m_msaa(0)
    {
        m_layer3d = std::make_unique<Sandbox3DLayer>();
#ifdef LAYER_2D
        m_layer2d = std::make_unique<Sandbox2DLayer>();
#endif
        PushLayer(m_layer3d.get());
    }

    virtual void generateOverlay()
    {
        Spark::Application::generateOverlay();

        ImGui::SetNextWindowSize(ImVec2(350, 170), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 170), ImGuiCond_Always);
        ImGui::Begin("Sandbox", NULL,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoNavInputs);
        if (ImGui::Combo("msaa", &m_msaa, "1\0002\0004\0008\00016"))
        {
            switch (m_msaa)
            {
            case 0:
                GetRenderer().setMSAA(1);
                break;
            case 1:
                GetRenderer().setMSAA(2);
                break;
            case 2:
                GetRenderer().setMSAA(4);
                break;
            case 3:
                GetRenderer().setMSAA(8);
                break;
            case 4:
                GetRenderer().setMSAA(16);
                break;
            default:
                break;
            }
        }

#ifdef LAYER_2D
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
#endif

        ImGui::End();
    }

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
    int m_msaa;
};

Spark::Application *Spark::CreateApplication()
{
    return new Sandbox();
}
