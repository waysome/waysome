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
 * @addtogroup tests "Testing"
 *
 * This module contains all the test sources for the check framework based
 * tests.
 *
 * @{
 */

#ifndef __WS_TESTS_TESTS_H__
#define __WS_TESTS_TESTS_H__

#define WS_TESTS_CHECK_MAIN_ADJUST(genf,ckflags)                    \
    int                                                             \
    main(void) {                                                    \
        Suite* s;                                                   \
        SRunner* r;                                                 \
        int nfailed;                                                \
                                                                    \
        s = genf();                                                 \
        r = srunner_create(s);                                      \
                                                                    \
        srunner_run_all(r,ckflags);                                 \
        nfailed = srunner_ntests_failed(r);                         \
                                                                    \
        srunner_free(r);                                            \
        return nfailed;                                             \
    }


#define WS_TESTS_CHECK_MAIN(suite_generator_func)   \
        WS_TESTS_CHECK_MAIN_ADJUST(suite_generator_func,CK_NORMAL)

#endif // __WS_TESTS_TESTS_H__

/**
 * @}
 */
