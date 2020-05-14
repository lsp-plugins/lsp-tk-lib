/*
 * IStyleInitializer.h
 *
 *  Created on: 14 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_STYLE_ISTYLEINITIALIZER_H_
#define LSP_PLUG_IN_TK_STYLE_ISTYLEINITIALIZER_H_

namespace lsp
{
    namespace tk
    {
        class Style;
        class Schema;

        /**
         * Style initialization interface
         */
        class IStyleInitializer
        {
            private:
                IStyleInitializer & operator = (const IStyleInitializer &);

            public:
                explicit IStyleInitializer();
                virtual ~IStyleInitializer();

            public:
                /**
                 * Initialize style of the chema
                 * @param style style to initialize
                 * @return status of operation
                 */
                virtual status_t init(Style *style);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_STYLE_ISTYLEINITIALIZER_H_ */
