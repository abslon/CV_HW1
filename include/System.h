/*
 * Main header for all sources.
 */

#pragma once

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const double FPS = 29.99;

/// camera parameters of Galaxy Note 5
float fx = 959.562f;
float fy = 958.127f;
float cx = 625.232f;
float cy = 357.149f;

float k1 = 0.175740f;
float k2 = -0.332924f;
float k3 = -0.001955f;
float p1 = 0.0f;
float p2 = 0.0f;

float zfar = 10000.0f;
float znear = 0.00001f;