#include "editor_3d_layer.h"

Editor3DLayer::Editor3DLayer(Spark::Render::Camera &camera)
    : Spark::Layer3D("Editor 3d layer", camera)
    , m_camera(camera)
{

    m_xArrow = createArrow({1, 0, 0});
    m_xArrow->rotate(glm::radians(90.0f), {0, 1, 0});
    m_xArrow->setAsRelativeTransform();
    float relative = std::max(glm::distance(m_camera.getPosition(), m_xArrow->getPhysicsObject().getPosition()), 2.5f);
    m_xArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});

    m_yArrow = createArrow({0, 1, 0});
    m_yArrow->rotate(glm::radians(270.0f), {1, 0, 0});
    m_yArrow->setAsRelativeTransform();
    m_yArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});

    m_zArrow = createArrow({0, 0, 1});
    m_zArrow->setAsRelativeTransform();
    m_zArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
}

void Editor3DLayer::OnAttach()
{
    Layer3D::OnAttach();
}

void Editor3DLayer::OnDetach()
{
    Layer3D::OnDetach();
}

void Editor3DLayer::OnUpdate(Spark::Time &diffTime)
{
    Layer3D::OnUpdate(diffTime);
    float relative = std::max(glm::distance(m_camera.getPosition(), m_xArrow->getPhysicsObject().getPosition()), 2.5f);
    m_xArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_yArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_zArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
}

void Editor3DLayer::OnEvent(Spark::Event &e)
{
    Layer3D::OnEvent(e);
    Spark::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Spark::MouseMovedEvent>(SPARK_BIND_EVENT_FN(Editor3DLayer::handleMouseMoved));
    dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(Editor3DLayer::handleKeyPressed));
    dispatcher.Dispatch<Spark::MouseButtonPressedEvent>(SPARK_BIND_EVENT_FN(Editor3DLayer::handleMousePressed));
}

void Editor3DLayer::setObjectPosition(glm::vec3 pos)
{
    m_xArrow->setPosition(pos);
    m_yArrow->setPosition(pos);
    m_zArrow->setPosition(pos);
}

std::shared_ptr<Spark::Object3D> Editor3DLayer::createArrow(glm::vec3 color)
{
    std::shared_ptr<Spark::Object3D> arrowBody = Spark::createCylinder(glm::vec3(0, 0, 0), 0.2f, 0.2f, 2, color);
    addObject(*arrowBody);
    std::shared_ptr<Spark::Object3D> arrowHead = Spark::createCylinder(glm::vec3(0, 0, 1.25f), 0.5f, 0, 0.5f, color);
    addObject(*arrowHead);
    arrowHead->setParent(arrowBody.get());
    arrowBody->move({0, 0, 1});
    arrowBody->setAsRelativeTransform();
    return arrowBody;
}

bool Editor3DLayer::handleMouseMoved(Spark::MouseMovedEvent &e)
{
    return false;
}

bool Editor3DLayer::handleKeyPressed(Spark::KeyPressedEvent &e)
{
    return false;
}

bool Editor3DLayer::handleMousePressed(Spark::MouseButtonPressedEvent &e)
{
    return false;
}
