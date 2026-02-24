// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/scene/scene_manager.hpp>
#include <lge/systems/system.hpp>

namespace lge {

class transition_system: public system {
public:
	explicit transition_system(phase p, context &ctx, scene_manager &scenes): system{p, ctx}, scenes_{scenes} {}
	auto update(float dt) -> result<> override;

private:
	scene_manager &scenes_; // NOLINT(*-avoid-const-or-ref-data-members)
};

} // namespace lge