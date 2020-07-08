#pragma once
#include <string>
#include <array>
#include <vector>

#include "Etterna/Models/NoteData/NoteDataStructures.h"

struct CJDensityMod
{
	const CalcPatternMod _pmod = CJDensity;
	const std::string name = "CJDensityMod";
	const int _tap_size = quad;

#pragma region params

	float min_mod = 0.85F;
	float max_mod = 1.15F;
	float base = 0.F;

	float single_scaler = 2.F;
	float jump_scaler = 1.2F;
	float hand_scaler = 0.95F;
	float quad_scaler = 0.95F;

	const vector<std::pair<std::string, float*>> _params{
		{ "min_mod", &min_mod },
		{ "max_mod", &max_mod },
		{ "base", &base },

		{ "single_scaler", &single_scaler },
		{ "jump_scaler", &jump_scaler },
		{ "hand_scaler", &hand_scaler },
		{ "quad_scaler", &quad_scaler },
	};
#pragma endregion params and param map

	float pmod = neutral;

	auto operator()(const metaItvInfo& mitvi) -> float
	{
		const auto& itvi = mitvi._itvi;
		if (itvi.total_taps == 0) {
			return neutral;
		}

		const auto& t_taps = static_cast<float>(itvi.total_taps);
		auto a0 =
		  static_cast<float>(static_cast<float>(itvi.taps_by_size[single]) *
							 single_scaler) /
		  t_taps;
		auto a1 = static_cast<float>(
					static_cast<float>(itvi.taps_by_size[jump]) * jump_scaler) /
				  t_taps;
		auto a2 = static_cast<float>(
					static_cast<float>(itvi.taps_by_size[hand]) * hand_scaler) /
				  t_taps;
		auto a3 = static_cast<float>(
					static_cast<float>(itvi.taps_by_size[quad]) * quad_scaler) /
				  t_taps;

		auto aaa = a0 + a1 + a2 + a3;

		pmod = CalcClamp(base + fastsqrt(aaa), min_mod, max_mod);

		return pmod;
	}
};
