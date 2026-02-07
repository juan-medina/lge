// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/aabb.hpp>
#include <lge/components/dirty.hpp>
#include <lge/components/label.hpp>
#include <lge/log.hpp>
#include <lge/renderer.hpp>
#include <lge/result.hpp>
#include <lge/systems/label_aabb_system.hpp>

#include <cstdint>
#include <entt/entt.hpp>

namespace lge {

auto label_aabb_system::update(const float /*dt*/) -> result<> {
	for(const auto entity: world.view<dirty, label>()) {
		const auto &lbl = world.get<label>(entity);

		const auto text_size = renderer::get_label_size(lbl);

		float min_x = 0.F;
		float max_x = 0.F;
		float min_y = 0.F;
		float max_y = 0.F;

		switch(lbl.horizontal_align) {
		case horizontal_alignment::left:
			min_x = 0.F;
			max_x = text_size.x;
			break;
		case horizontal_alignment::center:
			min_x = -text_size.x * 0.5F;
			max_x = text_size.x * 0.5F;
			break;
		case horizontal_alignment::right:
			min_x = -text_size.x;
			max_x = 0.F;
			break;
		}

		switch(lbl.vertical_align) {
		case vertical_alignment::top:
			min_y = 0.F;
			max_y = text_size.y;
			break;
		case vertical_alignment::center:
			min_y = -text_size.y * 0.5F;
			max_y = text_size.y * 0.5F;
			break;
		case vertical_alignment::bottom:
			min_y = -text_size.y;
			max_y = 0.F;
			break;
		}

		auto aa_bb = local_aabb{.min = {min_x, min_y}, .max = {max_x, max_y}};
		LGE_DEBUG("calculated local AABB for entity {}: min({}, {}), max({}, {})",
				  static_cast<std::uint32_t>(entity),
				  aa_bb.min.x,
				  aa_bb.min.y,
				  aa_bb.max.x,
				  aa_bb.max.y);
		world.emplace_or_replace<local_aabb>(entity, aa_bb);
	}

	return true;
}

} // namespace lge