#
# Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
#
# This file is part of lsp-tk-lib
#
# lsp-tk-lib is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# lsp-tk-lib is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with lsp-tk-lib.  If not, see <https://www.gnu.org/licenses/>.
#

# List of dependencies
DEPENDENCIES = \
  LIBPTHREAD \
  LIBDL \
  LSP_COMMON_LIB \
  LSP_LLTL_LIB \
  LSP_R3D_IFACE \
  LSP_RUNTIME_LIB \
  LSP_DSP_LIB \
  LSP_WS_LIB

TEST_DEPENDENCIES = \
  LSP_TEST_FW

ifeq ($(PLATFORM),Linux)
  DEPENDENCIES             += \
    LIBSNDFILE \
    LIBX11 \
    LIBCAIRO \
    LIBFREETYPE

  TEST_DEPENDENCIES        += \
    LSP_R3D_BASE_LIB \
    LSP_R3D_GLX_LIB \
    LIBGL
endif

ifeq ($(PLATFORM),BSD)
  DEPENDENCIES             += \
    LIBSNDFILE \
    LIBX11 \
    LIBCAIRO \
    LIBICONV \
    LIBFREETYPE

  TEST_DEPENDENCIES        += \
    LSP_R3D_BASE_LIB \
    LSP_R3D_GLX_LIB \
    LIBGL
endif

ifeq ($(PLATFORM),Windows)
  DEPENDENCIES             += \
    LIBSHLWAPI \
    LIBWINMM \
    LIBMSACM
endif

ALL_DEPENDENCIES = \
  $(DEPENDENCIES) \
  $(TEST_DEPENDENCIES) \
  LIBSNDFILE \
  LIBCAIRO \
  LIBFREETYPE \
  LIBICONV \
  LIBX11 \
  LIBGL \
  LIBSHLWAPI \
  LIBWINMM \
  LIBMSACM \

