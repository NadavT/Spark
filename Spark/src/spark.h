#pragma once

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
#include "spark/objects/object.h"
#include "spark/objects/sphere.h"

//---------DRAWABLES--------
#include "spark/renderer/drawables/cube.h"
#include "spark/renderer/drawables/drawable.h"
#include "spark/renderer/drawables/drawable2d.h"
#include "spark/renderer/drawables/drawable3d.h"
#include "spark/renderer/drawables/quad.h"
#include "spark/renderer/drawables/sphere.h"

//---------PHYSICS----------
#include "spark/physics/bounding/sphere_bounding.h"
#include "spark/physics/object3d.h"
#include "spark/physics/ray_casting.h"
#include "spark/physics/sphere.h"
