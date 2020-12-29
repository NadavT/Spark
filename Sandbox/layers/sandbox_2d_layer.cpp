#include "sandbox_2d_layer.h"

Sandbox2DLayer::Sandbox2DLayer()
    : Spark::Layer2D("Sandbox 2d layer")
    , counter(0)
{
    const Spark::Texture &texture = Spark::ResourceManager::loadTexture("sandboxTexture", "textures/texture.jpg");
    Spark::ResourceManager::loadTexture("sandboxTexture2", "textures/test.jpg");
    m_quads.push_back(std::move(Spark::Render::createQuad(glm::vec2(0, 0), texture, glm::vec2(1))));
    addDrawable(m_quads.back());
}

void Sandbox2DLayer::OnAttach()
{
    SPARK_INFO("Attached {0}", getName());
    Layer2D::OnAttach();
}

void Sandbox2DLayer::OnDetach()
{
    Layer2D::OnDetach();
    SPARK_INFO("Detached {0}", getName());
}

bool Sandbox2DLayer::handleKeyPressed(Spark::KeyPressedEvent &e)
{
    switch (e.GetKeyCode())
    {
    case Spark::KeyCode::Left:
        dynamic_cast<Spark::Render::Quad *>(m_quads.front().get())->move({-0.05, 0});
        return true;
    case Spark::KeyCode::Right:
        dynamic_cast<Spark::Render::Quad *>(m_quads.front().get())->move({0.05, 0});
        return true;
    case Spark::KeyCode::Down:
        dynamic_cast<Spark::Render::Quad *>(m_quads.front().get())->move({0, 0.05});
        return true;
    case Spark::KeyCode::Up:
        dynamic_cast<Spark::Render::Quad *>(m_quads.front().get())->move({0, -0.05});
        return true;
    case Spark::KeyCode::N:
        counter++;
        m_quads.push_back(std::move(Spark::Render::createQuad(glm::vec2(-0.5 + counter / 10.0, -0.5),
                                                              *Spark::ResourceManager::getTexture("sandboxTexture2"))));
        addDrawable(m_quads.back());
        return true;
    case Spark::KeyCode::M:
        removeDrawable(m_quads.back().get());
        m_quads.pop_back();
        return true;
    case Spark::KeyCode::Z:
        dynamic_cast<Spark::Render::Quad *>(m_quads.front().get())->scale(glm::vec2(0.95f));
        return true;
    case Spark::KeyCode::X:
        dynamic_cast<Spark::Render::Quad *>(m_quads.front().get())->scale(glm::vec2(1.05f));
        return true;
    case Spark::KeyCode::C:
        dynamic_cast<Spark::Render::Quad *>(m_quads.front().get())->scale(glm::vec2(0.95f, 1));
        return true;
    case Spark::KeyCode::V:
        dynamic_cast<Spark::Render::Quad *>(m_quads.front().get())->scale(glm::vec2(1.05f, 1));
        return true;
    case Spark::KeyCode::F:
        dynamic_cast<Spark::Render::Quad *>(m_quads.front().get())->scale(glm::vec2(1, 0.95f));
        return true;
    case Spark::KeyCode::G:
        dynamic_cast<Spark::Render::Quad *>(m_quads.front().get())->scale(glm::vec2(1, 1.05f));
        return true;
    default:
        return false;
    }
}

void Sandbox2DLayer::OnEvent(Spark::Event &e)
{
    Layer2D::OnEvent(e);
    Spark::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox2DLayer::handleKeyPressed));
}
