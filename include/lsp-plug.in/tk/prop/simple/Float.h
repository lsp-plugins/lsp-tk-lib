/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 9 окт. 2019 г.
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

#ifndef UI_TK_SYS_LSPFLOAT_H_
#define UI_TK_SYS_LSPFLOAT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Floating-point property interface
         */
        class Float: public SimpleProperty
        {
            private:
                Float & operator = (const Float &);

            protected:
                class Listener: public IStyleListener
                {
                    protected:
                        Float      *pValue;

                    public:
                        inline Listener(Float *ptr)             { pValue = ptr;     }

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                float               fValue;
                Listener            sListener;

            protected:
                void                commit();

            protected:
                explicit Float(prop::Listener *listener = NULL);
                ~Float();

            public:
                /**
                 * Get value of the float property
                 * @return value of the float property
                 */
                inline float        get() const             { return fValue;                }

                /**
                 * Set value of the float property
                 * @param v value of the float property
                 * @return
                 */
                float               set(float v);

                /**
                 * Swap contents
                 * @param dst destination property to perform swap
                 */
                void                swap(Float *dst);

                inline float        add(float value)        { return set(fValue + value);   }
                inline float        sub(float value)        { return set(fValue - value);   }
        };

        namespace prop
        {
            /**
             * Floating-point property implementation
             */
            class Float: public tk::Float
            {
                private:
                    Float & operator = (const Float &);

                public:
                    explicit inline Float(prop::Listener *listener = NULL): tk::Float(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return SimpleProperty::bind(property, style, PT_FLOAT, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return SimpleProperty::bind(property, style, PT_FLOAT, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return SimpleProperty::bind(property, style, PT_FLOAT, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return SimpleProperty::unbind(&sListener); };

                    /**
                     * Init default value
                     * @param style style
                     * @param value default value
                     * @return status of operation
                     */
                    status_t            init(Style *style, float value);
                    status_t            override(Style *style, float value);

                    /**
                     * Change value without notification of any listener
                     * @param value value to set
                     */
                    inline void         commit(float value)                             { fValue        = value; }

                    inline void         listener(prop::Listener *listener)  { pListener = listener;                     }
            };
        }

    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_SYS_LSPFLOATPROPERTY_H_ */
