/**
 * @file device_properties.hxx
 * @author Cameron Shinn (ctshinn@ucdavis.edu)
 * @brief
 * @version 0.1
 * @date 2020-11-09
 *
 * @copyright Copyright (c) 2020
 *
 */
#pragma once

#include <type_traits>

namespace gunrock {
namespace cuda {

/**
 * @brief Struct holding kernel parameters will be passed in upon launch
 * @tparam block_dimensions_ Block dimensions to launch with
 * @tparam grid_dimensions_ Grid dimensions to launch with
 * @tparam shared_memory_bytes_ Amount of shared memory to allocate
 *
 * @todo dimensions should be dim3 instead of unsigned int
 */
template<
  unsigned int block_dimensions_,
  unsigned int grid_dimensions_,
  unsigned int shared_memory_bytes_ = 0
>
struct launch_params_t {
  enum : unsigned int {
    block_dimensions = block_dimensions_,
    grid_dimensions = grid_dimensions_,
    shared_memory_bytes = shared_memory_bytes_
  };
};

#define TEST_SM 75  // Temporary until we figure out how to get cabability combined

/**
 * @brief Struct holding kernel parameters for a specific SM version
 * @tparam combined_ver_ Combined major and minor compute capability version
 * @tparam block_dimensions_ Block dimensions to launch with
 * @tparam grid_dimensions_ Grid dimensions to launch with
 * @tparam shared_memory_bytes_ Amount of shared memory to allocate
 */
template<unsigned int combined_ver_, auto... launch_params_args_v>
struct sm_launch_params_t : launch_params_t<launch_params_args_v...> {
  enum : unsigned int {combined_ver = combined_ver_};
};

template<auto... launch_params_args_v>
struct fallback_launch_params_t : sm_launch_params_t<
                                    0,
                                    launch_params_args_v...
                                  > {};

// Easier declaration inside launch box template
template<auto... launch_params_args_v>
using fallback_t = fallback_launch_params_t<launch_params_args_v...>;

// Easier declaration inside launch box template
template<unsigned int combined_ver_, auto... launch_params_args_v>
using sm_t = sm_launch_params_t<combined_ver_, launch_params_args_v...>;

// Define named sm_launch_params_t structs for each SM version
#define SM_LAUNCH_PARAMS(combined) \
template<auto... launch_params_args_v>              \
using sm_##combined##_t = sm_launch_params_t<       \
                            combined,               \
                            launch_params_args_v... \
                          >;

SM_LAUNCH_PARAMS(86)
SM_LAUNCH_PARAMS(80)
SM_LAUNCH_PARAMS(75)
SM_LAUNCH_PARAMS(72)
SM_LAUNCH_PARAMS(70)
SM_LAUNCH_PARAMS(62)
SM_LAUNCH_PARAMS(61)
SM_LAUNCH_PARAMS(60)
SM_LAUNCH_PARAMS(53)
SM_LAUNCH_PARAMS(52)
SM_LAUNCH_PARAMS(50)
SM_LAUNCH_PARAMS(37)
SM_LAUNCH_PARAMS(35)
SM_LAUNCH_PARAMS(30)

#undef SM_LAUNCH_PARAMS

template<typename... sm_lp_v>
struct device_launch_params_t;

// 1st to (N-1)th sm_launch_param_t template parameters
template<typename sm_lp_t, typename... sm_lp_v>
struct device_launch_params_t<sm_lp_t, sm_lp_v...> :
std::conditional_t<
  sm_lp_t::combined_ver == 0,
  device_launch_params_t<sm_lp_v..., sm_lp_t>,  // If found, move fallback_launch_params_t to end of template parameter pack
  std::conditional_t<                           // Otherwise check sm_lp_t for device's SM version
    sm_lp_t::combined_ver == TEST_SM,
    sm_lp_t,
    device_launch_params_t<sm_lp_v...>
  >
> {};

//////////////// https://stackoverflow.com/a/3926854
// "false" but dependent on a template parameter so the compiler can't optimize it for static_assert()
template<typename T>
struct always_false {
    enum { value = false };
};

// Raises static (compile-time) assert when referenced
template<typename T>
struct raise_not_found_error_t {
  static_assert(always_false<T>::value, "launch_box_t could not find valid launch_params_t");
};
////////////////

// Nth sm_launch_param_t template parameter
template<typename sm_lp_t>
struct device_launch_params_t<sm_lp_t> :
std::conditional_t<
  sm_lp_t::combined_ver == TEST_SM || sm_lp_t::combined_ver == 0,
  sm_lp_t,
  raise_not_found_error_t<void>  // Raises a compiler error
> {};

/**
 * @brief Collection of kernel launch parameters for multiple architectures
 * @tparam sm_lp_v... Pack of sm_launch_params_t types for each desired arch
 */
template<typename... sm_lp_v>
struct launch_box_t : device_launch_params_t<sm_lp_v...> {
  // Some methods to make it easy to access launch_params
};

}  // namespace gunrock
}  // namespace cuda
