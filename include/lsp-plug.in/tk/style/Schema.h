/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_STYLE_SCHEMA_H_
#define LSP_PLUG_IN_TK_STYLE_SCHEMA_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/lltl/parray.h>
#include <lsp-plug.in/lltl/pphash.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/runtime/Color.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/IInSequence.h>
#include <lsp-plug.in/fmt/xml/PullParser.h>
#include <lsp-plug.in/resource/ILoader.h>

namespace lsp
{
    namespace tk
    {
        class Atoms;
        class Display;

        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(Root, Style)
                prop::Float                         sScaling;
                prop::Float                         sFontScaling;
                prop::Font                          sFont;
                prop::DrawMode                      sDrawMode;
                prop::Boolean                       sInvertMouseHScroll;
                prop::Boolean                       sInvertMouseVScroll;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * This class describes the system schema.
         * The schema provides complete style hierarchy and may be
         * reconfigured by applying another style sheet.
         */
        class Schema
        {
            protected:
                enum flags_t
                {
                    S_CONFIGURING   = 1 << 0,       // Schema is in configuration state
                    S_INITIALIZED   = 1 << 1,       // Schema is initialized
                };

                typedef struct raw_property_t
                {
                    const LSPString *name;
                    const LSPString *value;
                    ssize_t order;
                } raw_property_t;

                typedef struct property_value_t
                {
                    property_type_t     type;
                    union
                    {
                        bool            bvalue;
                        int             ivalue;
                        float           fvalue;
                    };
                    LSPString           svalue;
                } property_value_t;

            protected:
                mutable Atoms                      *pAtoms;
                mutable Display                    *pDisplay;
                size_t                              nFlags;
                Style                              *pRoot;
                lltl::pphash<LSPString, Style>      vBuiltin;
                lltl::pphash<LSPString, Style>      vStyles;
                lltl::pphash<LSPString, lsp::Color> vColors;

                prop::Float                         sScaling;
                prop::Float                         sFontScaling;
                prop::Font                          sFont;
                prop::DrawMode                      sDrawMode;
                prop::Boolean                       sInvertMouseHScroll;
                prop::Boolean                       sInvertMouseVScroll;

            protected:
                static ssize_t      compare_properties_by_order(const raw_property_t *a, const raw_property_t *b);
                static bool         make_raw_properties(StyleSheet::style_t *xs, lltl::darray<raw_property_t> *props);

            protected:
                status_t            create_builtin_style(IStyleFactory *init);
                status_t            create_style(const LSPString *name);
                status_t            create_missing_styles(const StyleSheet *sheet);
                status_t            unlink_styles();
                status_t            link_styles(const StyleSheet *sheet);
                status_t            configure_styles(const StyleSheet *sheet);
                static bool         check_parents_configured(Style *s);

                status_t            apply_settings(Style *s, StyleSheet::style_t *xs);
                status_t            apply_relations(Style *s, const lltl::parray<LSPString> *parents);
                status_t            apply_relations(Style *s, const char *parents);
                void                destroy_colors();
                status_t            init_colors_from_sheet(const StyleSheet *sheet);
                status_t            load_fonts_from_sheet(const StyleSheet *sheet, resource::ILoader *loader);
                static status_t     parse_property_value(property_value_t *v, const LSPString *text, property_type_t pt);

                void                bind(Style *root);

                status_t            apply_internal(const StyleSheet *sheet, resource::ILoader *loader);

            public:
                explicit Schema(Atoms *atoms, Display *dpy);
                Schema(const Schema &) = delete;
                Schema(Schema &&) = delete;
                virtual ~Schema();

                void                destroy();

                Schema & operator = (const Schema &) = delete;
                Schema & operator = (Schema &&) = delete;

                /**
                 * Initialize schema with the specified list of styles
                 * Can be run only once after the schema is instantiated. Otherwise
                 * it will return error
                 *
                 * @param list list of styles
                 * @return status of operation
                 */
                status_t            init(lltl::parray<IStyleFactory> *list);
                status_t            init(lltl::parray<IStyleFactory> &list);

                /**
                 * Initialize schema with the specified list of styles
                 * Can be run only once after the schema is instantiated. Otherwise
                 * it will return error
                 *
                 * @param list array of pointers to style initializers
                 * @param n number of elements in array
                 * @return status of operation
                 */
                status_t            init(IStyleFactory **list, size_t n);

                /**
                 * Register additional styles
                 * @param list list of styles to register
                 * @return status of operation
                 */
                status_t            add(lltl::parray<IStyleFactory> *list);
                status_t            add(lltl::parray<IStyleFactory> &list);

                /**
                 * Register additional styles
                 * @param list array of pointers to style initializers
                 * @param n number of elements in array
                 * @return status of operation
                 */
                status_t            add(IStyleFactory **list, size_t n);

                /**
                 * Register additional style
                 * @param factory style factory
                 * @return status of operation
                 */
                status_t            add(IStyleFactory *factory);

                /**
                 * Apply stylesheet settings to the schema
                 * @param sheet style sheet
                 * @param loader resource loader
                 * @return status of operation
                 */
                status_t            apply(const StyleSheet *sheet, resource::ILoader *loader = NULL);

            public:
                LSP_TK_PROPERTY(Float,          scaling,                &sScaling)
                LSP_TK_PROPERTY(Float,          font_scaling,           &sFontScaling)
                LSP_TK_PROPERTY(Font,           font,                   &sFont)
                LSP_TK_PROPERTY(DrawMode,       draw_mode,              &sDrawMode)
                LSP_TK_PROPERTY(Boolean,        invert_mouse_hscroll,   &sInvertMouseHScroll)
                LSP_TK_PROPERTY(Boolean,        invert_mouse_vscroll,   &sInvertMouseVScroll)

            public:
                /**
                 * Get color by identifier
                 * @param id color identifier
                 * @return color or NULL if not present
                 */
                lsp::Color         *color(const char *id);

                /**
                 * Get color by identifier
                 * @param id color identifier
                 * @return color or NULL if not present
                 */
                lsp::Color         *color(const LSPString *id);

                /**
                 * Get root style
                 * @return root style or NULL on error
                 */
                inline Style       *root() { return pRoot;  }

                /**
                 * Get style by class identifier.
                 * If style does not exists, it will be automatically created and bound to the root style
                 * @return style or NULL on error
                 */
                Style              *get(const char *id);

                /**
                 * Get style by class identifier.
                 * If style does not exists, it will be automatically created and bound to the root style
                 * @return style or NULL on error
                 */
                Style              *get(const LSPString *id);

                /**
                 * Get atom identifier by name
                 * @param name atom name
                 * @return atom identifier or negative error code
                 */
                atom_t              atom_id(const char *name) const;

                /**
                 * Get atom identifier by name
                 * @param name atom name
                 * @return atom identifier or negative error code
                 */
                atom_t              atom_id(const LSPString *name) const;

                /**
                 * Get atom name by identifier
                 * @param name atom name or NULL
                 * @return atom identifier
                 */
                const char         *atom_name(atom_t id) const;

                /**
                 * Get currently used language
                 * @param dst pointer to store the result
                 * @return status of operation
                 */
                status_t            get_language(LSPString *dst) const;

                /**
                 * Set currently used language
                 * @param lang language identifier
                 * @return status of operation
                 */
                status_t            set_lanugage(const LSPString *lang);
                status_t            set_lanugage(const char *lang);

                /**
                 * Check that schema is in configuration mode:
                 *   - no implicit overrides of style properties are allowed
                 *   - automatic property synchronization on change is enabled
                 * @return
                 */
                inline bool         config_mode() const           { return nFlags & S_CONFIGURING;  }

                /**
                 * Load font and add to the repository
                 * @param name font name in UTF-8
                 * @param path path to the file that contains the font data (UTF-8)
                 * @return status of operation
                 */
                status_t            add_font(const char *name, const char *path);

                /**
                 * Load font and add to the repository
                 * @param name font name in UTF-8
                 * @param path path to the file that contains the font data (UTF-8)
                 * @return status of operation
                 */
                status_t            add_font(const char *name, const io::Path *path);

                /**
                 * Load font and add to the repository
                 * @param name font name in UTF-8
                 * @param path path to the file that contains the font data (UTF-8)
                 * @return status of operation
                 */
                status_t            add_font(const char *name, const LSPString *path);

                /**
                 * Load font from stream and add to the repository
                 * @param name font name in UTF-8
                 * @param path path to the file that contains the font data (UTF-8)
                 * @return status of operation
                 */
                status_t            add_font(const char *name, io::IInStream *is);

                /**
                 * Add an alias to the font name
                 * @param name font name
                 * @param alias alias for the font name
                 * @return status of operation
                 */
                status_t            add_font_alias(const char *name, const char *alias);

                /**
                 * Remove font or font alias by identifier
                 * @param name name font name or font alias
                 * @return status of operation
                 */
                status_t            remove_font(const char *name);

                /**
                 * Remove all previously loaded custom fonts and aliases
                 */
                void                remove_all_fonts();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_STYLE_SCHEMA_H_ */
