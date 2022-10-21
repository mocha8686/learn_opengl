#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

template <typename K, typename V>
class Cache {
	private:
		std::unordered_map<K, std::shared_ptr<V>> cache;

	public:
		void set(K key, std::shared_ptr<V> value) {
			cache[key] = value;
		};

		std::optional<std::shared_ptr<V>> get(K key) {
			auto it = cache.find(key);
			if (it == cache.end()) return std::nullopt;
			return std::optional<std::shared_ptr<V>>(it->second);
		};
};