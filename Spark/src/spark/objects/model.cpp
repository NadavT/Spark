#include "model.h"

#include "spark/physics/complex_object3d.h"
#include "spark/renderer/drawables/drawable3d.h"

namespace Spark
{
ModelObject::ModelObject(const Spark::Model &model, glm::vec3 position, glm::vec3 scale)
    : Object3D(Render::createModelDrawable(model, position, scale),
               std::make_unique<Physics::ComplexObject3D>(model.getMeshes(), model.getBounds()))
{
    getPhysicsObject().setPosition(position);
    getPhysicsObject().scale(scale);
}

std::unique_ptr<Object3D> createModelObject(const Spark::Model &model, glm::vec3 position, glm::vec3 scale)
{
    return std::make_unique<ModelObject>(model, position, scale);
}

} // namespace Spark
