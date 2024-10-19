#pragma once

#include <optional>

namespace te
{
#define TRY_CATCH(X) [&] () -> std::optional<decltype(X)> { try { return std::make_optional(X); } catch (std::exception const&){ return std::nullopt; } }()

#define TRY_CATCH_OR_0(X) [&] () -> decltype(X) { try { return X; } catch (std::exception const&){ return {}; } }()
}
