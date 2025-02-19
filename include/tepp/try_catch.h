#pragma once

#include <optional>

#include "tepp/optional_ref.h"

namespace te
{
#define TRY_CATCH(X) [&]() -> std::optional<decltype(X)> { try { return std::make_optional(X); } catch (std::exception const&){ return std::nullopt; } }()
#define TRY_CATCH_REF(X) [&]()  { try { return te::optional_ref(X); } catch (std::exception const&){ return decltype(te::optional_ref(X))(te::nullopt); } }()

#define TRY_CATCH_OR_0(X) [&]() -> decltype(X) { try { return X; } catch (std::exception const&){ return {}; } }()
}
