#pragma once

#include <spark.h>

class Sandbox2DLayer : public Spark::Layer2D
{
  public:
    Sandbox2DLayer();

    virtual void OnAttach();
    virtual void OnDetach();
    bool handleKeyPressed(Spark::KeyPressedEvent &e);
    virtual void OnEvent(Spark::Event &e);

  private:
    int counter;
    std::vector<std::shared_ptr<Spark::Render::Drawable>> m_quads;
};