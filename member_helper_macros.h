#pragma once

#define CreateMemberInit(X, name, init) \
private: \
	X name##_data{ init }; \
\
public: \
	This& name(X const& arg) { \
		this->name##_data = arg; \
		return *this; \
	} \
	X& name() { \
		return name##_data; \
	} \
	X const& name() const { \
		return name##_data; \
	}

#define CreateMember(X, name) CreateMemberInit(X, name, )

#define CreateOptionalMember(X, name) \
private: \
	std::optional<X> name##_data; \
\
public: \
	This& name(X const& arg) { \
		this->name##_data = arg; \
		return *this; \
	} \
	std::optional<X>& name() { \
		return name##_data; \
	} \
	std::optional<X> const& name() const { \
		return name##_data; \
	}

namespace te
{
}
