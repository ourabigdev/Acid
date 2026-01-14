#pragma once

#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace acid
{

	using TypeId = std::size_t;

	template<typename T>
	class TypeInfo
	{
	public:
		TypeInfo() = delete;

		template<typename K, typename = std::enable_if_t<std::is_convertible_v<K *, T *>>>
		static TypeId GetTypeId() noexcept
		{
			std::type_index typeIndex(typeid(K));
			if (auto it = typeMap.find(typeIndex); it != typeMap.end())
				return it->second;

			const auto id = nextTypeId++;
			typeMap[typeIndex] = id;
			return id;
		}

	private:
		inline static TypeId nextTypeId = 0;
		inline static std::unordered_map<std::type_index, TypeId> typeMap{};
	};

} // namespace acid
