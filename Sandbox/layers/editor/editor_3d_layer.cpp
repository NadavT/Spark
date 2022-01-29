#include "editor_3d_layer.h"

#include <memory>

Editor3DLayer::Editor3DLayer(Spark::Render::Camera &camera)
    : Spark::Layer3D("Editor 3d layer", camera)
    , m_camera(camera)
    , m_objectToEdit(nullptr)
    , m_moveTransform(*this)
    , m_rotateTransform(*this)
    , m_scaleTransform(*this)
    , m_selected(false)
    , m_selectedTransform(Transform::Move)
    , m_shownTransforms(ShownTransformMap::Move)
{
    m_moveTransform.addTransforms();
}

void Editor3DLayer::OnAttach()
{
    Layer3D::OnAttach();
    updateEditorObjects();
}

void Editor3DLayer::OnDetach()
{
    Layer3D::OnDetach();
}

void Editor3DLayer::OnUpdate(Spark::Time &diffTime)
{
    Layer3D::OnUpdate(diffTime);
    updateEditorObjects();
}

void Editor3DLayer::OnEvent(Spark::Event &e)
{
    Layer3D::OnEvent(e);
    Spark::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Spark::MouseMovedEvent>(SPARK_BIND_EVENT_FN(Editor3DLayer::handleMouseMoved));
    dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(Editor3DLayer::handleKeyPressed));
    dispatcher.Dispatch<Spark::MouseButtonPressedEvent>(SPARK_BIND_EVENT_FN(Editor3DLayer::handleMousePressed));
    dispatcher.Dispatch<Spark::MouseButtonReleasedEvent>(SPARK_BIND_EVENT_FN(Editor3DLayer::handleMouseReleased));
}

void Editor3DLayer::setObjectToEdit(Spark::Object3D &object)
{
    m_objectToEdit = &object;
}

bool Editor3DLayer::handleMouseMoved(Spark::MouseMovedEvent &e)
{
    if (m_selected)
    {
        switch (m_selectedTransform)
        {
        case Transform::Move:
            m_moveTransform.handleMoveTransformUpdate(m_camera, m_objectToEdit);
            break;
        case Transform::Rotate:
            m_rotateTransform.handleRotateTransformUpdate(m_camera, m_objectToEdit);
            break;
        case Transform::Scale:
            m_scaleTransform.handleScaleTransformUpdate(m_camera, m_objectToEdit);
            break;
        }
    }

    return false;
}

bool Editor3DLayer::handleKeyPressed(Spark::KeyPressedEvent &e)
{
    switch (e.GetKeyCode())
    {
    case Spark::KeyCode::G:
        if (m_shownTransforms != ShownTransformMap::Move)
        {
            removeAllTransforms();
            m_moveTransform.addTransforms();
            m_shownTransforms = ShownTransformMap::Move;
        }
        return true;
    case Spark::KeyCode::R:
        if (m_shownTransforms != ShownTransformMap::Rotate)
        {
            removeAllTransforms();
            m_rotateTransform.addTransforms();
            m_shownTransforms = ShownTransformMap::Rotate;
        }
        return true;
    case Spark::KeyCode::S:
        if (m_shownTransforms != ShownTransformMap::Scale)
        {
            removeAllTransforms();
            m_scaleTransform.addTransforms();
            m_shownTransforms = ShownTransformMap::Scale;
        }
        return true;
    case Spark::KeyCode::T:
        if (m_shownTransforms & ShownTransformMap::Move)
        {
            m_moveTransform.removeTransforms();
        }
        m_moveTransform.addTransforms(true);
        m_shownTransforms |= ShownTransformMap::Move;
        if (!(m_shownTransforms & ShownTransformMap::Rotate))
        {
            m_rotateTransform.addTransforms();
            m_shownTransforms |= ShownTransformMap::Rotate;
        }
        if (m_shownTransforms & ShownTransformMap::Scale)
        {
            m_scaleTransform.removeTransforms();
        }
        m_scaleTransform.addTransforms(true);
        m_shownTransforms |= ShownTransformMap::Scale;
        return true;
    default:
        return false;
    }
}

bool Editor3DLayer::handleMousePressed(Spark::MouseButtonPressedEvent &e)
{
    switch (e.GetMouseButton())
    {
    case Spark::MouseCode::ButtonLeft:
        findSelectedObject();
        if (m_selected)
        {
            switch (m_selectedTransform)
            {
            case Transform::Move:
                m_moveTransform.initializeMoveTransform(m_camera);
                return true;
            case Transform::Rotate:
                m_rotateTransform.initializeRotateTransform(m_camera);
                return true;
            case Transform::Scale:
                m_scaleTransform.initializeScaleTransform(m_camera);
                return true;
            }
        }
    default:
        return false;
    }
}

bool Editor3DLayer::handleMouseReleased(Spark::MouseButtonReleasedEvent &e)
{
    m_selected = false;
    m_moveTransform.release();
    m_rotateTransform.release();
    m_scaleTransform.release();

    return false;
}

void Editor3DLayer::updateEditorObjects()
{
    m_moveTransform.updateObjects(m_camera, m_objectToEdit);
    m_rotateTransform.updateObjects(m_camera, m_objectToEdit);
    m_scaleTransform.updateObjects(m_camera, m_objectToEdit);
}

void Editor3DLayer::findSelectedObject()
{
    float closest = -1;
    if (m_shownTransforms & ShownTransformMap::Move)
    {
        float closestMove = m_moveTransform.findSelectedMove(m_camera);
        if (closestMove != -1 && (closest == -1 || closestMove < closest))
        {
            closest = closestMove;
            m_selectedTransform = Transform::Move;
        }
    }
    if (m_shownTransforms & ShownTransformMap::Rotate)
    {
        float closestRotate = m_rotateTransform.findSelectedRotate(m_camera);
        if (closestRotate != -1 && (closest == -1 || closestRotate < closest))
        {
            closest = closestRotate;
            m_selectedTransform = Transform::Rotate;
        }
    }
    if (m_shownTransforms & ShownTransformMap::Scale)
    {
        float closestRotate = m_scaleTransform.findSelectedScale(m_camera);
        if (closestRotate != -1 && (closest == -1 || closestRotate < closest))
        {
            closest = closestRotate;
            m_selectedTransform = Transform::Scale;
        }
    }

    m_selected = closest != -1;
}

void Editor3DLayer::removeAllTransforms()
{
    if (m_shownTransforms & ShownTransformMap::Move)
    {
        m_moveTransform.removeTransforms();
    }
    if (m_shownTransforms & ShownTransformMap::Rotate)
    {
        m_rotateTransform.removeTransforms();
    }
    if (m_shownTransforms & ShownTransformMap::Scale)
    {
        m_scaleTransform.removeTransforms();
    }
}
