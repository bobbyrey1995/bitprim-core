/**
 * Copyright (c) 2011-2018 Bitprim developers (see AUTHORS)
 *
 * This file is part of Bitprim.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHAINV2_HEADER_HPP_
#define LIBBITCOIN_CHAINV2_HEADER_HPP_

#include <cstddef>
#include <cstdint>
#include <istream>
#include <string>
#include <memory>
#include <vector>
#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin { namespace chainv2 {

class BC_API header {
public:
    typedef std::vector<header> list;
    typedef std::shared_ptr<header> ptr;
    typedef std::shared_ptr<const header> const_ptr;
    typedef std::vector<header> ptr_list;
    typedef std::vector<const_ptr> const_ptr_list;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    struct validation {
        size_t height = 0;
        uint32_t median_time_past = 0;
    };

    // Constructors.
    //-----------------------------------------------------------------------------

    header();

    header(header&& other);
    header(const header& other);

    header(header&& other, hash_digest&& hash);
    header(const header& other, const hash_digest& hash);

    header(uint32_t version, const hash_digest& previous_block_hash,
        const hash_digest& merkle, uint32_t timestamp, uint32_t bits,
        uint32_t nonce);
    header(uint32_t version, hash_digest&& previous_block_hash,
        hash_digest&& merkle, uint32_t timestamp, uint32_t bits, uint32_t nonce);

    // Operators.
    //-----------------------------------------------------------------------------

    /// This class is move and copy assignable.
    header& operator=(header&& other);
    header& operator=(const header& other);

    bool operator==(const header& other) const;
    bool operator!=(const header& other) const;

    // Deserialization.
    //-----------------------------------------------------------------------------

    static header factory_from_data(const data_chunk& data, bool wire=true);
    static header factory_from_data(std::istream& stream, bool wire=true);
    static header factory_from_data(reader& source, bool wire=true);

    bool from_data(const data_chunk& data, bool wire=true);
    bool from_data(std::istream& stream, bool wire=true);
    bool from_data(reader& source, bool wire=true);

    bool is_valid() const;

    // Serialization.
    //-----------------------------------------------------------------------------

    data_chunk to_data(bool wire=true) const;
    void to_data(std::ostream& stream, bool wire=true) const;
    void to_data(writer& sink, bool wire=true) const;

    // Properties (size, accessors, cache).
    //-----------------------------------------------------------------------------
    static uint256_t proof(uint32_t bits);
    uint256_t proof() const;

    static size_t satoshi_fixed_size();
    size_t serialized_size(bool wire=true) const;

    uint32_t version() const;
    void set_version(uint32_t value);

    // Deprecated (unsafe).
    hash_digest& previous_block_hash();

    const hash_digest& previous_block_hash() const;
    void set_previous_block_hash(const hash_digest& value);
    void set_previous_block_hash(hash_digest&& value);

    // Deprecated (unsafe).
    hash_digest& merkle();

    const hash_digest& merkle() const;
    void set_merkle(const hash_digest& value);
    void set_merkle(hash_digest&& value);

    uint32_t timestamp() const;
    void set_timestamp(uint32_t value);

    uint32_t bits() const;
    void set_bits(uint32_t value);

    uint32_t nonce() const;
    void set_nonce(uint32_t value);

    hash_digest hash() const;

#ifdef BITPRIM_CURRENCY_LTC
    hash_digest litecoin_proof_of_work_hash() const;
#endif //BITPRIM_CURRENCY_LTC

    // Validation.
    //-----------------------------------------------------------------------------

    bool is_valid_timestamp() const;
    bool is_valid_proof_of_work() const;

    code check() const;
    code accept(chain::chain_state const& state) const;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    mutable validation validation;

protected:
    // So that block may call reset from its own.
    friend class block;

    void reset();
    void invalidate_cache() const;

private:
    mutable upgrade_mutex mutex_;
    mutable std::shared_ptr<hash_digest> hash_;

    uint32_t version_;
    hash_digest previous_block_hash_;
    hash_digest merkle_;
    uint32_t timestamp_;
    uint32_t bits_;
    uint32_t nonce_;
};

}} // namespace libbitcoin::chainv2

#endif // LIBBITCOIN_CHAINV2_HEADER_HPP_