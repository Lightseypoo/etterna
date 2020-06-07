#pragma once
#include <string>
#include <array>
#include <vector>

#include "Etterna/Models/NoteData/NoteDataStructures.h"
#include "Etterna/Globals/MinaCalc/Dependent/IntervalHandInfo.h"


using std::pair;
using std::vector;

// big brain stuff
static const float wrjt_cv_factor = 3.F;

// should update detection so it's more similar to updated wrr
// probably needs better debugoutput
struct WideRangeJumptrillMod
{
	const CalcPatternMod _pmod = { WideRangeJumptrill };
	const std::string name = "WideRangeJumptrillMod";

#pragma region params

	float window_param = 3.F;

	float min_mod = 0.25F;
	float max_mod = 1.F;
	float base = 0.4F;

	float cv_reset = 0.5F;
	float cv_threshhold = 0.15F;

	const vector<pair<std::string, float*>> _params{
		{ "window_param", &window_param },

		{ "min_mod", &min_mod },
		{ "max_mod", &max_mod },
		{ "base", &base },

		{ "cv_reset", &cv_reset },
		{ "cv_threshhold", &cv_threshhold },
	};
#pragma endregion params and param map

	int window = 0;
	CalcMovingWindow<int> _mw_jt;
	int jt_counter = 0;

	bool bro_is_this_file_for_real = false;
	bool last_passed_check = false;

	float pmod = neutral;

	std::array<float, 3> seq_ms = { 0.F, 0.F, 0.F };

	// put this back again? seems to work well for wrr, however wrr is already
	// more generalized anyway
	// float moving_cv = moving_cv_init;

#pragma region generic functions

	inline void full_reset()
	{
		_mw_jt.zero();
		jt_counter = 0;
		seq_ms.fill(0.f);

		bro_is_this_file_for_real = false;
		last_passed_check = false;
		pmod = neutral;
	}

	inline void setup()
	{
		window =
		  CalcClamp(static_cast<int>(window_param), 1, max_moving_window_size);
	}

#pragma endregion

	inline auto check_last_mt(const meta_type& mt) -> bool
	{
		if (mt == meta_acca || mt == meta_ccacc || mt == meta_cccccc) {
			if (last_passed_check) {
				return true;
			}
		}
		return false;
	}

	inline void bibblybop(const meta_type& mt)
	{
		++jt_counter;
		if (bro_is_this_file_for_real) {
			++jt_counter;
		}
		if (check_last_mt(mt)) {
			++jt_counter;
			bro_is_this_file_for_real = true;
		}
	}

	inline void advance_sequencing(base_type& bt,
								   const meta_type& mt,
								   const meta_type& _last_mt,
								   CalcMovingWindow<float>& ms_any)
	{
		// ignore if we hit a jump
		if (bt == base_jump_jump || bt == base_single_jump) {
			return;
		}

		// note to self, im pretty sure we can use tc_ms to do half these checks
		// more reliably and faster, and without dropping the const qualification,
		// but i need to think about it

		// look for stuff thats jumptrillyable.. if that stuff... then leads
		// into more stuff.. that is jumptrillyable... then .... badonk it
		switch (mt) {
			case meta_cccccc:
				if (ms_any.roll_timing_check(wrjt_cv_factor, cv_threshhold)) {
					bibblybop(_last_mt);
					return;
				}
				break;
			case meta_ccacc:
				if (ms_any.ccacc_timing_check(wrjt_cv_factor, cv_threshhold)) {
					bibblybop(_last_mt);
					return;
				}
				break;
			case meta_acca:
				// don't bother adding if the ms values look benign
				if (ms_any.acca_timing_check(wrjt_cv_factor, cv_threshhold)) {
					bibblybop(_last_mt);
					return;
				}
				break;
			default:
				break;
		}

		bro_is_this_file_for_real = false;
	}

	inline auto operator()(const ItvHandInfo& itvhi) -> float
	{
		_mw_jt(jt_counter);

		// no taps, no jt
		if (itvhi.get_taps_windowi(window) == 0 ||
			_mw_jt.get_total_for_window(window) == 0) {
			return neutral;
		}

		pmod =
		  itvhi.get_taps_windowf(window) / _mw_jt.get_total_for_windowf(window);

		pmod = CalcClamp(pmod, min_mod, max_mod);

		interval_reset();

		return pmod;
	}

	inline void interval_reset()
	{
		// we could count these in metanoteinfo but let's do it here for now,
		// reset every interval when finished
		jt_counter = 0;
	}
};