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

# Variables that describe dependencies
LSP_COMMON_LIB_VERSION     := 1.0.14
LSP_COMMON_LIB_NAME        := lsp-common-lib
LSP_COMMON_LIB_TYPE        := src
LSP_COMMON_LIB_URL         := git@github.com:lsp-plugins/$(LSP_COMMON_LIB_NAME).git

LSP_LLTL_LIB_VERSION       := 0.5.7
LSP_LLTL_LIB_NAME          := lsp-lltl-lib
LSP_LLTL_LIB_TYPE          := src
LSP_LLTL_LIB_URL           := git@github.com:lsp-plugins/$(LSP_LLTL_LIB_NAME).git

LSP_RUNTIME_LIB_VERSION    := 0.5.10
LSP_RUNTIME_LIB_NAME       := lsp-runtime-lib
LSP_RUNTIME_LIB_TYPE       := src
LSP_RUNTIME_LIB_URL        := git@github.com:lsp-plugins/$(LSP_RUNTIME_LIB_NAME).git

LSP_R3D_IFACE_VERSION      := 0.5.2
LSP_R3D_IFACE_NAME         := lsp-r3d-iface
LSP_R3D_IFACE_TYPE         := src
LSP_R3D_IFACE_URL          := git@github.com:lsp-plugins/$(LSP_R3D_IFACE_NAME).git

LSP_R3D_BASE_LIB_VERSION   := 0.5.5
LSP_R3D_BASE_LIB_NAME      := lsp-r3d-base-lib
LSP_R3D_BASE_LIB_TYPE      := src
LSP_R3D_BASE_LIB_URL       := git@github.com:lsp-plugins/$(LSP_R3D_BASE_LIB_NAME).git

LSP_R3D_GLX_LIB_VERSION    := 0.5.4
LSP_R3D_GLX_LIB_NAME       := lsp-r3d-glx-lib
LSP_R3D_GLX_LIB_TYPE       := src
LSP_R3D_GLX_LIB_URL        := git@github.com:lsp-plugins/$(LSP_R3D_GLX_LIB_NAME).git

LSP_WS_LIB_VERSION         := 0.5.4
LSP_WS_LIB_NAME            := lsp-ws-lib
LSP_WS_LIB_TYPE            := src
LSP_WS_LIB_URL             := git@github.com:lsp-plugins/$(LSP_WS_LIB_NAME).git

LSP_DSP_LIB_VERSION        := 0.5.14
LSP_DSP_LIB_NAME           := lsp-dsp-lib
LSP_DSP_LIB_TYPE           := src
LSP_DSP_LIB_URL            := git@github.com:lsp-plugins/$(LSP_DSP_LIB_NAME).git

LSP_TEST_FW_VERSION        := 1.0.8
LSP_TEST_FW_NAME           := lsp-test-fw
LSP_TEST_FW_TYPE           := src
LSP_TEST_FW_URL            := git@github.com:lsp-plugins/$(LSP_TEST_FW_NAME).git

LIBPTHREAD_VERSION         := system
LIBPTHREAD_NAME            := libpthread
LIBPTHREAD_TYPE            := opt
LIBPTHREAD_LDFLAGS         := -lpthread

LIBDL_VERSION              := system
LIBDL_NAME                 := libdl
LIBDL_TYPE                 := opt
LIBDL_LDFLAGS              := -ldl

LIBICONV_VERSION           := system
LIBICONV_NAME              := libiconv
LIBICONV_TYPE              := opt
LIBICONV_LDFLAGS           := -liconv

LIBSHLWAPI_VERSION         := system
LIBSHLWAPI_NAME            := libshlwapi
LIBSHLWAPI_TYPE            := opt
LIBSHLWAPI_LDFLAGS         := -lshlwapi

LIBWINMM_VERSION           := system
LIBWINMM_NAME              := libwinmm
LIBWINMM_TYPE              := opt
LIBWINMM_LDFLAGS           := -lwinmm

LIBMSACM_VERSION           := system
LIBMSACM_NAME              := libmsacm
LIBMSACM_TYPE              := opt
LIBMSACM_LDFLAGS           := -lmsacm32

LIBSNDFILE_VERSION         := system
LIBSNDFILE_NAME            := sndfile
LIBSNDFILE_TYPE            := pkg

LIBX11_VERSION             := system
LIBX11_NAME                := x11
LIBX11_TYPE                := pkg

LIBGL_VERSION              := system
LIBGL_NAME                 := gl
LIBGL_TYPE                 := pkg

LIBCAIRO_VERSION           := system
LIBCAIRO_NAME              := cairo
LIBCAIRO_TYPE              := pkg

LIBFREETYPE_VERSION        := system
LIBFREETYPE_NAME           := freetype2
LIBFREETYPE_TYPE           := pkg
