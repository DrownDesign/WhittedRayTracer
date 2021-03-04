#include "SceneLight.h"
#include "glm/glm.hpp"

SceneLight::SceneLight(const glm::vec3 & p, const glm::vec3 &i) : pos(p), intensity(i) {}
