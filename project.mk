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

# Package version
ARTIFACT_NAME               = lsp-tk-lib
ARTIFACT_DESC               = Graphical toolkit library for Linux Studio Plugins Project
ARTIFACT_VARS               = LSP_TK_LIB
ARTIFACT_HEADERS            = lsp-plug.in
ARTIFACT_EXPORT_ALL         = 1
ARTIFACT_VERSION            = 0.5.0-devel

# List of dependencies
TEST_DEPENDENCIES = \
  LSP_TEST_FW

DEPENDENCIES = \
  STDLIB \
  LSP_COMMON_LIB \
  LSP_LLTL_LIB \
  LSP_R3D_BASE_LIB \
  LSP_RUNTIME_LIB \
  LSP_DSP_LIB \
  LSP_WS_LIB

LINUX_DEPENDENCIES = \
  LIBSNDFILE \
  XLIB \
  CAIRO
  
BSD_DEPENDENCIES = \
  LIBSNDFILE XLIB CAIRO

# For Linux-based systems, use libsndfile and xlib
ifeq ($(PLATFORM),Linux)
  DEPENDENCIES             += $(LINUX_DEPENDENCIES)
endif

# For BSD-based systems, use libsndfile and xlib
ifeq ($(PLATFORM),BSD)
  DEPENDENCIES             += $(LINUX_DEPENDENCIES)
endif

ALL_DEPENDENCIES = \
  $(DEPENDENCIES) \
  $(TEST_DEPENDENCIES) \
  $(LINUX_DEPENDENCIES) \
  $(BSD_DEPENDENCIES)
