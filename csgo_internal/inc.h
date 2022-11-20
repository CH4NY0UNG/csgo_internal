#pragma once

#include <Windows.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <tuple>
#include <intrin.h>
#include <utility>
#include <type_traits>
#include <random>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using namespace std;

#include "dllmain.h"
#include "lazy_importer.h"
#include "xor.h"
#include "offsets.h"
#include "binding.h"
#include "vector.h"
#include "calc.h"
#include "hook.h"
#include "draw.h"
#include "player.h"
#include "raytrace.h"
#include "func.h"