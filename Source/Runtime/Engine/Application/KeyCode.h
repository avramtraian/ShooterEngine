/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreTypes.h>

namespace SE
{

enum class KeyCode : u32
{
	Unknown = 0,
	// clang-format off

	A, B, C, D, E, F, G, H, I, J, K, L, M,
	N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

	Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,

	Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
	Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
	
	Space,
	Shift,
	Control,
	Alt,

	// clang-format on
};

enum class MouseButton : u8
{
    Unknown = 0,
	Left,
	Middle,
	Right,
};

} // namespace SE
