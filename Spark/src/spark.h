#ifndef SPARK_H
#define SPARK_H

//-----------CORE-----------
#include "spark/core/application.h"
#include "spark/core/core.h"
#include "spark/core/log.h"

//----------EVENTS----------
#include "spark/event/application_event.h"
#include "spark/event/event.h"
#include "spark/event/keyboard_event.h"
#include "spark/event/mouse_event.h"

//----------LAYERS----------
#include "spark/layer/layer.h"
#include "spark/layer/layer2d.h"
#include "spark/layer/layer3d.h"

//----------INPUTS----------
#include "spark/core/input.h"

//---------RESOURCES--------
#include "spark/resource/resource_manager.h"

//----------OBJECTS---------
#include "spark/objects/box.h"
#include "spark/objects/cylinder.h"
#include "spark/objects/model.h"
#include "spark/objects/object.h"
#include "spark/objects/pipe.h"
#include "spark/objects/sphere.h"

//----------CAMERAS---------
#include "spark/renderer/free_camera.h"
#include "spark/renderer/locked_camera.h"

//---------DRAWABLES--------
#include "spark/renderer/drawables/box.h"
#include "spark/renderer/drawables/cylinder.h"
#include "spark/renderer/drawables/drawable.h"
#include "spark/renderer/drawables/drawable2d.h"
#include "spark/renderer/drawables/drawable3d.h"
#include "spark/renderer/drawables/quad.h"
#include "spark/renderer/drawables/sphere.h"

//---------PHYSICS----------
#include "spark/physics/box.h"
#include "spark/physics/object3d.h"
#include "spark/physics/pipe.h"
#include "spark/physics/ray_casting.h"
#include "spark/physics/sphere.h"

//----------UTILS-----------
#include "spark/utils/file.h"
#include "spark/utils/math.h"

#endif /* SPARK_H */
