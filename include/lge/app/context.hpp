// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/dispatcher/dispatcher.hpp>
#include <lge/interface/input.hpp>
#include <lge/interface/renderer.hpp>
#include <lge/interface/resource_manager.hpp>
#include "entity/fwd.hpp"

#include <entt/entt.hpp>

namespace lge {

struct context {
	renderer &render;			 // NOLINT(*-avoid-const-or-ref-data-members)
	input &actions;				 // NOLINT(*-avoid-const-or-ref-data-members)
	resource_manager &resources; // NOLINT(*-avoid-const-or-ref-data-members)
	entt::registry &world;		 // NOLINT(*-avoid-const-or-ref-data-members)
	dispatcher &events;			 // NOLINT(*-avoid-const-or-ref-data-members)
};

} // namespace lge