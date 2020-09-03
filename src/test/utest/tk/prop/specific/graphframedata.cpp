/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 3 сент. 2020 г.
 *
 * lsp-tk-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-tk-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-tk-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/tk/tk.h>

namespace
{
    static const float default_row[] = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };

    static const float src_row1[] =    { 2.0f, 0.1f, 0.2f, -0.1f, -0.2f };
    static const float src_row2[] =    { 0.1f, 0.2f, 0.2f, 0.1f, 0.0f };
    static const float src_row3[] =    { 0.25f, -0.25f, 0.5f, -0.5f, 1.0f };

    static const float dst_row1[] =    { 1.0f, 0.1f, 0.2f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
    static const float dst_row2[] =    { 0.1f, 0.2f, 0.2f, 0.1f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
    static const float dst_row3[] =    { 0.25f, 0.0f, 0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
}

UTEST_BEGIN("tk.prop.specific", graphframedata)

    bool check_row(const float *row, const float *data, size_t count)
    {
        if (row == NULL)
            return false;
        for (size_t i=0; i<count; ++i)
            if (!float_equals_adaptive(row[i], data[i], count))
                return false;
        return true;
    }

    void test_frame(uint32_t rowid)
    {
        tk::prop::GraphFrameData fd;

        printf("Testing GraphFrameData with start rowid=%x\n", int32_t(rowid));

        fd.set_min(0.0f);
        fd.set_max(1.0f);
        fd.set_default(0.5f);
        UTEST_ASSERT(fd.set_rows(5));
        UTEST_ASSERT(fd.set_columns(5));
        fd.advance();

        // Check initial position
        UTEST_ASSERT(fd.bottom() == uint32_t(-8));
        UTEST_ASSERT(fd.top() == uint32_t(0));
        UTEST_ASSERT(fd.changes() == 0);
        UTEST_ASSERT(fd.last() == 0);
        UTEST_ASSERT(fd.first() == uint32_t(-fd.rows()));
        UTEST_ASSERT(fd.pending() == fd.last());
        UTEST_ASSERT(fd.capacity()== 8);

        // Check that buffer contains default data
        printf("  Verifying buffer emptiness...\n");
        for (size_t i=0; i<fd.capacity(); ++i)
        {
            printf("    Checking row %x...\n", int32_t(fd.bottom() + i));
            UTEST_ASSERT(check_row(fd.row(fd.bottom() + i), default_row, 5));
        }
        printf("    Checking row %x...\n", int32_t(fd.bottom() - 1));
        UTEST_ASSERT(!check_row(fd.row(fd.bottom() - 1), NULL, 5));
        printf("    Checking row %x...\n", int32_t(fd.top()));
        UTEST_ASSERT(!check_row(fd.row(fd.top()), NULL, 5));

        // Fill buffer with data
        printf("  Filling Buffer with data...\n");

        printf("    Filling row %x...\n", int32_t(rowid+0));
        fd.set_row(rowid+0, dst_row1);
        printf("    Filling row %x...\n", int32_t(rowid+2));
        fd.set_row(rowid+2, dst_row2);
        printf("    Filling row %x...\n", int32_t(rowid+4));
        fd.set_row(rowid+4, dst_row3);

        printf("  Checking state...\n");
        UTEST_ASSERT(fd.top()    == uint32_t(rowid + 5));
        UTEST_ASSERT(fd.bottom() == uint32_t(rowid - fd.capacity() + 5));
        UTEST_ASSERT(fd.changes()== 5);
        UTEST_ASSERT(fd.pending()== rowid);

        // Check that buffer contains default data
        printf("  Verifying buffer state...\n");
        for (size_t i=0; i<fd.capacity(); ++i)
        {
            const float *row = (i == 3) ? dst_row1 :
                               (i == 5) ? dst_row2 :
                               (i == 7) ? dst_row3 :
                               default_row;

            printf("    Checking row %x...\n", int32_t(fd.bottom() + i));
            UTEST_ASSERT(check_row(fd.row(fd.bottom() + i), row, 5));
        }

        // Resize row (small, no allocation)
        printf("  Resizing buffer (without allocation)...\n");
        size_t cap      = fd.capacity();
        size_t stride   = fd.stride();
        UTEST_ASSERT(fd.set_size(6, 6));
        UTEST_ASSERT(fd.capacity()== cap);
        UTEST_ASSERT(fd.stride() == stride);
        UTEST_ASSERT(fd.top()    == uint32_t(rowid + 5));
        UTEST_ASSERT(fd.bottom() == uint32_t(rowid - fd.capacity() + 5));
        UTEST_ASSERT(fd.changes()== 6);
        UTEST_ASSERT(fd.pending()== uint32_t(rowid - 1));

        for (size_t i=0; i<fd.capacity(); ++i)
        {
            const float *row = (i == 3) ? dst_row1 :
                               (i == 5) ? dst_row2 :
                               (i == 7) ? dst_row3 :
                               default_row;

            printf("    Checking row %x...\n", int32_t(fd.bottom() + i));
            UTEST_ASSERT(check_row(fd.row(fd.bottom() + i), row, 6));
        }

        // Resize row (with allocation)
        printf("  Resizing buffer (with allocation)...\n");
        UTEST_ASSERT(fd.set_size(16, 32));
        UTEST_ASSERT(fd.capacity()== 16);
        UTEST_ASSERT(fd.capacity()!= cap);
        UTEST_ASSERT(fd.stride() != stride);
        UTEST_ASSERT(fd.top()    == uint32_t(rowid + 5));
        UTEST_ASSERT(fd.bottom() == uint32_t(rowid - fd.capacity() + 5));
        UTEST_ASSERT(fd.changes()== 16);
        UTEST_ASSERT(fd.pending()== uint32_t(rowid - 11));

        for (size_t i=0; i<fd.capacity(); ++i)
        {
            const float *row = (i == 11) ? dst_row1 :
                               (i == 13) ? dst_row2 :
                               (i == 15) ? dst_row3 :
                               default_row;

            printf("    Checking row %x...\n", int32_t(fd.bottom() + i));
            UTEST_ASSERT(check_row(fd.row(fd.bottom() + i), row, 10));
        }
    }

    UTEST_MAIN
    {
        test_frame(0);
        test_frame(10);
        test_frame(-12);
    }

UTEST_END


