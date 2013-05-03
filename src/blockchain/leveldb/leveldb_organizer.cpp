#include "leveldb_organizer.hpp"

#include <bitcoin/utility/assert.hpp>

#include "leveldb_validate_block.hpp"

namespace libbitcoin {

leveldb_organizer::leveldb_organizer(leveldb_common_ptr common,
    orphans_pool_ptr orphans, leveldb_chain_keeper_ptr chain,
    subscriber_ptr reorganize_subscriber)
  : organizer(orphans, chain), common_(common), chain_(chain),
    reorganize_subscriber_(reorganize_subscriber)
{
}

std::error_code leveldb_organizer::verify(int fork_index,
    const block_detail_list& orphan_chain, int orphan_index)
{
    BITCOIN_ASSERT(orphan_index < orphan_chain.size());
    const message::block& current_block = orphan_chain[orphan_index]->actual();
    size_t depth = fork_index + orphan_index + 1;
    BITCOIN_ASSERT(depth != 0);
    leveldb_validate_block validate(common_, fork_index, orphan_chain,
        orphan_index, depth, chain_->txn(), current_block);
    return validate.start();
}
void leveldb_organizer::reorganize_occured(
    size_t fork_point,
    const blockchain::block_list& arrivals,
    const blockchain::block_list& replaced)
{
    reorganize_subscriber_->relay(std::error_code(),
        fork_point, arrivals, replaced);
}

} // namespace libbitcoin

