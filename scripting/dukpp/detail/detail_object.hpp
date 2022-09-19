//
// Created by Stanislav "Koncord" Zhukov on 17.01.2021.
//

# pragma once

namespace dukpp::detail {
    inline void object_invalidate(duk_context *ctx, void *obj_ptr) noexcept {
        detail::RefManager::find_and_invalidate_native_object(ctx, obj_ptr);
    }
}
