#
# Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#------------------------------------------------------------------------------
# List of common dependencies
DEPENDENCIES = \
  LSP_COMMON_LIB \
  LSP_LLTL_LIB \
  LSP_R3D_IFACE \
  LSP_RUNTIME_LIB \
  LSP_DSP_LIB \
  LSP_WS_LIB

TEST_DEPENDENCIES = \
  LSP_TEST_FW

#------------------------------------------------------------------------------
# Linux dependencies
LINUX_DEPENDENCIES = \
  LIBCAIRO \
  LIBDL \
  LIBFONTCONFIG \
  LIBFREETYPE \
  LIBPTHREAD \
  LIBRT \
  LIBSNDFILE \
  LIBX11 \
  LIBXRANDR

LINUX_TEST_DEPENDENCIES = \
  LSP_R3D_BASE_LIB \
  LSP_R3D_GLX_LIB \
  LIBGL

ifeq ($(PLATFORM),Linux)
  DEPENDENCIES             += $(LINUX_DEPENDENCIES)
  TEST_DEPENDENCIES        += $(LINUX_TEST_DEPENDENCIES)
endif

#------------------------------------------------------------------------------
# BSD dependencies
BSD_DEPENDENCIES = \
  LIBCAIRO \
  LIBDL \
  LIBFONTCONFIG \
  LIBFREETYPE \
  LIBICONV \
  LIBPTHREAD \
  LIBRT \
  LIBSNDFILE \
  LIBX11 \
  LIBXRANDR

BSD_TEST_DEPENDENCIES = \
  LSP_R3D_BASE_LIB \
  LSP_R3D_GLX_LIB \
  LIBGL

ifeq ($(PLATFORM),BSD)
  DEPENDENCIES             += $(BSD_DEPENDENCIES)
  TEST_DEPENDENCIES        += $(BSD_TEST_DEPENDENCIES)
endif

#------------------------------------------------------------------------------
# Windows dependencies
WINDOWS_DEPENDENCIES = \
  LIBSHLWAPI \
  LIBWINMM \
  LIBMSACM \
  LIBMPR \
  LIBGDI32 \
  LIBD2D1 \
  LIBOLE \
  LIBWINCODEC \
  LIBDWRITE \
  LIBUUID

WINDOWS_TEST_DEPENDENCIES = \
  LSP_R3D_BASE_LIB \
  LSP_R3D_WGL_LIB \
  LIBOPENGL32

ifeq ($(PLATFORM),Windows)
  DEPENDENCIES             += $(WINDOWS_DEPENDENCIES)
  TEST_DEPENDENCIES        += $(WINDOWS_TEST_DEPENDENCIES)
endif

#------------------------------------------------------------------------------
# MacOS dependencies
MACOS_DEPENDENCIES = \
  LIBAUDIOTOOLBOX \
  LIBCOREFOUNDATION \
  LIBICONV  

MACOS_TEST_DEPENDENCIES =

ifeq ($(PLATFORM),MacOS)
  DEPENDENCIES             += $(MACOS_DEPENDENCIES)
  TEST_DEPENDENCIES        += $(MACOS_TEST_DEPENDENCIES)
endif

#------------------------------------------------------------------------------
# All possible dependencies
ALL_DEPENDENCIES = \
  $(DEPENDENCIES) \
  $(LINUX_DEPENDENCIES) \
  $(BSD_DEPENDENCIES) \
  $(WINDOWS_DEPENDENCIES) \
  $(MACOS_DEPENDENCIES) \
  $(TEST_DEPENDENCIES) \
  $(LINUX_TEST_DEPENDENCIES) \
  $(BSD_TEST_DEPENDENCIES) \
  $(WINDOWS_TEST_DEPENDENCIES) \
  $(MACOS_TEST_DEPENDENCIES)

