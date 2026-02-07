// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/aabb.hpp>
#include <lge/components/dirty.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/components/label.hpp>
#include <lge/log.hpp>
#include <lge/result.hpp>
#include <lge/systems/label_aabb_system.hpp>

#include <raylib.h>

#include <entity/fwd.hpp>

namespace lge {

auto label_aabb_system::update(const float /*dt*/) -> result<> {
	for(const auto entity: world.view<dirty, label>()) {
		const auto &lbl = world.get<label>(entity);

		const float text_width = static_cast<float>(MeasureText(lbl.text.c_str(), static_cast<int>(lbl.size)));
		const float text_height = lbl.size;

		float min_x = 0.f;
		float max_x = 0.f;
		float min_y = 0.f;
		float max_y = 0.f;

		switch(lbl.horizontal_align) {
		case horizontal_alignment::left:
			min_x = 0.f;
			max_x = text_width;
			break;
		case horizontal_alignment::center:
			min_x = -text_width * 0.5f;
			max_x = text_width * 0.5f;
			break;
		case horizontal_alignment::right:
			min_x = -text_width;
			max_x = 0.f;
			break;
		}

		switch(lbl.vertical_align) {
		case vertical_alignment::top:
			min_y = 0.f;
			max_y = text_height;
			break;
		case vertical_alignment::center:
			min_y = -text_height * 0.5f;
			max_y = text_height * 0.5f;
			break;
		case vertical_alignment::bottom:
			min_y = -text_height;
			max_y = 0.f;
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