/*
 * waysome - wayland based window manager
 *
 * Copyright in alphabetical order:
 *
 * Copyright (C) 2014-2015 Julian Ganz
 * Copyright (C) 2014-2015 Manuel Messner
 * Copyright (C) 2014-2015 Marcel Müller
 * Copyright (C) 2014-2015 Matthias Beyer
 * Copyright (C) 2014-2015 Nadja Sommerfeld
 *
 * This file is part of waysome.
 *
 * waysome is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * waysome is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with waysome. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @addtogroup utils "(internal) utilities"
 *
 * @{
 */

#ifndef __WS_UTIL_ATTRIBUTES_H__
#define __WS_UTIL_ATTRIBUTES_H__

/*
 * @file attributes.h
 *
 * @brief Aliasing of compiler supported macros
 *
 * This file contains aliases for macros which are supported by the compiler.
 * For example, we alias __ws_pure__ to __attribute__((pure)) here if we compile
 * with a gcc compiler.
 *
 * If the compiler does not support such macros, we define them to nothing, to
 * ensure we can still compile our code.
 *
 * Each alias is written in snake_case with two leading and two least
 * underscores, to produce a hint to the __attribute__ macro.
 *
 * We also define some aliases for ourselves in here, such as WS_FORCE_INLINE,
 * which evaluates to 'inline __ws_always_inline__'. As this is not _just_ an alias
 * for an compiler macro, we use the normal naming convention here.
 *
 */

#ifdef __GNUC__

#define __ws_always_inline__        __attribute__((always_inline))
#define __ws_no_inline__            __attribute__((noinline))
#define __ws_cold__                 __attribute__((cold))
#define __ws_hot__                  __attribute__((hot))
#define __ws_pure__                 __attribute__((pure))
#define __ws_const__                __attribute__((const))
#define __ws_weak__                 __attribute__((weak))
#define __ws_alias__(x)             __attribute__((alias(x)))
#define __ws_deprecated__           __attribute__((deprecated))
#define __ws_fastcall__             __attribute__((fastcall))
#define __ws_format__(x,y,z)        __attribute__((format(x,y,z)))
#define __ws_format_arg__(n)        __attribute__((format_arg(n)))
#define __ws_malloc__               __attribute__((malloc))
#define __ws_nonnull__(...)         __attribute__((nonnull (__VA_ARGS__)))
#define __ws_returns_nonnull__      __attribute__((returns_nonnull))
#define __ws_noreturn__             __attribute__((noreturn))
#define __ws_unused__               __attribute__((unused))
#define __ws_visibility__(x)        __attribute__((visibility(x)))

#define __ws_vis_default__          __ws_visibility__(default)
#define __ws_vis_hidden__           __ws_visibility__(hidden)
#define __ws_vis_internal__         __ws_visibility__(internal)
#define __ws_vis_protected__        __ws_visibility__(protected)

#define __ws_alloc_size__(x)          __attribute__((alloc_size(x)))
#define __ws_alloc_size2__(x,y)        __attribute__((alloc_size(x,y)))

#define __ws_warn_unused_result__   __attribute__((warn_unused_result))

#define WS_FORCE_INLINE             inline __ws_always_inline__

#else // __GNUC__

#define __ws_always_inline__
#define __ws_no_inline__
#define __ws_cold__
#define __ws_hot__
#define __ws_pure__
#define __ws_const__
#define __ws_weak__
#define __ws_alias__(x)
#define __ws_deprecated__
#define __ws_fastcall__
#define __ws_format__(x,y,z)
#define __ws_format_arg__(n)
#define __ws_malloc__
#define __ws_nonnull__(...)
#define __ws_returns_nonnull__
#define __ws_noreturn__
#define __ws_unused__
#define __ws_visibility__(x)

#define __ws_default__
#define __ws_hidden__
#define __ws_internal__
#define __ws_protected__

#define __ws_alloc_size__(x)
#define __ws_alloc_size2__(x,y)

#define __ws_warn_unused_result__

#define R_FORCE_INLINE

#endif // __GNUC__

#endif // __WS_UTIL_ATTRIBUTES_H__

/**
 * @}
 */
