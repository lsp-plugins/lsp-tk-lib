/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 28 сент. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_COLLECTION_FLOATARRAY_H_
#define LSP_PLUG_IN_TK_PROP_COLLECTION_FLOATARRAY_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Array of floats
         */
        class FloatArray: public Property
        {
            private:
                FloatArray & operator = (const FloatArray &);
                FloatArray(const FloatArray &);

            protected:
                lltl::darray<float>     vItems;

            protected:
                explicit FloatArray(prop::Listener *listener = NULL);
                virtual ~FloatArray();

            public:
                /**
                 * Get all values stored in the raw array
                 * @return pointer to all values, may be NULL for empty array
                 */
                inline const float *values() const          { return vItems.array();        }

                /**
                 * Get size of the array in elements
                 * @return size of the array in elements
                 */
                inline size_t       size() const            { return vItems.size();         }

                /**
                 * Get capacity of the array
                 * @return capacity
                 */
                inline size_t       capacity() const        { return vItems.capacity();     }

                /**
                 * Get the value by specified index
                 * @param index index of the value
                 * @return the value or 0 if the bad index has been specified
                 */
                float               get(size_t index) const;

            public:
                /**
                 * Clear the collection, trims the array to 0 elements
                 */
                void                clear();

                /**
                 * Reserve space but do not resize
                 * @param size size of collection
                 * @return status of operation
                 */
                inline status_t     reserve(size_t size)                    { return (vItems.reserve(size)) ? STATUS_OK : STATUS_NO_MEM;    }

                /**
                 * Change size of the collection, the additional values will be initialized with zeros
                 * @param size size of collection
                 * @return status of operation
                 */
                status_t            resize(size_t size);

                /**
                 * Prepend values
                 * @param v value to prepend
                 * @param count number of values to append
                 * @return status of operation
                 */
                status_t            prepend(const float *v, size_t count);

                /**
                 * Prepend value
                 * @param v value to prepend
                 * @return status of operation
                 */
                inline status_t     prepend(float v)                        { return prepend(&v, 1);        }

                /**
                 * Append values
                 * @param v values to append
                 * @param count number of values to append
                 * @return status of operation
                 */
                status_t            append(const float *v, size_t count);

                /**
                 * Append value
                 * @param v value to append
                 * @return status of operation
                 */
                inline status_t     append(float v)                         { return append(&v, 1);             }

                /**
                 * Add value (same as append)
                 * @param v value to add
                 * @return status of operation
                 */
                inline status_t     add(float v)                            { return append(&v, 1);             }

                /**
                 * Add values (same as append)
                 * @param v values to append
                 * @param count number of values to add
                 * @return status of operation
                 */
                inline status_t     add(const float *v, size_t count)       { return append(v, count);          }

                /**
                 * Insert values
                 * @param idx index to insert
                 * @param v values to insert
                 * @param count number of values to insert
                 * @return status of operation
                 */
                status_t            insert(size_t idx, const float *v, size_t count);

                /**
                 * Insert value
                 * @param idx index to insert
                 * @param v values to insert
                 * @return status of operation
                 */
                inline status_t     insert(size_t idx, float v)             { return insert(idx, &v, 1);        }

                /**
                 * Remove items
                 * @param idx index of item to remove
                 * @param count number of items to remove
                 * @return status of operation
                 */
                status_t            remove(size_t idx, size_t count);

                /**
                 * Remove the item
                 * @param idx index of item to remove
                 * @return status of operation
                 */
                inline status_t     remove(size_t idx)                      { return remove(idx, 1);            }

                /**
                 * Set the items
                 * @param v values to set
                 * @param count number of elements
                 * @return status of operation
                 */
                status_t            set(const float *v, size_t count);

                /**
                 * Set values to item
                 * @param idx beginning index of the items to change
                 * @param v values
                 * @param count number of items to change
                 * @return status of operation
                 */
                status_t            set(size_t idx, const float *v, size_t count);

                /**
                 * Set value to item
                 * @param idx index of the item to change
                 * @param v value
                 * @return status of operation
                 */
                status_t            set(size_t idx, float v);

                /**
                 * Swap contents with another array
                 * @param src array to perform swap
                 */
                void                swap(FloatArray *src);
                inline void         swap(FloatArray &src)                   { swap(&src);                           }
        };

        namespace prop
        {
            class FloatArray: public tk::FloatArray
            {
                private:
                    FloatArray & operator = (const FloatArray &);
                    FloatArray(const FloatArray &);

                public:
                    explicit inline FloatArray(prop::Listener *listener = NULL): tk::FloatArray(listener) {}
            };
        }
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_COLLECTION_FLOATARRAY_H_ */
